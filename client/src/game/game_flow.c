#include "game_flow.h"
#include "screens/ip_input_screen.h"
#include "screens/title_screen.h"
#include "../network/connection.h"
#include "../utils/constants.h"
#include "../rendering/sprite_test.h"
#include <stdio.h>
#include <string.h>

bool game_flow_run(void) {
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
    
    while (running && conn->connected) {
        MenuOption selected = show_title_screen();
        
        switch (selected) {
            case MENU_PLAY:
                printf("DEBUG: Selected Play\n");
                // TODO Phase 3: show_player_screen(conn);
                break;
                
            case MENU_SPECTATE:
                printf("DEBUG: Selected Spectate\n");
                // TODO Phase 4: show_player_selection_screen(conn);
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