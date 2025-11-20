#include "player.h"
#include "level.h"
#include "../rendering/sprite_manager.h"
#include <stdio.h>
#include <math.h>

// External reference to level (for collision testing)
extern Level* g_current_level;

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
    player->second_vine_id = -1;
    player->dual_vine_climbing = false;
    player->lateral_position = 0;
    player->state = STATE_IDLE;
    player->direction = DIR_RIGHT;
    player->animation_time = 0;
    player->current_frame = 0;
    
    printf("✓ Player initialized at (%.0f, %.0f)\n", x, y);
}

// ============================================================
// INPUT HANDLING
// ============================================================

void player_handle_input(Player* player) {
    if (player->climbing) {
        // Update dual climbing status
        player->dual_vine_climbing = (player->second_vine_id >= 0);
        
        // Choose climb speed based on vine count
        float climb_speed = player->dual_vine_climbing ? CLIMB_SPEED_DUAL : CLIMB_SPEED;
        
        // Climbing mode - vertical movement
        if (IsKeyDown(KEY_UP)) {
            player->velocity_y = -climb_speed;
            player->state = STATE_CLIMBING;
        } else if (IsKeyDown(KEY_DOWN)) {
            player->velocity_y = climb_speed;
            player->state = STATE_CLIMBING;
        } else {
            player->velocity_y = 0;
        }
        
        // Lateral movement on vine
        if (IsKeyPressed(KEY_LEFT)) {
            player->direction = DIR_LEFT;
            
            // If in dual mode, switch to left vine only
            if (player->dual_vine_climbing) {
                player->attached_vine_id = player->second_vine_id < player->attached_vine_id ? 
                                          player->second_vine_id : player->attached_vine_id;
                player->second_vine_id = -1;
                player->dual_vine_climbing = false;
                player->lateral_position = 0;
                printf("Switched to left vine only\n");
            } else if (player->lateral_position == 0) {
                // Move to left side of current vine
                player->lateral_position = -1;
                printf("Moved to left side of vine\n");
            } else if (player->lateral_position == -1) {
                // Try to grab left vine
                if (g_current_level) {
                    Vine* current_vine = NULL;
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        if (g_current_level->vines[i].id == player->attached_vine_id) {
                            current_vine = &g_current_level->vines[i];
                            break;
                        }
                    }
                    
                    if (current_vine) {
                        // Look for vine to the left
                        Vine* left_vine = NULL;
                        for (int i = 0; i < g_current_level->vine_count; i++) {
                            Vine* v = &g_current_level->vines[i];
                            if (v->visible && v->id != player->attached_vine_id &&
                                v->x < current_vine->x &&
                                fabs(v->x - current_vine->x) < 150 &&
                                player->y >= v->y_top - 50 && player->y + PLAYER_HEIGHT <= v->y_bottom + 50) {
                                left_vine = v;
                                break;
                            }
                        }
                        
                        if (left_vine) {
                            // Verify player is actually within vine bounds
                            if (player->y >= left_vine->y_top - 50 && 
                                player->y + PLAYER_HEIGHT <= left_vine->y_bottom + 50) {
                                // Grab second vine
                                player->second_vine_id = left_vine->id;
                                player->dual_vine_climbing = true;
                                player->lateral_position = 0;
                                printf("Grabbed left vine %d! Now holding two vines\n", left_vine->id);
                            } else {
                                // Not in range - fall
                                player->climbing = false;
                                player->attached_vine_id = -1;
                                player->second_vine_id = -1;
                                player->lateral_position = 0;
                                player->state = STATE_FALLING;
                                printf("Out of vine range - falling!\n");
                            }
                        } else {
                            // Fall from vine
                            player->climbing = false;
                            player->attached_vine_id = -1;
                            player->second_vine_id = -1;
                            player->lateral_position = 0;
                            player->state = STATE_FALLING;
                            printf("No vine found - falling!\n");
                        }
                    }
                }
            } else if (player->lateral_position == 1) {
                // Return to center from right side
                player->lateral_position = 0;
                printf("Returned to center\n");
            }
        } else if (IsKeyPressed(KEY_RIGHT)) {
            player->direction = DIR_RIGHT;
            
            // If in dual mode, switch to right vine only
            if (player->dual_vine_climbing) {
                player->attached_vine_id = player->second_vine_id > player->attached_vine_id ? 
                                          player->second_vine_id : player->attached_vine_id;
                player->second_vine_id = -1;
                player->dual_vine_climbing = false;
                player->lateral_position = 0;
                printf("Switched to right vine only\n");
            } else if (player->lateral_position == 0) {
                // Move to right side of current vine
                player->lateral_position = 1;
                printf("Moved to right side of vine\n");
            } else if (player->lateral_position == 1) {
                // Try to grab right vine
                if (g_current_level) {
                    Vine* current_vine = NULL;
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        if (g_current_level->vines[i].id == player->attached_vine_id) {
                            current_vine = &g_current_level->vines[i];
                            break;
                        }
                    }
                    
                    if (current_vine) {
                        // Look for vine to the right
                        Vine* right_vine = NULL;
                        for (int i = 0; i < g_current_level->vine_count; i++) {
                            Vine* v = &g_current_level->vines[i];
                            if (v->visible && v->id != player->attached_vine_id &&
                                v->x > current_vine->x &&
                                fabs(v->x - current_vine->x) < 150 &&
                                player->y >= v->y_top - 50 && player->y + PLAYER_HEIGHT <= v->y_bottom + 50) {
                                right_vine = v;
                                break;
                            }
                        }
                        
                        if (right_vine) {
                            // Verify player is actually within vine bounds
                            if (player->y >= right_vine->y_top - 50 && 
                                player->y + PLAYER_HEIGHT <= right_vine->y_bottom + 50) {
                                // Grab second vine
                                player->second_vine_id = right_vine->id;
                                player->dual_vine_climbing = true;
                                player->lateral_position = 0;
                                printf("Grabbed right vine %d! Now holding two vines\n", right_vine->id);
                            } else {
                                // Not in range - fall
                                player->climbing = false;
                                player->attached_vine_id = -1;
                                player->second_vine_id = -1;
                                player->lateral_position = 0;
                                player->state = STATE_FALLING;
                                printf("Out of vine range - falling!\n");
                            }
                        } else {
                            // Fall from vine
                            player->climbing = false;
                            player->attached_vine_id = -1;
                            player->second_vine_id = -1;
                            player->lateral_position = 0;
                            player->state = STATE_FALLING;
                            printf("No vine found - falling!\n");
                        }
                    }
                }
            } else if (player->lateral_position == -1) {
                // Return to center from left side
                player->lateral_position = 0;
                printf("Returned to center\n");
            }
        }
        
        // Release vine(s) with SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            player->climbing = false;
            player->attached_vine_id = -1;
            player->second_vine_id = -1;
            player->dual_vine_climbing = false;
            player->lateral_position = 0;
            player->state = STATE_JUMPING;
            printf("Released vine(s)!\n");
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
                printf("Player jumped!\n");
            } else {
                // Try to grab nearby vine
                if (g_current_level) {
                    for (int i = 0; i < g_current_level->vine_count; i++) {
                        Vine* vine = &g_current_level->vines[i];
                        if (!vine->visible) continue;
                        
                        float player_center_x = player->x + PLAYER_WIDTH / 2;
                        float distance = fabs(player_center_x - vine->x);
                        
                        // Check if player is close to vine and within vine's Y range
                        if (distance < GRAB_RANGE &&
                            player->y >= vine->y_top - 50 &&
                            player->y + PLAYER_HEIGHT <= vine->y_bottom + 50) {
                            
                            player->climbing = true;
                            player->attached_vine_id = vine->id;
                            player->velocity_y = 0;
                            player->x = vine->x - PLAYER_WIDTH / 2;  // Snap to vine
                            player->state = STATE_CLIMBING;
                            printf("Grabbed vine %d!\n", vine->id);
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
    // Update animation timer
    player->animation_time += deltaTime;
    if (player->animation_time >= ANIMATION_SPEED) {
        player->animation_time = 0;
        player->current_frame++;
        // Frame count depends on sprite (will handle in render)
    }
    
    if (!player->climbing) {
        // Apply gravity
        player->velocity_y += GRAVITY * deltaTime;
        
        // Clamp fall speed
        if (player->velocity_y > MAX_FALL_SPEED) {
            player->velocity_y = MAX_FALL_SPEED;
        }
    } else {
        // Climbing: adjust position based on lateral position and vines
        if (g_current_level && player->attached_vine_id >= 0) {
            Vine* attached_vine = NULL;
            Vine* second_vine = NULL;
            
            for (int i = 0; i < g_current_level->vine_count; i++) {
                if (g_current_level->vines[i].id == player->attached_vine_id) {
                    attached_vine = &g_current_level->vines[i];
                }
                if (player->second_vine_id >= 0 && g_current_level->vines[i].id == player->second_vine_id) {
                    second_vine = &g_current_level->vines[i];
                }
            }
            
            if (attached_vine) {
                // Position player based on lateral position
                if (player->dual_vine_climbing && second_vine) {
                    // Between two vines
                    float center_x = (attached_vine->x + second_vine->x) / 2.0f;
                    player->x = center_x - PLAYER_WIDTH / 2;
                } else {
                    // On one vine with lateral offset
                    float offset = player->lateral_position * 20.0f; // 20 pixels offset
                    player->x = attached_vine->x - PLAYER_WIDTH / 2 + offset;
                }
                
                // Can't go above top
                if (player->y < attached_vine->y_top) {
                    player->y = attached_vine->y_top;
                    player->velocity_y = 0;
                }
                // Can't go below bottom
                if (player->y + PLAYER_HEIGHT > attached_vine->y_bottom) {
                    player->y = attached_vine->y_bottom - PLAYER_HEIGHT;
                    player->velocity_y = 0;
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
    
    // Simple ground collision (with first platform)
    if (!player->climbing && g_current_level && g_current_level->platform_count > 0) {
        Platform* platform = &g_current_level->platforms[0];
        
        // Check if player is on platform (adjusted so feet are ON visual surface)
        // Platform visual is ~40px tall, so detect at platform->y + 40
        if (player->y + PLAYER_HEIGHT >= platform->y + 35 &&
            player->y + PLAYER_HEIGHT <= platform->y + 55 &&
            player->x + PLAYER_WIDTH > platform->x &&
            player->x < platform->x + platform->width &&
            player->velocity_y >= 0) {
            
            player->y = platform->y + 40 - PLAYER_HEIGHT;  // Stand on visual surface
            player->velocity_y = 0;
            player->on_ground = true;
            
            if (player->velocity_x == 0) {
                player->state = STATE_IDLE;
            }
        } else {
            player->on_ground = false;
            if (!player->climbing && player->velocity_y > 0) {
                player->state = STATE_FALLING;
            }
        }
    }
    
    // Check water collision
    if (player->y > WATER_LEVEL) {
        printf("Player fell in water! Respawning...\n");
        player->x = 200;
        player->y = 400;
        player->velocity_x = 0;
        player->velocity_y = 0;
        player->climbing = false;
        player->attached_vine_id = -1;
        player->second_vine_id = -1;
        player->dual_vine_climbing = false;
        player->lateral_position = 0;
        player->on_ground = false;
    }
    
    // Keep player on screen
    if (player->x < 0) player->x = 0;
    if (player->x + PLAYER_WIDTH > 1200) player->x = 1200 - PLAYER_WIDTH;
}

// ============================================================
// RENDERING
// ============================================================

void player_render(Player* player) {
    // Get appropriate sprite type based on state
    SpriteType sprite_type = SPRITE_JUNIOR_IDLE;
    int frame = 0;
    int max_frames = 1;
    
    if (player->climbing) {
        // Use different sprites based on vine count
        if (player->dual_vine_climbing) {
            // Two vines: use center climbing animation
            sprite_type = SPRITE_JUNIOR_CLIMB_CENTER;
        } else {
            // One vine: use directional climbing animation
            sprite_type = (player->direction == DIR_LEFT) ? 
                          SPRITE_JUNIOR_CLIMB_LEFT : SPRITE_JUNIOR_CLIMB_RIGHT;
        }
        max_frames = 2;
        frame = player->current_frame % max_frames;
    } else if (player->state == STATE_RUNNING) {
        sprite_type = (player->direction == DIR_LEFT) ? 
                      SPRITE_JUNIOR_RUN_LEFT : SPRITE_JUNIOR_RUN_RIGHT;
        max_frames = 3;
        frame = player->current_frame % max_frames;
    } else if (player->state == STATE_JUMPING) {
        sprite_type = (player->direction == DIR_LEFT) ? 
                      SPRITE_JUNIOR_JUMP_LEFT : SPRITE_JUNIOR_JUMP_RIGHT;
        frame = 0;
    } else if (player->state == STATE_FALLING) {
        sprite_type = (player->direction == DIR_LEFT) ? 
                      SPRITE_JUNIOR_JUMP_LEFT : SPRITE_JUNIOR_JUMP_RIGHT;
        frame = 0;
    } else {
        sprite_type = SPRITE_JUNIOR_IDLE;
        frame = 0;
    }
    
    // Get sprite from manager
    SpriteSheet* sprite = sprite_manager_get(sprite_type);
    
    if (sprite && sprite->loaded) {
        // Calculate source rectangle for current frame
        Rectangle src = {
            frame * (sprite->frame_width + sprite->spacing),
            0,
            sprite->frame_width,
            sprite->frame_height
        };
        
        Rectangle dest = {
            player->x,
            player->y,
            PLAYER_WIDTH,
            PLAYER_HEIGHT
        };
        
        Vector2 origin = {0, 0};
        DrawTexturePro(sprite->texture, src, dest, origin, 0.0f, WHITE);
    } else {
        // Fallback: draw colored rectangle
        Color player_color = player->climbing ? SKYBLUE : RED;
        DrawRectangle(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, player_color);
    }
    
    // Debug: draw hitbox
    DrawRectangleLines(player->x, player->y, PLAYER_WIDTH, PLAYER_HEIGHT, GREEN);
    
    // Debug: draw state info
    const char* state_name = "UNKNOWN";
    switch (player->state) {
        case STATE_IDLE: state_name = "IDLE"; break;
        case STATE_RUNNING: state_name = "RUNNING"; break;
        case STATE_JUMPING: state_name = "JUMPING"; break;
        case STATE_FALLING: state_name = "FALLING"; break;
        case STATE_CLIMBING: state_name = "CLIMBING"; break;
    }
    
    const char* lateral = player->lateral_position == -1 ? "L" : 
                         (player->lateral_position == 1 ? "R" : "C");
    const char* climb_mode = player->dual_vine_climbing ? "DUAL" : "SINGLE";
    
    DrawText(TextFormat("%s | Pos:(%.0f,%.0f) | Ground:%s", 
             state_name, player->x, player->y, player->on_ground ? "Y" : "N"), 10, 40, 18, WHITE);
    
    if (player->climbing) {
        DrawText(TextFormat("Climb:%s V1:%d V2:%d Side:%s", 
                 climb_mode, player->attached_vine_id, player->second_vine_id, lateral), 10, 60, 18, YELLOW);
    }
}


