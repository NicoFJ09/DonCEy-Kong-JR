#include "sprite_test.h"
#include "renderer.h"
#include "sprite_manager.h"
#include "animation.h"
#include "raylib.h"
#include <stdio.h>

#define MARGIN_LEFT 40
#define MARGIN_TOP 40
#define SPACING_X 20
#define SPACING_Y 20

/*
 * =================================================================
 * CREATE TEST SPRITES - DYNAMIC POSITIONING
 * Positions sprites based on actual dimensions + spacing
 * Automatically wraps to next row when reaching screen edge
 * =================================================================
 */
static void create_test_sprites(AnimatedObject* sprites, int* count) {
    *count = 0;
    int current_x = MARGIN_LEFT;
    int current_y = MARGIN_TOP;
    int row_max_height = 0;
    
    for (int i = 0; i < SPRITE_COUNT; i++) {
        SpriteSheet* sprite = sprite_manager_get(i);
        if (!sprite) continue;
        
        // Calculate scaled dimensions
        float scale = 3.0f;
        int sprite_width = (int)(sprite->frame_width * scale);
        int sprite_height = (int)(sprite->frame_height * scale);
        
        // Check if sprite fits in current row
        if (current_x + sprite_width > SCREEN_WIDTH - MARGIN_LEFT) {
            // Move to next row
            current_x = MARGIN_LEFT;
            current_y += row_max_height + SPACING_Y;
            row_max_height = 0;
        }
        
        // Create animated object at current position
        sprites[*count] = animated_object_create(current_x, current_y, scale);
        
        int speed = sprite->frame_count > 1 ? 8 : 1;
        Animation anim = animation_create(i, sprite->frame_count, speed);
        
        animated_object_add_animation(&sprites[*count], anim);
        animated_object_set_animation(&sprites[*count], 0);
        
        (*count)++;
        
        // Update position for next sprite
        current_x += sprite_width + SPACING_X;
        
        // Track tallest sprite in current row
        if (sprite_height > row_max_height) {
            row_max_height = sprite_height;
        }
    }
}

/*
 * =================================================================
 * DRAW ALL TEST SPRITES
 * Simple draw - no labels
 * =================================================================
 */
static void draw_test_sprites(AnimatedObject* sprites, int count) {
    for (int i = 0; i < count; i++) {
        animated_object_draw(&sprites[i]);
    }
}

/*
 * =================================================================
 * SPRITE TEST - MAIN LOOP
 * Clean sprite display with dynamic positioning
 * =================================================================
 */
void sprite_test_run(void) {
    printf("\n========================================\n");
    printf("Sprite Test Mode\n");
    printf("========================================\n");
    printf("Displaying all sprites with dynamic positioning\n");
    printf("Press ESC to exit\n\n");
    
    AnimatedObject test_sprites[SPRITE_COUNT];
    int sprite_count = 0;
    create_test_sprites(test_sprites, &sprite_count);
    
    printf("Loaded %d sprites\n", sprite_count);
    printf("Screen: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("Margins: %dpx, Spacing: %dx%dpx\n\n", MARGIN_LEFT, SPACING_X, SPACING_Y);
    
    while (!WindowShouldClose()) {
        // UPDATE
        for (int i = 0; i < sprite_count; i++) {
            animated_object_update(&test_sprites[i]);
        }
        
        // RENDER
        BeginDrawing();
            ClearBackground(BLACK);
            draw_test_sprites(test_sprites, sprite_count);
            DrawFPS(10, 10);
        EndDrawing();
    }
    
    printf("\nExiting sprite test...\n");
}