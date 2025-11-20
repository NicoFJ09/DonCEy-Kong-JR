#ifndef GAME_FLOW_H
#define GAME_FLOW_H

#include <stdbool.h>

/**
 * Main game flow controller
 * Handles ALL screens: IP input, title, gameplay, etc.
 * @return true if user wants to exit normally, false if window closed
 */
bool game_flow_run(void);

#endif