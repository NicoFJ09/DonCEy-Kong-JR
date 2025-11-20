#ifndef LOSE_SCREEN_H
#define LOSE_SCREEN_H

/**
 * Options for lose screen
 */
typedef enum {
    LOSE_PLAY_AGAIN,
    LOSE_RETURN_TITLE
} LoseOption;

/**
 * Display lose screen after player loses
 * Shows "YOU LOST" title and 2 options
 * Arrow keys to navigate, ENTER to select
 * @param client_id Client ID to display
 * @return Selected LoseOption (PLAY_AGAIN or RETURN_TITLE)
 */
LoseOption show_lose_screen(int client_id);

#endif