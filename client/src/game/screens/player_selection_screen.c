#include "player_selection_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// Mock player data (Phase 4 - hardcoded)
typedef struct {
    int id;
    const char* ip;
    int spectator_count;
    int max_spectators;
} MockPlayer;

static MockPlayer mock_players[] = {
    {1, "192.168.1.100", 1, 2},
    {2, "192.168.1.105", 0, 2}
};
static const int mock_player_count = 2;

// Selection indices
#define SELECTION_RETURN (mock_player_count)
#define SELECTION_REFRESH (mock_player_count + 1)

static void draw_player_selection_screen(int selected_index) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
    // Title: "ONLINE PLAYERS"
    const char* title = "ONLINE PLAYERS";
    int title_width = font_manager_measure_text(title, UI_FONT_SIZE_TITLE);
    int title_x = (UI_WINDOW_WIDTH - title_width) / 2;
    int title_y = 100;
    font_manager_draw_text(title, title_x, title_y, UI_FONT_SIZE_TITLE, UI_COLOR_TEXT);
    
    // Player list (centered)
    int start_y = 300;
    int spacing = 60;
    
    for (int i = 0; i < mock_player_count; i++) {
        MockPlayer* player = &mock_players[i];
        
        // Format: "Player #1 - 192.168.1.100 [1/2 spectators]"
        char player_text[256];
        snprintf(player_text, sizeof(player_text), 
                 "Player #%d - %s [%d/%d spectators]",
                 player->id, player->ip, player->spectator_count, player->max_spectators);
        
        // Selected = yellow, unselected = white
        Color color = (selected_index == i) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
        
        int text_width = font_manager_measure_text(player_text, UI_FONT_SIZE_NORMAL);
        int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
        int text_y = start_y + (i * spacing);
        
        font_manager_draw_text(player_text, text_x, text_y, UI_FONT_SIZE_NORMAL, color);
    }
    
    // Bottom buttons: [Return] (left) and [Refresh] (right)
    int button_y = UI_WINDOW_HEIGHT - 120;
    
    // [Return] button (bottom left)
    const char* return_text = "[Return]";
    Color return_color = (selected_index == SELECTION_RETURN) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
    int return_x = 100;
    font_manager_draw_text(return_text, return_x, button_y, UI_FONT_SIZE_NORMAL, return_color);
    
    // [Refresh] button (bottom right)
    const char* refresh_text = "[Refresh]";
    Color refresh_color = (selected_index == SELECTION_REFRESH) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
    int refresh_width = font_manager_measure_text(refresh_text, UI_FONT_SIZE_NORMAL);
    int refresh_x = UI_WINDOW_WIDTH - refresh_width - 100;
    font_manager_draw_text(refresh_text, refresh_x, button_y, UI_FONT_SIZE_NORMAL, refresh_color);
}

int show_player_selection_screen(void) {
    printf("\n========================================\n");
    printf("Player Selection Screen Active\n");
    printf("========================================\n");
    printf("Mock data: %d players online\n", mock_player_count);
    printf("Use arrow keys to navigate\n");
    printf("Press ENTER to select\n");
    printf("Options: Players, [Return], [Refresh]\n\n");
    
    int selected_index = 0;
    int total_options = mock_player_count + 2; // players + return + refresh
    bool done = false;
    int result = 0;
    
    while (!done && !WindowShouldClose()) {
        // Handle input
        if (IsKeyPressed(KEY_DOWN)) {
            selected_index = (selected_index + 1) % total_options;
        }
        
        if (IsKeyPressed(KEY_UP)) {
            selected_index = (selected_index - 1 + total_options) % total_options;
        }
        
        // Left/Right to jump between Return and Refresh
        if (IsKeyPressed(KEY_LEFT)) {
            if (selected_index >= SELECTION_RETURN) {
                selected_index = SELECTION_RETURN;
            }
        }
        
        if (IsKeyPressed(KEY_RIGHT)) {
            if (selected_index >= SELECTION_RETURN) {
                selected_index = SELECTION_REFRESH;
            }
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (selected_index < mock_player_count) {
                // Selected a player
                result = mock_players[selected_index].id;
                printf("DEBUG: Selected Player #%d\n", result);
            } else if (selected_index == SELECTION_RETURN) {
                // Selected [Return]
                result = 0;
                printf("DEBUG: Return button pressed\n");
            } else if (selected_index == SELECTION_REFRESH) {
                // Selected [Refresh]
                result = -1;
                printf("DEBUG: Refresh button pressed\n");
            }
            done = true;
        }
        
        // Render
        BeginDrawing();
            draw_player_selection_screen(selected_index);
        EndDrawing();
    }
    
    printf("DEBUG: Exiting player_selection_screen, returning: %d\n", result);
    return result;
}