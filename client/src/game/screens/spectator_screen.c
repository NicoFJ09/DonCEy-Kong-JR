#include "spectator_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../utils/message_listener.h"
#include "../../game/level.h"
#include "../../game/fruit.h"
#include "../../game/enemy.h"
#include "../../game/player.h"  // For player rendering
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// ============================================================
// SPECTATOR SCREEN
// Uses same listener/admin commands as player — simpler!
// ============================================================

// Shared player state (updated by message listener thread)
static Player g_spectator_player;
static bool g_player_state_received = false;
static volatile bool g_spectator_should_exit = false;  // Set when player leaves/disconnects
static char g_spectator_exit_reason[256] = "";

// Message callback to handle player state updates and game events
static void spectator_message_callback(const char* message, void* user_data) {
    (void)user_data;
    
    // PLAYER_STATE:x:y:vx:vy:state:dir:climbing:vineId:lateralPos:frame
    if (strncmp(message, PLAYER_STATE_UPDATE, strlen(PLAYER_STATE_UPDATE)) == 0) {
        const char* state_data = message + strlen(PLAYER_STATE_UPDATE);
        player_deserialize_state(&g_spectator_player, state_data);
        g_player_state_received = true;
        return;
    }
    
    // PLAYER_LEFT_SESSION:playerId - Player exited to menu
    if (strncmp(message, PROTO_PLAYER_LEFT_SESSION, strlen(PROTO_PLAYER_LEFT_SESSION)) == 0) {
        printf("[SPECTATOR] Player left session - returning to menu\n");
        snprintf(g_spectator_exit_reason, sizeof(g_spectator_exit_reason), 
                 "Player returned to menu");
        g_spectator_should_exit = true;
        return;
    }
    
    // PLAYER_DISCONNECTED:playerId - Player disconnected completely
    if (strncmp(message, PROTO_PLAYER_DISCONNECTED, strlen(PROTO_PLAYER_DISCONNECTED)) == 0) {
        printf("[SPECTATOR] Player disconnected - returning to menu\n");
        snprintf(g_spectator_exit_reason, sizeof(g_spectator_exit_reason), 
                 "Player disconnected");
        g_spectator_should_exit = true;
        return;
    }
    
    // SERVER_SHUTDOWN - Server closed
    if (strcmp(message, PROTO_SERVER_SHUTDOWN) == 0) {
        printf("[SPECTATOR] Server shutdown - disconnecting\n");
        snprintf(g_spectator_exit_reason, sizeof(g_spectator_exit_reason), 
                 "Server shutdown");
        g_spectator_should_exit = true;
        return;
    }
    
    // GAME_OVER:score - Player got game over (optional: show notification)
    if (strncmp(message, UPDATE_GAME_OVER, strlen(UPDATE_GAME_OVER)) == 0) {
        printf("[SPECTATOR] Player got GAME OVER\n");
        // Don't exit - player might continue playing
        return;
    }
}

bool show_spectator_screen(Connection* conn, int player_id, int client_id, char* kick_message, int kick_message_size) {
    printf("\n[SPECTATOR] Starting spectator mode\n");
    printf("[SPECTATOR] Watching Player #%d\n", player_id);
    printf("[SPECTATOR] Press Q to quit\n\n");

    bool voluntary_exit = false;
    bool running = true;
    
    // Reset spectator exit flags
    g_spectator_should_exit = false;
    g_spectator_exit_reason[0] = '\0';
    
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
    
    // Initialize spectator player (for rendering)
    player_init(&g_spectator_player, 0, 0);
    g_player_state_received = false;
    
    // Start the normal listener (same as player uses)
    pthread_t listener = message_listener_start(conn);
    printf("[SPECTATOR] Message listener started\n");
    
    // Set callback to receive player state updates
    message_listener_set_callback(spectator_message_callback, NULL);
    
    // Debug counter to show we're processing
    int frame_count = 0;
    
    // Main loop
    while (running && conn->connected && !WindowShouldClose()) {
        // Check if we should exit due to player disconnect/leaving
        if (g_spectator_should_exit) {
            printf("[SPECTATOR] Exiting: %s\n", g_spectator_exit_reason);
            if (kick_message && kick_message_size > 0) {
                snprintf(kick_message, kick_message_size, "%s", g_spectator_exit_reason);
            }
            running = false;
            break;
        }
        
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
            
            // Render player if we've received state
            if (g_player_state_received) {
                player_render(&g_spectator_player);
            }
            
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
    message_listener_set_callback(NULL, NULL);  // Clear callback
    message_listener_stop(listener);
    level_destroy(level);
    g_current_level = prev_level;
    g_player_state_received = false;
    g_spectator_should_exit = false;
    printf("[SPECTATOR] Cleanup complete\n");

    return voluntary_exit;
}