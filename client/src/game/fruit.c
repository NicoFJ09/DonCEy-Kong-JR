// fruit.h now includes level.h and player.h, so we just need fruit.h
#include "fruit.h"
#include "../rendering/sprite_manager.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ============================================================
// FRUIT CONSTANTS
// ============================================================

#define FRUIT_WIDTH 48.0f     // Fruit sprite width (16px scaled 3x)
#define FRUIT_HEIGHT 48.0f    // Fruit sprite height (16px scaled 3x)
#define FRUIT_COLLISION_RADIUS 24.0f  // Collision radius for fruit pickup

#define POPUP_DURATION 1.5f   // Popup displays for 1.5 seconds
#define POPUP_RISE_SPEED 30.0f  // Popup floats upward slowly

// Points values
#define FRUIT_POINTS_APPLE 200
#define FRUIT_POINTS_BANANA 400
#define FRUIT_POINTS_MANGO 800

// ============================================================
// FRUIT INITIALIZATION
// ============================================================

/**
 * Initialize fruits for the level
 * Places 3-5 fruits randomly on vines
 */
void fruit_initialize(Level* level) {
    if (!level) return;
    
    // Allocate space for up to 10 fruits (generous for future expansion)
    level->max_fruits = 10;
    level->fruit_count = 0;
    level->fruits = (Fruit*)calloc(level->max_fruits, sizeof(Fruit));
    
    if (!level->fruits) {
        printf("ERROR: Failed to allocate memory for fruits\n");
        return;
    }
    
    // Decide how many fruits to spawn (3-5)
    int num_fruits = 3 + (rand() % 3);  // Random between 3 and 5
    
    // Only spawn fruits on visible vines
    int visible_vine_count = 0;
    for (int i = 0; i < level->vine_count; i++) {
        if (level->vines[i].visible) {
            visible_vine_count++;
        }
    }
    
    if (visible_vine_count == 0) {
        printf("WARNING: No visible vines found, cannot spawn fruits\n");
        return;
    }
    
    // Spawn fruits on random vines
    for (int i = 0; i < num_fruits && level->fruit_count < level->max_fruits; i++) {
        // Pick a random visible vine
        int vine_index = -1;
        int attempts = 0;
        while (vine_index == -1 && attempts < 100) {
            int candidate = rand() % level->vine_count;
            if (level->vines[candidate].visible) {
                vine_index = candidate;
            }
            attempts++;
        }
        
        if (vine_index == -1) continue;
        
        Vine* vine = &level->vines[vine_index];
        
        // Random height ratio (0.2 to 0.8 to avoid top and bottom edges)
        float height_ratio = 0.2f + ((rand() % 60) / 100.0f);
        
        // Calculate Y position based on vine height and ratio
        float vine_height = vine->y_bottom - vine->y_top;
        float fruit_y = vine->y_top + (vine_height * height_ratio);
        
        // Center fruit on vine (fruit is 48px wide, vine is 24px wide)
        float fruit_x = vine->x - (FRUIT_WIDTH / 2.0f);
        
        // Random fruit type and points
        int fruit_type = rand() % 3;
        SpriteType sprite;
        int points;
        
        switch (fruit_type) {
            case 0:
                sprite = SPRITE_FRUIT_APPLE;
                points = FRUIT_POINTS_APPLE;
                break;
            case 1:
                sprite = SPRITE_FRUIT_BANANA;
                points = FRUIT_POINTS_BANANA;
                break;
            case 2:
                sprite = SPRITE_FRUIT_MANGO;
                points = FRUIT_POINTS_MANGO;
                break;
            default:
                sprite = SPRITE_FRUIT_APPLE;
                points = FRUIT_POINTS_APPLE;
        }
        
        // Create fruit
        Fruit* fruit = &level->fruits[level->fruit_count];
        fruit->id = level->fruit_count;
        fruit->x = fruit_x;
        fruit->y = fruit_y;
        fruit->vine_id = vine->id;
        fruit->height_ratio = height_ratio;
        fruit->points = points;
        fruit->collected = false;
        fruit->sprite = sprite;
        
        level->fruit_count++;
        
#if DEBUG_MODE
        printf("✓ Spawned fruit %d (type=%d, points=%d) at vine %d, position (%.0f, %.0f)\n",
               fruit->id, fruit_type, points, vine->id, fruit_x, fruit_y);
#endif
    }
    
    printf("✓ Initialized %d fruits on %d visible vines\n", level->fruit_count, visible_vine_count);
}

/**
 * Initialize points popup system
 */
void fruit_initialize_popups(Level* level) {
    if (!level) return;
    
    // Allocate space for up to 10 simultaneous popups
    level->max_popups = 10;
    level->popup_count = 0;
    level->popups = (PointsPopup*)calloc(level->max_popups, sizeof(PointsPopup));
    
    if (!level->popups) {
        printf("ERROR: Failed to allocate memory for popups\n");
        return;
    }
    
    // Initialize all popups as inactive
    for (int i = 0; i < level->max_popups; i++) {
        level->popups[i].active = false;
    }
    
#if DEBUG_MODE
    printf("✓ Initialized points popup system (max: %d)\n", level->max_popups);
#endif
}

// ============================================================
// FRUIT RENDERING
// ============================================================

/**
 * Render all fruits in the level
 */
void fruit_render(Level* level) {
    if (!level || !level->fruits) return;
    
    for (int i = 0; i < level->fruit_count; i++) {
        Fruit* fruit = &level->fruits[i];
        
        // Skip collected fruits
        if (fruit->collected) continue;
        
        // Get sprite
        SpriteSheet* sprite = sprite_manager_get(fruit->sprite);
        
        if (sprite && sprite->loaded) {
            // Source: native sprite dimensions
            Rectangle src = {
                0,
                0,
                sprite->frame_width,
                sprite->frame_height
            };
            
            // Destination: scaled 3x (16px → 48px)
            Rectangle dest = {
                fruit->x,
                fruit->y,
                FRUIT_WIDTH,
                FRUIT_HEIGHT
            };
            
            Vector2 origin = {0, 0};
            DrawTexturePro(sprite->texture, src, dest, origin, 0.0f, WHITE);
        } else {
            // Fallback: draw colored circle
            Color fruit_color;
            switch (fruit->sprite) {
                case SPRITE_FRUIT_APPLE:
                    fruit_color = RED;
                    break;
                case SPRITE_FRUIT_BANANA:
                    fruit_color = YELLOW;
                    break;
                case SPRITE_FRUIT_MANGO:
                    fruit_color = ORANGE;
                    break;
                default:
                    fruit_color = GREEN;
            }
            DrawCircle(fruit->x + FRUIT_WIDTH / 2, fruit->y + FRUIT_HEIGHT / 2, 
                      FRUIT_COLLISION_RADIUS, fruit_color);
        }
        
#if DEBUG_MODE
        // Draw collision circle
        DrawCircleLines(fruit->x + FRUIT_WIDTH / 2, fruit->y + FRUIT_HEIGHT / 2,
                       FRUIT_COLLISION_RADIUS, GREEN);
#endif
    }
}

/**
 * Render all active points popups
 */
void fruit_render_popups(Level* level) {
    if (!level || !level->popups) return;
    
    for (int i = 0; i < level->max_popups; i++) {
        PointsPopup* popup = &level->popups[i];
        
        if (!popup->active) continue;
        
        // Get appropriate sprite based on points value
        SpriteType sprite_type;
        switch (popup->points) {
            case 200:
                sprite_type = SPRITE_POINTS_200;
                break;
            case 400:
                sprite_type = SPRITE_POINTS_400;
                break;
            case 800:
                sprite_type = SPRITE_POINTS_800;
                break;
            default:
                sprite_type = SPRITE_POINTS_200;
        }
        
        SpriteSheet* sprite = sprite_manager_get(sprite_type);
        
        if (sprite && sprite->loaded) {
            // Points sprites are 8x8, scaled 3x = 24x24
            Rectangle src = {0, 0, sprite->frame_width, sprite->frame_height};
            Rectangle dest = {popup->x, popup->y, 24, 24};
            Vector2 origin = {0, 0};
            
            // Fade out as lifetime decreases
            float alpha = popup->lifetime / POPUP_DURATION;
            if (alpha > 1.0f) alpha = 1.0f;
            if (alpha < 0.0f) alpha = 0.0f;
            
            Color tint = {255, 255, 255, (unsigned char)(alpha * 255)};
            DrawTexturePro(sprite->texture, src, dest, origin, 0.0f, tint);
        } else {
            // Fallback: draw text
            DrawText(TextFormat("%d", popup->points), popup->x, popup->y, 20, YELLOW);
        }
    }
}

// ============================================================
// FRUIT UPDATE
// ============================================================

/**
 * Update points popups (decrease lifetime, move upward)
 */
void fruit_update_popups(Level* level, float deltaTime) {
    if (!level || !level->popups) return;
    
    for (int i = 0; i < level->max_popups; i++) {
        PointsPopup* popup = &level->popups[i];
        
        if (!popup->active) continue;
        
        // Decrease lifetime
        popup->lifetime -= deltaTime;
        
        // Move upward
        popup->y -= POPUP_RISE_SPEED * deltaTime;
        
        // Deactivate if lifetime expired
        if (popup->lifetime <= 0) {
            popup->active = false;
        }
    }
}

// ============================================================
// FRUIT COLLISION
// ============================================================

/**
 * Check collision between player and fruits
 */
void fruit_check_collision(Player* player, Level* level) {
    if (!player || !level || !level->fruits) return;
    
    // Player center point (use collision box center)
    float player_center_x = player->x + COLLISION_OFFSET_X + COLLISION_WIDTH / 2;
    float player_center_y = player->y + COLLISION_OFFSET_Y + COLLISION_HEIGHT / 2;
    
    for (int i = 0; i < level->fruit_count; i++) {
        Fruit* fruit = &level->fruits[i];
        
        // Skip collected fruits
        if (fruit->collected) continue;
        
        // Fruit center point
        float fruit_center_x = fruit->x + FRUIT_WIDTH / 2;
        float fruit_center_y = fruit->y + FRUIT_HEIGHT / 2;
        
        // Calculate distance
        float dx = player_center_x - fruit_center_x;
        float dy = player_center_y - fruit_center_y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        // Check collision
        if (distance < FRUIT_COLLISION_RADIUS) {
            // Collect fruit
            int points = fruit_collect(level, i);
            
#if DEBUG_MODE
            printf("✓ Player collected fruit %d, earned %d points!\n", fruit->id, points);
#endif
        }
    }
}

/**
 * Collect a fruit and award points
 * Also creates a points popup at the fruit location
 */
int fruit_collect(Level* level, int fruit_index) {
    if (!level || !level->fruits || fruit_index < 0 || fruit_index >= level->fruit_count) {
        return 0;
    }
    
    Fruit* fruit = &level->fruits[fruit_index];
    
    if (fruit->collected) {
        return 0;  // Already collected
    }
    
    // Mark as collected
    fruit->collected = true;
    int points = fruit->points;
    
    // Create popup at fruit location
    if (level->popups) {
        // Find an inactive popup slot
        for (int i = 0; i < level->max_popups; i++) {
            if (!level->popups[i].active) {
                PointsPopup* popup = &level->popups[i];
                popup->x = fruit->x + FRUIT_WIDTH / 2 - 12;  // Center popup (24px wide / 2)
                popup->y = fruit->y;
                popup->points = points;
                popup->lifetime = POPUP_DURATION;
                popup->active = true;
                break;
            }
        }
    }
    
    return points;
}

// ============================================================
// CLEANUP
// ============================================================

/**
 * Cleanup fruit resources
 */
void fruit_destroy(Level* level) {
    if (!level) return;
    
    if (level->fruits) {
        free(level->fruits);
        level->fruits = NULL;
        level->fruit_count = 0;
        level->max_fruits = 0;
    }
    
    if (level->popups) {
        free(level->popups);
        level->popups = NULL;
        level->popup_count = 0;
        level->max_popups = 0;
    }
    
#if DEBUG_MODE
    printf("✓ Fruit system destroyed\n");
#endif
}
