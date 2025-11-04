#include "message_handler.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <string.h>

MessageType message_parse(const char* buffer, ParsedMessage* parsed) {
    if (!buffer || !parsed) {
        return MSG_TYPE_UNKNOWN;
    }
    
    memset(parsed, 0, sizeof(ParsedMessage));
    
    // Lobby phase messages
    if (strncmp(buffer, PROTO_ACCEPTED, strlen(PROTO_ACCEPTED)) == 0) {
        parsed->type = MSG_TYPE_ACCEPTED;
        strcpy(parsed->content, buffer + strlen(PROTO_ACCEPTED));
        return MSG_TYPE_ACCEPTED;
    }
    
    if (strncmp(buffer, PROTO_REJECTED, strlen(PROTO_REJECTED)) == 0) {
        parsed->type = MSG_TYPE_REJECTED;
        strcpy(parsed->content, buffer + strlen(PROTO_REJECTED));
        return MSG_TYPE_REJECTED;
    }
    
    if (strncmp(buffer, PROTO_ERROR, strlen(PROTO_ERROR)) == 0) {
        parsed->type = MSG_TYPE_ERROR;
        strcpy(parsed->content, buffer + strlen(PROTO_ERROR));
        return MSG_TYPE_ERROR;
    }
    
    if (strcmp(buffer, PROTO_SESSION_START) == 0) {
        parsed->type = MSG_TYPE_SESSION_START;
        return MSG_TYPE_SESSION_START;
    }
    
    if (strcmp(buffer, PROTO_BYE) == 0) {
        parsed->type = MSG_TYPE_BYE;
        return MSG_TYPE_BYE;
    }
    
    if (strncmp(buffer, PROTO_CLIENT_ID, strlen(PROTO_CLIENT_ID)) == 0) {
        parsed->type = MSG_TYPE_CLIENT_ID;
        strcpy(parsed->content, buffer + strlen(PROTO_CLIENT_ID));
        return MSG_TYPE_CLIENT_ID;
    }
    
    // Game phase messages
    if (strncmp(buffer, PROTO_PLAYER_DISCONNECTED, strlen(PROTO_PLAYER_DISCONNECTED)) == 0) {
        parsed->type = MSG_TYPE_PLAYER_DISCONNECTED;
        strcpy(parsed->content, buffer + strlen(PROTO_PLAYER_DISCONNECTED));
        return MSG_TYPE_PLAYER_DISCONNECTED;
    }
    
    // ============================================================
    // TODO: Add new game message parsing here
    // ============================================================
    // Example:
    // if (strncmp(buffer, "GAME_STATE:", 11) == 0) {
    //     parsed->type = MSG_TYPE_GAME_STATE;
    //     strcpy(parsed->content, buffer + 11);
    //     return MSG_TYPE_GAME_STATE;
    // }
    
    // Lobby UI messages
    if (strcmp(buffer, PROTO_LOBBY) == 0 || strncmp(buffer, "========", 8) == 0) {
        parsed->type = MSG_TYPE_LOBBY_MENU;
        strcpy(parsed->content, buffer);
        return MSG_TYPE_LOBBY_MENU;
    }
    
    if (message_is_prompt(buffer)) {
        parsed->type = MSG_TYPE_PROMPT;
        strcpy(parsed->content, buffer);
        return MSG_TYPE_PROMPT;
    }
    
    // Default: informational message
    parsed->type = MSG_TYPE_INFO;
    strcpy(parsed->content, buffer);
    return MSG_TYPE_INFO;
}

bool message_is_prompt(const char* buffer) {
    return (strstr(buffer, "> ") != NULL || 
            strstr(buffer, "Enter player ID") != NULL ||
            strstr(buffer, "(or 'back')") != NULL);
}

void message_display(const char* buffer, bool is_prompt) {
    if (is_prompt) {
        printf("%s", buffer);
        fflush(stdout);
    } else {
        printf("%s\n", buffer);
    }
}

// ============================================================
// TODO: Add command builder functions here
// ============================================================
// Example:
// char* message_build_move_command(const char* direction) {
//     static char command[64];
//     snprintf(command, sizeof(command), "MOVE:%s", direction);
//     return command;
// }