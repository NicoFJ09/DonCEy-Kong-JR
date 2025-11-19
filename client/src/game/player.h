#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "raylib.h"

// ============================================================
// PLAYER CONSTANTS
// ============================================================

#define PLAYER_WIDTH 96
#define PLAYER_HEIGHT 48

#define MOVE_SPEED 300.0f
#define JUMP_SPEED 800.0f
#define CLIMB_SPEED 150.0f         // Single vine (slower)
#define CLIMB_SPEED_DUAL 250.0f    // Dual vine (faster)
#define GRAVITY 2400.0f
#define MAX_FALL_SPEED 1200.0f
#define GRAB_RANGE 20.0f

// Animation
#define ANIMATION_SPEED 0.15f  // Time per frame in seconds

// ============================================================
// PLAYER STATE
// ============================================================

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_JUMPING,
    STATE_CLIMBING,
    STATE_FALLING
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
    int second_vine_id;  // -1 if not holding second vine
    bool dual_vine_climbing;  // True when holding two vines
    int lateral_position;  // -1 = left side, 0 = center, 1 = right side
    
    // State
    PlayerState state;
    Direction direction;
    
    // Animation
    float animation_time;
    int current_frame;
    
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

#endif
