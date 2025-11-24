#include "player_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../level.h"
#include "../player.h"
#include "../enemy.h"
#include "raylib.h"
#include <stdio.h>

// ============================================================
// GLOBALS
// ============================================================

Level* g_current_level = NULL;

// ============================================================
// PLAYER SCREEN
// ============================================================

void show_player_screen(int client_id, Connection* conn) {
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

    // Create level from server JSON (REQUIRED)
    if (!conn || !conn->map_loaded || !conn->map_json) {
        printf("FATAL ERROR: No map data from server\n");
        return;
    }

    g_current_level = level_create_from_json(conn->map_json);
    if (!g_current_level) {
        printf("FATAL ERROR: Failed to parse server map JSON\n");
        return;
    }

    // Create player - start on platform center (convert blocks to pixels)
    Player player;
    player_init(&player, 
                PLAYER_SPAWN_X_BLOCK * PLATFORM_BLOCK_SIZE, 
                PLAYER_SPAWN_Y_BLOCK * PLATFORM_BLOCK_SIZE);

    // Main game loop - runs until window closes or ESC pressed
    while (!WindowShouldClose()) {
        // PAUSE GAME when window loses focus (minimized, alt-tabbed, etc.)
        if (IsWindowFocused()) {
            // Window has focus - update game normally
            float deltaTime = GetFrameTime();
            
            // CRITICAL FIX: Clamp deltaTime to prevent physics explosion
            // If somehow a large deltaTime gets through, limit it
            if (deltaTime > MAX_DELTA_TIME) {
                deltaTime = MAX_DELTA_TIME;
            }

            // Handle input
            player_handle_input(&player);

            // Update physics and animation
            player_update(&player, deltaTime);
            
            // Update all enemies
            for (int i = 0; i < g_current_level->enemy_count; i++) {
                enemy_update(&g_current_level->enemies[i], deltaTime);
                
                // Check collision with player
                if (enemy_collides_with_player(&g_current_level->enemies[i], player.x, player.y)) {
                    // TODO: Handle player death/respawn
                    printf("COLLISION! Player(%.0f,%.0f) Enemy[%d](%.0f,%.0f)\n", 
                           player.x, player.y, i, 
                           g_current_level->enemies[i].x, 
                           g_current_level->enemies[i].y);
                }
            }
        }
        // If window NOT focused: do nothing - game is PAUSED

        // ALWAYS render (even when paused) to keep window responsive
        BeginDrawing();
            ClearBackground(BLACK);

            // Render level (background)
            level_render(g_current_level);

            // Render enemies
            for (int i = 0; i < g_current_level->enemy_count; i++) {
                if (g_current_level->enemies[i].active) {
                    enemy_render(&g_current_level->enemies[i]);
                }
            }

            // Render player (foreground)
            player_render(&player);

            // UI overlay
            font_manager_draw_client_id(client_id, "Player");
            
            // Show pause overlay when window is unfocused
            if (!IsWindowFocused()) {
                // Semi-transparent black overlay
                DrawRectangle(0, 0, UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT, (Color){0, 0, 0, 128});
                
                // "PAUSED" text centered using font manager
                const char* pause_text = "PAUSED";
                int text_width = font_manager_measure_text(pause_text, 60);
                font_manager_draw_text(pause_text, (UI_WINDOW_WIDTH - text_width) / 2, UI_WINDOW_HEIGHT / 2 - 30, 60, WHITE);
            }

        EndDrawing();
    }

    // Cleanup
    level_destroy(g_current_level);
    g_current_level = NULL;

#if DEBUG_MODE
    printf("Test session ended\n");
#endif
}