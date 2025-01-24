﻿#include "MainMenu.h"
#include <raylib.h>

void UpdateMainMenu(Game* game)
{
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
                game->state = PLAYING;
                game->inMenu = false;
            break;

            case MENU_LEADERBOARD:
                // TODO: Implement leaderboard
            break;

            case MENU_TUTORIAL:
                game->state = TUTORIAL;
            break;

            case MENU_QUIT:
                CloseWindow();
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

    const char* menuOptions[] =
    {
        "PLAY",
        "LEADERBOARD",
        "TUTORIAL",
        "QUIT"
    };

    const int fontSize = 30;
    const int spacing = 50;
    const int arrowSpacing = 20;

    const Vector2 menuStart =
    {
        game.screenWidth / 2,
        game.screenHeight / 2 - ((MENU_COUNT - 1) * spacing) / 2
    };

    const char* title = "BREAKOUT-C";
    const int titleFontSize = 60;

    DrawText(title,
        game.screenWidth/2 - MeasureText(title, titleFontSize)/2,
        menuStart.y - 100,
        titleFontSize,
        WHITE);

    // Draw menu options
    for (int i = 0; i < MENU_COUNT; i++)
    {
        Color optionColor = (i == game.selectedOption) ? GREEN : WHITE;
        int textWidth = MeasureText(menuOptions[i], fontSize);

        if (i == game.selectedOption)
        {
            DrawText(">",
                menuStart.x - textWidth/2 - arrowSpacing,
                menuStart.y + i * spacing,
                fontSize,
                GREEN);

            DrawText("<",
                menuStart.x + textWidth/2 + arrowSpacing - 10,
                menuStart.y + i * spacing,
                fontSize,
                GREEN);
        }

        // Menu options
        DrawText(menuOptions[i],
            menuStart.x - textWidth/2,
            menuStart.y + i * spacing,
            fontSize,
            optionColor);
    }

    // Menu control hints
    const char* controlsText = "Navigate: UP/DOWN/W/S  |  Select: ENTER  |  Back: Q";
    DrawText(controlsText,
        game.screenWidth/2 - MeasureText(controlsText, 24)/2,
        game.screenHeight - 40,
        24,
        LIGHTGRAY);
}

void DrawTutorial(Game game)
{
    const char* title = "TUTORIAL";
    const int titleFontSize = 60;
    const int startY = game.screenHeight / 3;

    // Draw title
    DrawText(title,
        game.screenWidth/2 - MeasureText(title, titleFontSize)/2,
        startY - 100,
        titleFontSize,
        WHITE);

    const char* controls[] =
    {
        "LEFT/RIGHT - Move Paddle",
        "SHIFT - Dash",
        "SPACE - Shoot",
        "Press Q to return to Menu",
        ""
    };

    const int arraySize = sizeof(controls) / sizeof(controls[0]);
    const int fontSize = 30;

    for (int i = 0; i < arraySize; i++)
    {
        int textWidth = MeasureText(controls[i], fontSize);
        DrawText(controls[i],
                game.screenWidth/2 - textWidth/2,
                startY + i * 50,
                fontSize,
                WHITE);
    }
}