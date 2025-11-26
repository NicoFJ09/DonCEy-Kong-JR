#include "lose_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>

// ============================================================
// LOSE SCREEN
// ============================================================

LoseOption show_lose_screen(int final_score) {
    printf("\n========================================\n");
    printf("Game Over - Final Score: %d\n", final_score);
    printf("Press ENTER to return\n");
    printf("========================================\n");
    
    // Load PressStart2P font
    Font font = LoadFont("assets/ui/fonts/PressStart2P.ttf");
    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
    
    // Clear all pending key presses
    while (GetKeyPressed() != 0) {
        // Consume all queued key presses
    }
    
    float fade_in_time = 0.0f;
    const float FADE_DURATION = 0.5f;
    bool done = false;
    
    while (!done && !WindowShouldClose()) {
        fade_in_time += GetFrameTime();
        if (fade_in_time > FADE_DURATION) {
            fade_in_time = FADE_DURATION;
        }
        
        float alpha = fade_in_time / FADE_DURATION;
        
        // Wait for ENTER
        if (IsKeyPressed(KEY_ENTER)) {
            done = true;
        }
        
        // Render
        BeginDrawing();
            ClearBackground(BLACK);
            
            // YOU LOST text (centered, large)
            const char* lose_text = "YOU LOST";
            Vector2 lose_size = MeasureTextEx(font, lose_text, 48, 2);
            float lose_x = (UI_WINDOW_WIDTH - lose_size.x) / 2;
            float lose_y = UI_WINDOW_HEIGHT / 2 - 120;
            DrawTextEx(font, lose_text, 
                       (Vector2){lose_x, lose_y}, 
                       48, 2, 
                       Fade(WHITE, alpha));
            
            // Points: [score] text (centered, medium)
            char score_text[64];
            snprintf(score_text, sizeof(score_text), "Points: %d", final_score);
            Vector2 score_size = MeasureTextEx(font, score_text, 32, 2);
            float score_x = (UI_WINDOW_WIDTH - score_size.x) / 2;
            float score_y = UI_WINDOW_HEIGHT / 2 - 40;
            DrawTextEx(font, score_text, 
                       (Vector2){score_x, score_y}, 
                       32, 2, 
                       Fade(WHITE, alpha));
            
            // Press ENTER prompt (centered, yellow)
            const char* prompt_text = "Press ENTER to continue";
            Vector2 prompt_size = MeasureTextEx(font, prompt_text, 20, 1);
            float prompt_x = (UI_WINDOW_WIDTH - prompt_size.x) / 2;
            float prompt_y = UI_WINDOW_HEIGHT / 2 + 40;
            DrawTextEx(font, prompt_text, 
                       (Vector2){prompt_x, prompt_y}, 
                       20, 1, 
                       Fade(YELLOW, alpha));
            
            // Return to Title text (centered, small)
            const char* return_text = "Return to Title";
            Vector2 return_size = MeasureTextEx(font, return_text, 18, 1);
            float return_x = (UI_WINDOW_WIDTH - return_size.x) / 2;
            float return_y = UI_WINDOW_HEIGHT / 2 + 80;
            DrawTextEx(font, return_text, 
                       (Vector2){return_x, return_y}, 
                       18, 1, 
                       Fade(GRAY, alpha));
            
        EndDrawing();
    }
    
    // Cleanup font
    UnloadFont(font);
    
    printf("DEBUG: Returning to title\n");
    return LOSE_RETURN_TITLE;
}