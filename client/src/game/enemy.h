#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include "raylib.h"
#include "../utils/constants.h"

// Forward declaration
struct Level;

// ============================================================
// ENEMY TYPES
// ============================================================

typedef enum {
    ENEMY_RED_CROCODILE,    // Moves vertically on vines, can walk on platforms
    ENEMY_BLUE_CROCODILE    // Falls from top, dies in water
} EnemyType;

typedef enum {
    ENEMY_STATE_CLIMBING_UP,
    ENEMY_STATE_CLIMBING_DOWN,
    ENEMY_STATE_WALKING_LEFT,
    ENEMY_STATE_WALKING_RIGHT,
    ENEMY_STATE_FALLING,
    ENEMY_STATE_DEAD
} EnemyState;

// ============================================================
// ENEMY STRUCTURE
// ============================================================

typedef struct {
    // Position
    float x, y;
    float velocity_x, velocity_y;
    
    // Type and state
    EnemyType type;
    EnemyState state;
    
    // Movement tracking for red crocodile
    int attached_vine_id;       // Current vine (-1 if not on vine)
    int original_vine_id;       // Vine where enemy started (for returning)
    int target_platform_id;     // Platform walking on (-1 if not on platform)
    bool moving_up;             // Direction of vertical patrol
    bool ignore_platforms;      // If true, skip platform detection this cycle
    float walked_distance;      // Distance walked on platform (to avoid immediate return)
    float platform_timer;       // Time since taking platform (ignore vines briefly)
    float patrol_start_y;       // Where patrol started
    float patrol_end_y;         // Where patrol ends (top of vine or platform)
    
    // Animation
    float animation_time;
    int current_frame;
    
    // Active flag
    bool active;
    
} Enemy;

// ============================================================
// ENEMY FUNCTIONS
// ============================================================

/**
 * Initialize red crocodile on a vine
 * @param enemy Enemy to initialize
 * @param level Level containing the vines
 * @param vine_index Index in level->vines array (not vine ID)
 * @param start_y Starting Y position
 */
void enemy_init_red_crocodile(Enemy* enemy, void* level, int vine_index, float start_y);

/**
 * Initialize blue crocodile (falling)
 * @param enemy Enemy to initialize
 * @param start_x Starting X position
 * @param start_y Starting Y position
 */
void enemy_init_blue_crocodile(Enemy* enemy, float start_x, float start_y);

/**
 * Update enemy physics and AI
 * @param enemy Enemy to update
 * @param deltaTime Time since last frame
 */
void enemy_update(Enemy* enemy, float deltaTime);

/**
 * Render enemy sprite
 * @param enemy Enemy to render
 */
void enemy_render(Enemy* enemy);

/**
 * Check if enemy collides with player
 * @param enemy Enemy to check
 * @param player_x Player X position
 * @param player_y Player Y position
 * @return true if collision detected
 */
bool enemy_collides_with_player(Enemy* enemy, float player_x, float player_y);

#endif // ENEMY_H
