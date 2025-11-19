#include "player_selection_screen.h"
#include "../../utils/constants.h"
#include "../../utils/font_manager.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PLAYERS 2

typedef struct {
    int id;
    char ip[64];
    int spectator_count;
    int max_spectators;
} PlayerInfo;

typedef struct {
    PlayerInfo players[MAX_PLAYERS];
    int count;
    char error_message[256];
    bool show_error;
} PlayerList;

static bool fetch_player_list(Connection* conn, PlayerList* list) {
    list->show_error = false;
    list->error_message[0] = '\0';
    list->count = 0;
    
    if (!conn || !conn->connected) {
        printf("ERROR: Connection not valid\n");
        strcpy(list->error_message, "Connection lost");
        list->show_error = true;
        return false;
    }
    
    printf("DEBUG: Sending LIST_PLAYERS request\n");
    
    if (!connection_send(conn, CMD_LIST_PLAYERS)) {
        printf("ERROR: Failed to send LIST_PLAYERS request\n");
        strcpy(list->error_message, "Send failed");
        list->show_error = true;
        return false;
    }
    
    char buffer[BUFFER_SIZE];
    
    // Use timeout to prevent indefinite blocking
    if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 3.0)) {
        printf("ERROR: Timeout waiting for PLAYER_LIST_START\n");
        strcpy(list->error_message, "Server timeout - try again");
        list->show_error = true;
        return false;
    }
    
    printf("DEBUG: Received: %s\n", buffer);
    
    if (strcmp(buffer, PROTO_PLAYER_LIST_START) != 0) {
        printf("ERROR: Expected PLAYER_LIST_START, got: %s\n", buffer);
        strcpy(list->error_message, "Protocol error");
        list->show_error = true;
        return false;
    }
    
    printf("DEBUG: Started reading player list\n");
    
    bool list_ended = false;
    int safety_counter = 0;
    const int MAX_READS = 100;
    
    while (!list_ended && safety_counter < MAX_READS) {
        safety_counter++;
        
        // Use timeout for each item to prevent blocking
        if (!connection_receive_with_timeout(conn, buffer, BUFFER_SIZE, 2.0)) {
            printf("ERROR: Timeout while reading player list\n");
            strcpy(list->error_message, "Server slow - try again");
            list->show_error = true;
            return false;
        }
        
        printf("DEBUG: Received: %s\n", buffer);
        
        if (strcmp(buffer, PROTO_PLAYER_LIST_END) == 0) {
            printf("DEBUG: Finished reading player list (%d players)\n", list->count);
            list_ended = true;
            break;
        }
        
        if (strncmp(buffer, PROTO_PLAYER_INFO, strlen(PROTO_PLAYER_INFO)) == 0) {
            if (list->count >= MAX_PLAYERS) {
                printf("WARNING: Too many players, skipping\n");
                continue;
            }
            
            char temp[BUFFER_SIZE];
            strncpy(temp, buffer, BUFFER_SIZE - 1);
            temp[BUFFER_SIZE - 1] = '\0';
            
            char* saveptr = NULL;
            char* token = strtok_r(temp + strlen(PROTO_PLAYER_INFO), ":", &saveptr);
            
            if (token) {
                PlayerInfo* player = &list->players[list->count];
                player->id = atoi(token);
                
                token = strtok_r(NULL, ":", &saveptr);
                if (token) {
                    strncpy(player->ip, token, sizeof(player->ip) - 1);
                    player->ip[sizeof(player->ip) - 1] = '\0';
                }
                
                token = strtok_r(NULL, ":", &saveptr);
                if (token) player->spectator_count = atoi(token);
                
                token = strtok_r(NULL, ":", &saveptr);
                if (token) player->max_spectators = atoi(token);
                
                printf("DEBUG: Parsed player #%d: %s [%d/%d]\n", 
                       player->id, player->ip, player->spectator_count, player->max_spectators);
                
                list->count++;
            }
        }
    }
    
    if (!list_ended) {
        printf("ERROR: Player list did not end properly\n");
        strcpy(list->error_message, "Incomplete data");
        list->show_error = true;
        return false;
    }
    
    printf("DEBUG: Fetch complete - %d players\n", list->count);
    return true;
}

static void draw_player_selection_screen(PlayerList* list, int selected_index, int client_id) {
    ClearBackground(UI_COLOR_BACKGROUND);
    
    font_manager_draw_client_id(client_id, "Spectator");
    
    const char* title = "ONLINE PLAYERS";
    int title_width = font_manager_measure_text(title, UI_FONT_SIZE_TITLE);
    int title_x = (UI_WINDOW_WIDTH - title_width) / 2;
    int title_y = 100;
    font_manager_draw_text(title, title_x, title_y, UI_FONT_SIZE_TITLE, UI_COLOR_TEXT);
    
    if (list->show_error) {
        int error_width = font_manager_measure_text(list->error_message, UI_FONT_SIZE_ERROR);
        int error_x = (UI_WINDOW_WIDTH - error_width) / 2;
        font_manager_draw_text(list->error_message, error_x, 200, UI_FONT_SIZE_ERROR, UI_COLOR_ERROR);
    }
    
    int start_y = 300;
    int spacing = 60;
    
    if (list->count == 0 && !list->show_error) {
        const char* no_players = "No players online";
        int text_width = font_manager_measure_text(no_players, UI_FONT_SIZE_NORMAL);
        int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
        font_manager_draw_text(no_players, text_x, start_y, UI_FONT_SIZE_NORMAL, GRAY);
    } else {
        for (int i = 0; i < list->count; i++) {
            PlayerInfo* player = &list->players[i];
            
            char player_text[256];
            snprintf(player_text, sizeof(player_text), 
                     "Player #%d - %s [%d/%d spectators]",
                     player->id, player->ip, player->spectator_count, player->max_spectators);
            
            Color color = (selected_index == i) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
            
            int text_width = font_manager_measure_text(player_text, UI_FONT_SIZE_NORMAL);
            int text_x = (UI_WINDOW_WIDTH - text_width) / 2;
            int text_y = start_y + (i * spacing);
            
            font_manager_draw_text(player_text, text_x, text_y, UI_FONT_SIZE_NORMAL, color);
        }
    }
    
    int button_y = UI_WINDOW_HEIGHT - 120;
    int selection_return = list->count;
    int selection_refresh = list->count + 1;
    
    const char* return_text = "[Return]";
    Color return_color = (selected_index == selection_return) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
    font_manager_draw_text(return_text, 100, button_y, UI_FONT_SIZE_NORMAL, return_color);
    
    const char* refresh_text = "[Refresh]";
    Color refresh_color = (selected_index == selection_refresh) ? UI_COLOR_SELECTED : UI_COLOR_TEXT;
    int refresh_width = font_manager_measure_text(refresh_text, UI_FONT_SIZE_NORMAL);
    font_manager_draw_text(refresh_text, UI_WINDOW_WIDTH - refresh_width - 100, button_y, UI_FONT_SIZE_NORMAL, refresh_color);
}

int show_player_selection_screen(Connection* conn, int client_id, const char* error_message) {
    printf("\n========================================\n");
    printf("Player Selection Screen Active\n");
    printf("========================================\n");
    
    PlayerList list = {0};
    
    printf("Fetching player list from server...\n\n");
    fetch_player_list(conn, &list);
    
    bool show_external_error = false;
    double external_error_time = 0.0;
    if (error_message && strlen(error_message) > 0) {
        show_external_error = true;
        external_error_time = GetTime();
        printf("DEBUG: External error to display: %s\n", error_message);
    }
    
    int selected_index = 0;
    int total_options = list.count + 2;
    bool done = false;
    int result = 0;
    
    double last_manual_refresh = 0.0;
    const double MANUAL_REFRESH_COOLDOWN = 1.0;
    
    while (!done && conn->connected && !WindowShouldClose()) {
        double current_time = GetTime();
        
        if (IsKeyPressed(KEY_DOWN)) {
            selected_index = (selected_index + 1) % total_options;
        }
        
        if (IsKeyPressed(KEY_UP)) {
            selected_index = (selected_index - 1 + total_options) % total_options;
        }
        
        if (IsKeyPressed(KEY_LEFT)) {
            if (selected_index >= list.count) {
                selected_index = list.count;
            }
        }
        
        if (IsKeyPressed(KEY_RIGHT)) {
            if (selected_index >= list.count) {
                selected_index = list.count + 1;
            }
        }
        
        if (IsKeyPressed(KEY_ENTER)) {
            if (selected_index < list.count) {
                result = list.players[selected_index].id;
                printf("DEBUG: Selected Player #%d\n", result);
                done = true;
            } else if (selected_index == list.count) {
                result = 0;
                printf("DEBUG: Return button pressed\n");
                done = true;
            } else {
                if (current_time - last_manual_refresh >= MANUAL_REFRESH_COOLDOWN) {
                    printf("DEBUG: Manual refresh triggered\n");
                    if (!fetch_player_list(conn, &list)) {
                        printf("ERROR: Refresh failed, connection may be lost\n");
                    } else {
                        total_options = list.count + 2;
                        if (selected_index >= total_options) {
                            selected_index = total_options > 0 ? total_options - 1 : 0;
                        }
                    }
                    last_manual_refresh = current_time;
                } else {
                    printf("DEBUG: Refresh on cooldown (%.1fs remaining)\n", 
                           MANUAL_REFRESH_COOLDOWN - (current_time - last_manual_refresh));
                }
            }
        }
        
        if (IsKeyPressed(KEY_R)) {
            result = 0;
            printf("DEBUG: R key pressed - returning to title\n");
            done = true;
        }
        
        if (show_external_error && (current_time - external_error_time >= 3.0)) {
            show_external_error = false;
        }
        
        BeginDrawing();
            draw_player_selection_screen(&list, selected_index, client_id);
            
            if (show_external_error) {
                int error_width = font_manager_measure_text(error_message, UI_FONT_SIZE_ERROR);
                int error_x = (UI_WINDOW_WIDTH - error_width) / 2;
                int error_y = UI_WINDOW_HEIGHT - 50;
                font_manager_draw_text(error_message, error_x, error_y, UI_FONT_SIZE_ERROR, UI_COLOR_ERROR);
            }
        EndDrawing();
    }
    
    return result;
}