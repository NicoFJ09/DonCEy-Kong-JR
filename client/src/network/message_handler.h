#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <stdbool.h>
#include "connection.h"
#include "../utils/constants.h"
/**
 * MessageType - Enum for categorizing server messages
 * 
 * TODO: Add new message types here when server sends new protocols
 */
typedef enum {
    // Lobby phase
    MSG_TYPE_ACCEPTED,
    MSG_TYPE_REJECTED,
    MSG_TYPE_ERROR,
    MSG_TYPE_SESSION_START,
    MSG_TYPE_BYE,
    MSG_TYPE_CLIENT_ID,
    MSG_TYPE_LOBBY_MENU,
    MSG_TYPE_PROMPT,
    MSG_TYPE_INFO,
    
    // Game phase
    MSG_TYPE_PLAYER_DISCONNECTED,
    
    // TODO: Add game message types here
    // Example: MSG_TYPE_GAME_STATE, MSG_TYPE_SCORE_UPDATE, etc.
    
    MSG_TYPE_UNKNOWN
} MessageType;

/**
 * ParsedMessage - Structure containing parsed message data
 * 
 * TODO: Add fields here for game-specific data (player_id, score, position, etc.)
 */
typedef struct {
    MessageType type;
    char content[BUFFER_SIZE];
} ParsedMessage;

/**
 * Parse incoming server message and categorize it
 */
MessageType message_parse(const char* buffer, ParsedMessage* parsed);

/**
 * Check if message is a prompt (needs user input without newline)
 */
bool message_is_prompt(const char* buffer);

/**
 * Display message to user (handles formatting)
 */
void message_display(const char* buffer, bool is_prompt);

// TODO: Add command builder functions here
// Example: char* message_build_move_command(const char* direction);

#endif