#include "spectator_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../utils/message_listener.h"
#include "../../game/level.h"
#include "../../game/fruit.h"
#include "../../game/enemy.h"
#include "../../game/spectator_state.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

// Update entities for spectator
void update_level_from_snapshot(Level* level, GameStateSnapshot* snapshot) {
    printf("DEBUG: Updating from snapshot with %d entities\n", snapshot->entity_count);
    // Actualizar frutas
    for (int i = 0; i < snapshot->entity_count; i++) {
        EntitySnapshot* ent = &snapshot->entities[i];
        if (ent->type == ENTITY_FRUIT) {
            printf("DEBUG: Updating from snapshot with %d entities\n", snapshot->entity_count);
            // Buscar fruta por ID y actualizar su estado
            for (int j = 0; j < level->fruit_count; j++) {
                if (level->fruits[j].id == ent->id) {
                    level->fruits[j].collected = !ent->active; // Si no está activa, fue recolectada
                    level->fruits[j].x = ent->x;
                    level->fruits[j].y = ent->y;
                    printf("DEBUG: Updated enemy %d position\n", ent->id);    
                    break;
                }
            }
        }
        
        else if (ent->type == ENTITY_ENEMY_RED || ent->type == ENTITY_ENEMY_BLUE) {
            // Buscar enemigo por ID y actualizar posición
            for (int j = 0; j < level->enemy_count; j++) {
                // Update position and state
                level->enemies[j].x = ent->x;
                level->enemies[j].y = ent->y;
                level->enemies[j].state = ent->enemy_state;
                level->enemies[j].velocity_x = 0;
                level->enemies[j].velocity_y = 0;
            }
        }
    }
}


// ============================================================
// SPECTATOR SCREEN
// ============================================================

bool show_spectator_screen(Connection* conn, int player_id, int client_id, char* kick_message, int kick_message_size) {
    printf("\n========================================\n");
    printf("Spectator Screen Active\n");
    printf("========================================\n");
    printf("Spectating Player #%d\n", player_id);
    printf("Press Q to quit\n\n");

    bool voluntary_exit = false;
    bool running = true;
    GameStateSnapshot current_state = {0};
    bool has_state = false;
    
    // Create level
    if (!conn->map_loaded || !conn->map_json) {
        printf("ERROR: Map not loaded!\n");
        snprintf(kick_message, kick_message_size, "Map data missing");
        return false;
    }
    
    Level* level = level_create_from_json(conn->map_json);
    if (!level) {
        printf("ERROR: Failed to create level from JSON\n");
        snprintf(kick_message, kick_message_size, "Failed to load map");
        return false;
    }
    printf("Level created for spectator\n");


    // Enemies
    if (!level->enemies) {
        level->max_enemies = 20;  // Mismo valor que en player_screen
        level->enemy_count = 0;
        level->enemies = (Enemy*)calloc(level->max_enemies, sizeof(Enemy));
        if (!level->enemies) {
            printf("ERROR: Failed to allocate enemies array\n");
            level_destroy(level);
            return false;
        }
        
        // Inicializar todos como inactivos
        for (int i = 0; i < level->max_enemies; i++) {
            level->enemies[i].active = false;
        }
        printf("Initialized enemies array (max: %d)\n", level->max_enemies);
    }
    
    // Fruits (usa tu función existente)
    fruit_initialize(level);
    fruit_initialize_popups(level);

    // Crear buffer de estados
    StateBuffer* state_buffer = state_buffer_create();
    volatile bool listener_running = true;
    
    // Expose this level to admin command processors (they use g_current_level)
    extern Level* g_current_level;
    Level* prev_level = g_current_level;
    g_current_level = level;

    // Debug: show connection state before starting listener
    printf("DEBUG: Before listener - conn=%p, connected=%d\n", conn, conn ? conn->connected : 0);

    // Iniciar listener (listener_running is a flag owned by this function)
    pthread_t listener = spectator_listener_start(conn, state_buffer, &listener_running);
    
    // Main loop: use local `running` flag so we can stop the screen independently
    while (running && conn->connected && !WindowShouldClose()) {
        // Obtener siguiente estado del buffer
        if (state_buffer_pop(state_buffer, &current_state)) {
            has_state = true;
            update_level_from_snapshot(level, &current_state);
            printf("%s", has_state?"true":"false");
        }

        // Process any admin SPAWN/REMOVE commands queued by the listener
        message_listener_process_admin_commands(level, conn);
    
        // Check for user input
        if (IsKeyPressed(KEY_Q)) {
            printf("DEBUG: User pressed Q (voluntary exit) - setting listener_running=false\n");
            voluntary_exit = true;
            running = false;
            listener_running = false; // signal listener to stop
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
            
            // Always render the map so spectator sees the level immediately
            level_render(level);
            fruit_render(level);

            if (has_state) {
                // Render each enemy (level contains enemy array)
                for (int j = 0; j < level->enemy_count; j++) {
                    if (level->enemies[j].active) {  
                        enemy_render(&level->enemies[j]);
                    }
                }

                DrawText(TextFormat("SCORE: %d", current_state.score), 10, 10, 20, WHITE);
                DrawText(TextFormat("LIVES: %d", current_state.lives), 10, 35, 20, WHITE);
            } else {
                const char* placeholder = "Waiting for game state...";
                int placeholder_width = font_manager_measure_text(placeholder, UI_FONT_SIZE_NORMAL);
                int placeholder_x = (UI_WINDOW_WIDTH - placeholder_width) / 2;
                int placeholder_y = UI_WINDOW_HEIGHT / 2;
                font_manager_draw_text(placeholder, placeholder_x, placeholder_y, UI_FONT_SIZE_NORMAL, GRAY);
            }
        EndDrawing();
    }

    // Cleanup
    // Signal listener to stop and join it cleanly (avoid pthread_cancel)
    printf("DEBUG: Spectator cleanup - setting listener_running=false and joining\n");
    listener_running = false;
    pthread_join(listener, NULL);
    state_buffer_free(state_buffer);
    level_destroy(level);
    // Restore previous global level pointer
    g_current_level = prev_level;
    printf("Level properly destroyed for spectator");

    return voluntary_exit;
}