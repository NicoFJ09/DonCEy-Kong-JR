#ifndef PLAYER_SELECTION_SCREEN_H
#define PLAYER_SELECTION_SCREEN_H

/**
 * Display player selection screen for spectators
 * Shows list of online players (mock data for Phase 4)
 * Arrow keys to navigate, ENTER to select
 * Bottom options: [Return] (left) and [Refresh] (right)
 * @return Selected player ID (1-based), 0 if Return selected, -1 if Refresh selected
 */
int show_player_selection_screen(void);

#endif