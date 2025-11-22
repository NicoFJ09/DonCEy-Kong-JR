#include "player_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../level.h"
#include "../player.h"
#include "raylib.h"
#include <stdio.h>

#define PLAYER_TIMER_SECONDS 999999  // Infinite for testing

// Global level reference for player collision
Level* g_current_level = NULL;

void show_player_screen(int client_id) {
#if DEBUG_MODE
    printf("\n========================================\n");
    printf("Player Screen Active - TESTING MODE\n");
    printf("========================================\n");
    printf("Controls:\n");
    printf("  LEFT/RIGHT: Move\n");
    printf("  UP/DOWN: Climb (when on vine)\n");
    printf("  SPACE: Jump/Grab Vine\n");
    printf("  ESC: Exit\n\n");
#endif

    // Create test level
    g_current_level = level_create();

    // Create player - start on platform center
    Player player;
    player_init(&player, PLAYER_SPAWN_X, PLAYER_SPAWN_Y);

    // Game loop timer
    double start_time = GetTime();
    double elapsed = 0.0;

    while (elapsed < PLAYER_TIMER_SECONDS && !WindowShouldClose()) {
        elapsed = GetTime() - start_time;
        float deltaTime = GetFrameTime();

        // Handle input
        player_handle_input(&player);

        // Update physics and animation
        player_update(&player, deltaTime);

        // Render
        BeginDrawing();
            ClearBackground((Color){135, 206, 235, 255}); // Sky blue

            // Render level (background)
            level_render(g_current_level);

            // Render player (foreground)
            player_render(&player);

            // UI overlay
            font_manager_draw_client_id(client_id, "Player");

        EndDrawing();
    }

    // Cleanup
    level_destroy(g_current_level);
    g_current_level = NULL;

#if DEBUG_MODE
    printf("Test session ended\n");
#endif
}