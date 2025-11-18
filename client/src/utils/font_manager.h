#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include "raylib.h"
#include <stdbool.h>

/**
 * Initialize font manager and load all fonts
 * Call this once at startup
 */
void font_manager_init(void);

/**
 * Get the PressStart2P font
 * Returns default font if not loaded
 */
Font font_manager_get_press_start(void);

/**
 * Check if custom fonts are loaded
 */
bool font_manager_is_loaded(void);

/**
 * Cleanup fonts (called automatically by renderer_cleanup)
 */
void font_manager_cleanup(void);

/**
 * Helper: Draw text with PressStart2P font or fallback
 */
void font_manager_draw_text(const char* text, int x, int y, int font_size, Color color);

/**
 * Helper: Measure text width with PressStart2P font or fallback
 */
int font_manager_measure_text(const char* text, int font_size);

#endif
