#include "font_manager.h"
#include <stdio.h>

static Font press_start_font;
static bool font_loaded = false;

void font_manager_init(void) {
    if (!font_loaded) {
        press_start_font = LoadFont("assets/ui/fonts/PressStart2P.ttf");
        if (press_start_font.texture.id != 0) {
            font_loaded = true;
            printf("✓ Loaded PressStart2P font\n");
        } else {
            printf("✗ Failed to load PressStart2P font, using default\n");
        }
    }
}

Font font_manager_get_press_start(void) {
    return press_start_font;
}

bool font_manager_is_loaded(void) {
    return font_loaded;
}

void font_manager_cleanup(void) {
    if (font_loaded) {
        UnloadFont(press_start_font);
        font_loaded = false;
    }
}

void font_manager_draw_text(const char* text, int x, int y, int font_size, Color color) {
    if (font_loaded) {
        DrawTextEx(press_start_font, text, (Vector2){x, y}, font_size, 2, color);
    } else {
        DrawText(text, x, y, font_size, color);
    }
}

int font_manager_measure_text(const char* text, int font_size) {
    if (font_loaded) {
        Vector2 size = MeasureTextEx(press_start_font, text, font_size, 2);
        return (int)size.x;
    }
    return MeasureText(text, font_size);
}

void font_manager_draw_client_id(int client_id, const char* role_name) {
    if (client_id > 0) {
        char id_text[64];
        snprintf(id_text, sizeof(id_text), "%s #%d", role_name, client_id);
        int text_width = font_manager_measure_text(id_text, 16);
        int text_x = (1200 - text_width) / 2;  // UI_WINDOW_WIDTH
        font_manager_draw_text(id_text, text_x, 20, 16, YELLOW);
    }
}

