#include "title_screen.h"
#include "../../utils/constants.h"
#include "../../rendering/sprite_manager.h"
#include "raylib.h"
#include <stdio.h>

// Font reference (loaded from assets)
static Font press_start_font;
static bool font_loaded = false;

static void load_font_if_needed(void) {
    if (!font_loaded) {
        press_start_font = LoadFont("assets/ui/fonts/PressStart2P.ttf");
        if (press_start_font.texture.id != 0) {
            font_loaded = true;
        }
    }
}

static void draw_text_ex(const char* text, int x, int y, int font_size, Color color) {
    if (font_loaded) {
        DrawTextEx(press_start_font, text, (Vector2){x, y}, font_size, 2, color);
    } else {
        DrawText(text, x, y, font_size, color);
    }
}

static int measure_text_ex(const char* text, int font_size) {
    if (font_loaded) {
        Vector2 size = MeasureTextEx(press_start_font, text, font_size, 2);
        return (int)size.x;
    }
    return MeasureText(text, font_size);
}

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
        
        int text_width = measure_text_ex(options[i], UI_FONT_SIZE_NORMAL);
        int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
        int text_y = start_y + (i * spacing);
        
        draw_text_ex(options[i], text_x, text_y, UI_FONT_SIZE_NORMAL, color);
    }
}

MenuOption show_title_screen(void) {
    load_font_if_needed();
    
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