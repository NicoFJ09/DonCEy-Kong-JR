#include "enemy.h"
#include "level.h"
#include "../rendering/sprite_manager.h"
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
    enemy->target_platform_id = -1;
    enemy->moving_up = true;
    enemy->patrol_start_y = vine->y_bottom;  // Bottom of vine
    enemy->patrol_end_y = vine->y_top;       // Top of vine
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    
    printf("✓ Red crocodile spawned on vine index %d (ID=%d) at (%.0f, %.0f)\n", 
           vine_index, vine->id, enemy->x, enemy->y);
}

// ============================================================
// BLUE CROCODILE INITIALIZATION
// ============================================================

void enemy_init_blue_crocodile(Enemy* enemy, float start_x, float start_y) {
    enemy->type = ENEMY_BLUE_CROCODILE;
    enemy->state = ENEMY_STATE_FALLING;
    enemy->x = start_x;
    enemy->y = start_y;
    enemy->velocity_x = 0;
    enemy->velocity_y = BLUE_CROCODILE_FALL_SPEED;
    enemy->attached_vine_id = -1;
    enemy->target_platform_id = -1;
    enemy->moving_up = false;
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    
    printf("✓ Blue crocodile spawned at (%.0f, %.0f)\n", enemy->x, enemy->y);
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

static bool find_vine_on_platform(int platform_id, int current_vine_id, int* out_vine_id) {
    if (!g_current_level || platform_id < 0) return false;
    
    Platform* plat = &g_current_level->platforms[platform_id];
    float plat_top = plat->y;
    float plat_left = plat->x;
    float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
    
    // Look for vines that intersect with this platform
    for (int i = 0; i < g_current_level->vine_count; i++) {
        if (i == current_vine_id) continue;  // Skip current vine
        
        Vine* vine = &g_current_level->vines[i];
        float vine_bottom = vine->y_bottom;
        float vine_top = vine->y_top;
        
        // Check if vine passes through platform height and is on platform
        if (vine_top <= plat_top && vine_bottom >= plat_top &&
            vine->x >= plat_left && vine->x <= plat_right) {
            *out_vine_id = i;
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
            // Move up the vine
            enemy->y += enemy->velocity_y * deltaTime;
            
            // Check if reached top of vine OR passing through a platform
            if (enemy->y <= enemy->patrol_end_y) {
                // Reached top - check for platform at top
                int platform_id = -1;
                if (find_platform_at_height(enemy->x, enemy->patrol_end_y, &platform_id)) {
                    // Found platform at top! Switch to walking
                    enemy->y = g_current_level->platforms[platform_id].y;  // Snap to platform
                    enemy->state = ENEMY_STATE_WALKING_RIGHT;
                    enemy->target_platform_id = platform_id;
                    enemy->velocity_y = 0;
                    enemy->velocity_x = RED_CROCODILE_WALK_SPEED;
                    enemy->attached_vine_id = -1;
                    printf("  Red croc reached platform %d at top, walking right\n", platform_id);
                } else {
                    // No platform at top, reverse direction
                    enemy->y = enemy->patrol_end_y;  // Clamp to top
                    enemy->moving_up = false;
                    enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                    enemy->velocity_y = RED_CROCODILE_CLIMB_SPEED;
                    printf("  Red croc reached top, reversing\n");
                }
            }
            break;
        }
        
        case ENEMY_STATE_CLIMBING_DOWN: {
            // Move down the vine
            enemy->y += enemy->velocity_y * deltaTime;
            
            // Check if reached bottom of vine
            if (enemy->y >= enemy->patrol_start_y) {
                // Reached bottom - check for platform at bottom
                int platform_id = -1;
                if (find_platform_at_height(enemy->x, enemy->patrol_start_y, &platform_id)) {
                    // Found platform at bottom! Switch to walking
                    enemy->y = g_current_level->platforms[platform_id].y;  // Snap to platform
                    enemy->state = ENEMY_STATE_WALKING_LEFT;
                    enemy->target_platform_id = platform_id;
                    enemy->velocity_y = 0;
                    enemy->velocity_x = -RED_CROCODILE_WALK_SPEED;
                    enemy->attached_vine_id = -1;
                    printf("  Red croc reached platform %d at bottom, walking left\n", platform_id);
                } else {
                    // No platform at bottom, reverse direction
                    enemy->y = enemy->patrol_start_y;  // Clamp to bottom
                    enemy->moving_up = true;
                    enemy->state = ENEMY_STATE_CLIMBING_UP;
                    enemy->velocity_y = -RED_CROCODILE_CLIMB_SPEED;
                    printf("  Red croc reached bottom, reversing\n");
                }
            }
            break;
        }
        
        case ENEMY_STATE_WALKING_RIGHT: {
            // Move right on platform
            enemy->x += enemy->velocity_x * deltaTime;
            
            Platform* plat = &g_current_level->platforms[enemy->target_platform_id];
            float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
            
            // Look for vine on platform
            int new_vine_id = -1;
            if (find_vine_on_platform(enemy->target_platform_id, enemy->attached_vine_id, &new_vine_id)) {
                Vine* new_vine = &g_current_level->vines[new_vine_id];
                
                // Check if reached the vine
                if (fabs(enemy->x - new_vine->x) < VINE_DETECT_RANGE) {
                    // Climb the new vine
                    enemy->state = ENEMY_STATE_CLIMBING_UP;
                    enemy->attached_vine_id = new_vine_id;
                    enemy->target_platform_id = -1;
                    enemy->x = new_vine->x;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = -RED_CROCODILE_CLIMB_SPEED;
                    enemy->patrol_start_y = new_vine->y_bottom;
                    enemy->patrol_end_y = new_vine->y_top;
                    enemy->moving_up = true;
                    printf("  Red croc found vine %d, climbing up\n", new_vine_id);
                    return;
                }
            }
            
            // Check if reached edge of platform
            if (enemy->x >= plat_right - ENEMY_WIDTH) {
                // Reverse direction
                enemy->state = ENEMY_STATE_WALKING_LEFT;
                enemy->velocity_x = -RED_CROCODILE_WALK_SPEED;
                printf("  Red croc reached platform edge, reversing\n");
            }
            break;
        }
        
        case ENEMY_STATE_WALKING_LEFT: {
            // Move left on platform
            enemy->x += enemy->velocity_x * deltaTime;
            
            Platform* plat = &g_current_level->platforms[enemy->target_platform_id];
            float plat_left = plat->x;
            
            // Look for vine on platform
            int new_vine_id = -1;
            if (find_vine_on_platform(enemy->target_platform_id, enemy->attached_vine_id, &new_vine_id)) {
                Vine* new_vine = &g_current_level->vines[new_vine_id];
                
                // Check if reached the vine
                if (fabs(enemy->x - new_vine->x) < VINE_DETECT_RANGE) {
                    // Climb the new vine
                    enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                    enemy->attached_vine_id = new_vine_id;
                    enemy->target_platform_id = -1;
                    enemy->x = new_vine->x;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = RED_CROCODILE_CLIMB_SPEED;
                    enemy->patrol_start_y = new_vine->y_bottom;
                    enemy->patrol_end_y = new_vine->y_top;
                    enemy->moving_up = false;
                    printf("  Red croc found vine %d, climbing down\n", new_vine_id);
                    return;
                }
            }
            
            // Check if reached edge of platform
            if (enemy->x <= plat_left) {
                // Reverse direction
                enemy->state = ENEMY_STATE_WALKING_RIGHT;
                enemy->velocity_x = RED_CROCODILE_WALK_SPEED;
                printf("  Red croc reached platform edge, reversing\n");
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
// BLUE CROCODILE UPDATE
// ============================================================

static void update_blue_crocodile(Enemy* enemy, float deltaTime) {
    // Simple falling behavior
    enemy->y += enemy->velocity_y * deltaTime;
    
    // Check if hit water
    if (enemy->y >= WATER_LEVEL) {
        enemy->active = false;
        enemy->state = ENEMY_STATE_DEAD;
        printf("  Blue croc fell into water\n");
    }
    
    // Update animation
    enemy->animation_time += deltaTime;
    if (enemy->animation_time >= 0.15f) {
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
    if (!enemy || !enemy->active) return;
    
    // TODO: Add enemy sprites - for now render as colored rectangles for testing
    Color enemy_color;
    
    if (enemy->type == ENEMY_RED_CROCODILE) {
        enemy_color = RED;
    } else {
        enemy_color = BLUE;
    }
    
    // Draw enemy as rectangle (24x24 like player hitbox)
    DrawRectangle((int)enemy->x - 12, (int)enemy->y - 12, 24, 24, enemy_color);
    
    // Draw direction indicator
    if (enemy->state == ENEMY_STATE_CLIMBING_UP) {
        DrawTriangle(
            (Vector2){enemy->x, enemy->y - 15},
            (Vector2){enemy->x - 5, enemy->y - 10},
            (Vector2){enemy->x + 5, enemy->y - 10},
            WHITE
        );
    } else if (enemy->state == ENEMY_STATE_CLIMBING_DOWN) {
        DrawTriangle(
            (Vector2){enemy->x, enemy->y + 15},
            (Vector2){enemy->x - 5, enemy->y + 10},
            (Vector2){enemy->x + 5, enemy->y + 10},
            WHITE
        );
    } else if (enemy->state == ENEMY_STATE_WALKING_LEFT) {
        DrawTriangle(
            (Vector2){enemy->x - 15, enemy->y},
            (Vector2){enemy->x - 10, enemy->y - 5},
            (Vector2){enemy->x - 10, enemy->y + 5},
            WHITE
        );
    } else if (enemy->state == ENEMY_STATE_WALKING_RIGHT) {
        DrawTriangle(
            (Vector2){enemy->x + 15, enemy->y},
            (Vector2){enemy->x + 10, enemy->y - 5},
            (Vector2){enemy->x + 10, enemy->y + 5},
            WHITE
        );
    }
}

// ============================================================
// COLLISION DETECTION
// ============================================================

bool enemy_collides_with_player(Enemy* enemy, float player_x, float player_y) {
    if (!enemy || !enemy->active) return false;
    
    // Simple AABB collision
    bool x_overlap = enemy->x < player_x + PLAYER_WIDTH && 
                     enemy->x + ENEMY_WIDTH > player_x;
    bool y_overlap = enemy->y < player_y + PLAYER_HEIGHT && 
                     enemy->y + ENEMY_HEIGHT > player_y;
    
    return x_overlap && y_overlap;
}
