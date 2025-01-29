#include <raylib.h>
#include "Game.h"

int main()
{
    const int width = 1920;
    const int height = 1080;

    InitWindow(width, height, "Block Kuzushi!");
    SetTargetFPS(400);

    Game game = InitGame(width, height);

    while ((!WindowShouldClose() && !game.shouldClose))
    {
        UpdateGame(&game);
        DrawGame(game);
    }

    // In my coding rush, I forgot to prevent a memory leak of my render textures.
    UnloadRenderTexture(game.gameTexture);
    UnloadBackground(&game.background);

    CloseWindow();

    return 0;
}
