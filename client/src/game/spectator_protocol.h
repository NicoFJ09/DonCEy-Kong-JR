#ifndef SPECTATOR_PROTOCOL_H
#define SPECTATOR_PROTOCOL_H

#include "spectator_state.h"
#include <stdbool.h>

/**
 * Parse GAME_STATE: message from server
 * @param message Full message including "GAME_STATE:" prefix
 * @param snapshot Output snapshot
 * @return true if parsed successfully
 */
bool parse_game_state_message(const char* message, GameStateSnapshot* snapshot);

#endif