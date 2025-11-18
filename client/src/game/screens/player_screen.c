#include "player_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>

#define PLAYER_TIMER_SECONDS 10

void show_player_screen(int client_id) {
    printf("\n========================================\n");
    printf("Player Screen Active\n");
    printf("========================================\n");
    printf("Timer: %d seconds (hidden from player)\n\n", PLAYER_TIMER_SECONDS);
    
    // Start timer
    double start_time = GetTime();
    double elapsed = 0.0;
    
    while (elapsed < PLAYER_TIMER_SECONDS && !WindowShouldClose()) {
        elapsed = GetTime() - start_time;
        
        // Render
        BeginDrawing();
            ClearBackground(UI_COLOR_BACKGROUND);
            
            // Draw client ID at top center
            font_manager_draw_client_id(client_id, "Player");
        EndDrawing();
    }
    
    printf("DEBUG: Player lost after 10 seconds\n");
}