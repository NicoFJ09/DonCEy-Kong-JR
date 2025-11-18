#include "title_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../rendering/sprite_manager.h"
#include "raylib.h"
#include <stdio.h>

static void draw_title_screen(MenuOption selected) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
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

MenuOption show_title_screen(void) {
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
            draw_title_screen(selected);
        EndDrawing();
    }
    
    return selected;
}