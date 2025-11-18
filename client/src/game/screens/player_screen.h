#ifndef PLAYER_SCREEN_H
#define PLAYER_SCREEN_H

/**
 * Display player screen
 * Black background, hidden 10-second timer
 * Automatically transitions to lose screen after timer expires
 * No user input - just waits for timer
 */
void show_player_screen(void);

#endif