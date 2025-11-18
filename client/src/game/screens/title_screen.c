#include "title_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../rendering/sprite_manager.h"
#include "raylib.h"
#include <stdio.h>

static void draw_title_screen(MenuOption selected, int client_id, const char* error_message) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
    // Draw client ID at top center
    font_manager_draw_client_id(client_id, "Client");
    
    // Draw error message if present
    if (error_message && error_message[0] != '\0') {
        int error_width = font_manager_measure_text(error_message, UI_FONT_SIZE_ERROR);
        int error_x = (UI_WINDOW_WIDTH - error_width) / 2;
        font_manager_draw_text(error_message, error_x, 700, UI_FONT_SIZE_ERROR, UI_COLOR_ERROR);
    }
    
    // Draw logo (centered at top)
    SpriteSheet* logo_sprite = sprite_manager_get(SPRITE_LOGO);
    if (logo_sprite && logo_sprite->loaded) {
        float logo_scale = 2.0f;
        int logo_width = (int)(logo_sprite->frame_width * logo_scale);
        int logo_x = (UI_WINDOW_WIDTH - logo_width) / 2;
        int logo_y = 150;
        sprite_manager_draw_frame(SPRITE_LOGO, 0, logo_x, logo_y, logo_scale, WHITE);
    }
    
    // Menu options (centered vertically below logo)
    const char* options[] = {
        "PLAY",
        "SPECTATE",
        "EXIT"
    };
    
    int start_y = 450;
    int spacing = 60;
    
    for (MenuOption i = MENU_PLAY; i <= MENU_EXIT; i++) {
        // Selected option = yellow, unselected = white
        Color color = (selected == i) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
        
        int text_width = font_manager_measure_text(options[i], UI_FONT_SIZE_NORMAL);
        int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
        int text_y = start_y + (i * spacing);
        
        font_manager_draw_text(options[i], text_x, text_y, UI_FONT_SIZE_NORMAL, color);
    }
}

MenuOption show_title_screen(int client_id, const char* error_message) {
    MenuOption selected = MENU_PLAY;
    bool done = false;
    
    while (!done && !WindowShouldClose()) {
        // Handle input
        if (IsKeyPressed(KEY_DOWN)) {
            selected = (selected + 1) % 3;
        }
        
        if (IsKeyPressed(KEY_UP)) {
            selected = (selected - 1 + 3) % 3;
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            done = true;
        }
        
        // Render
        BeginDrawing();
            draw_title_screen(selected, client_id, error_message);
        EndDrawing();
    }
    
    return selected;
}