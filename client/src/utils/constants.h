#ifndef CONSTANTS_H
#define CONSTANTS_H

// Network configuration
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

// ============================================================
// LOBBY PHASE PROTOCOL
// ============================================================

#define TYPE_PLAYER "1"
#define TYPE_SPECTATOR "2"
#define TYPE_EXIT "exit"

#define CMD_BACK "back"
#define CMD_EXIT "exit"
#define CMD_QUIT "quit"

#define PROTO_ACCEPTED "ACCEPTED:"
#define PROTO_REJECTED "REJECTED:"
#define PROTO_ERROR "ERROR:"
#define PROTO_SESSION_START "SESSION_START"
#define PROTO_BYE "BYE"
#define PROTO_CLIENT_ID "CLIENT_ID:"
#define PROTO_PLAYER_DISCONNECTED "PLAYER_DISCONNECTED:"
#define PROTO_LOBBY "LOBBY"

// ============================================================
// TODO: Add game phase protocol constants here
// ============================================================
// Example:
// #define PROTO_GAME_STATE "GAME_STATE:"
// #define PROTO_SCORE "SCORE:"
// #define CMD_MOVE_UP "MOVE:UP"
// #define CMD_JUMP "JUMP"

#endif