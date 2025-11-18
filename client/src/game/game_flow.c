#include "game_flow.h"
#include "screens/ip_input_screen.h"
#include "screens/title_screen.h"
#include "screens/player_screen.h"
#include "screens/lose_screen.h"
#include "screens/player_selection_screen.h"
#include "../network/connection.h"
#include "../utils/constants.h"
#include "../utils/font_manager.h"
#include "../rendering/sprite_test.h"
#include <stdio.h>
#include <string.h>

bool game_flow_run(void) {
    font_manager_init();

    char server_ip[256];
    Connection* conn = NULL;
    
    // Get IP from user
    bool got_ip = show_ip_input_screen(server_ip, sizeof(server_ip), false);
    
    if (!got_ip) {
        printf("User closed window during IP input\n");
        return false;
    }
    
    // SPECIAL: If user types "test", run sprite test
    if (strcmp(server_ip, "test") == 0) {
        printf("Entering sprite test mode...\n");
        sprite_test_run();
        return true;
    }
    
    // Connection loop with retry
    while (!conn) {
        printf("\nAttempting connection to %s:%d...\n", server_ip, SERVER_PORT);
        conn = connection_create(server_ip, SERVER_PORT);
        
        if (!conn) {
            printf("Connection failed!\n");
            
            // Show error screen and allow retry
            got_ip = show_ip_input_screen(server_ip, sizeof(server_ip), true);
            
            if (!got_ip) {
                printf("User closed window\n");
                return false;
            }
            
            // Check for test mode again
            if (strcmp(server_ip, "test") == 0) {
                printf("Entering sprite test mode...\n");
                sprite_test_run();
                return true;
            }
        }
    }
    
    printf("✓ Connected successfully!\n\n");
    
    // Main game loop - title screen and gameplay
    bool running = true;
    
    while (running && conn->connected && !WindowShouldClose()) {
        MenuOption selected = show_title_screen();
        
        switch (selected) {
            case MENU_PLAY:
                printf("DEBUG: Selected Play\n");
                
                // Player screen loop (with lose screen)
                bool playing = true;
                while (playing && conn->connected && !WindowShouldClose()) {
                    show_player_screen();
                    
                    LoseOption choice = show_lose_screen();
                    
                    switch (choice) {
                        case LOSE_PLAY_AGAIN:
                            break;
                            
                        case LOSE_RETURN_TITLE:
                            playing = false;
                            break;
                    }
                }
                break;
                
            case MENU_SPECTATE:
                printf("DEBUG: Selected Spectate\n");
                
                // Player selection loop
                bool in_spectate = true;
                while (in_spectate && conn->connected && !WindowShouldClose()) {
                    int player_id = show_player_selection_screen();
                    
                    if (player_id > 0) {
                        // Player selected
                        printf("DEBUG: User selected Player #%d\n", player_id);
                        // TODO Phase 5: show_spectator_screen(conn, player_id);
                        // For now, stay in selection loop
                    } else if (player_id == -1) {
                        // Refresh selected - loop back to show updated list
                        printf("DEBUG: Refresh - reloading player list\n");
                    } else {
                        // Return selected (player_id == 0)
                        printf("DEBUG: Return - going back to title\n");
                        in_spectate = false;
                    }
                }
                break;
                
            case MENU_EXIT:
                printf("DEBUG: Selected Exit\n");
                running = false;
                break;
        }
    }
    
    // Cleanup connection
    connection_close(conn);
    printf("\nDisconnected\n");
    
    return true;
}