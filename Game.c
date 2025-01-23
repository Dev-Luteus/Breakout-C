#include "Game.h"
#include <math.h>
#include <raymath.h>
#include <stdio.h>
#include <PowerUp.h>
#include <stdlib.h>
#include <time.h>

#define PADDING_TOP 40
#define PADDING_SIDE 60
#define FONT_SIZE 28

Game InitGame(int width, int height)
{
    Game game = {
        .screenWidth = width,
        .screenHeight = height,

        .state = TUTORIAL,
        .combo = 0,
        .maxCombo = 0,

        .lastScoreGained = 0,
        .lastScoreTimer = 0.0f,
        .dashEffect = 0.0f,

        .powerUpCount = 0,
        .spawnSystem = InitPowerUpSpawnSystem()
    };

    // Player, Ball, Blocks
    game.player = InitPlayer(width, height);

    Vector2 initialBallPos =
    {
        game.player.position.x + game.player.width / 2,
        game.player.position.y - 20
    };
    game.ball = InitBall(initialBallPos);

    InitBlocks(game.blocks, width, height);

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
        game->ball.direction.x = sinf(reflectionAngle);
        game->ball.direction.y = -fabs(cosf(reflectionAngle));  // Force upward

        // Here, we normalize our direction vector by taking its total length X and Y / 2
        float length = sqrtf
        (
            game->ball.direction.x * game->ball.direction.x +
            game->ball.direction.y * game->ball.direction.y
        );

        game->ball.direction.x /= length;
        game->ball.direction.y /= length;
    }

    // Give score to the player on ball/block collision and combo!
    for (int row = 0; row < BLOCK_ROWS; row++)
    {
        for (int col = 0; col < BLOCK_COLUMNS; col++)
        {
            if (CheckBlockCollision(&game->blocks[row][col], &game->ball))
            {
                game->combo++;
                game->maxCombo = fmax(game->combo, game->maxCombo);

                int baseScore = 100;
                float comboMultiplier = 1.0f + (game->combo * 0.1f); // 10% per combo
                int finalScore = baseScore * comboMultiplier;

                game->player.score += finalScore;

                // Store the last score gained for display + popUp time
                game->lastScoreGained = finalScore;
                game->lastScoreTimer = 1.0f;

                if (CheckPowerUpSpawn(&game->spawnSystem, game->combo, game->player.score, GetFrameTime()))
                {
                    Vector2 spawnPosition =
                    {
                        game->blocks[row][col].position.x + game->blocks[row][col].width / 2,
                        game->blocks[row][col].position.y + game->blocks[row][col].height / 2
                    };

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
            }
        }
    }
}

void UpdateGame(Game* game)
{
    float deltaTime = GetFrameTime();

    switch(game->state)
    {
        case TUTORIAL:
            if (IsKeyPressed(KEY_ENTER))
            {
                game->state = PLAYING;
            }
        break;

        case PLAYING:
        {
            // Reset dash effect at the start of each frame
            game->dashEffect = fmax(game->dashEffect - deltaTime * 5, 0.0f);

            if (game->lastScoreTimer > 0)
            {
                game->lastScoreTimer -= deltaTime;
            }

            // Player movement
            if (IsKeyDown(KEY_RIGHT))
            {
                game->player.position.x += game->player.speed * deltaTime;

                if (IsKeyDown(KEY_LEFT_SHIFT))
                {
                    game->player.position.x += game->player.speed * deltaTime * PLAYER_SPEED_BOOST;
                    game->dashEffect = fmin(game->dashEffect + deltaTime * 10, 1.0f);
                }
            }

            if (IsKeyDown(KEY_LEFT))
            {
                game->player.position.x -= game->player.speed * deltaTime;

                if (IsKeyDown(KEY_LEFT_SHIFT))
                {
                    game->player.position.x -= game->player.speed * deltaTime * PLAYER_SPEED_BOOST;
                    game->dashEffect = fmin(game->dashEffect + deltaTime * 10, 1.0f);
                }
            }

            game->player.position.x = Clamp
            (
                game->player.position.x,
                0,
                game->screenWidth -game->player.width
            );

            if (IsKeyPressed(KEY_SPACE) && !game->ball.active)
            {
                Vector2 startPosition = (Vector2)
                {
                    game->player.position.x + game->player.width / 2,
                    game->player.position.y - game->ball.radius
                };

                ShootBall(&game->ball, startPosition, (Vector2){0, -1}, game->player);
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
                        game->state = GAME_OVER;
                    }
                }
            }
            // Check win condition
            if (AreAllBlocksDestroyed(game->blocks))
            {
                game->state = WIN;
            }

            // Debug testing
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

        } break;

        case GAME_OVER:
        case WIN:
            if (IsKeyPressed(KEY_R))
            {
                ResetGame(game);
            }
        break;
    }
}

void DrawGame(Game game)
{
    BeginDrawing();
    {
        ClearBackground(BLACK);

        switch(game.state)
        {
            case TUTORIAL:
            {
                const char* controls[] = {
                    "LEFT/RIGHT - Move Paddle",
                    "SHIFT - Dash",
                    "SPACE - Shoot",
                    "Press ENTER to Play"
                };

                int startY = game.screenHeight / 3;

                for (int i = 0; i < 4; i++)
                {
                    int textWidth = MeasureText(controls[i], 30);

                    DrawText(controls[i],
                            game.screenWidth/2 - textWidth/2,
                            startY + i * 50,
                            30,
                            WHITE);
                }
            } break;

            case PLAYING:
            {
                // Here we Draw the player dash trail effect when dashing!
                if (game.dashEffect > 0 && IsKeyDown(KEY_LEFT_SHIFT))
                {
                    bool movingRight = IsKeyDown(KEY_RIGHT);
                    bool movingLeft = IsKeyDown(KEY_LEFT);

                    if (movingRight || movingLeft)
                    {
                        for (int i = 1; i <= 5; i++)
                        {
                            // Decrease alpha as i increases!
                            float alpha = (5 - i) / 5.0f * game.dashEffect;
                            Color trailColor = {255, 255, 255, (unsigned char)(alpha * 128)};

                            float offset = (movingRight ? -1 : 1) * i * 15;

                            DrawRectangle(
                                game.player.position.x,
                                game.player.position.y,
                                game.player.width,
                                game.player.height,
                                ColorAlpha(PLAYER_COLOR, alpha * 0.5f)
                            );
                        }
                    }
                }

                // Draw our player after our trail!
                DrawRectangle(
                    game.player.position.x,
                    game.player.position.y,
                    game.player.width,
                    game.player.height,
                    PLAYER_COLOR
                );

                DrawBlocks(game.blocks);
                DrawBall(game.ball);
                DrawPowerUps(&game);

                char comboText[64];
                if (game.combo > 0)
                {
                    float multiplier = 1.0f + (game.combo * 0.1f);
                    sprintf(comboText, "Combo: %d (x%.1f)", game.combo, multiplier);
                }
                else
                {
                    sprintf(comboText, "Combo: 0");
                }

                DrawText(comboText,
                    game.screenWidth/2 - MeasureText(comboText, FONT_SIZE)/2,
                    PADDING_TOP,
                    FONT_SIZE,
                    game.combo > 0 ? GREEN : WHITE);

                // Here we want to show the last score gained, if our timer is active
                if (game.lastScoreTimer > 0)
                {
                    char scorePopup[32];
                    sprintf(scorePopup, "+%d", game.lastScoreGained);

                    // Here we add a fade out effect to our timer!
                    float alpha = game.lastScoreTimer;
                    Color popupColor = {0, 255, 0, (unsigned char)(alpha * 255)};

                    DrawText(scorePopup,
                        game.screenWidth/2 - MeasureText(scorePopup, FONT_SIZE)/2,
                        PADDING_TOP + FONT_SIZE + 10,
                        FONT_SIZE,
                        popupColor);
                }

                char scoreText[32];
                sprintf(scoreText, "Score: %d", game.player.score);
                int scoreTextWidth = MeasureText(scoreText, FONT_SIZE);
                DrawText(scoreText,
                    PADDING_SIDE,
                    PADDING_TOP,
                    FONT_SIZE, WHITE);

                char livesText[32];
                sprintf(livesText, "Lives: %d", game.player.lives);
                int livesTextWidth = MeasureText(livesText, FONT_SIZE);
                DrawText(livesText,
                    game.screenWidth - livesTextWidth - PADDING_SIDE,
                    PADDING_TOP,
                    FONT_SIZE, WHITE);
            } break;

            case GAME_OVER:
            {
                DrawText("GAME OVER", game.screenWidth/2 - 125, game.screenHeight/2, 40, RED);
                DrawText("Press R to Restart", game.screenWidth/2 - 100, game.screenHeight/2 + 50, 20, WHITE);
            } break;

            case WIN:
            {
                DrawText("YOU WIN!", game.screenWidth/2 - 100, game.screenHeight/2, 40, GREEN);
                DrawText("Press R to Restart", game.screenWidth/2 - 100, game.screenHeight/2 + 50, 20, WHITE);
            } break;
        }
    }
    EndDrawing();
}

// Reinitialise everything on reset 'R' !
void ResetGame(Game* game)
{
    // Reset power-ups to not save them through retries
    game->powerUpCount = 0;

    for (int i = 0; i < 10; i++)
    {
        game->powerUps[i].active = false;
    }

    // Default values
    game->player = InitPlayer(game->screenWidth, game->screenHeight);
    game->ball = InitBall((Vector2){0, 0});

    InitBlocks(game->blocks, game->screenWidth, game->screenHeight);
    game->state = PLAYING;
}