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
 * Shows "YOU LOST" title with final score
 * Press ENTER to return to title
 * @param final_score Final score achieved
 * @return Selected LoseOption (always LOSE_RETURN_TITLE)
 */
LoseOption show_lose_screen(int final_score);

#endif