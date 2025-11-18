#ifndef SPECTATOR_SCREEN_H
#define SPECTATOR_SCREEN_H

#include "../../network/connection.h"
#include <stdbool.h>

/**
 * Display spectator screen while watching a player
 * Top-left: "Press Q to quit"
 * Top-right: "Spectating Player #X"
 * Q key returns to player selection
 * Handles PLAYER_LEFT_SESSION and PLAYER_DISCONNECTED kicks
 * @param conn Active server connection
 * @param player_id ID of player being spectated
 * @param client_id Client ID to display
 * @param kick_message Buffer to store kick reason if kicked
 * @param kick_message_size Size of kick_message buffer
 * @return true if user pressed Q, false if kicked by server
 */
bool show_spectator_screen(Connection* conn, int player_id, int client_id, char* kick_message, int kick_message_size);

#endif