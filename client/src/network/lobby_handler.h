#ifndef LOBBY_HANDLER_H
#define LOBBY_HANDLER_H

#include <stdbool.h>
#include "connection.h"

/**
 * Handle complete lobby interaction flow
 * Loops until client is accepted or disconnects
 * @param conn Connection pointer
 * @return true if accepted and moving to game session, false if disconnected
 */
bool lobby_handle(Connection* conn);

#endif