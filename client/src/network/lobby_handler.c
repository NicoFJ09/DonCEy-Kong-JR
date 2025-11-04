#include "lobby_handler.h"
#include "message_handler.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <string.h>

static void display_lobby_menu(Connection* conn);
static bool handle_user_selection(Connection* conn);
static bool process_server_response(Connection* conn);

bool lobby_handle(Connection* conn) {
    if (!conn || !conn->connected) {
        return false;
    }
    
    while (conn->connected) {
        display_lobby_menu(conn);
        
        if (!handle_user_selection(conn)) {
            return false;
        }
        
        if (process_server_response(conn)) {
            return true;  // Accepted, move to game session
        }
    }
    
    return false;
}

static void display_lobby_menu(Connection* conn) {
    char buffer[BUFFER_SIZE];
    
    while (connection_receive(conn, buffer, BUFFER_SIZE)) {
        ParsedMessage parsed;
        MessageType type = message_parse(buffer, &parsed);
        
        bool is_prompt = message_is_prompt(buffer);
        message_display(buffer, is_prompt);
        
        if (type == MSG_TYPE_PROMPT) {
            break;
        }
    }
}

static bool handle_user_selection(Connection* conn) {
    char input[BUFFER_SIZE];
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    input[strcspn(input, "\n")] = 0;
    
    if (!connection_send(conn, input)) {
        printf("Error sending selection\n");
        return false;
    }
    
    if (strcmp(input, TYPE_EXIT) == 0) {
        char buffer[BUFFER_SIZE];
        if (connection_receive(conn, buffer, BUFFER_SIZE)) {
            printf("%s\n", buffer);
        }
        conn->connected = false;
        return false;
    }
    
    return true;
}

static bool process_server_response(Connection* conn) {
    char buffer[BUFFER_SIZE];
    
    while (connection_receive(conn, buffer, BUFFER_SIZE)) {
        ParsedMessage parsed;
        MessageType type = message_parse(buffer, &parsed);
        
        switch (type) {
            case MSG_TYPE_PROMPT:
                message_display(buffer, true);
                
                char input[BUFFER_SIZE];
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    input[strcspn(input, "\n")] = 0;
                    connection_send(conn, input);
                }
                continue;
                
            case MSG_TYPE_ACCEPTED:
                printf("%s\n", buffer);
                while (connection_receive(conn, buffer, BUFFER_SIZE)) {
                    printf("%s\n", buffer);
                    if (strcmp(buffer, PROTO_SESSION_START) == 0) {
                        return true;
                    }
                }
                break;
                
            case MSG_TYPE_REJECTED:
            case MSG_TYPE_ERROR:
                printf("%s\n\n", buffer);
                return false;
                
            case MSG_TYPE_LOBBY_MENU:
                return false;
                
            default:
                printf("%s\n", buffer);
        }
    }
    
    return false;
}