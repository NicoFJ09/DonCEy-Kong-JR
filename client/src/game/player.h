#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "raylib.h"
#include "../utils/constants.h"

// ============================================================
// PLAYER STATE
// ============================================================

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_CLIMBING,
    STATE_FALLING,
    STATE_DYING
} PlayerState;

typedef enum {
    DIR_LEFT,
    DIR_RIGHT
} Direction;

// ============================================================
// PLAYER STRUCTURE
// ============================================================

typedef struct {
    // Position
    float x, y;
    float velocity_x, velocity_y;
    
    // Physics
    bool on_ground;
    bool climbing;
    int attached_vine_id;
    int lateral_position;  // -1 = left side, 0 = center, 1 = right side
    
    // State
    PlayerState state;
    Direction direction;
    
    // Animation
    float animation_time;
    int current_frame;
    
    // Death
    float death_timer;
    
    // Game state
    int lives;
    int score;
    
} Player;

// ============================================================
// PLAYER FUNCTIONS
// ============================================================

/**
 * Create and initialize player
 */
void player_init(Player* player, float x, float y);

/**
 * Update player (physics, animation)
 */
void player_update(Player* player, float deltaTime);

/**
 * Render player
 */
void player_render(Player* player);

/**
 * Handle keyboard input for player
 */
void player_handle_input(Player* player);

/**
 * Reset player to spawn position after death
 */
void player_reset(Player* player, float spawn_x, float spawn_y);

/**
 * Serialize player state to string for network transmission
 * Format: "x:y:vx:vy:state:dir:climbing:vineId:lateralPos:frame"
 * @param player Player to serialize
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 */
void player_serialize_state(Player* player, char* buffer, int buffer_size);

/**
 * Deserialize player state from network message
 * Format: "x:y:vx:vy:state:dir:climbing:vineId:lateralPos:frame"
 * @param player Player to update
 * @param state_string State string (without "PLAYER_STATE:" prefix)
 */
void player_deserialize_state(Player* player, const char* state_string);

#endif
