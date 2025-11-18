#include "renderer.h"
#include "sprite_manager.h"
#include <stdio.h>

static bool initialized = false;

bool renderer_init(const char* window_title, const char* assets_path) {
    if (initialized) {
        printf("Warning: Renderer already initialized\n");
        return true;
    }
    
    printf("========================================\n");
    printf("Initializing renderer...\n");
    printf("  Window: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("  Title: %s\n", window_title);
    printf("========================================\n");
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, window_title);
    SetTargetFPS(60);
    
    if (!sprite_manager_init(assets_path)) {
        printf("Warning: Some sprites failed to load\n");
    }
    
    initialized = true;
    printf("✓ Renderer initialized\n\n");
    return true;
}

void renderer_cleanup(void) {
    if (!initialized) return;
    
    printf("\nCleaning up renderer...\n");
    sprite_manager_cleanup();
    CloseWindow();
    initialized = false;
    printf("✓ Renderer cleanup complete\n");
}

bool renderer_should_close(void) {
    return WindowShouldClose();
}