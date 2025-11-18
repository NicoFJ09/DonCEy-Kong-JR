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
 * @return Selected MenuOption (PLAY, SPECTATE, or EXIT)
 */
MenuOption show_title_screen(void);

#endif