// fruit.h now includes level.h and player.h, so we just need fruit.h
#include "fruit.h"
#include "../rendering/sprite_manager.h"
#include "../network/game_events.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================
// FRUIT CONSTANTS
// ============================================================

#define FRUIT_WIDTH 48.0f     // Fruit sprite width (16px scaled 3x)
#define FRUIT_HEIGHT 48.0f    // Fruit sprite height (16px scaled 3x)
#define FRUIT_COLLISION_RADIUS 24.0f  // Collision radius for fruit pickup

#define POPUP_DURATION 1.0f   // Popup displays for 1.0 seconds
#define POPUP_RISE_SPEED 0.0f  // Popup stays in place (no floating)

// Points values
#define FRUIT_POINTS_APPLE 200
#define FRUIT_POINTS_BANANA 400
#define FRUIT_POINTS_MANGO 800

// ============================================================
// FRUIT INITIALIZATION
// ============================================================

/**
 * Initialize fruits for the level
 * Only allocates memory, no random spawning
 * All fruits are spawned via admin panel (SPAWN_FRUIT command)
 */
void fruit_initialize(Level* level) {
    if (!level) return;
    
    // Allocate space for up to 20 fruits (admin can spawn many)
    level->max_fruits = 20;
    level->fruit_count = 0;
    level->fruits = (Fruit*)calloc(level->max_fruits, sizeof(Fruit));
    
    if (!level->fruits) {
        printf("ERROR: Failed to allocate memory for fruits\n");
        return;
    }
    
    printf("✓ Fruit system initialized (max: %d, ready for admin spawning)\n", level->max_fruits);
}

// ============================================================
// ADMIN PANEL SPAWN
// ============================================================

bool fruit_spawn_admin(Level* level, int fruit_id, int vine_id, int position_y, const char* type_str) {
    if (!level || !type_str) {
        printf("[ADMIN] Error: Invalid level or type_str\n");
        return false;
    }
    
    // Check if we have space
    if (level->fruit_count >= level->max_fruits) {
        printf("[ADMIN] Error: Max fruits reached (%d)\n", level->max_fruits);
        return false;
    }
    
    // Find vine by ID
    Vine* target_vine = NULL;
    for (int i = 0; i < level->vine_count; i++) {
        if (level->vines[i].id == vine_id && level->vines[i].visible) {
            target_vine = &level->vines[i];
            break;
        }
    }
    
    if (!target_vine) {
        printf("[ADMIN] Error: Vine ID %d not found or not visible\n", vine_id);
        return false;
    }
    
    // Parse fruit type and points from type_str ("Mango- 200 pts")
    SpriteType sprite;
    int points;
    
    if (strstr(type_str, "Mango")) {
        sprite = SPRITE_FRUIT_MANGO;
        points = FRUIT_POINTS_MANGO;
    } else if (strstr(type_str, "Banana")) {
        sprite = SPRITE_FRUIT_BANANA;
        points = FRUIT_POINTS_BANANA;
    } else if (strstr(type_str, "Manzana") || strstr(type_str, "Apple")) {
        sprite = SPRITE_FRUIT_APPLE;
        points = FRUIT_POINTS_APPLE;
    } else {
        printf("[ADMIN] Error: Unknown fruit type: %s\n", type_str);
        return false;
    }
    
    // Calculate position on vine
    // position_y is block number from vine start (0 = first block, n = last block)
    // Each block is 24px (VINE_HEIGHT constant)
    const float BLOCK_HEIGHT = 24.0f;
    
    // Calculate vine height in blocks
    float vine_height_px = target_vine->y_bottom - target_vine->y_top;
    int vine_height_blocks = (int)(vine_height_px / BLOCK_HEIGHT);
    
    // Validate position_y is within vine bounds
    if (position_y < 0 || position_y > vine_height_blocks) {
        printf("[ADMIN] Warning: position_y=%d out of bounds for vine %d (max blocks: %d), clamping\n",
               position_y, vine_id, vine_height_blocks);
        if (position_y < 0) position_y = 0;
        if (position_y > vine_height_blocks) position_y = vine_height_blocks;
    }
    
    // Calculate actual Y position: vine top + (block number * block height)
    float fruit_y = target_vine->y_top + (position_y * BLOCK_HEIGHT);
    
    // Calculate height ratio for compatibility
    float height_ratio = (float)position_y / (float)vine_height_blocks;
    
    // Center fruit on vine
    float fruit_x = target_vine->x - (FRUIT_WIDTH / 2.0f);
    
    // Create fruit
    Fruit* fruit = &level->fruits[level->fruit_count];
    fruit->id = fruit_id;  // Use server-provided ID
    fruit->x = fruit_x;
    fruit->y = fruit_y;
    fruit->vine_id = target_vine->id;
    fruit->height_ratio = height_ratio;
    fruit->points = points;
    fruit->collected = false;
    fruit->sprite = sprite;
    
    level->fruit_count++;
    
    printf("[ADMIN] ✓ Spawned fruit ID=%d (%s, %d pts) at vine %d, position (%.0f, %.0f)\n",
           fruit_id, type_str, points, vine_id, fruit_x, fruit_y);
    
    return true;
}

// ============================================================
// FRUIT REMOVAL
// ============================================================

bool fruit_remove_by_id(Level* level, int fruit_id) {
    if (!level || !level->fruits) {
        printf("[ADMIN] Error: Invalid level\n");
        return false;
    }
    
    // Find fruit with matching ID
    for (int i = 0; i < level->fruit_count; i++) {
        if (level->fruits[i].id == fruit_id && !level->fruits[i].collected) {
            // Mark as collected (effectively removes it from rendering)
            level->fruits[i].collected = true;
            
            printf("[ADMIN] ✓ Removed fruit ID=%d\n", fruit_id);
            return true;
        }
    }
    
    printf("[ADMIN] Error: Fruit ID %d not found or already collected\n", fruit_id);
    return false;
}

// ============================================================
// FRUIT POPUP SYSTEM
// ============================================================

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
        // Points sprites are 8x8, scaled 4.5x = 36x36 (balanced visibility)
        Rectangle src = {0, 0, sprite->frame_width, sprite->frame_height};
        Rectangle dest = {popup->x, popup->y, 36, 36};
        Vector2 origin = {0, 0};            // Fade out as lifetime decreases (1.0 second total)
            float alpha = popup->lifetime / POPUP_DURATION;
            if (alpha > 1.0f) alpha = 1.0f;
            if (alpha < 0.0f) alpha = 0.0f;
            
            Color tint = {255, 255, 255, (unsigned char)(alpha * 255)};
            DrawTexturePro(sprite->texture, src, dest, origin, 0.0f, tint);
        } else {
            // Fallback: draw text with fade
            float alpha = popup->lifetime / POPUP_DURATION;
            if (alpha > 1.0f) alpha = 1.0f;
            if (alpha < 0.0f) alpha = 0.0f;
            Color color = {255, 255, 0, (unsigned char)(alpha * 255)};
            DrawText(TextFormat("+%d", popup->points), popup->x, popup->y, 20, color);
        }
    }
}

// ============================================================
// FRUIT UPDATE
// ============================================================

/**
 * Update points popups (decrease lifetime, NO floating)
 */
void fruit_update_popups(Level* level, float deltaTime) {
    if (!level || !level->popups) return;
    
    for (int i = 0; i < level->max_popups; i++) {
        PointsPopup* popup = &level->popups[i];
        
        if (!popup->active) continue;
        
        // Decrease lifetime (1 second total)
        popup->lifetime -= deltaTime;
        
        // NO floating - popup stays in place
        
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
 * Check collision between player and fruits using AABB (hitbox overlap)
 * Works when jumping, on vine, or moving - any time hitboxes touch
 */
void fruit_check_collision(Player* player, Level* level, Connection* conn) {
    if (!player || !level || !level->fruits) return;
    
    // Player hitbox bounds (using collision box from constants.h)
    float player_left = player->x + COLLISION_OFFSET_X;
    float player_right = player_left + COLLISION_WIDTH;
    float player_top = player->y + COLLISION_OFFSET_Y;
    float player_bottom = player_top + COLLISION_HEIGHT;
    
    for (int i = 0; i < level->fruit_count; i++) {
        Fruit* fruit = &level->fruits[i];
        
        // Skip collected fruits
        if (fruit->collected) continue;
        
        // Fruit hitbox bounds (48x48 sprite, full size for collision)
        float fruit_left = fruit->x;
        float fruit_right = fruit->x + FRUIT_WIDTH;
        float fruit_top = fruit->y;
        float fruit_bottom = fruit->y + FRUIT_HEIGHT;
        
        // AABB collision detection: check if rectangles overlap
        bool collision = (player_left < fruit_right &&
                         player_right > fruit_left &&
                         player_top < fruit_bottom &&
                         player_bottom > fruit_top);
        
        if (collision) {
            // Collect fruit locally (immediate feedback)
            int points = fruit_collect(level, i);
            
            // Notify server (async, non-blocking)
            if (conn) {
                event_send_fruit_collected(conn, fruit->id, points);
            }
            
#if DEBUG_MODE
            printf("✓ Player hitbox collided with fruit %d, earned %d points!\n", fruit->id, points);
            printf("  Player box: (%.0f,%.0f) to (%.0f,%.0f)\n", 
                   player_left, player_top, player_right, player_bottom);
            printf("  Fruit box:  (%.0f,%.0f) to (%.0f,%.0f)\n", 
                   fruit_left, fruit_top, fruit_right, fruit_bottom);
#endif
        }
    }
}

/**
 * Collect a fruit and create popup (score handled by server)
 * @param level Level containing the fruit
 * @param fruit_index Index of fruit to collect
 * @return Points awarded (for popup display only, server updates actual score)
 */
int fruit_collect(Level* level, int fruit_index) {
    if (!level || !level->fruits || fruit_index < 0 || fruit_index >= level->fruit_count) {
        return 0;
    }
    
    Fruit* fruit = &level->fruits[fruit_index];
    
    if (fruit->collected) {
        return 0;  // Already collected
    }
    
    // Mark as collected locally (optimistic update)
    fruit->collected = true;
    int points = fruit->points;
    
    // NOTE: Do NOT update player->score here
    // Score will be updated when server confirms via SCORE_UPDATE
    printf("✨ [FRUIT] Collected locally! Points: %d (Server will confirm)\n", points);
    
    // Create popup at fruit location (immediate feedback)
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
