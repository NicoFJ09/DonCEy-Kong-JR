#include "player_screen.h"
#include "../../network/game_events.h"
#include "../../utils/message_listener.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../rendering/sprite_manager.h"
#include "../level.h"
#include "../player.h"
#include "../enemy.h"
#include "../fruit.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================
// GLOBALS
// ============================================================

Level* g_current_level = NULL;

// Player data that can be updated from message listener thread
typedef struct {
    int* score_ptr;
    int* lives_ptr;
    int* level_ptr;
    float* speed_multiplier_ptr;
    bool* game_over_ptr;
    bool* level_complete_ptr;
    Player* player_ptr;  // For instant HUD updates
    Connection** conn_ptr;  // For server shutdown detection
} PlayerUpdateData;

static PlayerUpdateData g_player_update_data = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

// ============================================================
// HUD RESOURCES
// ============================================================

static Font g_hud_font;
static Texture2D g_life_texture;
static bool g_hud_initialized = false;

// ============================================================
// HUD FUNCTIONS
// ============================================================

static void hud_initialize(void) {
    if (g_hud_initialized) return;
    
    // Load PressStart2P font
    g_hud_font = LoadFont("assets/ui/fonts/PressStart2P.ttf");
    
    // Load life icon (24x24)
    g_life_texture = LoadTexture("assets/ui/lives/life.png");
    
    // Keep pixel-perfect rendering
    SetTextureFilter(g_hud_font.texture, TEXTURE_FILTER_POINT);
    SetTextureFilter(g_life_texture, TEXTURE_FILTER_POINT);
    
    g_hud_initialized = true;
    printf("HUD initialized\n");
}

static void hud_cleanup(void) {
    if (!g_hud_initialized) return;
    
    UnloadFont(g_hud_font);
    UnloadTexture(g_life_texture);
    
    g_hud_initialized = false;
}

static void render_lives(Player* player) {
    // Render life icons (36x36px scaled, 6px spacing = 42px total per icon)
    for (int i = 0; i < player->lives; i++) {
        DrawTextureEx(
            g_life_texture,
            (Vector2){ 10 + i * 42, 10 },  // 10px margin, 42px per icon
            0,
            1.5f,  // Scale 1.5x (24px -> 36px)
            WHITE
        );
    }
}

static void render_score(Player* player) {
    // Format score with zero-padding (arcade style)
    char buf[32];
    snprintf(buf, sizeof(buf), "%06d", player->score);
    
    float fontSize = 24;  // Larger font size
    float spacing = 1;
    Vector2 size = MeasureTextEx(g_hud_font, buf, fontSize, spacing);
    
    // Draw at top-right (10px margin)
    DrawTextEx(
        g_hud_font,
        buf,
        (Vector2){ UI_WINDOW_WIDTH - size.x - 10, 10 },
        fontSize,
        spacing,
        WHITE
    );
}

static void render_level_number(Level* level) {
    // Format level number
    char buf[16];
    snprintf(buf, sizeof(buf), "Lv %d", level->level_number);
    
    float fontSize = 20;  // Larger font size
    float spacing = 1;
    Vector2 size = MeasureTextEx(g_hud_font, buf, fontSize, spacing);
    
    // Draw below score (right-aligned)
    DrawTextEx(
        g_hud_font,
        buf,
        (Vector2){ UI_WINDOW_WIDTH - size.x - 10, 40 },  // Adjusted position
        fontSize,
        spacing,
        WHITE
    );
}

static void render_client_id(int client_id) {
    // Draw client ID at top-left below lives
    char buf[32];
    snprintf(buf, sizeof(buf), "Player #%d", client_id);
    
    float fontSize = 18;
    float spacing = 1;
    
    DrawTextEx(
        g_hud_font,
        buf,
        (Vector2){ 10, 56 },  // Below the life icons
        fontSize,
        spacing,
        WHITE
    );
}

static void render_hud(Player* player, Level* level, int client_id) {
    render_lives(player);
    render_score(player);
    render_level_number(level);
    render_client_id(client_id);
}

// ============================================================
// HELPER FUNCTIONS
// ============================================================

// Animation state for goal objects
static float dk_animation_time = 0.0f;
static int dk_current_frame = 0;
static float mario_animation_time = 0.0f;
static int mario_current_frame = 0;

void render_goal_objects(Level* level, float deltaTime) {
    // Update Donkey Kong animation (7 frames, slower animation)
    dk_animation_time += deltaTime;
    if (dk_animation_time >= 0.15f) {  // 6.67 fps animation
        dk_animation_time = 0.0f;
        dk_current_frame = (dk_current_frame + 1) % 7;
    }
    
    // Render Donkey Kong in cage
    SpriteSheet* dk_cage = sprite_manager_get(SPRITE_DK_CAGED);
    if (dk_cage && dk_cage->loaded) {
        // DK cage sprite: 48x34, scale 3x = 144x102, 7 frames with 2px spacing
        Rectangle source = {
            dk_current_frame * (dk_cage->frame_width + dk_cage->spacing),
            0,
            dk_cage->frame_width,
            dk_cage->frame_height
        };
        Rectangle dest = {
            level->cage_x,
            level->cage_y,
            dk_cage->frame_width * 3.0f,
            dk_cage->frame_height * 3.0f
        };
        DrawTexturePro(dk_cage->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
    
    // Update Mario animation (2 frames)
    mario_animation_time += deltaTime;
    if (mario_animation_time >= 0.3f) {  // 3.33 fps animation (slower blink)
        mario_animation_time = 0.0f;
        mario_current_frame = (mario_current_frame + 1) % 2;
    }
    
    // Render Mario
    SpriteSheet* mario = sprite_manager_get(SPRITE_MARIO_STARE);
    if (mario && mario->loaded) {
        // Mario sprite: 16x16, scale 3x = 48x48, 2 frames with 2px spacing
        Rectangle source = {
            mario_current_frame * (mario->frame_width + mario->spacing),
            0,
            mario->frame_width,
            mario->frame_height
        };
        Rectangle dest = {
            level->mario_x,
            level->mario_y,
            mario->frame_width * 3.0f,
            mario->frame_height * 3.0f
        };
        DrawTexturePro(mario->texture, source, dest, (Vector2){0, 0}, 0, WHITE);
    }
}



// ============================================================
// SERVER MESSAGE HANDLER
// ============================================================

/**
 * Handle messages from server (runs in listener thread)
 * Updates player score/lives based on server responses
 */
static void handle_server_message(const char* message, void* user_data) {
    (void)user_data;  // Unused
    
    // SCORE_UPDATE:value
    if (strncmp(message, "SCORE_UPDATE:", 13) == 0) {
        int new_score = atoi(message + 13);
        printf("[DEBUG] Received SCORE_UPDATE: %d (ptr: %p)\n", new_score, (void*)g_player_update_data.score_ptr);
        if (g_player_update_data.score_ptr) {
            int old_score = *g_player_update_data.score_ptr;
            *g_player_update_data.score_ptr = new_score;
            
            // Update HUD immediately (no delay)
            if (g_player_update_data.player_ptr) {
                g_player_update_data.player_ptr->score = new_score;
            }
            
            printf("[SERVER] ✓ Score updated: %d → %d (HUD synced)\n", old_score, new_score);
        } else {
            printf("[ERROR] score_ptr is NULL!\n");
        }
        return;
    }
    
    // LIVES_UPDATE:value
    if (strncmp(message, "LIVES_UPDATE:", 13) == 0) {
        int new_lives = atoi(message + 13);
        printf("[DEBUG] Received LIVES_UPDATE: %d (ptr: %p)\n", new_lives, (void*)g_player_update_data.lives_ptr);
        if (g_player_update_data.lives_ptr) {
            int old_lives = *g_player_update_data.lives_ptr;
            *g_player_update_data.lives_ptr = new_lives;
            
            // Update HUD immediately (no delay)
            if (g_player_update_data.player_ptr) {
                g_player_update_data.player_ptr->lives = new_lives;
            }
            
            printf("[SERVER] ✓ Lives updated: %d → %d (HUD synced)\n", old_lives, new_lives);
        } else {
            printf("[ERROR] lives_ptr is NULL!\n");
        }
        return;
    }
    
    // LEVEL_UPDATE:level:lives:speed
    if (strncmp(message, "LEVEL_UPDATE:", 13) == 0) {
        // Parse: level:lives:speed
        int level, lives;
        float speed;
        if (sscanf(message + 13, "%d:%d:%f", &level, &lives, &speed) == 3) {
            if (g_player_update_data.lives_ptr) {
                *g_player_update_data.lives_ptr = lives;
            }
            if (g_player_update_data.level_ptr) {
                *g_player_update_data.level_ptr = level;
            }
            if (g_player_update_data.speed_multiplier_ptr) {
                *g_player_update_data.speed_multiplier_ptr = speed;
            }
            if (g_player_update_data.level_complete_ptr) {
                *g_player_update_data.level_complete_ptr = true;
            }
            
            // Update HUD immediately
            if (g_player_update_data.player_ptr) {
                g_player_update_data.player_ptr->lives = lives;
            }
            
            printf("[SERVER] ✓ Level complete! Now Level %d, Lives: %d, Speed: %.1fx\n", 
                   level, lives, speed);
        }
        return;
    }
    
    // GAME_OVER:finalScore
    if (strncmp(message, "GAME_OVER:", 10) == 0) {
        int final_score = atoi(message + 10);
        if (g_player_update_data.game_over_ptr) {
            *g_player_update_data.game_over_ptr = true;
        }
        
        // Update HUD immediately
        if (g_player_update_data.player_ptr) {
            g_player_update_data.player_ptr->lives = 0;
            g_player_update_data.player_ptr->score = final_score;
        }
        
        printf("[SERVER] ☠️ GAME OVER! Final Score: %d (game_over flag set)\n", final_score);
        return;
    }
    
    // SERVER_SHUTDOWN - Server closed cleanly, disconnect and return to IP input
    if (strcmp(message, PROTO_SERVER_SHUTDOWN) == 0) {
        printf("[SERVER] ⚠ Server shutdown detected - disconnecting\n");
        
        if (g_player_update_data.conn_ptr) {
            (*g_player_update_data.conn_ptr)->connected = false;
        }
        
        if (g_player_update_data.game_over_ptr) {
            *g_player_update_data.game_over_ptr = true;
        }
        
        return;
    }
}

// ============================================================
// PLAYER SCREEN
// ============================================================

int show_player_screen(int client_id, Connection* conn) {
    // Create level from server JSON (REQUIRED)
    if (!conn || !conn->map_loaded || !conn->map_json) {
        printf("FATAL ERROR: No map data from server\n");
        return 0;
    }

    // Initialize HUD
    hud_initialize();
    
    // Game state variables (persist across levels) - SERVER AUTHORITATIVE
    // CRITICAL: Reset all state when entering player screen (fresh start)
    int level_number = 1;
    float enemy_speed_multiplier = 1.0f;
    bool game_over = false;          // MUST start false (server session is reset)
    bool level_complete = false;
    int player_lives = 3;            // Will be updated by server
    int player_score = 0;            // Will be updated by server
    
    // Player object will be initialized in game loop
    Player player;
    
    // Set up server message handler to update these variables
    g_player_update_data.score_ptr = &player_score;
    g_player_update_data.lives_ptr = &player_lives;
    g_player_update_data.level_ptr = &level_number;
    g_player_update_data.speed_multiplier_ptr = &enemy_speed_multiplier;
    g_player_update_data.game_over_ptr = &game_over;
    g_player_update_data.level_complete_ptr = &level_complete;
    g_player_update_data.player_ptr = &player;  // For instant HUD updates
    g_player_update_data.conn_ptr = &conn;  // For server shutdown detection
    message_listener_set_callback(handle_server_message, NULL);
    
    printf("[PLAYER] Server message handler active (SERVER AUTHORITATIVE MODE)\n");
    printf("[PLAYER] Fresh session - game_over=%s, lives=%d, score=%d\n", 
           game_over ? "true" : "false", player_lives, player_score);
    
    // Main game loop - continues until out of lives or quit
    while (!game_over && !WindowShouldClose() && conn->connected) {
        // Create/recreate level for each attempt
        g_current_level = level_create_from_json(conn->map_json);
        if (!g_current_level) {
            printf("FATAL ERROR: Failed to parse server map JSON\n");
            hud_cleanup();
            return 0;
        }
        
        // Set level state
        g_current_level->level_number = level_number;
        g_current_level->speed_multiplier = enemy_speed_multiplier;

        // Initialize player - start on platform center
        player_init(&player, 
                    PLAYER_SPAWN_X_BLOCK * PLATFORM_BLOCK_SIZE, 
                    PLAYER_SPAWN_Y_BLOCK * PLATFORM_BLOCK_SIZE);
        
        // Restore persistent game state
        player.lives = player_lives;
        player.score = player_score;
        
        // CRITICAL: Prevent immediate respawn trigger on first frame
        // death_timer == 0.0f is used to detect "just died", so set to -1.0f
        player.death_timer = -1.0f;

        bool level_active = true;
        
        // Player state sync - send every N frames to reduce network load
        int sync_counter = 0;
        const int SYNC_INTERVAL = 2;  // Send every 2 frames (~30 updates/sec at 60fps)
        
        // Level loop - runs until level complete, death, or quit
        while (level_active && !WindowShouldClose() && conn->connected) {
            // Always get deltaTime for animations (even when paused)
            float deltaTime = GetFrameTime();
            
            // Clamp deltaTime to prevent physics explosion
            if (deltaTime > MAX_DELTA_TIME) {
                deltaTime = MAX_DELTA_TIME;
            }
            
            // Sync display values from server (for HUD rendering)
            player.lives = player_lives;
            player.score = player_score;
            
            // Check game over IMMEDIATELY (server authoritative)
            if (game_over) {
                printf("[PLAYER] ☠️ GAME OVER DETECTED! Exiting level immediately. Final Score: %d\n", player_score);
                level_active = false;
                continue;
            }
            
            // PAUSE GAME when window loses focus
            if (IsWindowFocused()) {
                // Window has focus - update game normally

                // Handle input (only if not dying)
                if (player.state != STATE_DYING) {
                    player_handle_input(&player);
                }

                // Update physics and animation
                player_update(&player, deltaTime);
                
                // Send player state to server (for spectators) every N frames
                sync_counter++;
                if (sync_counter >= SYNC_INTERVAL) {
                    sync_counter = 0;
                    char state_buffer[256];
                    player_serialize_state(&player, state_buffer, sizeof(state_buffer));
                    char message[512];
                    snprintf(message, sizeof(message), "%s%s", PLAYER_STATE_UPDATE, state_buffer);
                    connection_send(conn, message);
                }
                
                // Update fruit popups
                fruit_update_popups(g_current_level, deltaTime);
                
                // Check fruit collision (only if player is alive)
                if (player.state != STATE_DYING) {
                    fruit_check_collision(&player, g_current_level, conn);
                }
                
                // If player is dying, count down timer
                if (player.state == STATE_DYING) {
                    player.death_timer += deltaTime;
                    if (player.death_timer >= 1.0f) {
                        // Death animation complete - use special marker value to trigger respawn check
                        printf("[DEBUG] Death animation complete. Lives: %d, Score: %d, GameOver: %s\n", 
                               player_lives, player_score, game_over ? "true" : "false");
                        
                        // Use special marker (0.5f) to indicate "just died, ready to respawn"
                        // (0.0f = just died, -1.0f = normal gameplay, 0.5f = ready for respawn check)
                        player.death_timer = 0.5f;
                        
                        // Change state to allow game over check at top of loop
                        player.state = STATE_IDLE;
                        
                        printf("[DEBUG] State changed to IDLE, will check game_over in next frame\n");
                    }
                }
                
                // After death animation, check if should respawn or game over
                // Check death_timer == 0.5f (marker set above when death animation completes)
                if (player.state == STATE_IDLE && player.death_timer == 0.5f) {
                    // Just transitioned from death - check game over
                    if (game_over) {
                        printf("[PLAYER] ☠️ GAME OVER! Final Score: %d\n", player_score);
                        level_active = false;
                    } else {
                        printf("[PLAYER] Respawning - Lives remaining: %d (from server)\n", player_lives);
                        player_reset(&player, 
                                    PLAYER_SPAWN_X_BLOCK * PLATFORM_BLOCK_SIZE,
                                    PLAYER_SPAWN_Y_BLOCK * PLATFORM_BLOCK_SIZE);
                        level_reset(g_current_level);
                        
                        // Notify server that player respawned (session state: DEAD -> PLAYING)
                        // CRITICAL: Only send if not game over (prevents old messages after rejoin)
                        if (!game_over) {
                            event_send_player_respawn(conn);
                        }
                        
                        // Mark as handled
                        player.death_timer = -1.0f;
                    }
                }
                
                // ============================================================
                // ADMIN COMMAND PROCESSING (PHASE 1)
                // Process any pending admin commands from server
                // Commands are queued by message_listener thread, processed here in main thread
                // ============================================================
                message_listener_process_admin_commands(g_current_level, conn);
                
                // ============================================================
                // AUTOMATIC ENEMY SPAWNING (COMMENTED OUT - NOW ADMIN CONTROLLED)
                // Enemies will spawn via admin panel commands in Phase 2
                // ============================================================
                
                // Update red crocodile spawn timer (temporary - will be admin controlled)
                // g_current_level->red_spawn_timer += deltaTime;
                // if (g_current_level->red_spawn_timer >= 3.0f) {  // Spawn every 3 seconds
                //     g_current_level->red_spawn_timer = 0.0f;
                //     ... spawn logic ...
                // }

                // Update blue crocodile spawn timer (temporary - will be admin controlled)
                // g_current_level->blue_spawn_timer += deltaTime;
                // if (g_current_level->blue_spawn_timer >= 5.0f) {  // Spawn every 5 seconds (less frequent)
                //     g_current_level->blue_spawn_timer = 0.0f;
                //     ... spawn logic ...
                // }
                
                // Check if player reached DK cage (victory condition)
                float player_center_x = player.x + 48;
                float player_center_y = player.y + 24;
                float cage_center_x = g_current_level->cage_x + 72;
                float cage_center_y = g_current_level->cage_y + 72;
                
                float dist_x = player_center_x - cage_center_x;
                float dist_y = player_center_y - cage_center_y;
                float distance = sqrtf(dist_x * dist_x + dist_y * dist_y);
                
                if (distance < 80.0f && !level_complete) {
                    // Level complete! Notify server and wait for confirmation
                    level_complete = true;  // Prevent multiple sends
                    
                    printf("[LEVEL] Reached DK cage! Waiting for server confirmation...\n");
                    
                    // Notify server - server will send LEVEL_UPDATE with new values
                    event_send_level_completed(conn);
                }
                
                // Check if server confirmed level completion
                if (level_complete && level_active) {
                    // Server sent LEVEL_UPDATE with new level/lives/speed
                    printf("[LEVEL] Server confirmed level complete! Level %d, Lives: %d, Speed: %.1fx\n",
                           level_number, player_lives, enemy_speed_multiplier);
                    
                    // Exit level to recreate with new parameters
                    level_active = false;
                    level_complete = false;  // Reset for next level
                }
                
                // Check collision with Mario (static enemy at spawn point)
                if (player.state != STATE_DYING) {
                    // Mario hitbox: 48x48 sprite (16x16 scaled 3x)
                    float mario_left = g_current_level->mario_x;
                    float mario_right = g_current_level->mario_x + 48.0f;
                    float mario_top = g_current_level->mario_y;
                    float mario_bottom = g_current_level->mario_y + 48.0f;
                    
                    // Player hitbox (using collision constants)
                    float player_left = player.x + COLLISION_OFFSET_X;
                    float player_right = player_left + COLLISION_WIDTH;
                    float player_top = player.y + COLLISION_OFFSET_Y;
                    float player_bottom = player_top + COLLISION_HEIGHT;
                    
                    // AABB collision check
                    bool mario_collision = (player_left < mario_right &&
                                          player_right > mario_left &&
                                          player_top < mario_bottom &&
                                          player_bottom > mario_top);
                    
                    if (mario_collision) {
                        // Player death by Mario (server will update lives)
                        player.state = STATE_DYING;
                        player.death_timer = 0.0f;
                        printf("[PLAYER] Killed by Mario! Notifying server...\n");
                        
                        // Notify server - server will send LIVES_UPDATE
                        // CRITICAL: Only send if not game over (prevents old messages after rejoin)
                        if (!game_over) {
                            event_send_player_died(conn, "mario");
                        }
                    }
                }
                
                // Update all enemies
                for (int i = 0; i < g_current_level->max_enemies; i++) {
                    if (!g_current_level->enemies[i].active) continue;
                    
                    enemy_update(&g_current_level->enemies[i], deltaTime);
                    
                    // Check collision with player only if still alive
                    if (player.state != STATE_DYING && enemy_collides_with_player(&g_current_level->enemies[i], player.x, player.y)) {
                        // Player death by enemy (server will update lives)
                        player.state = STATE_DYING;
                        player.death_timer = 0.0f;
                        printf("[PLAYER] Killed by enemy! Notifying server...\n");
                        
                        // Notify server - server will send LIVES_UPDATE
                        // CRITICAL: Only send if not game over (prevents old messages after rejoin)
                        if (!game_over) {
                            event_send_player_died(conn, "enemy");
                        }
                        break;
                    }
                }
                
                // Check if player fell in water (same as enemy collision)
                // Water level is at 888px, player spawn is at 768px
                // Only trigger if player is BELOW water surface and falling
                float water_surface = WATER_LEVEL;  // 888.0f
                if (player.state != STATE_DYING &&
                    player.y + PLAYER_HEIGHT >= water_surface &&
                    player.velocity_y > 0 &&
                    player.death_timer < 0.0f) {  // Only if not just respawned (prevent instant death)

                    // Player death by drowning (server will update lives)
                    player.state = STATE_DYING;
                    player.death_timer = 0.0f;
                    printf("[PLAYER] Drowned! Lives remaining: %d\n", player.lives);

                    // Notify server
                    // CRITICAL: Only send if not game over (prevents old messages after rejoin)
                    if (!game_over) {
                        event_send_player_died(conn, "water");
                    }
                }
            }
            // If window NOT focused: game is PAUSED

            // ALWAYS render (even when paused)
            BeginDrawing();
                ClearBackground(BLACK);

                // Render level (background)
                level_render(g_current_level);

                // Render DK cage and Mario (goal) - pass deltaTime for animation
                render_goal_objects(g_current_level, deltaTime);

                // Render enemies
                for (int i = 0; i < g_current_level->max_enemies; i++) {
                    if (g_current_level->enemies[i].active) {
                        enemy_render(&g_current_level->enemies[i]);
                    }
                }

                // Render player (foreground)
                player_render(&player);
                
                // Render fruit popups (on top of player)
                fruit_render_popups(g_current_level);

                // Render HUD on top of everything
                render_hud(&player, g_current_level, client_id);

                // Show pause overlay if window lost focus
                if (!IsWindowFocused()) {
                    // Gray filter overlay
                    DrawRectangle(0, 0, UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT, (Color){0, 0, 0, 180});
                    
                    // "PAUSED" text centered
                    const char* pause_text = "PAUSED";
                    float fontSize = 48;
                    float spacing = 2;
                    Vector2 textSize = MeasureTextEx(g_hud_font, pause_text, fontSize, spacing);
                    
                    DrawTextEx(
                        g_hud_font,
                        pause_text,
                        (Vector2){
                            (UI_WINDOW_WIDTH - textSize.x) / 2,
                            (UI_WINDOW_HEIGHT - textSize.y) / 2
                        },
                        fontSize,
                        spacing,
                        WHITE
                    );
                }

            EndDrawing();
        }

        // Cleanup level
        level_destroy(g_current_level);
        g_current_level = NULL;
    }
    
    // Clear server message handler
    g_player_update_data.score_ptr = NULL;
    g_player_update_data.lives_ptr = NULL;
    g_player_update_data.game_over_ptr = NULL;
    g_player_update_data.conn_ptr = NULL;
    message_listener_set_callback(NULL, NULL);
    
    printf("[PLAYER] Server message handler cleared\n");
    
    // Cleanup HUD
    hud_cleanup();
    
    // Game over - game_flow.c will handle showing lose screen
    g_current_level = NULL;
    
    printf("[PLAYER] Returning final score: %d\n", player_score);
    return player_score;
}