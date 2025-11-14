#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 1000

bool renderer_init(const char* window_title, const char* assets_path);
void renderer_cleanup(void);

void renderer_begin_frame(void);
void renderer_end_frame(void);
bool renderer_should_close(void);

void renderer_draw_background(void);
void renderer_draw_text_centered(const char* text, int y, int font_size, Color color);

#endif