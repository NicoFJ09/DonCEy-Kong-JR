#include "ip_input_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

// Input state
typedef struct {
    char text[UI_INPUT_MAX_CHARS + 1];
    int cursor_pos;
    int cursor_blink_counter;
    bool show_cursor;
    bool show_error;
} InputState;



static void handle_text_input(InputState* state) {
    static int backspace_hold_frames = 0;
    
    // Handle paste
    bool paste_pressed = false;
    #ifdef __APPLE__
    if (IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER)) {
        if (IsKeyPressed(KEY_V)) paste_pressed = true;
    }
    #else
    if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
        if (IsKeyPressed(KEY_V)) paste_pressed = true;
    }
    #endif
    
    if (paste_pressed) {
        const char* clipboard = GetClipboardText();
        if (clipboard != NULL) {
            state->text[0] = '\0';
            state->cursor_pos = 0;
            int i = 0;
            while (clipboard[i] != '\0' && state->cursor_pos < UI_INPUT_MAX_CHARS) {
                char c = clipboard[i];
                if ((c >= '0' && c <= '9') || c == '.') {
                    state->text[state->cursor_pos] = c;
                    state->cursor_pos++;
                }
                i++;
            }
            state->text[state->cursor_pos] = '\0';
            state->show_error = false;
        }
    }
    
    // Character input
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= '0' && key <= '9') || key == '.') {
            if (state->cursor_pos < UI_INPUT_MAX_CHARS) {
                state->text[state->cursor_pos] = (char)key;
                state->cursor_pos++;
                state->text[state->cursor_pos] = '\0';
                state->show_error = false;
            }
        }
        key = GetCharPressed();
    }
    
    // Backspace
    if (IsKeyDown(KEY_BACKSPACE)) {
        backspace_hold_frames++;
        if (backspace_hold_frames == 1 && state->cursor_pos > 0) {
            state->cursor_pos--;
            state->text[state->cursor_pos] = '\0';
            state->show_error = false;
        } else if (backspace_hold_frames > 15 && state->cursor_pos > 0) {
            if (backspace_hold_frames % 2 == 0) {
                state->cursor_pos--;
                state->text[state->cursor_pos] = '\0';
                state->show_error = false;
            }
        }
    } else {
        backspace_hold_frames = 0;
    }
}

static void update_cursor_blink(InputState* state) {
    state->cursor_blink_counter++;
    if (state->cursor_blink_counter >= UI_CURSOR_BLINK_RATE) {
        state->cursor_blink_counter = 0;
        state->show_cursor = !state->show_cursor;
    }
}

static void draw_ip_input_screen(InputState* state) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
    // Title
    const char* title = "ENTER IP:";
    int title_width = font_manager_measure_text(title, UI_FONT_SIZE_TITLE);
    int title_x = (UI_WINDOW_WIDTH - title_width) / 2;
    int title_y = 300;
    font_manager_draw_text(title, title_x, title_y, UI_FONT_SIZE_TITLE, UI_COLOR_TEXT);
    
    // Input box
    int input_x = (UI_WINDOW_WIDTH - UI_INPUT_BOX_WIDTH) / 2;
    int input_y = 400;
    DrawRectangleLines(input_x - 2, input_y - 2, 
                       UI_INPUT_BOX_WIDTH + 4, UI_INPUT_BOX_HEIGHT + 4, 
                       DARKGRAY);
    DrawRectangle(input_x, input_y, UI_INPUT_BOX_WIDTH, UI_INPUT_BOX_HEIGHT, BLACK);
    
    // Input text
    if (strlen(state->text) > 0) {
        int text_width = font_manager_measure_text(state->text, UI_FONT_SIZE_INPUT);
        int text_x = input_x + 10;
        int text_y = input_y + (UI_INPUT_BOX_HEIGHT - UI_FONT_SIZE_INPUT) / 2;
        font_manager_draw_text(state->text, text_x, text_y, UI_FONT_SIZE_INPUT, UI_COLOR_INPUT);
        
        if (state->show_cursor) {
            int cursor_x = text_x + text_width + 5;
            int cursor_y = input_y + 5;
            DrawRectangle(cursor_x, cursor_y, 2, UI_INPUT_BOX_HEIGHT - 10, UI_COLOR_INPUT);
        }
    } else {
        if (state->show_cursor) {
            int cursor_x = input_x + 10;
            int cursor_y = input_y + 5;
            DrawRectangle(cursor_x, cursor_y, 2, UI_INPUT_BOX_HEIGHT - 10, UI_COLOR_INPUT);
        }
    }
    
    // ERROR MESSAGE - NEW (centered below box)
    if (state->show_error) {
        const char* error_msg = "Could not connect";
        int error_width = font_manager_measure_text(error_msg, UI_FONT_SIZE_ERROR);
        int error_x = (UI_WINDOW_WIDTH - error_width) / 2;
        int error_y = input_y + UI_INPUT_BOX_HEIGHT + 20;
        font_manager_draw_text(error_msg, error_x, error_y, UI_FONT_SIZE_ERROR, RED);
    }
    
    // Instruction (same position as before, but moves down if error shown)
    const char* instruction = "Press ENTER to connect";
    int instruction_width = font_manager_measure_text(instruction, UI_FONT_SIZE_ERROR);
    int instruction_x = (UI_WINDOW_WIDTH - instruction_width) / 2;
    int instruction_y = state->show_error ? 
        input_y + UI_INPUT_BOX_HEIGHT + 60 :
        input_y + UI_INPUT_BOX_HEIGHT + 30;
    font_manager_draw_text(instruction, instruction_x, instruction_y, UI_FONT_SIZE_ERROR, GRAY);
}

bool show_ip_input_screen(char* ip_buffer, size_t buffer_size, bool show_error) {
    // Initialize
    InputState state = {0};
    
    // If retrying, keep previous IP
    if (show_error && strlen(ip_buffer) > 0) {
        strncpy(state.text, ip_buffer, UI_INPUT_MAX_CHARS);
        state.cursor_pos = strlen(state.text);
    } else {
        strcpy(state.text, "127.0.0.1");
        state.cursor_pos = strlen(state.text);
    }
    
    state.show_cursor = true;
    state.show_error = show_error;
    
    bool done = false;
    bool success = false;
    
    while (!done && !WindowShouldClose()) {
        handle_text_input(&state);
        update_cursor_blink(&state);
        
        if (IsKeyPressed(KEY_ENTER) && strlen(state.text) > 0) {
            strncpy(ip_buffer, state.text, buffer_size - 1);
            ip_buffer[buffer_size - 1] = '\0';
            done = true;
            success = true;
        }
        
        BeginDrawing();
            draw_ip_input_screen(&state);
        EndDrawing();
    }
    
    return success;
}