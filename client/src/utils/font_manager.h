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

/**
 * Helper: Draw client ID at top center of screen
 * @param client_id Client ID to display (or -1 if not assigned)
 * @param role_name Role name to display ("Client", "Player", "Spectator")
 */
void font_manager_draw_client_id(int client_id, const char* role_name);

#endif
