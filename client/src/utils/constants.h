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

// Window dimensions
#define UI_WINDOW_WIDTH 1200
#define UI_WINDOW_HEIGHT 900

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

#endif