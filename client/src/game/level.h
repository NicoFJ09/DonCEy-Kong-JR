#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

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
 * Column - Vertical structures from water
 */
typedef struct {
    float x;
    float height;
    bool wide;  // true = wide stem/grass, false = normal
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
    
    // Goal positions
    float cage_x, cage_y;
    float mario_x, mario_y;
    
} Level;

// ============================================================
// LEVEL FUNCTIONS
// ============================================================

/**
 * Create and initialize hardcoded level
 * @return Pointer to initialized level
 */
Level* level_create(void);

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
