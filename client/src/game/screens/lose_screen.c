#include "lose_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>

static void draw_lose_screen(LoseOption selected, int client_id) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
    // Draw client ID at top center
    font_manager_draw_client_id(client_id, "Player");
    
    // Title: "YOU LOST"
    const char* title = "YOU LOST";
    int title_width = font_manager_measure_text(title, UI_FONT_SIZE_TITLE);
    int title_x = (UI_WINDOW_WIDTH - title_width) / 2;
    int title_y = 300;
    font_manager_draw_text(title, title_x, title_y, UI_FONT_SIZE_TITLE, UI_COLOR_TEXT);
    
    // Options (centered below title)
    const char* options[] = {
        "Play Again",
        "Return to Title"
    };
    
    int start_y = 450;
    int spacing = 60;
    
    for (LoseOption i = LOSE_PLAY_AGAIN; i <= LOSE_RETURN_TITLE; i++) {
        // Selected option = yellow, unselected = white
        Color color = (selected == i) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
        
        int text_width = font_manager_measure_text(options[i], UI_FONT_SIZE_NORMAL);
        int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
        int text_y = start_y + (i * spacing);
        
        font_manager_draw_text(options[i], text_x, text_y, UI_FONT_SIZE_NORMAL, color);
    }
}

LoseOption show_lose_screen(int client_id) {
    printf("\n========================================\n");
    printf("Lose Screen Active\n");
    printf("========================================\n");
    printf("Use arrow keys to navigate\n");
    printf("Press ENTER to select\n\n");
    
    LoseOption selected = LOSE_PLAY_AGAIN;
    bool done = false;
    
    while (!done && !WindowShouldClose()) {
        // Handle input
        if (IsKeyPressed(KEY_DOWN)) {
            selected = (selected + 1) % 2;
        }
        
        if (IsKeyPressed(KEY_UP)) {
            selected = (selected - 1 + 2) % 2;
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            done = true;
        }
        
        // Render
        BeginDrawing();
            draw_lose_screen(selected, client_id);
        EndDrawing();
    }
    
    // Debug output
    switch (selected) {
        case LOSE_PLAY_AGAIN:
            printf("DEBUG: Selected Play Again\n");
            break;
        case LOSE_RETURN_TITLE:
            printf("DEBUG: Selected Return to Title\n");
            break;
    }
    
    return selected;
}