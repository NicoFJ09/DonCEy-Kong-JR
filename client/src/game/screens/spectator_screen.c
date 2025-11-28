#include "spectator_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../utils/message_listener.h"
#include "../../game/level.h"
#include "../../game/fruit.h"
#include "../../game/enemy.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// ============================================================
// SPECTATOR SCREEN
// Uses same listener/admin commands as player — simpler!
// ============================================================

bool show_spectator_screen(Connection* conn, int player_id, int client_id, char* kick_message, int kick_message_size) {
    printf("\n[SPECTATOR] Starting spectator mode\n");
    printf("[SPECTATOR] Watching Player #%d\n", player_id);
    printf("[SPECTATOR] Press Q to quit\n\n");

    bool voluntary_exit = false;
    bool running = true;
    
    // Create level from server map
    if (!conn->map_loaded || !conn->map_json) {
        printf("[SPECTATOR] ERROR: Map not loaded!\n");
        snprintf(kick_message, kick_message_size, "Map data missing");
        return false;
    }
    
    Level* level = level_create_from_json(conn->map_json);
    if (!level) {
        printf("[SPECTATOR] ERROR: Failed to create level from JSON\n");
        snprintf(kick_message, kick_message_size, "Failed to load map");
        return false;
    }
    printf("[SPECTATOR] Level created\n");

    // Expose this level to admin command processors (same as player)
    extern Level* g_current_level;
    Level* prev_level = g_current_level;
    g_current_level = level;
    
    // Start the normal listener (same as player uses)
    pthread_t listener = message_listener_start(conn);
    printf("[SPECTATOR] Message listener started\n");
    
    // Debug counter to show we're processing
    int frame_count = 0;
    
    // Main loop
    while (running && conn->connected && !WindowShouldClose()) {
        // Get deltaTime for animations
        float deltaTime = GetFrameTime();
        
        // Clamp deltaTime to prevent physics explosion
        if (deltaTime > MAX_DELTA_TIME) {
            deltaTime = MAX_DELTA_TIME;
        }
        
        // Debug: Print every 60 frames (1 second at 60fps)
        frame_count++;
        if (frame_count % 60 == 0) {
            int active_enemies = 0;
            int active_fruits = 0;
            for (int i = 0; i < level->max_enemies; i++) {
                if (level->enemies[i].active) active_enemies++;
            }
            for (int i = 0; i < level->max_fruits; i++) {
                if (!level->fruits[i].collected) active_fruits++;
            }
            printf("[SPECTATOR DEBUG] Frame %d: %d enemies, %d fruits active\n", 
                   frame_count, active_enemies, active_fruits);
        }
        
        // Process admin SPAWN/REMOVE commands from server (same as player)
        message_listener_process_admin_commands(level, conn);
        
        // Update fruit popups (same as player - for point animations)
        fruit_update_popups(level, deltaTime);
        
        // Update all enemies (same as player - for movement/animation)
        for (int i = 0; i < level->max_enemies; i++) {
            if (!level->enemies[i].active) continue;
            enemy_update(&level->enemies[i], deltaTime);
        }
    
        // Check for user input
        if (IsKeyPressed(KEY_Q)) {
            printf("[SPECTATOR] User pressed Q - exiting\n");
            voluntary_exit = true;
            running = false;
        }
        
        BeginDrawing();
            ClearBackground(UI_COLOR_BACKGROUND);
            
            // Draw client ID at top center
            font_manager_draw_client_id(client_id, "Spectator");
            
            const char* quit_text = "Press Q to quit";
            font_manager_draw_text(quit_text, 20, 50, UI_FONT_SIZE_ERROR, UI_COLOR_TEXT);
            
            char spectating_text[64];
            snprintf(spectating_text, sizeof(spectating_text), "Watching Player #%d", player_id);
            int text_width = font_manager_measure_text(spectating_text, UI_FONT_SIZE_ERROR);
            int text_x = UI_WINDOW_WIDTH - text_width - 20;
            font_manager_draw_text(spectating_text, text_x, 50, UI_FONT_SIZE_ERROR, UI_COLOR_SELECTED);
            
            // Render level (same as player): map, fruits, enemies
            level_render(level);
            fruit_render(level);
            
            // Render all active enemies
            for (int j = 0; j < level->max_enemies; j++) {
                if (level->enemies[j].active) {
                    enemy_render(&level->enemies[j]);
                }
            }
            
            // Render fruit popups (point animations)
            fruit_render_popups(level);
            
            // Show spectator info
            const char* spec_text = "SPECTATOR MODE - Watching Admin Spawns";
            int text_width2 = font_manager_measure_text(spec_text, UI_FONT_SIZE_SMALL);
            font_manager_draw_text(spec_text, (UI_WINDOW_WIDTH - text_width2) / 2, UI_WINDOW_HEIGHT - 30, UI_FONT_SIZE_SMALL, YELLOW);
        EndDrawing();
    }

    // Cleanup
    message_listener_stop(listener);
    level_destroy(level);
    g_current_level = prev_level;
    printf("[SPECTATOR] Cleanup complete\n");

    return voluntary_exit;
}