#ifndef PLAYER_SELECTION_SCREEN_H
#define PLAYER_SELECTION_SCREEN_H

#include "../../network/connection.h"

/**
 * Display player selection screen for spectators
 * Fetches real player list from server
 * Auto-refreshes every 5 seconds
 * Arrow keys to navigate, ENTER to select
 * Bottom options: [Return] (left) and [Refresh] (right)
 * @param conn Active server connection
 * @param client_id Client ID to display
 * @param error_message Optional error message to display (can be NULL or empty)
 * @return Selected player ID (1-based), 0 if Return selected, -1 if Refresh selected
 */
int show_player_selection_screen(Connection* conn, int client_id, const char* error_message);

#endif