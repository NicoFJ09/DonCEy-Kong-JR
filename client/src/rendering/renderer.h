#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include <stdbool.h>

// Use UI constants from utils/constants.h
// Both multiples of 24 for grid alignment
#define SCREEN_WIDTH 1200   // 50 blocks * 24px
#define SCREEN_HEIGHT 912   // 38 blocks * 24px

/**
 * Initialize renderer and window
 * Also initializes sprite manager
 */
bool renderer_init(const char* window_title, const char* assets_path);

/**
 * Cleanup renderer and all resources
 */
void renderer_cleanup(void);

/**
 * Check if window should close
 */
bool renderer_should_close(void);

#endif