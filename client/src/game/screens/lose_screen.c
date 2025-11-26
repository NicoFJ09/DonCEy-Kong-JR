#include "lose_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>

// ============================================================
// RENDERING
// ============================================================

static void draw_lose_screen(int client_id) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
    // Draw client ID at top center
    font_manager_draw_client_id(client_id, "Player");
    
    // Title: "YOU LOST"
    const char* title = "YOU LOST";
    int title_width = font_manager_measure_text(title, UI_FONT_SIZE_TITLE);
    int title_x = (UI_WINDOW_WIDTH - title_width) / 2;
    int title_y = 300;
    font_manager_draw_text(title, title_x, title_y, UI_FONT_SIZE_TITLE, UI_COLOR_TEXT);
    
    // Single option: "Return to Title"
    const char* option = "Return to Title";
    int text_width = font_manager_measure_text(option, UI_FONT_SIZE_NORMAL);
    int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
    int text_y = 450;
    
    // Always yellow (selected)
    font_manager_draw_text(option, text_x, text_y, UI_FONT_SIZE_NORMAL, UI_COLOR_SELECTED);
}

// ============================================================
// LOSE SCREEN
// ============================================================

LoseOption show_lose_screen(int client_id) {
    printf("\n========================================\n");
    printf("Game Over - Press ENTER to return\n");
    printf("========================================\n");
    
    // Clear all pending key presses
    while (GetKeyPressed() != 0) {
        // Consume all queued key presses
    }
    
    bool done = false;
    
    while (!done && !WindowShouldClose()) {
        // Wait for ENTER
        if (IsKeyPressed(KEY_ENTER)) {
            done = true;
        }
        
        // Render
        BeginDrawing();
            draw_lose_screen(client_id);
        EndDrawing();
    }
    
    printf("DEBUG: Returning to title\n");
    return LOSE_RETURN_TITLE;
}