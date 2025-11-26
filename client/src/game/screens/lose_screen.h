#ifndef LOSE_SCREEN_H
#define LOSE_SCREEN_H

/**
 * Options for lose screen
 */
typedef enum {
    LOSE_RETURN_TITLE = 0
} LoseOption;

/**
 * Display lose screen after player loses
 * Shows "YOU LOST" title and 2 options
 * Arrow keys to navigate, ENTER to select
 * @param client_id Client ID to display
 * @return Selected LoseOption (always LOSE_RETURN_TITLE)
 */
LoseOption show_lose_screen(int client_id);

#endif