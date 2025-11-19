#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

// ============================================================
// LEVEL CONSTANTS
// ============================================================

#define LEVEL_WIDTH 1200
#define LEVEL_HEIGHT 900
#define WATER_LEVEL 850.0f  // Water at bottom

// ============================================================
// LEVEL STRUCTURES
// ============================================================

/**
 * Platform - Horizontal floating platforms
 */
typedef struct {
    float x, y;
    float width;
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
