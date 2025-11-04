#include <stdio.h>
#include "network/connection.h"
#include "network/lobby_handler.h"
#include "network/session_handler.h"
#include "utils/constants.h"

static void print_header(void) {
    printf("========================================\n");
    printf("DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

int main(void) {
    print_header();
    
    // Phase 1: Connect to server
    Connection* conn = connection_create(SERVER_IP, SERVER_PORT);
    if (!conn) {
        return 1;
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