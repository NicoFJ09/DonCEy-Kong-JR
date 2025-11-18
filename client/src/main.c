#include <stdio.h>
#include <string.h>
#include "network/connection.h"
#include "network/lobby_handler.h"
#include "network/session_handler.h"
#include "utils/constants.h"
#include "rendering/renderer.h"
#include "rendering/sprite_test.h"
#include "game/screens/ip_input_screen.h"

static void print_header(void) {
    printf("========================================\n");
    printf("DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

int main(void) {
    print_header();
    
    char server_ip[256];
    Connection* conn = NULL;
    
    // Initialize graphics FIRST
    printf("Initializing graphics...\n");
    renderer_init("DonCEy Kong Jr - Client", "assets/");
    printf("✓ Graphics window opened\n\n");
    
    // PHASE 1: IP Input Screen (graphical)
    bool got_ip = show_ip_input_screen(server_ip, sizeof(server_ip), false);
    
    if (!got_ip) {
        printf("User closed window during IP input\n");
        renderer_cleanup();
        return 0;
    }
    
    // SPECIAL: If user types "test", run isolated sprite test
    if (strcmp(server_ip, "test") == 0) {
        printf("Entering sprite test mode...\n");
        sprite_test_run();
        renderer_cleanup();
        return 0;
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
                renderer_cleanup();
                return 0;
            }
            
            // Check for test mode again
            if (strcmp(server_ip, "test") == 0) {
                printf("Entering sprite test mode...\n");
                sprite_test_run();
                renderer_cleanup();
                return 0;
            }
        }
    }
    
    printf("✓ Connected successfully!\n\n");
    
    // REST OF THE FLOW REMAINS TERMINAL-BASED FOR NOW
    // (Will be replaced in Phase 2+)
    
    // Lobby selection (terminal)
    bool accepted = lobby_handle(conn);
    
    // Game session (terminal)
    if (accepted && conn->connected) {
        session_handle(conn);
    }
    
    // Cleanup
    connection_close(conn);
    printf("\nDisconnected\n");
    renderer_cleanup();
    
    return 0;
}