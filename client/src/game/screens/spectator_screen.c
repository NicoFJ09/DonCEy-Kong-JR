#include "spectator_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

bool show_spectator_screen(Connection* conn, int player_id, int client_id, char* kick_message, int kick_message_size) {
    printf("\n========================================\n");
    printf("Spectator Screen Active\n");
    printf("========================================\n");
    printf("Spectating Player #%d\n", player_id);
    printf("Press Q to quit\n\n");
    
    bool voluntary_exit = false;
    bool running = true;
    char buffer[512];
    
    while (running && conn->connected && !WindowShouldClose()) {
        // Check for user input
        if (IsKeyPressed(KEY_Q)) {
            printf("DEBUG: User pressed Q (voluntary exit)\n");
            voluntary_exit = true;
            running = false;
        }
        
        // Check for server messages (non-blocking)
        if (connection_has_data(conn)) {
            if (connection_receive(conn, buffer, sizeof(buffer))) {
                printf("DEBUG: Spectator received: %s\n", buffer);
                
                // Player left session - kicked back to lobby
                if (strncmp(buffer, PROTO_PLAYER_LEFT_SESSION, strlen(PROTO_PLAYER_LEFT_SESSION)) == 0) {
                    printf("DEBUG: Player left session - spectator kicked\n");
                    snprintf(kick_message, kick_message_size, "Player #%d returned to lobby", player_id);
                    voluntary_exit = false;
                    running = false;
                }
                // Player disconnected completely
                else if (strncmp(buffer, PROTO_PLAYER_DISCONNECTED, strlen(PROTO_PLAYER_DISCONNECTED)) == 0) {
                    printf("DEBUG: Player disconnected - spectator kicked\n");
                    snprintf(kick_message, kick_message_size, "Player #%d disconnected", player_id);
                    voluntary_exit = false;
                    running = false;
                }
            } else {
                // Connection lost
                printf("ERROR: Lost connection to server\n");
                snprintf(kick_message, kick_message_size, "Connection lost");
                voluntary_exit = false;
                running = false;
            }
        }
        
        BeginDrawing();
            ClearBackground(UI_COLOR_BACKGROUND);
            
            // Draw client ID at top center
            font_manager_draw_client_id(client_id, "Spectator");
            
            const char* quit_text = "Press Q to quit";
            font_manager_draw_text(quit_text, 20, 50, UI_FONT_SIZE_ERROR, UI_COLOR_TEXT);
            
            char spectating_text[64];
            snprintf(spectating_text, sizeof(spectating_text), "Watching Player #%d", player_id);
            int text_width = font_manager_measure_text(spectating_text, UI_FONT_SIZE_ERROR);
            int text_x = UI_WINDOW_WIDTH - text_width - 20;
            font_manager_draw_text(spectating_text, text_x, 50, UI_FONT_SIZE_ERROR, UI_COLOR_SELECTED);
            
            const char* placeholder = "Waiting for game state...";
            int placeholder_width = font_manager_measure_text(placeholder, UI_FONT_SIZE_NORMAL);
            int placeholder_x = (UI_WINDOW_WIDTH - placeholder_width) / 2;
            int placeholder_y = UI_WINDOW_HEIGHT / 2;
            font_manager_draw_text(placeholder, placeholder_x, placeholder_y, UI_FONT_SIZE_NORMAL, GRAY);
            
        EndDrawing();
    }
    
    return voluntary_exit;
}