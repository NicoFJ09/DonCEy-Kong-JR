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
        printf("%s\n", buffer);
        
        // Check if we reached the prompt
        if (strstr(buffer, "> ") != NULL) {
            break;
        }
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
            printf("%s\n", buffer);
            
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
            
            // Check for player selection prompt
            if (strstr(buffer, "Enter player ID") != NULL) {
                break; // Let user input player ID
            }
        }
    }
}

void handle_game_session(Connection* conn, char* buffer) {
    printf("\n========================================\n");
    printf("Game Session Active\n");
    printf("========================================\n");
    printf("Type 'exit' to disconnect\n\n");
    
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