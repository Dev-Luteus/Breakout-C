#ifndef CORE_H
#define CORE_H

typedef enum GameState
{
    MAIN_MENU,
    TUTORIAL,
    LEADERBOARD,
    PLAYING,
    GAME_OVER,
    WIN
} GameState;

typedef enum MenuOption
{
    MENU_PLAY,
    MENU_LEADERBOARD,
    MENU_TUTORIAL,
    MENU_QUIT,
    MENU_COUNT
} MenuOption;

#endif // CORE_H