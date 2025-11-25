#include "game_flow.h"
#include "screens/ip_input_screen.h"
#include "screens/title_screen.h"
#include "screens/player_screen.h"
#include "screens/lose_screen.h"
#include "screens/player_selection_screen.h"
#include "screens/spectator_screen.h"
#include "../network/connection.h"
#include "../utils/constants.h"
#include "../utils/font_manager.h"
#include "../utils/message_listener.h"
#include "../rendering/sprite_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================
// MAIN GAME FLOW
// ============================================================

bool game_flow_run(void) {
    font_manager_init();

    char server_ip[256];
    Connection* conn = NULL;
    
    bool got_ip = show_ip_input_screen(server_ip, sizeof(server_ip), false);
    
    if (!got_ip) {
        printf("User closed window during IP input\n");
        return false;
    }
    
    if (strcmp(server_ip, "7357") == 0) {
        printf("Entering sprite test mode...\n");
        sprite_test_run();
        return true;
    }
    
    while (!conn) {
        printf("\nAttempting connection to %s:%d...\n", server_ip, SERVER_PORT);
        conn = connection_create(server_ip, SERVER_PORT);
        
        if (!conn) {
            printf("Connection failed!\n");
            
            got_ip = show_ip_input_screen(server_ip, sizeof(server_ip), true);
            
            if (!got_ip) {
                printf("User closed window\n");
                return false;
            }
            
            if (strcmp(server_ip, "7357") == 0) {
                printf("Entering sprite test mode...\n");
                sprite_test_run();
                return true;
            }
        }
    }
    
    printf("✓ Connected successfully!\n");
    
    // Read initial CLIENT_ID from server
    char buffer[BUFFER_SIZE];
    if (connection_receive(conn, buffer, BUFFER_SIZE)) {
        printf("DEBUG: Received initial message: %s\n", buffer);
        if (strncmp(buffer, "CLIENT_ID:", 10) == 0) {
            conn->client_id = atoi(buffer + 10);
            printf("✓ Assigned Client ID: %d\n\n", conn->client_id);
        }
    }
    
    bool game_running = true;
    
    while (game_running && !WindowShouldClose()) {
        bool running = true;
        char error_message[256] = {0};
        bool show_error = false;
        double error_display_time = 0.0;
        const double ERROR_DISPLAY_DURATION = 3.0; // Show errors for 3 seconds
        
        // Request cooldown to prevent server spam
        static double last_join_attempt = 0.0;
        const double JOIN_COOLDOWN = 1.0; // 1 second between join attempts
    
        while (running && conn->connected && !WindowShouldClose()) {
        // Clear error after timeout
        if (show_error && GetTime() - error_display_time >= ERROR_DISPLAY_DURATION) {
            show_error = false;
            error_message[0] = '\0';
        }
        
        MenuOption selected = show_title_screen(conn->client_id, show_error ? error_message : NULL);
        
        switch (selected) {
            case MENU_PLAY: {
                // Check cooldown to prevent request spam
                double current_time = GetTime();
                if (current_time - last_join_attempt < JOIN_COOLDOWN) {
                    printf("DEBUG: Join attempt on cooldown (%.1fs remaining)\n", 
                           JOIN_COOLDOWN - (current_time - last_join_attempt));
                    snprintf(error_message, sizeof(error_message), "Please wait %.0fs", 
                             JOIN_COOLDOWN - (current_time - last_join_attempt) + 0.5);
                    show_error = true;
                    error_display_time = current_time;
                    break;
                }
                last_join_attempt = current_time;
                
                printf("DEBUG: Selected Play\n");
                printf("DEBUG: Sending join request: %s\n", CMD_JOIN_PLAYER);
                
                if (!connection_send(conn, CMD_JOIN_PLAYER)) {
                    printf("ERROR: Failed to send join request\n");
                    break;
                }
                
                char buffer[BUFFER_SIZE];
                if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 5.0)) {
                    printf("ERROR: Timeout waiting for join response\n");
                    snprintf(error_message, sizeof(error_message), "Server timeout - try again");
                    show_error = true;
                    error_display_time = GetTime();
                    break;
                }
                
                printf("DEBUG: Received: %s\n", buffer);
                
                if (strncmp(buffer, PROTO_ACCEPTED, strlen(PROTO_ACCEPTED)) == 0) {
                    printf("✓ Accepted as PLAYER\n");
                    
                    // Read CLIENT_ID (may be redundant if already set)
                    if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 3.0)) {
                        printf("ERROR: Timeout receiving CLIENT_ID\n");
                        break;
                    }
                    printf("DEBUG: Received: %s\n", buffer);
                    if (strncmp(buffer, "CLIENT_ID:", 10) == 0) {
                        conn->client_id = atoi(buffer + 10);
                        printf("DEBUG: Client ID updated to %d\n", conn->client_id);
                    }
                    
                    // Read SESSION_START
                    if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 3.0)) {
                        printf("ERROR: Timeout receiving SESSION_START\n");
                        break;
                    }
                    printf("DEBUG: Received: %s\n", buffer);
                    if (strncmp(buffer, "SESSION_START", 13) != 0) {
                        printf("WARNING: Expected SESSION_START, got: %s\n", buffer);
                    }

                    // Read MAP_DATA
                    char map_buffer[MAP_JSON_BUFFER_SIZE];
                    if (!connection_receive_with_timeout(conn, map_buffer, MAP_JSON_BUFFER_SIZE, 5.0)) {
                        printf("ERROR: Timeout receiving MAP_DATA\n");
                        break;
                    }
                    printf("DEBUG: Received: %.50s...\n", map_buffer);
                    if (strncmp(map_buffer, PROTO_MAP_DATA, strlen(PROTO_MAP_DATA)) != 0) {
                        printf("ERROR: Expected MAP_DATA, got: %s\n", map_buffer);
                        break;
                    }
                    // Extract JSON and store in connection
                    const char* json_data = map_buffer + strlen(PROTO_MAP_DATA);
                    conn->map_json = strdup(json_data);
                    conn->map_loaded = true;
                    printf("✓ Map data loaded (%zu bytes)\n", strlen(conn->map_json));

                    pthread_t listener_thread = message_listener_start(conn);

                    // Player screen loop
                    bool playing = true;
                    while (playing && conn->connected && !WindowShouldClose()) {
                        show_player_screen(conn->client_id, conn);
                        
                        LoseOption choice = show_lose_screen(conn->client_id);
                        
                        switch (choice) {
                            case LOSE_PLAY_AGAIN:
                                printf("DEBUG: Playing again (continuing in session)\n");
                                // Just continue the loop - stay in player session
                                break;
                                
                            case LOSE_RETURN_TITLE:
                                printf("DEBUG: Returning to title - sending DISCONNECT to exit session\n");
                                if (!connection_send_immediate(conn, CMD_DISCONNECT)) {
                                    printf("ERROR: Failed to send DISCONNECT\n");
                                }
                                printf("DEBUG: Ready to return to title screen\n");
                                
                                playing = false;
                                break;
                        }
                    }
                    
                    message_listener_stop(listener_thread);
                    
                } else if (strncmp(buffer, PROTO_REJECTED, strlen(PROTO_REJECTED)) == 0) {
                    const char* reason = buffer + strlen(PROTO_REJECTED);
                    printf("✗ Rejected: %s\n", reason);
                    snprintf(error_message, sizeof(error_message), "Cannot join: %s", reason);
                    show_error = true;
                    error_display_time = GetTime();
                } else {
                    printf("ERROR: Unexpected response: %s\n", buffer);
                }
                break;
            }
                
            case MENU_SPECTATE: {
                printf("DEBUG: Selected Spectate\n");
                
                bool in_spectate = true;
                while (in_spectate && conn->connected && !WindowShouldClose()) {
                    // Pass error message if showing, otherwise NULL
                    const char* current_error = (show_error && (GetTime() - error_display_time < 3.0)) ? error_message : NULL;
                    int player_id = show_player_selection_screen(conn, conn->client_id, current_error);
                    
                    // Clear error after showing
                    if (show_error && (GetTime() - error_display_time >= 3.0)) {
                        show_error = false;
                    }
                    
                    if (player_id > 0) {
                        // Join as spectator
                        char join_msg[64];
                        snprintf(join_msg, sizeof(join_msg), "%s%d", CMD_JOIN_SPECTATOR_PREFIX, player_id);
                        
                        printf("DEBUG: Sending spectator join: %s\n", join_msg);
                        
                        if (!connection_send(conn, join_msg)) {
                            printf("ERROR: Failed to send spectator join request\n");
                            continue;
                        }
                        
                        char buffer[BUFFER_SIZE];
                        if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 5.0)) {
                            printf("ERROR: Timeout waiting for spectator join response\n");
                            snprintf(error_message, sizeof(error_message), "Server timeout - try again");
                            show_error = true;
                            error_display_time = GetTime();
                            continue;
                        }
                        
                        printf("DEBUG: Received: %s\n", buffer);
                        
                        if (strncmp(buffer, PROTO_ACCEPTED, strlen(PROTO_ACCEPTED)) == 0) {
                            printf("✓ Accepted as SPECTATOR for Player #%d\n", player_id);
                            
                            // Read CLIENT_ID (may be redundant if already set)
                            if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 3.0)) {
                                printf("ERROR: Timeout receiving CLIENT_ID\n");
                                continue;
                            }
                            printf("DEBUG: Received: %s\n", buffer);
                            if (strncmp(buffer, "CLIENT_ID:", 10) == 0) {
                                conn->client_id = atoi(buffer + 10);
                                printf("DEBUG: Client ID updated to %d\n", conn->client_id);
                            }
                            
                            // Read SESSION_START
                            if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 3.0)) {
                                printf("ERROR: Timeout receiving SESSION_START\n");
                                continue;
                            }
                            printf("DEBUG: Received: %s\n", buffer);
                            if (strncmp(buffer, "SESSION_START", 13) != 0) {
                                printf("WARNING: Expected SESSION_START, got: %s\n", buffer);
                            }

                            // Read MAP_DATA
                            char map_buffer[MAP_JSON_BUFFER_SIZE];
                            if (!connection_receive_with_timeout(conn, map_buffer, MAP_JSON_BUFFER_SIZE, 5.0)) {
                                printf("ERROR: Timeout receiving MAP_DATA\n");
                                continue;
                            }
                            printf("DEBUG: Received: %.50s...\n", map_buffer);
                            if (strncmp(map_buffer, PROTO_MAP_DATA, strlen(PROTO_MAP_DATA)) != 0) {
                                printf("ERROR: Expected MAP_DATA, got: %s\n", map_buffer);
                                continue;
                            }
                            // Extract JSON and store in connection
                            const char* json_data = map_buffer + strlen(PROTO_MAP_DATA);
                            conn->map_json = strdup(json_data);
                            conn->map_loaded = true;
                            printf("✓ Map data loaded (%zu bytes)\n", strlen(conn->map_json));

                            // Enter spectator screen with kick message buffer
                            char kick_message[256] = "";
                            bool voluntary = show_spectator_screen(conn, player_id, conn->client_id, kick_message, sizeof(kick_message));
                            
                            // Always send DISCONNECT to properly exit session (whether voluntary or kicked)
                            printf("DEBUG: Exiting spectator session - sending DISCONNECT\n");
                            if (!connection_send_immediate(conn, CMD_DISCONNECT)) {
                                printf("ERROR: Failed to send DISCONNECT\n");
                                break;
                            }
                            
                            if (voluntary) {
                                printf("DEBUG: Spectator left voluntarily\n");
                            } else {
                                printf("DEBUG: Spectator was kicked: %s\n", kick_message);
                                // Set error message to display in player selection
                                if (strlen(kick_message) > 0) {
                                    snprintf(error_message, sizeof(error_message), "%s", kick_message);
                                    show_error = true;
                                    error_display_time = GetTime();
                                }
                            }
                            
                            printf("DEBUG: Ready to return to player selection\n");
                            // Continue in spectate loop to allow selecting another player
                        } else if (strncmp(buffer, PROTO_REJECTED, strlen(PROTO_REJECTED)) == 0) {
                            const char* reason = buffer + strlen(PROTO_REJECTED);
                            printf("✗ Rejected: %s\n", reason);
                            snprintf(error_message, sizeof(error_message), "Cannot spectate: %s", reason);
                            show_error = true;
                            error_display_time = GetTime();
                            // Continue in loop - user can try another player or press R to return
                        } else {
                            printf("ERROR: Unexpected response: %s\n", buffer);
                            // Continue anyway - let user retry
                        }
                    } else if (player_id == -1) {
                        printf("DEBUG: Refresh completed\n");
                    } else {
                        printf("DEBUG: Return - going back to title\n");
                        in_spectate = false;
                    }
                }
                break;
            }
                
            case MENU_EXIT:
                printf("DEBUG: Selected Exit\n");
                connection_send_immediate(conn, CMD_EXIT);
                running = false;
                game_running = false; // Exit completely
                break;
        }
    }
    
    // Check if connection was lost (server disconnected)
    if (!conn->connected && game_running) {
        printf("\n⚠ Lost connection to server\n");
        connection_close(conn);
        conn = NULL;
        
        // Show IP input again with error message
        got_ip = show_ip_input_screen(server_ip, sizeof(server_ip), true);
        
        if (!got_ip) {
            printf("User closed window\n");
            game_running = false;
        } else {
            // Try to reconnect
            printf("\nAttempting reconnection to %s:%d...\n", server_ip, SERVER_PORT);
            conn = connection_create(server_ip, SERVER_PORT);
            
            if (!conn) {
                printf("Reconnection failed!\n");
                // Will loop back to show IP input again
            } else {
                printf("✓ Reconnected successfully!\n");
                
                // Read initial CLIENT_ID from server
                char buffer[BUFFER_SIZE];
                if (connection_receive(conn, buffer, BUFFER_SIZE)) {
                    printf("DEBUG: Received: %s\n", buffer);
                    if (strncmp(buffer, "CLIENT_ID:", 10) == 0) {
                        conn->client_id = atoi(buffer + 10);
                        printf("✓ Assigned Client ID: %d\n\n", conn->client_id);
                    }
                }
            }
        }
    } else {
        // Normal exit or window closed
        if (conn) {
            connection_close(conn);
        }
        game_running = false;
    }
}
    
    printf("\nExiting game\n");
    return true;
}
