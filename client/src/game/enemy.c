#include "enemy.h"
#include "level.h"
#include "../rendering/sprite_manager.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

// External reference to level (for collision testing)
extern Level* g_current_level;

// ============================================================
// CONSTANTS
// ============================================================

#define RED_CROCODILE_CLIMB_SPEED 100.0f     // Speed climbing vines
#define RED_CROCODILE_WALK_SPEED 80.0f       // Speed walking on platforms
#define BLUE_CROCODILE_FALL_SPEED 200.0f     // Falling speed
#define ENEMY_WIDTH 102.0f                    // 34px * 3 scale
#define ENEMY_HEIGHT 48.0f                    // 16px * 3 scale
#define PLATFORM_DETECT_RANGE 50.0f          // How far to look for platforms
#define VINE_DETECT_RANGE 50.0f              // How far to look for vines

// ============================================================
// RED CROCODILE INITIALIZATION
// ============================================================

void enemy_init_red_crocodile(Enemy* enemy, void* level_ptr, int vine_index, float start_y) {
    Level* level = (Level*)level_ptr;
    if (!level || vine_index < 0 || vine_index >= level->vine_count) {
        printf("ERROR: Invalid vine index %d for red crocodile (total vines: %d)\n", 
               vine_index, level ? level->vine_count : 0);
        return;
    }
    
    Vine* vine = &level->vines[vine_index];
    
    enemy->type = ENEMY_RED_CROCODILE;
    enemy->state = ENEMY_STATE_CLIMBING_UP;
    enemy->x = vine->x;
    enemy->y = start_y;
    enemy->velocity_x = 0;
    enemy->velocity_y = -RED_CROCODILE_CLIMB_SPEED;
    enemy->attached_vine_id = vine_index;
    enemy->original_vine_id = vine_index;  // Remember starting vine
    enemy->target_platform_id = -1;
    enemy->moving_up = true;
    enemy->ignore_platforms = (GetRandomValue(0, 100) < 50);  // 50% chance to ignore platforms initially
    enemy->walked_distance = 0;
    enemy->platform_timer = 0;
    enemy->patrol_start_y = vine->y_top;      // Top of vine (smaller Y value)
    enemy->patrol_end_y = vine->y_bottom;     // Bottom of vine (larger Y value)
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    enemy->speed_multiplier = 1.0f;  // Default speed, will be updated by level
    
    printf("✓ Red crocodile spawned on vine index %d (ID=%d) at (%.0f, %.0f) patrol_range=(%.0f to %.0f) ACTIVE=%d\n", 
           vine_index, vine->id, enemy->x, enemy->y, enemy->patrol_start_y, enemy->patrol_end_y, enemy->active);
}

// ============================================================
// BLUE CROCODILE INITIALIZATION
// ============================================================

void enemy_init_blue_crocodile(Enemy* enemy, void* level_ptr, float start_x, float start_y) {
    (void)start_y;  // Unused - blue croc spawns on top platform regardless
    Level* level = (Level*)level_ptr;
    
    enemy->type = ENEMY_BLUE_CROCODILE;
    
    // Find the TWO highest platforms
    float platform_y_values[2] = {LEVEL_HEIGHT, LEVEL_HEIGHT};
    
    // Sort all platforms by Y position to find the 2 highest
    for (int i = 0; i < level->platform_count; i++) {
        Platform* plat = &level->platforms[i];
        if (plat->y < platform_y_values[0]) {
            platform_y_values[1] = platform_y_values[0];
            platform_y_values[0] = plat->y;
        } else if (plat->y < platform_y_values[1]) {
            platform_y_values[1] = plat->y;
        }
    }
    
    printf("Searching vines from top 2 platforms: y=%.0f and y=%.0f\n", 
           platform_y_values[0], platform_y_values[1]);
    
    // Find all visible vines that hang from EITHER of the top 2 platforms
    int valid_vines[50];
    int valid_count = 0;
    
    for (int i = 0; i < level->vine_count && valid_count < 50; i++) {
        Vine* vine = &level->vines[i];
        
        if (!vine->visible) continue;
        
        // Check if vine hangs from either top platform or second platform
        for (int p = 0; p < 2; p++) {
            float platform_y = platform_y_values[p];
            
            // Vine should start near the platform level (within 50px below)
            if (vine->y_top >= platform_y && 
                vine->y_top <= platform_y + 50.0f) {
                
                valid_vines[valid_count++] = i;
                printf("  Found valid vine %d at x=%.0f, y_top=%.0f (platform %d at y=%.0f)\n", 
                       i, vine->x, vine->y_top, p+1, platform_y);
                break;  // Don't count same vine twice
            }
        }
    }
    
    // Manually add vine 16 if visible and not already included
    if (level->vine_count > 16 && level->vines[16].visible) {
        bool already_included = false;
        for (int i = 0; i < valid_count; i++) {
            if (valid_vines[i] == 16) {
                already_included = true;
                break;
            }
        }
        if (!already_included && valid_count < 50) {
            valid_vines[valid_count++] = 16;
            printf("  Added vine 16 manually at x=%.0f, y_top=%.0f (edge vine)\n",
                   level->vines[16].x, level->vines[16].y_top);
        }
    }
    
    if (valid_count == 0) {
        printf("ERROR: No valid vines found on top 2 platforms!\n");
        enemy->active = false;
        return;
    }
    
    // Choose random vine from valid ones
    int chosen_index = GetRandomValue(0, valid_count - 1);
    int vine_index = valid_vines[chosen_index];
    Vine* target_vine = &level->vines[vine_index];
    
    printf("✓ Blue croc will walk to vine %d at x=%.0f\n", vine_index, target_vine->x);
    
    // Start at Mario's position, ON the top platform surface
    enemy->x = start_x;
    enemy->y = platform_y_values[0] - 24.0f;  // Start on highest platform
    
    // Remember which vine to go to
    enemy->target_platform_id = vine_index;
    enemy->attached_vine_id = -1;
    
    // Start walking right
    enemy->state = ENEMY_STATE_WALKING_RIGHT;
    enemy->velocity_x = 80.0f;
    enemy->velocity_y = 0;
    
    enemy->original_vine_id = -1;
    enemy->moving_up = false;
    
    // Animation
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    enemy->speed_multiplier = 1.0f;
    
    enemy->ignore_platforms = false;
    enemy->walked_distance = 0;
    enemy->platform_timer = 0;
    
    enemy->patrol_start_y = platform_y_values[0];
    enemy->patrol_end_y = level->water_level;
}

// ============================================================
// BLUE CROCODILE UPDATE
// ============================================================

static void update_blue_crocodile(Enemy* enemy, float deltaTime) {
    // Update animation
    enemy->animation_time += deltaTime;
    if (enemy->animation_time >= 0.2f) {
        enemy->animation_time = 0;
        enemy->current_frame = (enemy->current_frame + 1) % 2;
    }
    
    switch (enemy->state) {
        case ENEMY_STATE_WALKING_RIGHT: {
            // Walk right on the top platform
            enemy->x += enemy->velocity_x * enemy->speed_multiplier * deltaTime;
            
            // Apply gravity to fall to lower platforms
            enemy->velocity_y = 200.0f;  // Gravity
            enemy->y += enemy->velocity_y * deltaTime;
            
            // Check collision with platforms to stop falling
            for (int i = 0; i < g_current_level->platform_count; i++) {
                Platform* plat = &g_current_level->platforms[i];
                float plat_left = plat->x;
                float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
                float plat_top = plat->y;
                float plat_bottom = plat->y + PLATFORM_BLOCK_SIZE;
                
                // Check if enemy is on this platform
                if (enemy->x >= plat_left && enemy->x <= plat_right &&
                    enemy->y >= plat_top - 30.0f && enemy->y <= plat_bottom) {
                    enemy->y = plat_top - 24.0f;  // Stand on platform
                    enemy->velocity_y = 0;
                    break;
                }
            }
            
            // Check if we've reached the target vine
            if (enemy->target_platform_id >= 0 && enemy->target_platform_id < g_current_level->vine_count) {
                Vine* target_vine = &g_current_level->vines[enemy->target_platform_id];
                
                // If we're at or past the target vine X position
                if (enemy->x >= target_vine->x - 5.0f) {
                    // Snap to vine and start descending
                    enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                    enemy->x = target_vine->x;
                    enemy->y = target_vine->y_top;
                    enemy->attached_vine_id = enemy->target_platform_id;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = 120.0f;  // Descend speed
                    return;
                }
            }
            
            // Safety: if walked too far, despawn
            if (enemy->x > LEVEL_WIDTH) {
                enemy->active = false;
                enemy->state = ENEMY_STATE_DEAD;
            }
            break;
        }
        
        case ENEMY_STATE_CLIMBING_DOWN: {
            // Descend the vine
            enemy->y += enemy->velocity_y * enemy->speed_multiplier * deltaTime;
            
            // Check if reached water - die and become inactive
            if (enemy->y >= g_current_level->water_level) {
                enemy->active = false;
                enemy->state = ENEMY_STATE_DEAD;
            }
            break;
        }
        
        default:
            break;
    }
}

// ============================================================
// RED CROCODILE AI - Path Finding
// ============================================================

static bool find_platform_at_height(float x, float y, int* out_platform_id) {
    if (!g_current_level) return false;
    
    for (int i = 0; i < g_current_level->platform_count; i++) {
        Platform* plat = &g_current_level->platforms[i];
        float plat_top = plat->y;
        float plat_left = plat->x;
        float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
        
        // Check if vine position intersects with platform
        // Must be at platform height (within small tolerance) and on the platform horizontally
        if (fabs(y - plat_top) < 12.0f &&  // Very small tolerance (half block)
            x >= plat_left && x <= plat_right) {  // Must be ON platform
            *out_platform_id = i;
            return true;
        }
    }
    return false;
}

// ============================================================
// RED CROCODILE UPDATE
// ============================================================

static void update_red_crocodile(Enemy* enemy, float deltaTime) {
    if (!g_current_level) return;
    
    switch (enemy->state) {
        case ENEMY_STATE_CLIMBING_UP: {
            // Move up the vine (apply speed multiplier)
            enemy->y += enemy->velocity_y * enemy->speed_multiplier * deltaTime;
            
            // Check for platforms on the way up (not just at extremes)
            int platform_id = -1;
            bool found_platform = find_platform_at_height(enemy->x, enemy->y, &platform_id);
            
            // Check if reached top of vine
            if (enemy->y <= enemy->patrol_start_y) {
                // Reached top - decide what to do
                if (found_platform && !enemy->ignore_platforms) {
                    // Found platform and willing to use it! Choose random direction
                    bool go_right = (GetRandomValue(0, 100) < 50);
                    enemy->y = g_current_level->platforms[platform_id].y;  // Snap to platform
                    enemy->state = go_right ? ENEMY_STATE_WALKING_RIGHT : ENEMY_STATE_WALKING_LEFT;
                    enemy->target_platform_id = platform_id;
                    enemy->velocity_y = 0;
                    enemy->velocity_x = go_right ? RED_CROCODILE_WALK_SPEED : -RED_CROCODILE_WALK_SPEED;
                    enemy->attached_vine_id = -1;  // No longer attached to vine
                    enemy->walked_distance = 0;  // Reset walked distance
                    enemy->platform_timer = 0;   // Start timer to ignore vines briefly
                    Platform* p = &g_current_level->platforms[platform_id];
                    printf("  Red croc taking platform %d (x=%.0f, width=%d blocks), walking %s from x=%.0f\n", 
                           platform_id, p->x, p->width_blocks, go_right ? "right" : "left", enemy->x);
                } else {
                    // No platform or ignoring it, reverse direction
                    enemy->y = enemy->patrol_start_y;  // Clamp to top
                    enemy->moving_up = false;
                    enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                    enemy->velocity_y = RED_CROCODILE_CLIMB_SPEED;
                    enemy->ignore_platforms = (GetRandomValue(0, 100) < 50);  // New random decision
                    printf("  Red croc reached top, reversing (ignore_platforms=%d)\n", enemy->ignore_platforms);
                }
            } else if (found_platform && !enemy->ignore_platforms) {
                // Found platform while climbing and willing to use it - choose random direction
                bool go_right = (GetRandomValue(0, 100) < 50);
                enemy->y = g_current_level->platforms[platform_id].y;  // Snap to platform
                enemy->state = go_right ? ENEMY_STATE_WALKING_RIGHT : ENEMY_STATE_WALKING_LEFT;
                enemy->target_platform_id = platform_id;
                enemy->velocity_y = 0;
                enemy->velocity_x = go_right ? RED_CROCODILE_WALK_SPEED : -RED_CROCODILE_WALK_SPEED;
                enemy->attached_vine_id = -1;  // No longer attached to vine
                enemy->walked_distance = 0;  // Reset walked distance
                enemy->platform_timer = 0;   // Start timer to ignore vines briefly
                printf("  Red croc found platform %d while climbing up, walking %s\n", 
                       platform_id, go_right ? "right" : "left");
            }
            break;
        }
        
        case ENEMY_STATE_CLIMBING_DOWN: {
            // Move down the vine (apply speed multiplier)
            enemy->y += enemy->velocity_y * enemy->speed_multiplier * deltaTime;
            
            // Check for platforms on the way down (not just at extremes)
            int platform_id = -1;
            bool found_platform = find_platform_at_height(enemy->x, enemy->y, &platform_id);
            
            // Check if reached bottom of vine
            if (enemy->y >= enemy->patrol_end_y) {
                // Reached bottom - decide what to do
                if (found_platform && !enemy->ignore_platforms) {
                    // Found platform and willing to use it! Choose random direction
                    bool go_right = (GetRandomValue(0, 100) < 50);
                    enemy->y = g_current_level->platforms[platform_id].y;  // Snap to platform
                    enemy->state = go_right ? ENEMY_STATE_WALKING_RIGHT : ENEMY_STATE_WALKING_LEFT;
                    enemy->target_platform_id = platform_id;
                    enemy->velocity_y = 0;
                    enemy->velocity_x = go_right ? RED_CROCODILE_WALK_SPEED : -RED_CROCODILE_WALK_SPEED;
                    enemy->attached_vine_id = -1;
                    enemy->walked_distance = 0;  // Reset walked distance
                    enemy->platform_timer = 0;   // Start timer to ignore vines briefly
                    printf("  Red croc taking platform %d, walking %s\n", platform_id, go_right ? "right" : "left");
                } else {
                    // No platform or ignoring it, reverse direction
                    enemy->y = enemy->patrol_end_y;  // Clamp to bottom
                    enemy->moving_up = true;
                    enemy->state = ENEMY_STATE_CLIMBING_UP;
                    enemy->velocity_y = -RED_CROCODILE_CLIMB_SPEED;
                    enemy->ignore_platforms = (GetRandomValue(0, 100) < 50);  // New random decision
                    printf("  Red croc reached bottom, reversing (ignore_platforms=%d)\n", enemy->ignore_platforms);
                }
            } else if (found_platform && !enemy->ignore_platforms) {
                // Found platform while climbing and willing to use it - choose random direction
                bool go_right = (GetRandomValue(0, 100) < 50);
                enemy->y = g_current_level->platforms[platform_id].y;  // Snap to platform
                enemy->state = go_right ? ENEMY_STATE_WALKING_RIGHT : ENEMY_STATE_WALKING_LEFT;
                enemy->target_platform_id = platform_id;
                enemy->velocity_y = 0;
                enemy->velocity_x = go_right ? RED_CROCODILE_WALK_SPEED : -RED_CROCODILE_WALK_SPEED;
                enemy->attached_vine_id = -1;
                enemy->walked_distance = 0;  // Reset walked distance
                enemy->platform_timer = 0;   // Start timer to ignore vines briefly
                printf("  Red croc found platform %d while climbing down, walking %s\n", platform_id, go_right ? "right" : "left");
            }
            break;
        }
        
        case ENEMY_STATE_WALKING_RIGHT: {
            // Move right on platform (apply speed multiplier)
            float distance = enemy->velocity_x * enemy->speed_multiplier * deltaTime;
            enemy->x += distance;
            enemy->walked_distance += fabs(distance);
            enemy->platform_timer += deltaTime;  // Update timer
            
            Platform* plat = &g_current_level->platforms[enemy->target_platform_id];
            float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
            
            // SAFETY: Check if we're outside screen bounds
            if (enemy->x < 0 || enemy->x > LEVEL_WIDTH) {
                printf("  ERROR: Red croc went out of bounds at x=%.0f! Forcing return to vine %d\n", 
                       enemy->x, enemy->original_vine_id);
                if (enemy->original_vine_id >= 0) {
                    Vine* orig_vine = &g_current_level->vines[enemy->original_vine_id];
                    enemy->state = ENEMY_STATE_CLIMBING_UP;
                    enemy->attached_vine_id = enemy->original_vine_id;
                    enemy->target_platform_id = -1;
                    enemy->x = orig_vine->x;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = -RED_CROCODILE_CLIMB_SPEED;
                    enemy->patrol_start_y = orig_vine->y_bottom;
                    enemy->patrol_end_y = orig_vine->y_top;
                    enemy->moving_up = true;
                }
                return;
            }
            
            // Look for ANY vine on platform that we're near
            // Only detect vines after 0.75 seconds on platform (smooth transition)
            if (enemy->platform_timer >= 0.75f) {
                for (int i = 0; i < g_current_level->vine_count; i++) {
                    Vine* vine = &g_current_level->vines[i];
                    
                    // IMPORTANT: Only consider VISIBLE vines (skip invisible center vines)
                    if (!vine->visible) continue;
                    
                    // Check if we're near this vine AND it's on our platform
                    float plat_top = plat->y;
                    bool vine_on_platform = (vine->y_top <= plat_top && vine->y_bottom >= plat_top &&
                                            vine->x >= plat->x && vine->x <= plat_right);
                    
                    if (vine_on_platform && fabs(enemy->x - vine->x) < VINE_DETECT_RANGE) {
                        // Climb this vine - random direction
                        bool go_up = (GetRandomValue(0, 100) < 50);
                        enemy->state = go_up ? ENEMY_STATE_CLIMBING_UP : ENEMY_STATE_CLIMBING_DOWN;
                        enemy->attached_vine_id = i;
                        enemy->target_platform_id = -1;
                        enemy->x = vine->x;
                        enemy->velocity_x = 0;
                        enemy->velocity_y = go_up ? -RED_CROCODILE_CLIMB_SPEED : RED_CROCODILE_CLIMB_SPEED;
                        enemy->patrol_start_y = vine->y_top;      // Top (smaller Y)
                        enemy->patrol_end_y = vine->y_bottom;     // Bottom (larger Y)
                        enemy->moving_up = go_up;
                        enemy->ignore_platforms = (GetRandomValue(0, 100) < 50);
                        printf("  Red croc found VISIBLE vine %d at x=%.0f while walking RIGHT (walked %.0fpx), going %s\n", 
                               i, vine->x, enemy->walked_distance, go_up ? "up" : "down");
                        return;
                    }
                }
            }
            
            // Check if reached edge of platform
            if (enemy->x >= plat_right - ENEMY_WIDTH/2) {
                // Reverse direction to go back
                enemy->state = ENEMY_STATE_WALKING_LEFT;
                enemy->velocity_x = -RED_CROCODILE_WALK_SPEED;
                printf("  Red croc reached RIGHT edge at x=%.0f (plat_right=%.0f), reversing\n", enemy->x, plat_right);
            }
            break;
        }
        
        case ENEMY_STATE_WALKING_LEFT: {
            // Move left on platform (apply speed multiplier)
            float distance = enemy->velocity_x * enemy->speed_multiplier * deltaTime;
            enemy->x += distance;
            enemy->walked_distance += fabs(distance);
            enemy->platform_timer += deltaTime;  // Update timer
            
            Platform* plat = &g_current_level->platforms[enemy->target_platform_id];
            float plat_left = plat->x;
            
            // SAFETY: Check if we're outside screen bounds
            if (enemy->x < 0 || enemy->x > LEVEL_WIDTH) {
                printf("  ERROR: Red croc went out of bounds at x=%.0f! Forcing return to vine %d\n", 
                       enemy->x, enemy->original_vine_id);
                if (enemy->original_vine_id >= 0) {
                    Vine* orig_vine = &g_current_level->vines[enemy->original_vine_id];
                    enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                    enemy->attached_vine_id = enemy->original_vine_id;
                    enemy->target_platform_id = -1;
                    enemy->x = orig_vine->x;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = RED_CROCODILE_CLIMB_SPEED;
                    enemy->patrol_start_y = orig_vine->y_top;     // Top (smaller Y)
                    enemy->patrol_end_y = orig_vine->y_bottom;    // Bottom (larger Y)
                    enemy->moving_up = false;
                }
                return;
            }
            
            // Look for ANY vine on platform that we're near
            // Only detect vines after 0.75 seconds on platform (smooth transition)
            if (enemy->platform_timer >= 0.75f) {
                for (int i = 0; i < g_current_level->vine_count; i++) {
                    Vine* vine = &g_current_level->vines[i];
                    
                    // IMPORTANT: Only consider VISIBLE vines (skip invisible center vines)
                    if (!vine->visible) continue;
                    
                    // Check if we're near this vine AND it's on our platform
                    float plat_top = plat->y;
                    float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
                    bool vine_on_platform = (vine->y_top <= plat_top && vine->y_bottom >= plat_top &&
                                            vine->x >= plat->x && vine->x <= plat_right);
                    
                    if (vine_on_platform && fabs(enemy->x - vine->x) < VINE_DETECT_RANGE) {
                        // Climb this vine - random direction
                        bool go_up = (GetRandomValue(0, 100) < 50);
                        enemy->state = go_up ? ENEMY_STATE_CLIMBING_UP : ENEMY_STATE_CLIMBING_DOWN;
                        enemy->attached_vine_id = i;
                        enemy->target_platform_id = -1;
                        enemy->x = vine->x;
                        enemy->velocity_x = 0;
                        enemy->velocity_y = go_up ? -RED_CROCODILE_CLIMB_SPEED : RED_CROCODILE_CLIMB_SPEED;
                        enemy->patrol_start_y = vine->y_top;      // Top (smaller Y)
                        enemy->patrol_end_y = vine->y_bottom;     // Bottom (larger Y)
                        enemy->moving_up = go_up;
                        enemy->ignore_platforms = (GetRandomValue(0, 100) < 50);
                        printf("  Red croc found VISIBLE vine %d at x=%.0f while walking LEFT (walked %.0fpx), going %s\n", 
                               i, vine->x, enemy->walked_distance, go_up ? "up" : "down");
                        return;
                    }
                }
            }
            
            // Check if reached edge of platform
            if (enemy->x <= plat_left + ENEMY_WIDTH/2) {
                // Reverse direction to go back
                enemy->state = ENEMY_STATE_WALKING_RIGHT;
                enemy->velocity_x = RED_CROCODILE_WALK_SPEED;
                printf("  Red croc reached LEFT edge at x=%.0f (plat_left=%.0f), reversing\n", enemy->x, plat_left);
            }
            break;
        }
        
        default:
            break;
    }
    
    // Update animation
    enemy->animation_time += deltaTime;
    if (enemy->animation_time >= 0.2f) {
        enemy->current_frame = (enemy->current_frame + 1) % 2;
        enemy->animation_time = 0;
    }
}

// ============================================================
// MAIN UPDATE
// ============================================================

void enemy_update(Enemy* enemy, float deltaTime) {
    if (!enemy || !enemy->active) return;
    
    if (enemy->type == ENEMY_RED_CROCODILE) {
        update_red_crocodile(enemy, deltaTime);
    } else if (enemy->type == ENEMY_BLUE_CROCODILE) {
        update_blue_crocodile(enemy, deltaTime);
    }
}

// ============================================================
// RENDERING
// ============================================================

void enemy_render(Enemy* enemy) {
    if (!enemy || !enemy->active) {
        // Debug: Log why enemy is not rendering
        if (enemy) {
            static int debug_counter = 0;
            if (debug_counter++ % 60 == 0) {  // Print once per second (at 60fps)
                printf("  Enemy not rendering: active=%d\n", enemy->active);
            }
        }
        return;
    }
    
    SpriteType sprite_type;
    
    if (enemy->type == ENEMY_RED_CROCODILE) {
        // Choose sprite based on state
        switch (enemy->state) {
            case ENEMY_STATE_CLIMBING_UP:
                sprite_type = SPRITE_SNAPJAW_RED_UP;
                break;
            case ENEMY_STATE_CLIMBING_DOWN:
                sprite_type = SPRITE_SNAPJAW_RED_DOWN;
                break;
            case ENEMY_STATE_WALKING_LEFT:
                sprite_type = SPRITE_SNAPJAW_RED_LEFT;
                break;
            case ENEMY_STATE_WALKING_RIGHT:
                sprite_type = SPRITE_SNAPJAW_RED_RIGHT;
                break;
            default:
                sprite_type = SPRITE_SNAPJAW_RED_DOWN;
        }
    } else if (enemy->type == ENEMY_BLUE_CROCODILE) {
        // Blue crocodile sprites based on state
        if (enemy->state == ENEMY_STATE_WALKING_RIGHT) {
            sprite_type = SPRITE_SNAPJAW_BLUE_RIGHT;
        } else {
            // Climbing down or falling
            sprite_type = SPRITE_SNAPJAW_BLUE_DOWN;
        }
    } else {
        return;  // Unknown enemy type
    }
    
    SpriteSheet* sprite = sprite_manager_get(sprite_type);
    if (!sprite || !sprite->loaded) {
        // Fallback to rectangle if sprite not loaded
        DrawRectangle((int)enemy->x - 12, (int)enemy->y - 12, 24, 24, RED);
        return;
    }
    
    // Source: native sprite dimensions from texture (with spacing between frames)
    Rectangle src = {
        enemy->current_frame * (sprite->frame_width + sprite->spacing),
        0,
        sprite->frame_width,   // Should be 16 for snapjaws
        sprite->frame_height   // Should be 16 for snapjaws
    };
    
    // Destination: scaled 3x, centered on enemy position
    // Sprite is 16x16, scaled 3x = 48x48
    // Adjust Y offset so hitbox is centered on enemy body
    float scaled_width = sprite->frame_width * 3.0f;
    float scaled_height = sprite->frame_height * 3.0f;
    Rectangle dest = {
        enemy->x - scaled_width / 2.0f,
        enemy->y - scaled_height / 2.0f - 6.0f,  // Offset up 6px so hitbox centers better
        scaled_width,
        scaled_height
    };
    
    Vector2 origin = {0, 0};
    DrawTexturePro(sprite->texture, src, dest, origin, 0.0f, WHITE);
    
    // DEBUG: Draw hitbox (24x24 centered on enemy position)
    DrawRectangleLines((int)enemy->x - 12, (int)enemy->y - 12, 24, 24, YELLOW);
}

// ============================================================
// COLLISION DETECTION
// ============================================================

bool enemy_collides_with_player(Enemy* enemy, float player_x, float player_y) {
    if (!enemy || !enemy->active) return false;
    
    // Enemy position is CENTER coordinates
    // Enemy hitbox: 24x24 centered on (enemy->x, enemy->y)
    float enemy_left = enemy->x - 12;
    float enemy_right = enemy->x + 12;
    float enemy_top = enemy->y - 12;
    float enemy_bottom = enemy->y + 12;
    
    // Player position is TOP-LEFT corner of sprite (96x48)
    // Player collision box is offset: 16px from sides, 8px from top
    // Player hitbox: 64x40 starting at (player_x + 16, player_y + 8)
    float player_left = player_x + 16;
    float player_right = player_x + 16 + 64;
    float player_top = player_y + 8;
    float player_bottom = player_y + 8 + 40;
    
    // AABB collision
    bool x_overlap = enemy_left < player_right && enemy_right > player_left;
    bool y_overlap = enemy_top < player_bottom && enemy_bottom > player_top;
    
    return x_overlap && y_overlap;
}
