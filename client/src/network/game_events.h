#ifndef GAME_EVENTS_H
#define GAME_EVENTS_H

#include "../network/connection.h"
#include <stdbool.h>

/**
 * Game Events - Client-side event sending to server
 * 
 * These functions notify the server of game events that have already occurred
 * on the client. The client is authoritative for game logic, but the server
 * maintains the authoritative score, lives, and level state.
 * 
 * Flow:
 * 1. Client detects event (collision, death, victory)
 * 2. Client shows immediate feedback (animation, popup, sound)
 * 3. Client sends event to server using these functions
 * 4. Server updates authoritative state
 * 5. Server sends confirmation back to client
 * 6. Client updates HUD with server values
 */

/**
 * Notify server that player collected a fruit
 * Client has already hidden fruit and shown popup
 * 
 * @param conn Connection to server
 * @param fruit_id Fruit identifier (for server tracking)
 * @param points Points awarded (200, 400, or 800)
 * @return true if sent successfully, false otherwise
 */
bool event_send_fruit_collected(Connection* conn, int fruit_id, int points);

/**
 * Notify server that player died
 * Client has already triggered death animation
 * 
 * @param conn Connection to server
 * @param cause Death cause: "enemy", "mario", or "water"
 * @return true if sent successfully, false otherwise
 */
bool event_send_player_died(Connection* conn, const char* cause);

/**
 * Notify server that player completed level (reached DK cage)
 * Client has already triggered victory animation
 * 
 * @param conn Connection to server
 * @return true if sent successfully, false otherwise
 */
bool event_send_level_completed(Connection* conn);

/**
 * Notify server that player respawned after death
 * Client is respawning after death (not game over)
 * 
 * @param conn Connection to server
 * @return true if sent successfully, false otherwise
 */
bool event_send_player_respawn(Connection* conn);

#endif
