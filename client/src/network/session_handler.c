#include "session_handler.h"
#include "message_handler.h"
#include "../utils/constants.h"
#include <stdio.h>
#include <string.h>

void session_handle(Connection* conn) {
    if (!conn || !conn->connected) {
        return;
    }
    
    printf("\n========================================\n");
    printf("Game Session Active\n");
    printf("========================================\n");
    
    char buffer[BUFFER_SIZE];
    
    if (connection_receive(conn, buffer, BUFFER_SIZE)) {
        printf("%s\n\n", buffer);
    }
    
    // ============================================================
    // TODO: Replace this entire section with game loop
    // ============================================================
    // 1. Initialize Raylib window
    // 2. Game loop:
    //    - Check for server messages (game state updates)
    //    - Handle keyboard input (send commands to server)
    //    - Render game state
    // 3. Cleanup Raylib
    
    // PLACEHOLDER: Simple echo loop for testing network
    char input[BUFFER_SIZE];
    
    while (conn->connected) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        if (!connection_send(conn, input)) {
            printf("Error sending message\n");
            break;
        }
        
        if (connection_receive(conn, buffer, BUFFER_SIZE)) {
            ParsedMessage parsed;
            MessageType type = message_parse(buffer, &parsed);
            
            switch (type) {
                case MSG_TYPE_PLAYER_DISCONNECTED:
                    printf("\n⚠️  %s\n", buffer);
                    if (connection_receive(conn, buffer, BUFFER_SIZE)) {
                        printf("%s\n", buffer);
                    }
                    return;
                    
                case MSG_TYPE_BYE:
                    printf("\nServer closed connection\n");
                    return;
                
                // ============================================================
                // TODO: Handle new game message types here
                // ============================================================
                // Example:
                // case MSG_TYPE_GAME_STATE:
                //     session_update_state(parsed.content);
                //     break;
                    
                default:
                    printf("%s\n", buffer);
            }
        } else {
            printf("Connection lost\n");
            break;
        }
    }
}

// ============================================================
// TODO: Call game functions here
// ============================================================