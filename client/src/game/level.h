#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>
#include "enemy.h"

// ============================================================
// LEVEL CONSTANTS
// ============================================================

// Note: All level dimensions defined in utils/constants.h
// LEVEL_WIDTH = 1200 (50 blocks)
// LEVEL_HEIGHT = 912 (38 blocks)
// WATER_LEVEL = 888 (block 37)

// ============================================================
// LEVEL STRUCTURES
// ============================================================

/**
 * Platform - Horizontal floating platforms
 * x, y = position in pixels (top-left corner)
 * width_blocks = width in PLATFORM_BLOCK_SIZE blocks
 * height = ALWAYS 1 block (derived from PLATFORM_BLOCK_SIZE in constants.h)
 */
typedef struct {
    float x, y;           // Position in pixels
    int width_blocks;     // Width in blocks (each block = PLATFORM_BLOCK_SIZE)
    // Height is ALWAYS 1 block (PLATFORM_BLOCK_SIZE pixels)
} Platform;

/**
 * Column - Grass platforms with stems extending to water
 * Grass has collision (acts like platform), stems do not
 */
typedef struct {
    float x;           // X position (top-left of grass)
    float y;           // Y position (top of grass)
    bool wide;         // true = wide grass/stem (96px/48px), false = normal (72px/24px)
    int grass_width;   // Width of grass in pixels (72 or 96)
} Column;

/**
 * Vine - Vertical climbable vines
 */
typedef struct {
    int id;
    float x;
    float y_top;
    float y_bottom;
    bool visible;  // false = invisible lateral vines
} Vine;

/**
 * Level - Complete level structure (hardcoded)
 */
typedef struct {
    // Water
    float water_level;
    
    // Static elements
    Platform* platforms;
    int platform_count;
    
    Column* columns;
    int column_count;
    
    Vine* vines;
    int vine_count;
    
    // Enemies
    Enemy* enemies;
    int enemy_count;
    int max_enemies;  // Maximum capacity of enemies array

    // Enemy spawning timers (temporary - will be admin controlled)
    float blue_spawn_timer;
    float red_spawn_timer;

    // Goal positions
    float cage_x, cage_y;
    float mario_x, mario_y;
    
} Level;

// ============================================================
// LEVEL FUNCTIONS
// ============================================================

/**
 * Create level from server-provided JSON map data
 * Server MUST send valid map data or game will fail
 * @param json_data JSON string (without "MAP_DATA:" prefix)
 * @return Level* or NULL on parse error
 */
Level* level_create_from_json(const char* json_data);

/**
 * Render complete level
 * @param level Level to render
 */
void level_render(Level* level);

/**
 * Cleanup level resources
 * @param level Level to cleanup
 */
void level_destroy(Level* level);

#endif
