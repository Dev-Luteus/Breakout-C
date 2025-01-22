#include <raylib.h>
#include "Game.h"

int main()
{
    const int width = 1920;
    const int height = 1080;

    InitWindow(width, height, "Block Kuzushi!");
    SetTargetFPS(1000);

    Game game = InitGame(width, height);

    while (!WindowShouldClose())
    {
        UpdateGame(&game);
        DrawGame(game);
    }

    CloseWindow();

    return 0;
}
