#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "raylib.h"

// ============================================================
// PLAYER CONSTANTS
// ============================================================

// Player sprite dimensions (from sprite_manager.c)
// Junior sprites are 32x16 pixels, scaled 3x for rendering
#define PLAYER_SPRITE_WIDTH 32   // Native sprite width
#define PLAYER_SPRITE_HEIGHT 16  // Native sprite height
#define PLAYER_SCALE 3.0f        // Rendering scale

#define PLAYER_WIDTH (PLAYER_SPRITE_WIDTH * PLAYER_SCALE)   // 96 pixels
#define PLAYER_HEIGHT (PLAYER_SPRITE_HEIGHT * PLAYER_SCALE) // 48 pixels

// Sprite hand offset from center (hands are not centered in sprite)
#define HAND_OFFSET_FROM_CENTER 16.0f

#define MOVE_SPEED 300.0f
#define JUMP_SPEED 800.0f

// Climb speeds (different for center vs side vines, up vs down)
#define CLIMB_SPEED_CENTER_UP 250.0f    // Fast up on center vine
#define CLIMB_SPEED_CENTER_DOWN 250.0f  // Fast down on center vine
#define CLIMB_SPEED_SIDE_UP 150.0f      // Normal up on side vines
#define CLIMB_SPEED_SIDE_DOWN 250.0f    // Fast down on side vines

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
