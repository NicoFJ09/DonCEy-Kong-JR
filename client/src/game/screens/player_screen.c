#include "player_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "../../rendering/sprite_manager.h"
#include "../level.h"
#include "../player.h"
#include "../enemy.h"
#include "../fruit.h"
#include "raylib.h"
#include <stdio.h>
#include <math.h>

// ============================================================
// GLOBALS
// ============================================================

Level* g_current_level = NULL;

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

static void render_goal_objects(Level* level, float deltaTime) {
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
// PLAYER SCREEN
// ============================================================

void show_player_screen(int client_id, Connection* conn) {
    // Create level from server JSON (REQUIRED)
    if (!conn || !conn->map_loaded || !conn->map_json) {
        printf("FATAL ERROR: No map data from server\n");
        return;
    }

    // Initialize HUD
    hud_initialize();
    
    // Game state variables (persist across levels)
    int level_number = 1;
    float enemy_speed_multiplier = 1.0f;
    bool game_over = false;
    int player_lives = 3;      // Start with 3 lives
    int player_score = 0;      // Start with 0 points
    
    // Main game loop - continues until out of lives or quit
    while (!game_over && !WindowShouldClose()) {
        // Create/recreate level for each attempt
        g_current_level = level_create_from_json(conn->map_json);
        if (!g_current_level) {
            printf("FATAL ERROR: Failed to parse server map JSON\n");
            hud_cleanup();
            return;
        }
        
        // Set level state
        g_current_level->level_number = level_number;
        g_current_level->speed_multiplier = enemy_speed_multiplier;

        // Create player - start on platform center
        Player player;
        player_init(&player, 
                    PLAYER_SPAWN_X_BLOCK * PLATFORM_BLOCK_SIZE, 
                    PLAYER_SPAWN_Y_BLOCK * PLATFORM_BLOCK_SIZE);
        
        // Restore persistent game state
        player.lives = player_lives;
        player.score = player_score;

        bool level_active = true;
        
        // Level loop - runs until level complete, death, or quit
        while (level_active && !WindowShouldClose()) {
            // Always get deltaTime for animations (even when paused)
            float deltaTime = GetFrameTime();
            
            // Clamp deltaTime to prevent physics explosion
            if (deltaTime > MAX_DELTA_TIME) {
                deltaTime = MAX_DELTA_TIME;
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
                
                // Update fruit popups
                fruit_update_popups(g_current_level, deltaTime);
                
                // Check fruit collision (only if player is alive)
                if (player.state != STATE_DYING) {
                    fruit_check_collision(&player, g_current_level);
                }
                
                // If player is dying, count down timer
                if (player.state == STATE_DYING) {
                    player.death_timer += deltaTime;
                    if (player.death_timer >= 1.0f) {
                        // Death animation complete
                        if (player.lives <= 0) {
                            // Game Over - exit to lose screen
                            printf("[GAME] Game Over - Lives: %d, Final Score: %d\n", 
                                   player.lives, player.score);
                            level_active = false;
                            game_over = true;
                        } else {
                            // Save persistent state
                            player_lives = player.lives;
                            player_score = player.score;
                            
                            // Respawn player
                            printf("[PLAYER] Respawning - Lives remaining: %d\n", player.lives);
                            player_reset(&player, 
                                        PLAYER_SPAWN_X_BLOCK * PLATFORM_BLOCK_SIZE,
                                        PLAYER_SPAWN_Y_BLOCK * PLATFORM_BLOCK_SIZE);
                            level_reset(g_current_level);
                        }
                    }
                }
                
                // Update red crocodile spawn timer (temporary - will be admin controlled)
                g_current_level->red_spawn_timer += deltaTime;
                if (g_current_level->red_spawn_timer >= 3.0f) {  // Spawn every 3 seconds
                    g_current_level->red_spawn_timer = 0.0f;

                    // Find first inactive enemy slot or add new one
                    int spawn_index = -1;
                    for (int i = 0; i < g_current_level->enemy_count; i++) {
                        if (!g_current_level->enemies[i].active) {
                            spawn_index = i;
                            break;
                        }
                    }

                    // If no inactive slot found, add new enemy if space available
                    if (spawn_index == -1 && g_current_level->enemy_count < g_current_level->max_enemies) {
                        spawn_index = g_current_level->enemy_count;
                        g_current_level->enemy_count++;
                    }

                    // Spawn red crocodile at Mario's position
                    if (spawn_index != -1) {
                        enemy_init_red_crocodile(&g_current_level->enemies[spawn_index],
                                                 g_current_level,
                                                 g_current_level->mario_x,
                                                 g_current_level->mario_y);
                        // Apply level speed multiplier
                        g_current_level->enemies[spawn_index].speed_multiplier = g_current_level->speed_multiplier;
                    }
                }

                // Update blue crocodile spawn timer (temporary - will be admin controlled)
                g_current_level->blue_spawn_timer += deltaTime;
                if (g_current_level->blue_spawn_timer >= 5.0f) {  // Spawn every 5 seconds (less frequent)
                    g_current_level->blue_spawn_timer = 0.0f;

                    // Find first inactive enemy slot or add new one
                    int spawn_index = -1;
                    for (int i = 0; i < g_current_level->enemy_count; i++) {
                        if (!g_current_level->enemies[i].active) {
                            spawn_index = i;
                            break;
                        }
                    }

                    // If no inactive slot found, add new enemy if space available
                    if (spawn_index == -1 && g_current_level->enemy_count < g_current_level->max_enemies) {
                        spawn_index = g_current_level->enemy_count;
                        g_current_level->enemy_count++;
                    }

                    // Spawn blue crocodile at Mario's position
                    if (spawn_index != -1) {
                        enemy_init_blue_crocodile(&g_current_level->enemies[spawn_index],
                                                 g_current_level,
                                                 g_current_level->mario_x,
                                                 g_current_level->mario_y);
                        // Apply level speed multiplier
                        g_current_level->enemies[spawn_index].speed_multiplier = g_current_level->speed_multiplier;
                    }
                }
                
                // Check if player reached DK cage (victory condition)
                float player_center_x = player.x + 48;
                float player_center_y = player.y + 24;
                float cage_center_x = g_current_level->cage_x + 72;
                float cage_center_y = g_current_level->cage_y + 72;
                
                float dist_x = player_center_x - cage_center_x;
                float dist_y = player_center_y - cage_center_y;
                float distance = sqrtf(dist_x * dist_x + dist_y * dist_y);
                
                if (distance < 80.0f) {
                    // Level complete! Add life and increase difficulty
                    player.lives++;
                    level_active = false;
                    level_number++;
                    enemy_speed_multiplier += 0.2f;  // 20% speed increase each level
                    
                    // Save persistent state for next level
                    player_lives = player.lives;
                    player_score = player.score;
                    
                    printf("[LEVEL] Completed level %d! Lives: %d, Score: %d, Next speed: %.1fx\n",
                           level_number - 1, player.lives, player.score, enemy_speed_multiplier);
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
                        // Player death by Mario
                        player.state = STATE_DYING;
                        player.death_timer = 0.0f;
                        player.lives--;
                        printf("[PLAYER] Killed by Mario! Lives remaining: %d\n", player.lives);
                    }
                }
                
                // Update all enemies
                for (int i = 0; i < g_current_level->enemy_count; i++) {
                    enemy_update(&g_current_level->enemies[i], deltaTime);
                    
                    // Check collision with player only if still alive
                    if (player.state != STATE_DYING && enemy_collides_with_player(&g_current_level->enemies[i], player.x, player.y)) {
                        // Player death by enemy
                        player.state = STATE_DYING;
                        player.death_timer = 0.0f;
                        player.lives--;
                        printf("[PLAYER] Killed by enemy! Lives remaining: %d\n", player.lives);
                        break;
                    }
                }
                
                // Check if player fell in water (same as enemy collision)
                // Player stops at Y=816 due to ground collision
                // Set threshold lower to avoid false positives when walking on bottom platform
                float water_check = 850.0f;  // Safely below normal ground, but above water surface
                if (player.state != STATE_DYING && player.y >= water_check && player.velocity_y > 0) {
                    // Player death by drowning (only when falling down, not when on ground)
                    player.state = STATE_DYING;
                    player.death_timer = 0.0f;
                    player.lives--;
                    printf("[PLAYER] Drowned! Lives remaining: %d\n", player.lives);
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
                for (int i = 0; i < g_current_level->enemy_count; i++) {
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

            EndDrawing();
        }

        // Cleanup level
        level_destroy(g_current_level);
        g_current_level = NULL;
    }
    
    // Cleanup HUD
    hud_cleanup();
    
    // Game over - game_flow.c will handle showing lose screen
    g_current_level = NULL;
}