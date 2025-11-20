#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

/**
 * Menu options for title screen
 */
typedef enum {
    MENU_PLAY,
    MENU_SPECTATE,
    MENU_EXIT
} MenuOption;

/**
 * Display title screen and get user selection
 * Shows logo at top (centered), 3 options below
 * Arrow keys to navigate, ENTER to select
 * Selected option displays in yellow, others in white
 * @param client_id Client ID to display (or -1 if not assigned)
 * @param error_message Optional error message to display (NULL or empty for none)
 * @return Selected MenuOption (PLAY, SPECTATE, or EXIT)
 */
MenuOption show_title_screen(int client_id, const char* error_message);

#endif