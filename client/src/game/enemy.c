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
#define BLUE_CROCODILE_FALL_SPEED 200.0f     // Falling speed

// ============================================================
// RED CROCODILE INITIALIZATION
// ============================================================

void enemy_init_red_crocodile(Enemy* enemy, void* level_ptr, float start_x, float start_y) {
    (void)start_y;  // Unused - red croc spawns on top platform regardless
    Level* level = (Level*)level_ptr;

    enemy->type = ENEMY_RED_CROCODILE;

    // Find the TWO highest platforms
    float platform_y_values[2] = {LEVEL_HEIGHT, LEVEL_HEIGHT};

    for (int i = 0; i < level->platform_count; i++) {
        Platform* plat = &level->platforms[i];
        if (plat->y < platform_y_values[0]) {
            platform_y_values[1] = platform_y_values[0];
            platform_y_values[0] = plat->y;
        } else if (plat->y < platform_y_values[1]) {
            platform_y_values[1] = plat->y;
        }
    }

    // Find all visible vines that hang from EITHER of the top 2 platforms
    int valid_vines[50];
    int valid_count = 0;

    for (int i = 0; i < level->vine_count && valid_count < 50; i++) {
        Vine* vine = &level->vines[i];
        if (!vine->visible) continue;

        for (int p = 0; p < 2; p++) {
            float platform_y = platform_y_values[p];
            if (vine->y_top >= platform_y && vine->y_top <= platform_y + 50.0f) {
                valid_vines[valid_count++] = i;
                break;
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
        }
    }

    if (valid_count == 0) {
        printf("ERROR: Red croc - No valid vines found!\n");
        enemy->active = false;
        return;
    }

    // Choose random vine from valid ones
    int chosen_index = GetRandomValue(0, valid_count - 1);
    int vine_index = valid_vines[chosen_index];
    Vine* target_vine = &level->vines[vine_index];

    // Start at Mario's position, ON the top platform surface
    enemy->x = start_x;
    enemy->y = platform_y_values[0] - 24.0f;

    // Remember which vine to go to
    enemy->target_platform_id = vine_index;
    enemy->attached_vine_id = -1;

    // Start walking right
    enemy->state = ENEMY_STATE_WALKING_RIGHT;
    enemy->velocity_x = 80.0f;
    enemy->velocity_y = 0;

    // Animation
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    enemy->speed_multiplier = 1.0f;

    printf("✓ Red croc spawned, will walk to vine %d at x=%.0f\n", vine_index, target_vine->x);
}

// ============================================================
// BLUE CROCODILE INITIALIZATION
// ============================================================

void enemy_init_blue_crocodile(Enemy* enemy, void* level_ptr, float start_x, float start_y) {
    (void)start_y;  // Unused
    Level* level = (Level*)level_ptr;

    enemy->type = ENEMY_BLUE_CROCODILE;

    // Find the TWO highest platforms
    float platform_y_values[2] = {LEVEL_HEIGHT, LEVEL_HEIGHT};

    for (int i = 0; i < level->platform_count; i++) {
        Platform* plat = &level->platforms[i];
        if (plat->y < platform_y_values[0]) {
            platform_y_values[1] = platform_y_values[0];
            platform_y_values[0] = plat->y;
        } else if (plat->y < platform_y_values[1]) {
            platform_y_values[1] = plat->y;
        }
    }

    // Find all visible vines that hang from EITHER of the top 2 platforms
    int valid_vines[50];
    int valid_count = 0;

    for (int i = 0; i < level->vine_count && valid_count < 50; i++) {
        Vine* vine = &level->vines[i];
        if (!vine->visible) continue;

        for (int p = 0; p < 2; p++) {
            float platform_y = platform_y_values[p];
            if (vine->y_top >= platform_y && vine->y_top <= platform_y + 50.0f) {
                valid_vines[valid_count++] = i;
                break;
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
        }
    }

    if (valid_count == 0) {
        printf("ERROR: Blue croc - No valid vines found!\n");
        enemy->active = false;
        return;
    }

    // Choose random vine from valid ones
    int chosen_index = GetRandomValue(0, valid_count - 1);
    int vine_index = valid_vines[chosen_index];

    // Start at Mario's position, ON the top platform surface
    enemy->x = start_x;
    enemy->y = platform_y_values[0] - 24.0f;

    // Remember which vine to go to
    enemy->target_platform_id = vine_index;
    enemy->attached_vine_id = -1;

    // Start walking right
    enemy->state = ENEMY_STATE_WALKING_RIGHT;
    enemy->velocity_x = 80.0f;
    enemy->velocity_y = 0;

    // Animation
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    enemy->speed_multiplier = 1.0f;

    printf("✓ Blue croc spawned, will walk to vine %d\n", vine_index);
}

// ============================================================
// RED CROCODILE UPDATE
// ============================================================

static void update_red_crocodile(Enemy* enemy, float deltaTime) {
    if (!g_current_level) return;

    switch (enemy->state) {
        case ENEMY_STATE_WALKING_RIGHT: {
            // Walk right on the top platform
            enemy->x += enemy->velocity_x * enemy->speed_multiplier * deltaTime;

            // Apply gravity to fall to lower platforms
            enemy->velocity_y = 200.0f;
            enemy->y += enemy->velocity_y * deltaTime;

            // Check collision with platforms to stop falling
            for (int i = 0; i < g_current_level->platform_count; i++) {
                Platform* plat = &g_current_level->platforms[i];
                float plat_left = plat->x;
                float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
                float plat_top = plat->y;
                float plat_bottom = plat->y + PLATFORM_BLOCK_SIZE;

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

                if (enemy->x >= target_vine->x - 5.0f) {
                    // Snap to vine and start climbing UP AND DOWN
                    bool go_up = (GetRandomValue(0, 100) < 50);
                    enemy->state = go_up ? ENEMY_STATE_CLIMBING_UP : ENEMY_STATE_CLIMBING_DOWN;
                    enemy->x = target_vine->x;
                    enemy->y = target_vine->y_top;
                    enemy->attached_vine_id = enemy->target_platform_id;
                    enemy->target_platform_id = -1;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = go_up ? -RED_CROCODILE_CLIMB_SPEED : RED_CROCODILE_CLIMB_SPEED;
                    printf("✓ Red croc reached vine %d, climbing %s\n", enemy->attached_vine_id, go_up ? "UP" : "DOWN");
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

        case ENEMY_STATE_CLIMBING_UP: {
            // LOCKED TO VINE - never leave it
            Vine* current_vine = NULL;
            if (enemy->attached_vine_id >= 0 && enemy->attached_vine_id < g_current_level->vine_count) {
                current_vine = &g_current_level->vines[enemy->attached_vine_id];
            }

            if (!current_vine) {
                enemy->active = false;
                break;
            }

            // Lock X position to vine
            enemy->x = current_vine->x;

            // Move up the vine
            enemy->y += enemy->velocity_y * enemy->speed_multiplier * deltaTime;

            // Check if reached top of vine - reverse direction
            if (enemy->y <= current_vine->y_top) {
                enemy->y = current_vine->y_top;
                enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                enemy->velocity_y = RED_CROCODILE_CLIMB_SPEED;
            }
            break;
        }

        case ENEMY_STATE_CLIMBING_DOWN: {
            // LOCKED TO VINE - never leave it
            Vine* current_vine = NULL;
            if (enemy->attached_vine_id >= 0 && enemy->attached_vine_id < g_current_level->vine_count) {
                current_vine = &g_current_level->vines[enemy->attached_vine_id];
            }

            if (!current_vine) {
                enemy->active = false;
                break;
            }

            // Lock X position to vine
            enemy->x = current_vine->x;

            // Move down the vine
            enemy->y += enemy->velocity_y * enemy->speed_multiplier * deltaTime;

            // Check if reached bottom of vine - reverse direction
            if (enemy->y >= current_vine->y_bottom) {
                enemy->y = current_vine->y_bottom;
                enemy->state = ENEMY_STATE_CLIMBING_UP;
                enemy->velocity_y = -RED_CROCODILE_CLIMB_SPEED;
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
            enemy->velocity_y = 200.0f;
            enemy->y += enemy->velocity_y * deltaTime;

            // Check collision with platforms to stop falling
            for (int i = 0; i < g_current_level->platform_count; i++) {
                Platform* plat = &g_current_level->platforms[i];
                float plat_left = plat->x;
                float plat_right = plat->x + (plat->width_blocks * PLATFORM_BLOCK_SIZE);
                float plat_top = plat->y;
                float plat_bottom = plat->y + PLATFORM_BLOCK_SIZE;

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

                if (enemy->x >= target_vine->x - 5.0f) {
                    // Snap to vine and start descending
                    enemy->state = ENEMY_STATE_CLIMBING_DOWN;
                    enemy->x = target_vine->x;
                    enemy->y = target_vine->y_top;
                    enemy->attached_vine_id = enemy->target_platform_id;
                    enemy->velocity_x = 0;
                    enemy->velocity_y = 120.0f;  // Descend speed
                    printf("✓ Blue croc reached vine %d, descending\n", enemy->attached_vine_id);
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
            sprite_type = SPRITE_SNAPJAW_BLUE_DOWN;
        }
    } else {
        return;  // Unknown enemy type
    }

    SpriteSheet* sprite = sprite_manager_get(sprite_type);
    if (!sprite || !sprite->loaded) {
        DrawRectangle((int)enemy->x - 12, (int)enemy->y - 12, 24, 24, RED);
        return;
    }

    // Source: native sprite dimensions from texture (with spacing between frames)
    Rectangle src = {
        enemy->current_frame * (sprite->frame_width + sprite->spacing),
        0,
        sprite->frame_width,
        sprite->frame_height
    };

    // Destination: scaled 3x, centered on enemy position
    float scaled_width = sprite->frame_width * 3.0f;
    float scaled_height = sprite->frame_height * 3.0f;
    Rectangle dest = {
        enemy->x - scaled_width / 2.0f,
        enemy->y - scaled_height / 2.0f - 6.0f,
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

    // Enemy hitbox: 24x24 centered on (enemy->x, enemy->y)
    float enemy_left = enemy->x - 12;
    float enemy_right = enemy->x + 12;
    float enemy_top = enemy->y - 12;
    float enemy_bottom = enemy->y + 12;

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

// ============================================================
// ADMIN PANEL SPAWN FUNCTIONS (FASE 2)
// ============================================================

bool enemy_spawn_red_at_vine_id(Enemy* enemy, struct Level* level_ptr, int vine_id) {
    if (!enemy || !level_ptr) {
        printf("[ADMIN] Error: Invalid enemy or level pointer\n");
        return false;
    }
    
    Level* level = (Level*)level_ptr;
    
    // Find vine with matching id (server validates accessible vines)
    Vine* target_vine = NULL;
    int vine_index = -1;
    
    for (int i = 0; i < level->vine_count; i++) {
        if (level->vines[i].id == vine_id && level->vines[i].visible) {
            target_vine = &level->vines[i];
            vine_index = i;
            break;
        }
    }
    
    if (!target_vine) {
        printf("[ADMIN] Error: Vine ID %d not found or not visible\n", vine_id);
        return false;
    }
    
    // Initialize red crocodile with modified logic
    enemy->type = ENEMY_RED_CROCODILE;
    
    // Find the highest platform
    float platform_y = LEVEL_HEIGHT;
    for (int i = 0; i < level->platform_count; i++) {
        Platform* plat = &level->platforms[i];
        if (plat->y < platform_y) {
            platform_y = plat->y;
        }
    }
    
    // Start at Mario's position (top-left)
    enemy->x = level->mario_x;
    enemy->y = platform_y - 24.0f;
    
    // Set target vine
    enemy->target_platform_id = vine_index;
    enemy->attached_vine_id = -1;
    
    // Start walking right
    enemy->state = ENEMY_STATE_WALKING_RIGHT;
    enemy->velocity_x = 80.0f;
    enemy->velocity_y = 0;
    
    // Animation
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    enemy->speed_multiplier = level->speed_multiplier;
    
    printf("[ADMIN] ✓ Red croc spawned, walking to vine %d at x=%.0f\n", vine_id, target_vine->x);
    return true;
}

bool enemy_spawn_blue_at_vine_id(Enemy* enemy, struct Level* level_ptr, int vine_id) {
    if (!enemy || !level_ptr) {
        printf("[ADMIN] Error: Invalid enemy or level pointer\n");
        return false;
    }
    
    Level* level = (Level*)level_ptr;
    
    // Find vine with matching id (server validates accessible vines)
    Vine* target_vine = NULL;
    int vine_index = -1;
    
    for (int i = 0; i < level->vine_count; i++) {
        if (level->vines[i].id == vine_id && level->vines[i].visible) {
            target_vine = &level->vines[i];
            vine_index = i;
            break;
        }
    }
    
    if (!target_vine) {
        printf("[ADMIN] Error: Vine ID %d not found or not visible\n", vine_id);
        return false;
    }
    
    // Initialize blue crocodile
    enemy->type = ENEMY_BLUE_CROCODILE;
    
    // Find the highest platform
    float platform_y = LEVEL_HEIGHT;
    for (int i = 0; i < level->platform_count; i++) {
        Platform* plat = &level->platforms[i];
        if (plat->y < platform_y) {
            platform_y = plat->y;
        }
    }
    
    // Start at Mario's position (top-left)
    enemy->x = level->mario_x;
    enemy->y = platform_y - 24.0f;
    
    // Set target vine
    enemy->target_platform_id = vine_index;
    enemy->attached_vine_id = -1;
    
    // Start walking right
    enemy->state = ENEMY_STATE_WALKING_RIGHT;
    enemy->velocity_x = 80.0f;
    enemy->velocity_y = 0;
    
    // Animation
    enemy->animation_time = 0;
    enemy->current_frame = 0;
    enemy->active = true;
    enemy->speed_multiplier = level->speed_multiplier;
    
    printf("[ADMIN] ✓ Blue croc spawned, walking to vine %d\n", vine_id);
    return true;
}
