#include <stdio.h>
#include "rendering/renderer.h"
#include "game/game_flow.h"

static void print_header(void) {
    printf("========================================\n");
    printf("DonCEy Kong Jr - Client\n");
    printf("========================================\n\n");
}

int main(void) {
    print_header();
    
    // Initialize graphics
    printf("Initializing graphics...\n");
    renderer_init("DonCEy Kong Jr - Client", "assets/");
    printf("✓ Graphics window opened\n\n");
    
    // Run game flow (handles everything)
    game_flow_run();
    
    // Cleanup
    renderer_cleanup();
    
    return 0;
}