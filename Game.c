﻿#include "Game.h"
#include <MainMenu.h>
#include <math.h>
#include <stdio.h>
#include <PowerUp.h>
#include <raymath.h>
#include <stdlib.h>
#include <time.h>

#include "Level.h"

Game InitGame(int width, int height)
{
    Game game = {
        .screenWidth = width,
        .screenHeight = height,
        .background = InitBackground(width, height),
        .gameTexture = LoadRenderTexture(width, height),

        .state = MAIN_MENU,
        .selectedOption = MENU_PLAY, // default
        .menuArrowTimer = 0.0f,
        .combo = 0,
        .maxCombo = 0,

        .lastScoreGained = 0,
        .lastScoreTimer = 0.0f,
        .dashEffect = 0.0f,

        .powerUpCount = 0,
        .spawnSystem = InitPowerUpSpawnSystem(),
        .isTimewarpActive = false,
        .timeScale = 1.0f,
        .normalTimeScale = 1.0f,

        .leaderboard = InitLeaderboard(),
        .uiUpdateTimer = 0.0f,
        .UI_UPDATE_INTERVAL = 1.0f/30.0f, // We want to render UI at 30 fps!

        .inMenu = true,
        .shouldClose = false,
        .currentLevel = 1,
        .maxLevels = 5,
        .currentBlockRows = MIN_BLOCK_ROWS,
        .currentBlockColumns = MIN_BLOCK_COLUMNS,
        .player.score = 0,
    };

    game.gameTexture = LoadRenderTexture(width, height);
    game.background = InitBackground(width, height);

    // Initialise blocks before player/etc
    InitBlocks(game.blocks, width, height, game.currentBlockRows, game.currentBlockColumns, game.isTimewarpActive);

    // Player, Ball, Blocks
    game.player = InitPlayer(width, height);

    Vector2 initialBallPos = MyVector2Create
    (
        game.player.position.x + game.player.width / 2,
        game.player.position.y - 20
    );

    game.ball = InitBall(initialBallPos);

    // Initialize all powerups to inactive
    for (int i = 0; i < 10; i++)
    {
        game.powerUps[i].active = false;
    }

    // Set random seed for powerup spawning
    srand(time(NULL));

    return game;
}

void HandleCollisions (Game* game)
{
    Rectangle playerRect =
    {
        game->player.position.x,
        game->player.position.y,
        game->player.width,
        game->player.height
    };

    // Bounce ball on collision with the player, depending on its angle
    if (CheckCollisionCircleRec(game->ball.position, game->ball.radius, playerRect))
    {
        // -1 to 1!
        float paddleCenter = game->player.position.x + game->player.width/2;
        float hitPosition = (game->ball.position.x - paddleCenter) / (game->player.width/2);

        // Here I want to define a 45 degree (PI/4) angle, as our maximum bounce (reflection) angle on collision
        float maxAngle = PI/4;
        float reflectionAngle = hitPosition * maxAngle;

        // Here we calculate our balls new direction on collision: sin hori, cos verti,
        Vector2 newDirection = MyVector2Create
        (
            sinf(reflectionAngle),
            -fabs(cosf(reflectionAngle))  // Force upward
        );

        game->ball.direction = MyVector2Normalize(newDirection);
    }

    // Give score to the player on ball/block collision and combo!
    for (int row = 0; row < game->currentBlockRows; row++)
    {
        for (int col = 0; col < game->currentBlockColumns; col++)
        {
            if (CheckBlockCollision(&game->blocks[row][col], &game->ball, game->isTimewarpActive))
            {
                game->combo++;
                game->maxCombo = fmax(game->combo, game->maxCombo);

                float comboMultiplier = 1.0f + (game->combo * COMBO_MULTIPLIER);
                int finalScore = BASE_SCORE * comboMultiplier;

                game->player.score += finalScore;
                game->lastScoreGained = finalScore;
                game->lastScoreTimer = SCORE_POPUP_DURATION;

                if (CheckPowerUpSpawn(&game->spawnSystem, game->combo, game->player.score, GetFrameTime()))
                {
                    Vector2 spawnPosition = MyVector2Create
                    (
                        game->blocks[row][col].position.x + game->blocks[row][col].width / 2,
                        game->blocks[row][col].position.y + game->blocks[row][col].height / 2
                    );

                    // Lazy so using rand() to randomly select a power-up!
                    PowerUpType type = rand() % POWERUP_COUNT;

                    for (int i = 0; i < 10; i++)
                    {
                        if (!game->powerUps[i].active)
                        {
                            float duration;

                            switch (type) {
                                case POWERUP_SPEED:
                                    duration = PU_SPEED_DURATION;
                                break;

                                case POWERUP_GROWTH:
                                    duration = PU_GROWTH_DURATION;
                                break;

                                case POWERUP_GHOST:
                                    duration = PU_GHOST_DURATION;
                                break;

                                case POWERUP_TIMEWARP:
                                    duration = PU_TIMEWARP_DURATION;
                                break;

                                case POWERUP_DAMAGE:
                                    duration = PU_DAMAGE_DURATION;
                                break;

                                case POWERUP_LIFE:
                                    duration = PU_DEFAULT_DURATION;
                                break;

                                default:
                                    duration = PU_DEFAULT_DURATION;
                                break;
                            }

                            game->powerUps[i] = CreatePowerUp(spawnPosition, type, duration);
                            game->powerUpCount++;

                            break;
                        }
                    }
                }
                return; // Return early to prevent multiple block hits per frame
            }
        }
    }
}

void UpdateGame(Game* game)
{
    float deltaTime = GetFrameTime() * game->timeScale;
    game->spawnSystem.cooldownTimer -= deltaTime; // power ups

    UpdateBackground(&game->background, deltaTime, game->isTimewarpActive);
    UpdatePlayerColor(&game->player, game->isTimewarpActive);

    /* We've seperated UI onto a different layer from the game.
     * Many games I play tend to render UI at lower framerates to save on performance
     * I agreed with this idea, so I'm trying to do this here! */

    if (game->state == PLAYING)
    {
        game->uiUpdateTimer += deltaTime;

        if (game->uiUpdateTimer >= game->UI_UPDATE_INTERVAL)
        {
            DrawUI(game);
            game->uiUpdateTimer = 0.0f;
        }
    }
    else
    {
        BeginTextureMode(game->background.uiTexture);
        {
            ClearBackground(BLANK);
        }
        EndTextureMode();
    }

    switch(game->state)
    {
        case MAIN_MENU:
        case TUTORIAL:
        case LEADERBOARD:
            UpdateMainMenu(game);
        break;

        case PLAYING:
        {
            if (!game->inMenu)
            {
                if (game->lastScoreTimer > 0)
                {
                    game->lastScoreTimer -= deltaTime;
                }

                // Update player movement and trail
                UpdatePlayerMovement(&game->player, deltaTime, game->screenWidth);

                // Ball shooting
                if (IsKeyPressed(KEY_SPACE) && !game->ball.active)
                {
                    Vector2 startPosition = MyVector2Create(
                        game->player.position.x + game->player.width / 2,
                        game->player.position.y - game->ball.radius
                    );

                    Vector2 initialDirection = MyVector2Create(0, -1);
                    ShootBall(&game->ball, startPosition, initialDirection, game->player);
                }

                // Update ball and handle screen collisions!
                // I want to make sure my ball can bounce on screen edges, but also create a "killZone" at the bottom!
                if (game->ball.active)
                {
                    UpdateBall(&game->ball, deltaTime, game->screenWidth, game->screenHeight);
                    HandleCollisions(game);

                    // Here I handle our Killzone!
                    if (game->ball.position.y > game->screenHeight)
                    {
                        game->player.lives--;
                        game->ball.active = false;
                        game->combo = 0;  // Reset combo

                        if (game->player.lives <= 0)
                        {
                            AddLeaderboardEntry(&game->leaderboard, game->player.score, game->maxCombo);
                            game->state = GAME_OVER;
                        }
                    }
                }
                else // Ball is not active
                {
                    // Update ball position to follow player when not launched
                    game->ball.position = MyVector2Create
                    (
                        game->player.position.x + game->player.width / 2,
                        game->player.position.y - game->ball.radius
                    );
                }

                // Check win condition
                if (AreAllBlocksDestroyed(game->blocks, game->currentBlockRows, game->currentBlockColumns))
                {
                    if (game->currentLevel == game->maxLevels)
                    {
                        AddLeaderboardEntry(&game->leaderboard, game->player.score, game->maxCombo);
                        game->state = WIN;
                    }
                    else
                    {
                        game->state = LEVEL_COMPLETE;
                    }
                }

                // Debug power-up info
                for (int i = 0; i < PU_MAX_COUNT; i++)
                {
                    PowerUp* powerUp = &game->powerUps[i];

                    if (powerUp->active && powerUp->wasPickedUp)
                    {
                        printf("Active powerup %d: %.2f remaining, active=%d, picked=%d\n",
                               powerUp->type, powerUp->remainingDuration,
                               powerUp->active, powerUp->wasPickedUp);
                    }
                }

                UpdatePowerUps(game);
                HandlePowerUpCollisions(game);
            }
        } break;

        case LEVEL_COMPLETE:
            if (IsKeyPressed(KEY_SPACE))
            {
                LoadNextLevel(game);
            }
        break;

        case GAME_OVER:
        case WIN:
            if (IsKeyPressed(KEY_R))
            {
                ResetGame(game);
            }
            else if (IsKeyPressed(KEY_Q))
            {
                TransitionToMenu(game);
            }
        break;
    }
}

/* Just for clearer seperation of concers, I've moved the UI drawing to a seperate function
 * I also do this because we now draw the UI on a seperate layer from the rest of the game =) */
void DrawUI(Game* game)
{
    BeginTextureMode(game->background.uiTexture);
    ClearBackground(BLANK);

    BeginTextureMode(game->background.uiTexture);
    {
        if (game->state == PLAYING && !game->inMenu)
        {
            char comboText[64];

            if (game->combo > 0)
            {
                float multiplier = 1.0f + (game->combo * 0.1f);
                sprintf(comboText, "Combo: %d (x%.1f)", game->combo, multiplier);
            }
            else
            {
                sprintf(comboText, "Combo: 0");
            }

            DrawText(comboText,
                game->screenWidth/2 - MeasureText(comboText, FONT_SIZE)/2,
                PADDING_TOP,
                FONT_SIZE,
                game->combo > 0 ? PLAYER_COLOR : BALL_COLOR);

            // Score popup
            if (game->lastScoreTimer > 0)
            {
                char scorePopup[32];
                sprintf(scorePopup, "+%d", game->lastScoreGained);
                float alpha = game->lastScoreTimer;
                Color popupColor = {0, 255, 0, (unsigned char)(alpha * 255)};
                DrawText(scorePopup,
                    game->screenWidth/2 - MeasureText(scorePopup, FONT_SIZE)/2,
                    PADDING_TOP + FONT_SIZE + 10,
                    FONT_SIZE,
                    popupColor);
            }

            char scoreText[32];
            sprintf(scoreText, "Score: %d", game->player.score);
            DrawText(scoreText,
                PADDING_SIDE,
                PADDING_TOP,
                FONT_SIZE,
                WHITE);

            char livesText[32];
            sprintf(livesText, "Lives: %d", game->player.lives);
            int livesTextWidth = MeasureText(livesText, FONT_SIZE);
            DrawText(livesText,
                game->screenWidth - livesTextWidth - PADDING_SIDE,
                PADDING_TOP,
                FONT_SIZE,
                WHITE);

            char levelText[32];
            sprintf(levelText, "Level %d", game->currentLevel);
            int levelTextWidth = MeasureText(levelText, FONT_SIZE);
            DrawText(levelText,
                game->screenWidth - levelTextWidth - PADDING_SIDE,
                game->screenHeight - PADDING_TOP * 1.5,
                FONT_SIZE,
                BALL_COLOR);

            DrawPowerUpTimers(*game);
        }
        EndTextureMode();
    }
}

void DrawGame(Game game)
{
    BeginTextureMode(game.gameTexture); // Render all of this into our game.gameTexture
    {
        ClearBackground(BLACK);

        switch(game.state)
        {
            case PLAYING:
                DrawPlayerWithTrail(&game.player);
                DrawBlocks(game.blocks, game.currentBlockRows, game.currentBlockColumns);
                DrawBall(game.ball);
                DrawPowerUps(&game);
            break;

            case MAIN_MENU:
            case TUTORIAL:
            case LEADERBOARD:
                DrawMainMenu(game);
            break;

            case LEVEL_COMPLETE:
            {
                DrawLevelComplete(game);
            }
            break;

            case GAME_OVER:
            case WIN:
            {
                const char* restartText = "Press R to Restart";
                const char* menuText = "Press Q for Menu";

                const char* titleText = (game.state == WIN) ? "YOU WIN!" : "GAME OVER";
                Color titleColor = (game.state == WIN) ? PLAYER_COLOR : PU_DAMAGE_COLOR;

                char finalScoreText[64];
                sprintf(finalScoreText, "Final Score: %d", game.player.score);
                char maxComboText[64];
                sprintf(maxComboText, "Max Combo: %d", game.maxCombo);

                int titleWidth = MeasureText(titleText, TITLE_FONT_SIZE);
                int scoreWidth = MeasureText(finalScoreText, OPTIONS_FONT_SIZE);
                int comboWidth = MeasureText(maxComboText, OPTIONS_FONT_SIZE);
                int restartWidth = MeasureText(restartText, OPTIONS_FONT_SIZE);
                int menuWidth = MeasureText(menuText, OPTIONS_FONT_SIZE);

                int baseY = game.screenHeight/2 - BASE_Y_OFFSET;

                DrawText(titleText,
                    game.screenWidth/2 - titleWidth/2,
                    baseY,
                    TITLE_FONT_SIZE,
                    titleColor);

                DrawText(finalScoreText,
                    game.screenWidth/2 - scoreWidth/2,
                    baseY + TITLE_SPACING,
                    OPTIONS_FONT_SIZE,
                    WHITE);

                DrawText(maxComboText,
                    game.screenWidth/2 - comboWidth/2,
                    baseY + TITLE_SPACING + NORMAL_SPACING,
                    OPTIONS_FONT_SIZE,
                    PLAYER_COLOR);

                DrawText(restartText,
                    game.screenWidth/2 - restartWidth/2,
                    baseY + TITLE_SPACING + NORMAL_SPACING * 2,
                    OPTIONS_FONT_SIZE,
                    BALL_COLOR);

                DrawText(menuText,
                    game.screenWidth/2 - menuWidth/2,
                    baseY + TITLE_SPACING + NORMAL_SPACING * 3,
                    OPTIONS_FONT_SIZE,
                    PU_SPEED_COLOR);
            }
            break;
        }
    }
    EndTextureMode();

    BeginDrawing();
    {
        ClearBackground(BLACK);

        /* 1. Game Elements → game.gameTexture
         *   ↓
         * 2. Background Effects:
         *    a. Static Effects → background.staticEffects
         *    b. Dynamic Effects → background.effectTexture
         *    c. Distortion & Composition → background.finalTexture
         *    ↓
         * 3. UI Elements → background.uiTexture (DrawUI) =)
         *    ↓
         * 4. Final Screen Composition:
         *    - Background (with game elements and effects)
         *    - UI layer on top */

        // Then, we draw the game.gameTexture that we rendered in BeginTextureMode above^^
        DrawBackground(&game.background, game.screenWidth, game.screenHeight,
                      game.gameTexture.texture);

        // UI
        DrawTexturePro(game.background.uiTexture.texture,
              (Rectangle){ 0, 0,
                         game.background.uiTexture.texture.width,
                         -game.background.uiTexture.texture.height }, // - to flip vertically
              (Rectangle){ 0, 0,
                         game.screenWidth,
                         game.screenHeight },
              (Vector2){ 0, 0 }, 0, WHITE);

        // DrawTexturePro(
        //     texture,          // The texture to draw
        //     sourceRec,        // What part of the texture to use
        //     destRec,          // Where to draw it on screen
        //     origin,           // Rotation origin point
        //     rotation,
        //     tint
        // );
    }
    EndDrawing();
}

void TransitionToMenu(Game* game)
{
    game->state = MAIN_MENU;
    game->selectedOption = MENU_PLAY;
    game->inMenu = true;
    game->combo = 0;
    game->maxCombo = 0;
    game->currentLevel = 1;
    game->currentBlockRows = MIN_BLOCK_ROWS;

    ResetGame(game);

    // Override the PLAYING state set by ResetGame
    game->state = MAIN_MENU;
    game->inMenu = true;

    BeginTextureMode(game->background.uiTexture);
    {
        ClearBackground(BLANK);
    }
    EndTextureMode();
}

// Reinitialise everything on reset 'R' !
void ResetGame(Game* game)
{
    // Reset power-ups to not save them through retries
    game->powerUpCount = 0;
    game->spawnSystem = InitPowerUpSpawnSystem(); // Reset spawn system (timers etc)

    game->timeScale = game->normalTimeScale;

    for (int i = 0; i < 10; i++)
    {
        game->powerUps[i].active = false;
    }

    // Default values
    game->player = InitPlayer(game->screenWidth, game->screenHeight);
    game->ball = InitBall((Vector2){0, 0});

    game->combo = 0;
    game->maxCombo = 0;
    game->currentLevel = 1;
    game->currentBlockRows = MIN_BLOCK_ROWS;
    game->currentBlockColumns = MIN_BLOCK_COLUMNS;
    game->ball.speed = BALL_SPEED_MIN;
    game->player.width = game->player.baseWidth;
    game->player.score = 0;

    InitBlocks(game->blocks, game->screenWidth, game->screenHeight,
           game->currentBlockRows, game->currentBlockColumns,
           game->isTimewarpActive);

    game->state = PLAYING;
}