#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "network/connection.h"
#include "utils/constants.h"

void print_header() {
    printf("========================================\n");
    printf("🎮 DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

void display_lobby_menu(Connection* conn, char* buffer) {
    // Read and display lobby menu from server
    while (connection_receive(conn, buffer, BUFFER_SIZE)) {
        // Check if this is the prompt line
        if (strstr(buffer, "> ") != NULL) {
            printf("%s", buffer);  // Print without newline
            fflush(stdout);        // Force immediate display
            break;
        }
        
        // For all other lines, print normally with newline
        printf("%s\n", buffer);
    }
}

bool handle_lobby_selection(Connection* conn, char* buffer) {
    char input[100];
    
    // Get user input
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    // Remove newline
    input[strcspn(input, "\n")] = 0;
    
    // Send selection to server
    if (!connection_send(conn, input)) {
        printf("Error sending selection\n");
        return false;
    }
    
    // Check if user wants to exit
    if (strcmp(input, TYPE_EXIT) == 0) {
        connection_receive(conn, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);
        conn->connected = false;  // Mark connection as closed
        return false;
    }
    
    return true;
}

void handle_lobby(Connection* conn, char* buffer) {
    while (conn->connected) {
        // Display lobby menu
        display_lobby_menu(conn, buffer);
        
        // Handle user selection
        if (!handle_lobby_selection(conn, buffer)) {
            break;
        }
        
        // Check server response
        while (connection_receive(conn, buffer, BUFFER_SIZE)) {
            // Check for player selection prompt FIRST (before printing)
            if (strstr(buffer, "Enter player ID") != NULL) {
                // This is a prompt line, print without extra newline and handle input
                printf("%s", buffer);
                fflush(stdout);
                
                // Get user input for player ID
                char input[100];
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    input[strcspn(input, "\n")] = 0;  // Remove newline
                    connection_send(conn, input);      // Send to server
                }
                // Continue receiving server response
                continue;
            }
            
            printf("%s\n", buffer);
            
            // Check if server returned to lobby (user typed 'back')
            if (strncmp(buffer, "========", 8) == 0 || strcmp(buffer, "LOBBY") == 0) {
                break; // Return to lobby menu
            }
            
            // Check if accepted
            if (strncmp(buffer, "ACCEPTED:", 9) == 0) {
                // Continue reading until SESSION_START
                while (connection_receive(conn, buffer, BUFFER_SIZE)) {
                    printf("%s\n", buffer);
                    if (strcmp(buffer, "SESSION_START") == 0) {
                        return; // Move to game session
                    }
                }
            }
            
            // Check if rejected or error (returns to lobby menu)
            if (strncmp(buffer, "REJECTED:", 9) == 0 || 
                strncmp(buffer, "ERROR:", 6) == 0) {
                printf("\n"); // Add spacing before next menu
                break; // Return to lobby menu
            }
        }
    }
}

void handle_game_session(Connection* conn, char* buffer) {
    printf("\n========================================\n");
    printf("Game Session Active\n");
    printf("========================================\n");
    
    // Read initial message from server (INFO:Connected...)
    if (connection_receive(conn, buffer, BUFFER_SIZE)) {
        printf("%s\n\n", buffer);
    }
    
    char input[BUFFER_SIZE];
    
    // ═══════════════════════════════════════════════════════════
    // TODO: GAME CLIENT LOGIC GOES HERE
    // ═══════════════════════════════════════════════════════════
    // This is a placeholder loop for testing
    // In the future, this will be replaced with:
    // - Raylib game window
    // - Input handling (keyboard/mouse)
    // - Rendering game state
    // - Receiving game updates from server
    // ═══════════════════════════════════════════════════════════
    
    while (conn->connected) {
        printf("> ");
        fflush(stdout);  // ← AGREGAR: Para que el prompt aparezca inmediatamente
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        // Send to server
        if (!connection_send(conn, input)) {
            printf("Error sending message\n");
            break;
        }
        
        // Receive response
        if (connection_receive(conn, buffer, BUFFER_SIZE)) {
            // Check for special messages
            if (strncmp(buffer, "PLAYER_DISCONNECTED:", 20) == 0) {
                printf("\n⚠️ %s\n", buffer);
                // Read info message
                if (connection_receive(conn, buffer, BUFFER_SIZE)) {
                    printf("%s\n", buffer);
                }
                break; // Connection will close
            } else if (strcmp(buffer, "BYE") == 0) {
                printf("Server closed connection\n");
                break;
            } else {
                printf("%s\n", buffer);
            }
        } else {
            printf("Connection lost\n");
            break;
        }
    }
}

int main() {
    print_header();
    
    // Connect to server
    Connection* conn = connection_create(SERVER_IP, SERVER_PORT);
    if (!conn) {
        return 1;
    }
    
    char buffer[BUFFER_SIZE];
    
    // Phase 1: Lobby (select client type)
    handle_lobby(conn, buffer);
    
    // Phase 2: Game session
    if (conn->connected) {
        handle_game_session(conn, buffer);
    }
    
    // Cleanup
    connection_close(conn);
    printf("\n✓ Disconnected\n");
    
    return 0;
}