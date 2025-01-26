#include "MainMenu.h"
#include <math.h>
#include <raylib.h>

void UpdateMainMenu(Game* game)
{
    game->menuArrowTimer += GetFrameTime() * 12.0f;

    if (game->state == LEADERBOARD)
    {
        if (IsKeyPressed(KEY_Q))
        {
            game->state = MAIN_MENU;
            game->inMenu = true;
            game->selectedOption = MENU_PLAY;
        }
        return;
    }

    if (game->state == TUTORIAL)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            game->state = PLAYING;
            game->inMenu = false;
        }

        if (IsKeyPressed(KEY_Q))
        {
            game->state = MAIN_MENU;
            game->inMenu = true;
            game->selectedOption = MENU_PLAY;
        }
        return;
    }

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
    {
        game->selectedOption--;

        if (game->selectedOption < 0)
        {
            game->selectedOption = MENU_COUNT - 1;
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
    {
        game->selectedOption++;

        if (game->selectedOption >= MENU_COUNT)
        {
            game->selectedOption = 0;
        }
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        switch (game->selectedOption)
        {
            case MENU_PLAY:
                ResetGame(game);
                game->state = PLAYING;
                game->inMenu = false;
            break;

            case MENU_LEADERBOARD:
                game->state = LEADERBOARD;
                game->inMenu = true;
            break;

            case MENU_TUTORIAL:
                game->state = TUTORIAL;
            break;

            case MENU_QUIT:
                game->shouldClose = true;
            break;
        }
    }
}

void DrawMainMenu(Game game)
{
    if (game.state == TUTORIAL)
    {
        DrawTutorial(game);
        return;
    }

    if (game.state == LEADERBOARD)
    {
        DrawLeaderboardScreen(&game.leaderboard, game.screenWidth, game.screenHeight);
        return;
    }

    const char* menuOptions[] =
    {
        "PLAY",
        "LEADERBOARD",
        "TUTORIAL",
        "QUIT"
    };

    // Calculate arrow opacity (0-1) using sine wave
    float arrowAlpha = (sinf(game.menuArrowTimer) + 1.0f) * 0.5f;
    const int spacing = 50;
    const int arrowSpacing = 20;


    const Vector2 menuStart =
    {
        game.screenWidth / 2,
        game.screenHeight / 2 - ((MENU_COUNT - 1) * spacing) / 2
    };

    const char* title = "BREAKOUT-C";

    DrawText(title,
        game.screenWidth/2 - MeasureText(title, FONT_TITLE_SIZE)/2,
        menuStart.y - 100,
        FONT_TITLE_SIZE,
        WHITE);

    // Draw menu options
    for (int i = 0; i < MENU_COUNT; i++)
    {
        Color optionColor = (i == game.selectedOption) ? GREEN : WHITE;
        int textWidth = MeasureText(menuOptions[i], FONT_SIZE);

        if (i == game.selectedOption)
        {
            // Create pulsing arrow color
            Color arrowColor = GREEN;
            arrowColor.a = (unsigned char)(255 * arrowAlpha);

            DrawText(">",
                menuStart.x - textWidth/2 - arrowSpacing,
                menuStart.y + i * spacing,
                FONT_SIZE,
                arrowColor);

            DrawText("<",
                menuStart.x + textWidth/2 + arrowSpacing - 10,
                menuStart.y + i * spacing,
                FONT_SIZE,
                arrowColor);
        }

        // Menu options
        DrawText(menuOptions[i],
            menuStart.x - textWidth/2,
            menuStart.y + i * spacing,
            FONT_SIZE,
            optionColor);
    }

    // Menu control hints
    const char* controlsText = "Navigate: UP/DOWN/W/S  |  Select: ENTER  |  Back: Q";
    const int controlsFontSize = 30;

    DrawText(controlsText,
        game.screenWidth/2 - MeasureText(controlsText, controlsFontSize)/2,
        game.screenHeight - 40,
        controlsFontSize,
        LIGHTGRAY);
}

void DrawTutorial(Game game)
{
    const char* title = "TUTORIAL";
    const int titleFontSize = 60;
    const int startY = game.screenHeight / 3;

    // Draw title
    DrawText(title,
        game.screenWidth/2 - MeasureText(title, titleFontSize)/2, //
        startY - 100,
        titleFontSize,
        WHITE);

    const char* controls[] =
    {
        "LEFT/RIGHT",
        "Move Paddle",
        "",
        "SHIFT",
        "Dash",
        "",
        "SPACE",
        "Shoot",
        "",
        "Press Q to return to Menu",
        ""
    };

    const int arraySize = sizeof(controls) / sizeof(controls[0]);

    for (int i = 0; i < arraySize; i++)
    {
        int textWidth = MeasureText(controls[i], FONT_SIZE);
        DrawText(controls[i],
                game.screenWidth/2 - textWidth/2,
                startY + i * 50,
                FONT_SIZE,
                WHITE);
    }
}