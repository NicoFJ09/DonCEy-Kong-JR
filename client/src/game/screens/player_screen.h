#ifndef PLAYER_SCREEN_H
#define PLAYER_SCREEN_H

/**
 * Display player screen
 * Black background, hidden 10-second timer
 * Automatically transitions to lose screen after timer expires
 * No user input - just waits for timer
 * @param client_id Client ID to display
 */
void show_player_screen(int client_id);

#endif