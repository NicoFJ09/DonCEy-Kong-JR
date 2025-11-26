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

    // Vine tracking
    int attached_vine_id;       // Current vine (-1 if not attached)
    int target_platform_id;     // Target vine index during walk phase

    // Animation
    float animation_time;
    int current_frame;

    // Active flag
    bool active;

    // Speed multiplier (increases with each level completion)
    float speed_multiplier;

} Enemy;

// ============================================================
// ENEMY FUNCTIONS
// ============================================================

/**
 * Initialize red crocodile (spawns at Mario's position, walks to a vine)
 * @param enemy Enemy to initialize
 * @param level Level pointer
 * @param start_x Starting X position
 * @param start_y Starting Y position
 */
void enemy_init_red_crocodile(Enemy* enemy, void* level, float start_x, float start_y);

/**
 * Initialize blue crocodile (falling)
 * @param enemy Enemy to initialize
 * @param level Level pointer
 * @param start_x Starting X position
 * @param start_y Starting Y position
 */
void enemy_init_blue_crocodile(Enemy* enemy, void* level, float start_x, float start_y);

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

// ============================================================
// ADMIN PANEL SPAWN FUNCTIONS (FASE 2)
// ============================================================

/**
 * Spawn red crocodile at specific vine ID (admin controlled)
 * @param enemy Enemy to initialize
 * @param level Level pointer
 * @param vine_id Target vine ID (1-11 from admin panel)
 * @return true if spawn successful, false if vine invalid
 */
bool enemy_spawn_red_at_vine_id(Enemy* enemy, struct Level* level, int vine_id);

/**
 * Spawn blue crocodile at specific vine ID (admin controlled)
 * @param enemy Enemy to initialize
 * @param level Level pointer
 * @param vine_id Target vine ID (1-11 from admin panel)
 * @return true if spawn successful, false if vine invalid
 */
bool enemy_spawn_blue_at_vine_id(Enemy* enemy, struct Level* level, int vine_id);

#endif // ENEMY_H
