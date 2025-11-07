#include <stdio.h>
#include <string.h>
#include "network/connection.h"
#include "network/lobby_handler.h"
#include "network/session_handler.h"
#include "utils/constants.h"

static void print_header(void) {
    printf("========================================\n");
    printf("DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

static bool get_server_ip(char* ip_buffer, size_t buffer_size) {
    printf("Enter server IP address: ");
    fflush(stdout);
    
    if (fgets(ip_buffer, buffer_size, stdin) == NULL) {
        return false;
    }
    
    // Remove trailing newline
    ip_buffer[strcspn(ip_buffer, "\n")] = '\0';
    
    // Basic validation: not empty
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
    
    // Phase 1: Get server IP and connect (with retry)
    while (conn == NULL) {
        if (!get_server_ip(server_ip, sizeof(server_ip))) {
            printf("Invalid input. Try again.\n\n");
            continue;
        }
        
        conn = connection_create(server_ip, SERVER_PORT);
        
        if (!conn) {
            printf("\nConnection failed. Please check the IP address and try again.\n");
            printf("Press Enter to retry, or type 'quit' to exit: ");
            fflush(stdout);
            
            char retry[10];
            if (fgets(retry, sizeof(retry), stdin) != NULL) {
                retry[strcspn(retry, "\n")] = '\0';
                if (strcmp(retry, "quit") == 0 || strcmp(retry, "exit") == 0) {
                    printf("Exiting...\n");
                    return 1;
                }
            }
            printf("\n");
        }
    }
    
    // Phase 2: Lobby selection
    bool accepted = lobby_handle(conn);
    
    // Phase 3: Game session
    if (accepted && conn->connected) {
        session_handle(conn);
    }
    
    // Phase 4: Cleanup
    connection_close(conn);
    printf("\nDisconnected\n");
    
    return 0;
}