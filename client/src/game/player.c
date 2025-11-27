#include "player.h"
#include "level.h"
#include "../rendering/sprite_manager.h"
#include "../utils/constants.h"
#include "../utils/font_manager.h"
#include <stdio.h>
#include <math.h>

// External reference to level (for collision testing)
extern Level* g_current_level;

// ============================================================
// ANIMATION HELPER FUNCTIONS
// ============================================================

/**
 * Determines if player should animate based on current movement state
 */
static bool should_animate(Player* player) {
    switch (player->state) {
        case STATE_IDLE:
            return false;  // No animation when idle

        case STATE_RUNNING:
            return player->velocity_x != 0;  // Animate only when moving

        case STATE_CLIMBING:
            return player->velocity_y != 0;  // Animate only when climbing up/down

        case STATE_JUMPING:
        case STATE_FALLING:
            return false;  // Single frame for jump/fall

        case STATE_DYING:
            return true;  // Animate death sequence

        default:
            return false;
    }
}

/**
 * Gets the appropriate sprite type for current player state
 */
static SpriteType get_player_sprite(Player* player, int* out_max_frames) {
    *out_max_frames = 1;  // Default to single frame

    if (player->state == STATE_DYING) {
        *out_max_frames = 2;
        return SPRITE_JUNIOR_DEATH;
    }

    if (player->climbing) {
        *out_max_frames = 2;

        // Select sprite based on lateral position
        if (player->lateral_position == -1) {
            // LEFT side of vine → use LEFT climb sprite
            return SPRITE_JUNIOR_CLIMB_LEFT;
        } else if (player->lateral_position == 1) {
            // RIGHT side of vine → use RIGHT climb sprite
            return SPRITE_JUNIOR_CLIMB_RIGHT;
        } else {
            // CENTER position (0) → use CENTER climb sprite
            return SPRITE_JUNIOR_CLIMB_CENTER;
        }
    }

    switch (player->state) {
        case STATE_RUNNING:
            *out_max_frames = 3;
            return (player->direction == DIR_LEFT) ?
                   SPRITE_JUNIOR_RUN_LEFT : SPRITE_JUNIOR_RUN_RIGHT;

        case STATE_JUMPING:
        case STATE_FALLING:
            return (player->direction == DIR_LEFT) ?
                   SPRITE_JUNIOR_JUMP_LEFT : SPRITE_JUNIOR_JUMP_RIGHT;

        case STATE_IDLE:
        default:
            return SPRITE_JUNIOR_IDLE;
    }
}

// ============================================================
// PLAYER INITIALIZATION
// ============================================================

void player_init(Player* player, float x, float y) {
    player->x = x;
    player->y = y;
    player->velocity_x = 0;
    player->velocity_y = 0;
    player->on_ground = false;
    player->climbing = false;
    player->attached_vine_id = -1;
    player->lateral_position = 0;
    player->state = STATE_IDLE;
    player->direction = DIR_RIGHT;
    player->animation_time = 0;
    player->current_frame = 0;
    player->death_timer = 0;
    
    // Game state
    player->lives = 3;
    player->score = 0;

#if DEBUG_MODE
    printf("✓ Player initialized at (%.0f, %.0f)\n", x, y);
#endif
}

/**
 * Reset player to spawn position after death
 * Keeps lives and score intact (arcade accurate)
 */
void player_reset(Player* player, float spawn_x, float spawn_y) {
    player->x = spawn_x;
    player->y = spawn_y;
    player->velocity_x = 0;
    player->velocity_y = 0;
    player->on_ground = false;
    player->climbing = false;
    player->attached_vine_id = -1;
    player->lateral_position = 0;
    player->state = STATE_IDLE;
    player->direction = DIR_RIGHT;
    player->animation_time = 0;
    player->current_frame = 0;
    player->death_timer = 0;
    
    printf("[PLAYER] Reset to spawn position (%.0f, %.0f) - Lives: %d, Score: %d\n",
           spawn_x, spawn_y, player->lives, player->score);
}

// ============================================================
// COLLISION HELPER
// ============================================================

// Check if a specific lateral position would collide with platforms
// Returns true if the given lateral_pos would put player hitbox inside a platform

#define collision_position_fix 0.5f  // Minimum separation required (pixels)

static bool would_collide_at_position(Player* player, int lateral_pos) {
    if (!g_current_level) return false;
    
    // Get current vine position
    Vine* attached_vine = NULL;
    for (int i = 0; i < g_current_level->vine_count; i++) {
        if (g_current_level->vines[i].id == player->attached_vine_id) {
            attached_vine = &g_current_level->vines[i];
            break;
        }
    }
    if (!attached_vine) return false;

    // Calculate where player X would be with this lateral position
    float test_x;
    if (lateral_pos == 0) {
        // CENTER position
        test_x = attached_vine->x - PLAYER_WIDTH / 2;
    } else {
        // LEFT (-1) or RIGHT (1) side
        float hand_adjustment = (lateral_pos == -1) ? HAND_OFFSET_FROM_CENTER : -HAND_OFFSET_FROM_CENTER;
        float lateral_offset = lateral_pos * VINE_LATERAL_OFFSET;
        test_x = attached_vine->x - PLAYER_WIDTH / 2 + hand_adjustment + lateral_offset;
    }

    // Calculate collision box at test position
    float collision_left, collision_right;
    if (lateral_pos == -1) {
        // LEFT side: only left half has collision
        collision_left = test_x + COLLISION_OFFSET_X;
        collision_right = test_x + PLAYER_WIDTH / 2;
    } else if (lateral_pos == 1) {
        // RIGHT side: only right half has collision
        collision_left = test_x + PLAYER_WIDTH / 2;
        collision_right = test_x + COLLISION_OFFSET_X + COLLISION_WIDTH;
    } else {
        // CENTER: full collision box
        collision_left = test_x + COLLISION_OFFSET_X;
        collision_right = test_x + COLLISION_OFFSET_X + COLLISION_WIDTH;
    }

    float collision_top = player->y + COLLISION_OFFSET_Y;
    float collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;

    for (int p = 0; p < g_current_level->platform_count; p++) {
        Platform* platform = &g_current_level->platforms[p];
        float platform_width_px = platform->width_blocks * PLATFORM_BLOCK_SIZE;
        float platform_bottom = platform->y + PLATFORM_BLOCK_SIZE;

        // Collision box must be completely clear of platform
        bool would_collide = (
            collision_right > platform->x + collision_position_fix &&                      // Right edge past platform left + margin
            collision_left < platform->x + platform_width_px - collision_position_fix &&  // Left edge before platform right - margin
            collision_bottom > platform->y + collision_position_fix &&                   // Bottom edge past platform top + margin
            collision_top < platform_bottom - collision_position_fix                    // Top edge before platform bottom - margin
        );

        if (would_collide) {
            return true;  // Would collide!
        }
    }
    return false;  // No collision
}

// ============================================================
// INPUT HANDLING
// ============================================================

void player_handle_input(Player* player) {
    // No input during death animation
    if (player->state == STATE_DYING) {
        return;
    }

    if (player->climbing) {
        // Determine climb speed based on vine type and direction
        // CENTER vine (lateral_position == 0) = FAST both ways
        // SIDE vines (lateral_position == -1 or 1) = NORMAL up, FAST down
        float climb_speed_up, climb_speed_down;

        if (player->lateral_position == 0) {
            // Center vine: fast both ways
            climb_speed_up = CLIMB_SPEED_CENTER_UP;
            climb_speed_down = CLIMB_SPEED_CENTER_DOWN;
        } else {
            // Side vine: normal up, fast down
            climb_speed_up = CLIMB_SPEED_SIDE_UP;
            climb_speed_down = CLIMB_SPEED_SIDE_DOWN;
        }

        // Climbing mode - vertical movement
        if (IsKeyDown(KEY_UP)) {
            player->velocity_y = -climb_speed_up;
            player->state = STATE_CLIMBING;
        } else if (IsKeyDown(KEY_DOWN)) {
            player->velocity_y = climb_speed_down;
            player->state = STATE_CLIMBING;
        } else {
            player->velocity_y = 0;
        }
        
        // ========================================
        // VINE RULESET: Real center vines with height-based existence
        // lateral_position: -1=LEFT side, 0=CENTER, 1=RIGHT side
        // ========================================

        if (IsKeyPressed(KEY_LEFT)) {
            if (player->lateral_position == -1) {
                // ON_LEFT_SIDE + press LEFT → Try to move to CENTER vine
                if (g_current_level) {
                    Vine* current_vine = NULL;
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        if (g_current_level->vines[i].id == player->attached_vine_id) {
                            current_vine = &g_current_level->vines[i];
                            break;
                        }
                    }

                    if (current_vine && current_vine->visible) {
                        // Find center vine to the left (invisible, left of current)
                        Vine* center_vine = NULL;
                        float vine_bottom_tolerance = 20.0f;  // Allow reaching visual bottom
                        for (int i = 0; i < g_current_level->vine_count; i++) {
                            Vine* v = &g_current_level->vines[i];
                            if (!v->visible &&
                                v->x < current_vine->x &&
                                fabs(v->x - (current_vine->x - (VINE_SPACING / 2.0f))) < VINE_TRANSFER_TOLERANCE &&
                                player->y >= v->y_top &&
                                player->y + PLAYER_HEIGHT <= v->y_bottom + vine_bottom_tolerance) {
                                center_vine = v;
                                break;
                            }
                        }

                        if (center_vine) {
                            player->attached_vine_id = center_vine->id;
                            player->lateral_position = 0;
                        } else {
                            // No center vine at this height → FALL
                            player->climbing = false;
                            player->attached_vine_id = -1;
                            player->state = STATE_FALLING;
                        }
                    }
                }
            }
            else if (player->lateral_position == 0) {
                // ON_CENTER + press LEFT → Move to left visible vine
                // Note: We DON'T check collision here because we're moving to a different vine
                // The vine position itself determines if we can move there
                
                if (g_current_level) {
                    Vine* current_vine = NULL;
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        if (g_current_level->vines[i].id == player->attached_vine_id) {
                            current_vine = &g_current_level->vines[i];
                            break;
                        }
                    }

                    if (current_vine && !current_vine->visible) {
                        // Find left visible vine
                        Vine* left_vine = NULL;
                        float vine_bottom_tolerance = 20.0f;  // Allow reaching visual bottom
                        for (int i = 0; i < g_current_level->vine_count; i++) {
                            Vine* v = &g_current_level->vines[i];
                            if (v->visible &&
                                v->x < current_vine->x &&
                                fabs(v->x - (current_vine->x - (VINE_SPACING / 2.0f))) < 5.0f &&
                                player->y >= v->y_top &&
                                player->y + PLAYER_HEIGHT <= v->y_bottom + vine_bottom_tolerance) {
                                left_vine = v;
                                break;
                            }
                        }

                        if (left_vine) {
                            // Moving LEFT from center → land on RIGHT SIDE of left vine
                            player->attached_vine_id = left_vine->id;
                            player->lateral_position = 1;
                            player->direction = DIR_RIGHT;
                        } else {
                            // No left vine at this height → FALL
                            player->climbing = false;
                            player->attached_vine_id = -1;
                            player->state = STATE_FALLING;
                        }
                    }
                }
            }
            else if (player->lateral_position == 1) {
                // ON_RIGHT_SIDE + press LEFT → Switch to LEFT SIDE (same vine)
                // Check if switching sides would cause collision
                if (would_collide_at_position(player, -1)) {
                    // Cannot switch to left side - would collide
                    return;
                }
                player->lateral_position = -1;
                player->direction = DIR_LEFT;
            }
        }
        else if (IsKeyPressed(KEY_RIGHT)) {
            if (player->lateral_position == 1) {
                // ON_RIGHT_SIDE + press RIGHT → Try to move to CENTER vine
                if (g_current_level) {
                    Vine* current_vine = NULL;
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        if (g_current_level->vines[i].id == player->attached_vine_id) {
                            current_vine = &g_current_level->vines[i];
                            break;
                        }
                    }

                    if (current_vine && current_vine->visible) {
                        // Find center vine to the right (invisible, right of current)
                        Vine* center_vine = NULL;
                        float vine_bottom_tolerance = 20.0f;  // Allow reaching visual bottom
                        for (int i = 0; i < g_current_level->vine_count; i++) {
                            Vine* v = &g_current_level->vines[i];
                            if (!v->visible &&
                                v->x > current_vine->x &&
                                fabs(v->x - (current_vine->x + (VINE_SPACING / 2.0f))) < VINE_TRANSFER_TOLERANCE &&
                                player->y >= v->y_top &&
                                player->y + PLAYER_HEIGHT <= v->y_bottom + vine_bottom_tolerance) {
                                center_vine = v;
                                break;
                            }
                        }

                        if (center_vine) {
                            player->attached_vine_id = center_vine->id;
                            player->lateral_position = 0;
                        } else {
                            // No center vine at this height → FALL
                            player->climbing = false;
                            player->attached_vine_id = -1;
                            player->state = STATE_FALLING;
                        }
                    }
                }
            }
            else if (player->lateral_position == 0) {
                // ON_CENTER + press RIGHT → Move to right visible vine
                // Note: We DON'T check collision here because we're moving to a different vine
                // The vine position itself determines if we can move there
                
                if (g_current_level) {
                    Vine* current_vine = NULL;
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        if (g_current_level->vines[i].id == player->attached_vine_id) {
                            current_vine = &g_current_level->vines[i];
                            break;
                        }
                    }

                    if (current_vine && !current_vine->visible) {
                        // Find right visible vine
                        Vine* right_vine = NULL;
                        float vine_bottom_tolerance = 20.0f;  // Allow reaching visual bottom
                        for (int i = 0; i < g_current_level->vine_count; i++) {
                            Vine* v = &g_current_level->vines[i];
                            if (v->visible &&
                                v->x > current_vine->x &&
                                fabs(v->x - (current_vine->x + (VINE_SPACING / 2.0f))) < 5.0f &&
                                player->y >= v->y_top &&
                                player->y + PLAYER_HEIGHT <= v->y_bottom + vine_bottom_tolerance) {
                                right_vine = v;
                                break;
                            }
                        }

                        if (right_vine) {
                            // Moving RIGHT from center → land on LEFT SIDE of right vine
                            player->attached_vine_id = right_vine->id;
                            player->lateral_position = -1;
                            player->direction = DIR_LEFT;
                        } else {
                            // No right vine at this height → FALL
                            player->climbing = false;
                            player->attached_vine_id = -1;
                            player->state = STATE_FALLING;
                        }
                    }
                }
            }
            else if (player->lateral_position == -1) {
                // ON_LEFT_SIDE + press RIGHT → Switch to RIGHT SIDE (same vine)
                // Check if switching sides would cause collision
                if (would_collide_at_position(player, 1)) {
                    // Cannot switch to right side - would collide
                    return;
                }
                player->lateral_position = 1;
                player->direction = DIR_RIGHT;
            }
        }

        // Release vine with SPACE (jump off)
        if (IsKeyPressed(KEY_SPACE)) {
            player->climbing = false;
            player->attached_vine_id = -1;
            player->lateral_position = 0;
            player->state = STATE_JUMPING;
        }
    } else {
        // Normal mode
        if (IsKeyDown(KEY_LEFT)) {
            player->velocity_x = -MOVE_SPEED;
            player->direction = DIR_LEFT;
            if (player->on_ground) {
                player->state = STATE_RUNNING;
            }
        } else if (IsKeyDown(KEY_RIGHT)) {
            player->velocity_x = MOVE_SPEED;
            player->direction = DIR_RIGHT;
            if (player->on_ground) {
                player->state = STATE_RUNNING;
            }
        } else {
            player->velocity_x = 0;
            if (player->on_ground) {
                player->state = STATE_IDLE;
            }
        }
        
        // Jump or grab vine with SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            if (player->on_ground) {
                player->velocity_y = -JUMP_SPEED;
                player->on_ground = false;
                player->state = STATE_JUMPING;
            } else {
                // Try to grab nearby vine
                if (g_current_level) {
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        Vine* vine = &g_current_level->vines[i];
                        if (!vine->visible) continue;

                        // Use collision box for vine grabbing
                        float collision_center_x = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH / 2;
                        float distance = fabs(collision_center_x - vine->x);
                        float collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;

                        // Check if player is close to vine and within vine's Y range
                        if (distance < GRAB_RANGE &&
                            player->y + COLLISION_OFFSET_Y >= vine->y_top - VINE_Y_TOLERANCE &&
                            collision_bottom <= vine->y_bottom + VINE_Y_TOLERANCE) {

                            player->climbing = true;
                            player->attached_vine_id = vine->id;
                            player->velocity_y = 0;
                            player->state = STATE_CLIMBING;

                            // RULESET: Snap to LEFT or RIGHT side based on approach
                            if (collision_center_x > vine->x) {
                                // Approaching from right → snap to RIGHT SIDE
                                player->lateral_position = 1;  // RIGHT
                                player->direction = DIR_RIGHT;  // Face right (right-side climb)
                            } else {
                                // Approaching from left → snap to LEFT SIDE
                                player->lateral_position = -1;  // LEFT
                                player->direction = DIR_LEFT;  // Face left (left-side climb)
                            }

                            break;
                        }
                    }
                }
            }
        }
    }
}

// ============================================================
// PHYSICS UPDATE
// ============================================================

void player_update(Player* player, float deltaTime) {
    // Handle death state
    if (player->state == STATE_DYING) {
        player->death_timer += deltaTime;
        
        // Update death animation
        player->animation_time += deltaTime;
        if (player->animation_time >= ANIMATION_SPEED) {
            player->animation_time = 0;
            player->current_frame++;
        }
        
        // After 1 second, respawn
        if (player->death_timer >= DEATH_ANIMATION_DURATION) {
            player->x = PLAYER_SPAWN_X_BLOCK * PLATFORM_BLOCK_SIZE;
            player->y = PLAYER_SPAWN_Y_BLOCK * PLATFORM_BLOCK_SIZE;
            player->velocity_x = 0;
            player->velocity_y = 0;
            player->climbing = false;
            player->attached_vine_id = -1;
            player->lateral_position = 0;
            player->on_ground = false;
            player->state = STATE_IDLE;
            player->death_timer = 0;
            player->current_frame = 0;
            player->animation_time = 0;
        }
        
        return;  // Skip all other updates during death
    }
    
    // Update animation only if player should be animating
    if (should_animate(player)) {
        player->animation_time += deltaTime;
        if (player->animation_time >= ANIMATION_SPEED) {
            player->animation_time = 0;
            player->current_frame++;
        }
    } else {
        // Reset to frame 0 when not animating
        player->current_frame = 0;
        player->animation_time = 0;
    }
    
    if (!player->climbing) {
        // Apply gravity
        player->velocity_y += GRAVITY * deltaTime;
        
        // Clamp fall speed
        if (player->velocity_y > MAX_FALL_SPEED) {
            player->velocity_y = MAX_FALL_SPEED;
        }
    } else {
        // Climbing: adjust position based on lateral position
        if (g_current_level && player->attached_vine_id >= 0) {
            Vine* attached_vine = NULL;

            for (int i = 0; i < g_current_level->vine_count; i++) {
                if (g_current_level->vines[i].id == player->attached_vine_id) {
                    attached_vine = &g_current_level->vines[i];
                    break;
                }
            }

            if (attached_vine) {
                // Position player based on lateral position
                if (player->lateral_position == 0) {
                    // CENTER position - no hand adjustment, just center on vine
                    player->x = attached_vine->x - PLAYER_WIDTH / 2;
                } else {
                    // LEFT (-1) or RIGHT (1) side - adjust for hand position AND lateral offset
                    float hand_adjustment = (player->direction == DIR_LEFT) ?
                                          HAND_OFFSET_FROM_CENTER : -HAND_OFFSET_FROM_CENTER;
                    float lateral_offset = player->lateral_position * VINE_LATERAL_OFFSET;
                    player->x = attached_vine->x - PLAYER_WIDTH / 2 + hand_adjustment + lateral_offset;
                }
            }
        }
    }
    
    // Update position
    if (!player->climbing) {
        // Normal movement - apply velocity
        player->x += player->velocity_x * deltaTime;
    }
    player->y += player->velocity_y * deltaTime;

    // Check collision with ALL platforms and grass (columns) FIRST
    // Platforms work both when walking AND when climbing (act as barriers on vines)
    // This MUST happen before vine boundary checks so platforms take priority
    if (g_current_level) {
        bool collided = false;

        // Check platform collisions
        for (int p = 0; p < g_current_level->platform_count; p++) {
            Platform* platform = &g_current_level->platforms[p];

            // Calculate platform width in pixels from blocks
            float platform_width_px = platform->width_blocks * PLATFORM_BLOCK_SIZE;
            float platform_bottom = platform->y + PLATFORM_BLOCK_SIZE;  // Platforms are 24px tall

            // Collision box depends on lateral position when climbing
            float collision_left, collision_right, collision_top, collision_bottom;

            if (player->climbing) {
                // On vines: collision only on the side we're on
                float player_center_x = player->x + PLAYER_WIDTH / 2;

                if (player->lateral_position == -1) {
                    // LEFT side: only left half of sprite has collision
                    collision_left = player->x + COLLISION_OFFSET_X;
                    collision_right = player_center_x;  // Only up to center
                } else if (player->lateral_position == 1) {
                    // RIGHT side: only right half of sprite has collision
                    collision_left = player_center_x;  // Start from center
                    collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
                } else {
                    // CENTER: full collision box
                    collision_left = player->x + COLLISION_OFFSET_X;
                    collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
                }

                collision_top = player->y + COLLISION_OFFSET_Y;
                collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;
            } else {
                // Not climbing: use full collision box
                collision_left = player->x + COLLISION_OFFSET_X;
                collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
                collision_top = player->y + COLLISION_OFFSET_Y;
                collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;
            }

            // Check if player is horizontally aligned with platform (STRICT: use epsilon)
            bool horizontally_aligned = (collision_right > platform->x + collision_position_fix &&
                                        collision_left < platform->x + platform_width_px - collision_position_fix);

            // Check if player is vertically aligned with platform (STRICT: use epsilon)
            bool vertically_aligned = (collision_bottom > platform->y + collision_position_fix &&
                                      collision_top < platform_bottom - collision_position_fix);

            if (horizontally_aligned) {
                // TOP collision - player lands ON top of platform
                // STRICT: collision must be within tight tolerance (0.1-1.5px)
                if (collision_bottom >= platform->y - collision_position_fix &&
                    collision_bottom <= platform->y + PLATFORM_COLLISION_TOLERANCE &&
                    player->velocity_y >= 0) {

                    // Adjust player position to account for collision offset
                    player->y = platform->y - COLLISION_OFFSET_Y - COLLISION_HEIGHT;
                    player->velocity_y = 0;

                    // Only detach from vine and set on_ground if NOT climbing
                    if (!player->climbing) {
                        player->on_ground = true;
                        collided = true;
                        if (player->velocity_x == 0) {
                            player->state = STATE_IDLE;
                        }
                    }
                    // If climbing, just stop movement but stay on vine
                    continue;  // Check other platforms too (don't break)
                }

                // BOTTOM collision - player hits bottom of platform (blocks upward movement)
                // Prevents climbing through platforms from below 
                if (collision_top <= platform_bottom + collision_position_fix &&
                    collision_top >= platform_bottom - PLATFORM_COLLISION_TOLERANCE &&
                    player->velocity_y < 0) {

                    // Adjust player position to account for collision offset
                    player->y = platform_bottom - COLLISION_OFFSET_Y;
                    player->velocity_y = 0;
                    // Stay on vine, just blocked by platform
                    continue;  // Check other platforms too (don't break)
                }
            }

            if (vertically_aligned && !player->climbing) {
                // LEFT side collision - player hits left edge of platform (only when NOT climbing)
                if (collision_right >= platform->x - collision_position_fix &&
                    collision_right <= platform->x + PLATFORM_COLLISION_TOLERANCE &&
                    player->velocity_x > 0) {

                    // Push player back to left of platform to ensure full separation
                    player->x = platform->x - COLLISION_OFFSET_X - COLLISION_WIDTH - collision_position_fix;
                    player->velocity_x = 0;
                    // Continue checking platforms - we might still be standing on another platform
                    continue;
                }

                // RIGHT side collision - player hits right edge of platform (only when NOT climbing)
                if (collision_left <= platform->x + platform_width_px + collision_position_fix &&
                    collision_left >= platform->x + platform_width_px - PLATFORM_COLLISION_TOLERANCE &&
                    player->velocity_x < 0) {

                    // Push player back to right of platform to ensure full separation
                    player->x = platform->x + platform_width_px - COLLISION_OFFSET_X + collision_position_fix;
                    player->velocity_x = 0;
                    // Continue checking platforms - we might still be standing on another platform
                    continue;
                }
            }
        }

        // Check grass (column) collisions - same as platforms
        if (!collided) {
            for (int c = 0; c < g_current_level->column_count; c++) {
                Column* column = &g_current_level->columns[c];

                // Grass collision - player stands ON top of grass (same as platform)
                // Grass is 24px tall, positioned at column->y
                float grass_top = column->y;

                // Use tighter collision box for grass detection
                float collision_left = player->x + COLLISION_OFFSET_X;
                float collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
                float collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;

                if (collision_bottom >= grass_top - PLATFORM_COLLISION_TOLERANCE &&
                    collision_bottom <= grass_top + PLATFORM_COLLISION_TOLERANCE &&
                    collision_right > column->x &&
                    collision_left < column->x + column->grass_width &&
                    player->velocity_y >= 0) {

                    // Adjust player position to account for collision offset
                    player->y = grass_top - COLLISION_OFFSET_Y - COLLISION_HEIGHT;
                    player->velocity_y = 0;
                    player->on_ground = true;
                    collided = true;

                    // If climbing and hit grass, detach from vine
                    if (player->climbing) {
                        player->climbing = false;
                        player->attached_vine_id = -1;
                        player->lateral_position = 0;
                    }

                    if (player->velocity_x == 0) {
                        player->state = STATE_IDLE;
                    }
                    break;  // Stop checking once we hit grass
                }
            }
        }

        if (!collided && !player->climbing) {
            player->on_ground = false;
            if (player->velocity_y > 0) {
                player->state = STATE_FALLING;
            }
        }
    }

    // Vine boundary checks - MUST happen AFTER platform collision checks
    // Platforms take priority over vine boundaries
    if (player->climbing && g_current_level && player->attached_vine_id >= 0) {
        Vine* attached_vine = NULL;
        for (int i = 0; i < g_current_level->vine_count; i++) {
            if (g_current_level->vines[i].id == player->attached_vine_id) {
                attached_vine = &g_current_level->vines[i];
                break;
            }
        }

        if (attached_vine) {
            // Can't go above top - clamp position and stop velocity
            if (player->y < attached_vine->y_top) {
                player->y = attached_vine->y_top;
                player->velocity_y = 0;
            }
            // DROP OFF BOTTOM: If player moves past bottom, they FALL (no clamp)
            // Use a tolerance so player can reach the visual bottom before dropping
            // Player is 48px tall, vines use 24px multiples, so allow ~20px past bottom
            float bottom_tolerance = 20.0f;
            if (player->y + PLAYER_HEIGHT > attached_vine->y_bottom + bottom_tolerance) {
                // Player dropped off the bottom → FALL
                player->climbing = false;
                player->attached_vine_id = -1;
                player->lateral_position = 0;
                player->state = STATE_FALLING;
            }
        }
    }

    // Water collision is now handled in player_screen.c (loses a life)

    // Keep player on screen (use collision box edges)
    float collision_left = player->x + COLLISION_OFFSET_X;
    float collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
    if (collision_left < 0) player->x = -COLLISION_OFFSET_X;
    if (collision_right > UI_WINDOW_WIDTH) player->x = UI_WINDOW_WIDTH - COLLISION_OFFSET_X - COLLISION_WIDTH;
}

// ============================================================
// RENDERING
// ============================================================

void player_render(Player* player) {
    // Get sprite type and max frames using helper function
    int max_frames = 1;
    SpriteType sprite_type = get_player_sprite(player, &max_frames);

    // Calculate current frame (wrap around max_frames)
    int frame = player->current_frame % max_frames;

    // Get sprite from manager
    SpriteSheet* sprite = sprite_manager_get(sprite_type);

    if (sprite && sprite->loaded) {
        // Source: native sprite dimensions from texture
        Rectangle src = {
            frame * (sprite->frame_width + sprite->spacing),
            0,
            sprite->frame_width,   // Should be 32 for Junior
            sprite->frame_height   // Should be 16 for Junior
        };

        // Destination: scaled to match collision box
        // PLAYER_WIDTH/HEIGHT = sprite dimensions * PLAYER_SCALE (3x)
        Rectangle dest = {
            player->x,
            player->y,
            PLAYER_WIDTH,   // 32 * 3 = 96
            PLAYER_HEIGHT   // 16 * 3 = 48
        };

        Vector2 origin = {0, 0};
        DrawTexturePro(sprite->texture, src, dest, origin, 0.0f, WHITE);
    } else {
        // Fallback: draw colored rectangle
        Color player_color = player->climbing ? SKYBLUE : RED;
        DrawRectangle(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, player_color);
    }

#if DEBUG_MODE
    // Debug: draw sprite bounding box (full size)
    DrawRectangleLines(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, BLUE);

    // Debug: draw actual collision box (depends on lateral position)
    float collision_left, collision_right, collision_top, collision_bottom;

    if (player->climbing) {
        // On vines: collision only on the side we're on
        float player_center_x = player->x + PLAYER_WIDTH / 2;

        if (player->lateral_position == -1) {
            // LEFT side: only left half
            collision_left = player->x + COLLISION_OFFSET_X;
            collision_right = player_center_x;
        } else if (player->lateral_position == 1) {
            // RIGHT side: only right half
            collision_left = player_center_x;
            collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
        } else {
            // CENTER: full collision box
            collision_left = player->x + COLLISION_OFFSET_X;
            collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
        }

        collision_top = player->y + COLLISION_OFFSET_Y;
        collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;
    } else {
        // Not climbing: use full collision box
        collision_left = player->x + COLLISION_OFFSET_X;
        collision_right = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH;
        collision_top = player->y + COLLISION_OFFSET_Y;
        collision_bottom = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT;
    }

    DrawRectangleLines(
        collision_left,
        collision_top,
        collision_right - collision_left,
        collision_bottom - collision_top,
        GREEN
    );

#endif
}


