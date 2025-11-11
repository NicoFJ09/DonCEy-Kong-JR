#include <stdio.h>
#include <string.h>
#include "network/connection.h"
#include "network/lobby_handler.h"
#include "network/session_handler.h"
#include "utils/constants.h"
#include "rendering/renderer.h"
#include "rendering/sprite_test.h"

static void print_header(void) {
    printf("========================================\n");
    printf("DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

static bool get_server_ip(char* ip_buffer, size_t buffer_size) {
    printf("Enter server IP address (or 'test' for sprite test): ");
    fflush(stdout);
    
    if (fgets(ip_buffer, buffer_size, stdin) == NULL) {
        return false;
    }
    
    ip_buffer[strcspn(ip_buffer, "\n")] = '\0';
    
    if (strlen(ip_buffer) == 0) {
        printf("Error: IP address cannot be empty\n");
        return false;
    }
    
    return true;
}

int main(void) {
    print_header();
    
    char server_ip[256];
    Connection* conn = NULL;
    
    // Initialize graphics FIRST
    printf("Initializing graphics...\n");
    renderer_init("DonCEy Kong Jr - Client", "assets/");
    printf("✓ Graphics window opened\n\n");
    
    // Get server IP
    while (conn == NULL) {
        if (!get_server_ip(server_ip, sizeof(server_ip))) {
            printf("Invalid input. Try again.\n\n");
            continue;
        }
        
        // SPECIAL: If user types "test", run isolated sprite test
        if (strcmp(server_ip, "test") == 0) {
            sprite_test_run();
            renderer_cleanup();
            return 0;
        }
        
        conn = connection_create(server_ip, SERVER_PORT);
        
        if (!conn) {
            printf("\nConnection failed. Check IP and try again.\n");
            printf("Press Enter to retry, or type 'quit' to exit: ");
            fflush(stdout);
            
            char retry[10];
            if (fgets(retry, sizeof(retry), stdin) != NULL) {
                retry[strcspn(retry, "\n")] = '\0';
                if (strcmp(retry, "quit") == 0 || strcmp(retry, "exit") == 0) {
                    printf("Exiting...\n");
                    renderer_cleanup();
                    return 1;
                }
            }
            printf("\n");
        }
    }
    
    // Lobby selection
    bool accepted = lobby_handle(conn);
    
    // Game session
    if (accepted && conn->connected) {
        session_handle(conn);
    }
    
    // Cleanup
    connection_close(conn);
    printf("\nDisconnected\n");
    renderer_cleanup();
    
    return 0;
}