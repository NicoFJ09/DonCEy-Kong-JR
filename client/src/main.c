#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")  // Link Winsock library
    #define STDIN_FILENO 0
#else
    #include <unistd.h>
    #include <sys/select.h>
#endif

#include "network/connection.h"
#include "utils/constants.h"

/**
 * main.c - Client entry point
 * 
 * Flow:
 * 1. Connect to server
 * 2. Lobby: Select client type (player/spectator)
 * 3. Game Session: Placeholder echo loop
 * 4. Disconnect and cleanup
 */

// === Display Functions ===

void print_header() {
    printf("========================================\n");
    printf("🎮 DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

/**
 * Display lobby menu from server
 * Reads until prompt ("> ") is received
 * @param conn Connection pointer
 * @param buffer Buffer for receiving data
 */
void display_lobby_menu(Connection* conn, char* buffer) {
    while (connection_receive(conn, buffer, BUFFER_SIZE)) {
        // Check if this is the prompt line
        if (strstr(buffer, "> ") != NULL) {
            printf("%s", buffer);
            fflush(stdout);  // Force display without newline
            break;
        }
        printf("%s\n", buffer);
    }
}

// === Lobby Phase ===

/**
 * Handle user input for lobby selection
 * Sends selection to server and checks for exit
 * @param conn Connection pointer
 * @param buffer Buffer for communication
 * @return true to continue, false to exit
 */
bool handle_lobby_selection(Connection* conn, char* buffer) {
    char input[BUFFER_SIZE];
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return false;
    }
    
    input[strcspn(input, "\n")] = 0;  // Strip newline
    
    if (!connection_send(conn, input)) {
        printf("Error sending selection\n");
        return false;
    }
    
    // Check if user wants to exit
    if (strcmp(input, TYPE_EXIT) == 0) {
        connection_receive(conn, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);
        conn->connected = false;
        return false;
    }
    
    return true;
}

/**
 * Handle lobby interactions
 * Loop until client is accepted as player/spectator or disconnects
 * @param conn Connection pointer
 * @param buffer Buffer for communication
 */
void handle_lobby(Connection* conn, char* buffer) {
    while (conn->connected) {
        // Display menu
        display_lobby_menu(conn, buffer);
        
        // Get user selection
        if (!handle_lobby_selection(conn, buffer)) {
            break;
        }
        
        // Process server response
        while (connection_receive(conn, buffer, BUFFER_SIZE)) {
            // Handle spectator player selection prompt
            if (strstr(buffer, "Enter player ID") != NULL) {
                printf("%s", buffer);
                fflush(stdout);
                
                // Get player ID input
                char input[BUFFER_SIZE];
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    input[strcspn(input, "\n")] = 0;
                    connection_send(conn, input);
                }
                continue;
            }
            
            printf("%s\n", buffer);
            
            // Check if returned to lobby (user typed 'back')
            if (strncmp(buffer, "========", 8) == 0 || strcmp(buffer, "LOBBY") == 0) {
                break;
            }
            
            // Check if accepted - read until SESSION_START
            if (strncmp(buffer, "ACCEPTED:", 9) == 0) {
                while (connection_receive(conn, buffer, BUFFER_SIZE)) {
                    printf("%s\n", buffer);
                    if (strcmp(buffer, "SESSION_START") == 0) {
                        return;  // Move to game session
                    }
                }
            }
            
            // Check if rejected or error - return to lobby
            if (strncmp(buffer, "REJECTED:", 9) == 0 || 
                strncmp(buffer, "ERROR:", 6) == 0) {
                printf("\n");
                break;
            }
        }
    }
}

// === Game Session Phase ===

/**
 * Handle game session
 * Uses select() to monitor both socket and stdin simultaneously
 * This allows immediate response to server messages (like player disconnect or game frame updates in the future)
 * 
 * TODO: Replace with Raylib game window:
 * - Initialize Raylib window
 * - Input handling (keyboard/mouse)
 * - Render game state
 * - Receive game updates from server
 * 
 * @param conn Connection pointer
 * @param buffer Buffer for communication
 */

void handle_game_session(Connection* conn, char* buffer) {
    printf("\n========================================\n");
    printf("Game Session Active\n");
    printf("========================================\n");
    
    if (connection_receive(conn, buffer, BUFFER_SIZE)) {
        printf("%s\n\n", buffer);
    }
    
    char input[BUFFER_SIZE];
    bool should_exit = false;
    
    while (conn->connected && !should_exit) {
        printf("> ");
        fflush(stdout);
        
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(conn->socket_fd, &read_fds);
        
        int max_fd = (conn->socket_fd > STDIN_FILENO) ? conn->socket_fd : STDIN_FILENO;
        
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        
        if (activity < 0) {
            printf("Error in select()\n");
            break;
        }
        
        if (FD_ISSET(conn->socket_fd, &read_fds)) {
            if (connection_receive(conn, buffer, BUFFER_SIZE)) {
                if (strncmp(buffer, "PLAYER_DISCONNECTED:", 20) == 0) {
                    printf("\n %s\n", buffer);
                    if (connection_receive(conn, buffer, BUFFER_SIZE)) {
                        printf("%s\n", buffer);
                    }
                    should_exit = true;
                    continue;
                }
                
                if (strcmp(buffer, "BYE") == 0) {
                    printf("\nServer closed connection\n");
                    should_exit = true;
                    continue;
                }
                
                printf("\n%s\n", buffer);
            } else {
                printf("\nConnection lost\n");
                should_exit = true;
            }
        }
        
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(input, sizeof(input), stdin) == NULL) {
                should_exit = true;
                continue;
            }
            
            input[strcspn(input, "\n")] = 0;
            
            if (!connection_send(conn, input)) {
                printf("Error sending message\n");
                should_exit = true;
            }
        }
    }
}

// === Entry Point ===

int main() {
    print_header();
    
    // Phase 1: Connect to server
    Connection* conn = connection_create(SERVER_IP, SERVER_PORT);
    if (!conn) {
        return 1;
    }
    
    char buffer[BUFFER_SIZE];
    
    // Phase 2: Lobby selection
    handle_lobby(conn, buffer);
    
    // Phase 3: Game session
    if (conn->connected) {
        handle_game_session(conn, buffer);
    }
    
    // Phase 4: Cleanup
    connection_close(conn);
    printf("\n✓ Disconnected\n");
    
    return 0;
}