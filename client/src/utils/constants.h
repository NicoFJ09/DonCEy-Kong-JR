#ifndef CONSTANTS_H
#define CONSTANTS_H

// Network configuration
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

// ============================================================
// CLIENT → SERVER COMMANDS
// ============================================================

#define CMD_LIST_PLAYERS "LIST_PLAYERS"
#define CMD_JOIN_PLAYER "1"
#define CMD_JOIN_SPECTATOR_PREFIX "JOIN_SPECTATOR:"
#define CMD_DISCONNECT "DISCONNECT"
#define CMD_PLAY_AGAIN "PLAY_AGAIN"
#define CMD_EXIT "exit"

// ============================================================
// SERVER → CLIENT PROTOCOL
// ============================================================

#define PROTO_ACCEPTED "ACCEPTED:"
#define PROTO_REJECTED "REJECTED:"
#define PROTO_ERROR "ERROR:"
#define PROTO_SESSION_START "SESSION_START"
#define PROTO_BYE "BYE"
#define PROTO_CLIENT_ID "CLIENT_ID:"
#define PROTO_PLAYER_DISCONNECTED "PLAYER_DISCONNECTED:"
#define PROTO_PLAYER_LEFT_SESSION "PLAYER_LEFT_SESSION:"

// Player list protocol
#define PROTO_PLAYER_LIST_START "PLAYER_LIST_START"
#define PROTO_PLAYER_LIST_END "PLAYER_LIST_END"
#define PROTO_PLAYER_INFO "PLAYER:"

// ============================================================
// UI CONSTANTS
// ============================================================

// Window dimensions - both multiples of 24 for grid alignment
#define UI_WINDOW_WIDTH 1200   // 50 blocks * 24px
#define UI_WINDOW_HEIGHT 912   // 38 blocks * 24px

// Font sizes
#define UI_FONT_SIZE_TITLE 32
#define UI_FONT_SIZE_NORMAL 24
#define UI_FONT_SIZE_INPUT 20
#define UI_FONT_SIZE_ERROR 18
#define UI_FONT_SIZE_SMALL 16

// Colors (Raylib color definitions)
#define UI_COLOR_BACKGROUND (Color){0, 0, 0, 255}      // Black
#define UI_COLOR_TEXT (Color){255, 255, 255, 255}      // White
#define UI_COLOR_INPUT (Color){255, 255, 0, 255}       // Yellow
#define UI_COLOR_ERROR (Color){255, 0, 0, 255}         // Red
#define UI_COLOR_SELECTED (Color){255, 255, 0, 255}    // Yellow

// Input box dimensions
#define UI_INPUT_BOX_WIDTH 400
#define UI_INPUT_BOX_HEIGHT 40
#define UI_INPUT_MAX_CHARS 15

// Cursor blink rate (frames)
#define UI_CURSOR_BLINK_RATE 30

// ============================================================
// GAME CONSTANTS
// ============================================================

// Level dimensions - must match UI window dimensions
#define LEVEL_WIDTH 1200   // 50 blocks * 24px
#define LEVEL_HEIGHT 912   // 38 blocks * 24px

// Water level (in blocks from top, not pixels)
// Screen is 912px tall = 38 blocks. Water at block 37 = last block, flush with bottom
#define WATER_LEVEL_BLOCKS 37  // 37 blocks * 24px = 888px from top (one block from bottom)
#define WATER_LEVEL (WATER_LEVEL_BLOCKS * 24.0f)  // 888.0f

// Platform constants
#define PLATFORM_BLOCK_SIZE 24
#define PLATFORM_COLLISION_TOLERANCE 10

// Vine constants
#define VINE_SPRITE_HEIGHT 24
#define VINE_WIDTH 72.0f
#define VINE_HEIGHT 72.0f
#define VINE_Y_TOLERANCE 50.0f
#define VINE_LATERAL_OFFSET 20.0f
#define VINE_SPACING 60.0f
#define GRAB_RANGE 40.0f
#define VINE_TRANSFER_TOLERANCE 5.0f

// Player dimensions
#define PLAYER_WIDTH 96.0f
#define PLAYER_HEIGHT 48.0f

// Player physics
#define MOVE_SPEED 300.0f
#define JUMP_SPEED 800.0f
#define GRAVITY 2400.0f
#define MAX_FALL_SPEED 1200.0f

// Climb speeds (different for center vs side vines, up vs down)
#define CLIMB_SPEED_CENTER_UP 250.0f    // Fast up on center vine
#define CLIMB_SPEED_CENTER_DOWN 250.0f  // Fast down on center vine
#define CLIMB_SPEED_SIDE_UP 150.0f      // Normal up on side vines
#define CLIMB_SPEED_SIDE_DOWN 250.0f    // Fast down on side vines

// Animation
#define ANIMATION_SPEED 0.15f  // Time per frame in seconds

// Player spawn point in BLOCKS (converted to pixels in code)
#define PLAYER_SPAWN_X_BLOCK 25   // x = 25 blocks (600px)
#define PLAYER_SPAWN_Y_BLOCK 21   // y = 21 blocks (504px) - 2 blocks above bottom platform

// Hand offset for vine grabbing (sprite-specific, hands not centered)
#define HAND_OFFSET_FROM_CENTER 16.0f

// Game loop
#define MAX_DELTA_TIME 0.1f  // 100ms max to prevent physics explosion

// UI Layout
#define UI_MENU_SPACING 60
#define UI_BUTTON_MARGIN 100

// Debug mode
#define DEBUG_MODE 1

#endif