#ifndef PLAYER_SCREEN_H
#define PLAYER_SCREEN_H

#include "../../network/connection.h"

/**
 * Display player screen
 * Black background, hidden 10-second timer
 * Automatically transitions to lose screen after timer expires
 * No user input - just waits for timer
 * @param client_id Client ID to display
 * @param conn Connection pointer (for map JSON access)
 */
void show_player_screen(int client_id, Connection* conn);

#endif