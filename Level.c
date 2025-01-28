#include <Level.h>
#include <raymath.h>
#include <stdio.h>
#include <tgmath.h>

void DrawLevelComplete(Game game)
{
    const char* completeText = "LEVEL COMPLETE!";
    const char* nextText = "Press SPACE to continue";

    char levelText[32];
    sprintf(levelText, "Level %d Complete!", game.currentLevel);

    // Base score
    char scoreText[64];
    sprintf(scoreText, "Score: %d", game.player.score - game.lastScoreGained);

    char comboText[64];
    sprintf(comboText, "Combo: x%d", game.maxCombo);

    // Calculate bonuses
    int baseBonus = LEVEL_BONUS_MULTIPLIER * game.currentLevel;
    int scoreBonus = (game.player.score - game.lastScoreGained) * SCORE_BONUS_MULTIPLIER;

    char baseBonusText[64];
    sprintf(baseBonusText, "Level Bonus: %d (1000 × Level %d)",
            baseBonus, game.currentLevel);

    char scoreBonusText[64];
    sprintf(scoreBonusText, "Score Bonus: %d (25%% of current score)",
            scoreBonus);

    char totalBonusText[64];
    sprintf(totalBonusText, "Total Bonus: %d", baseBonus + scoreBonus);

    char finalScoreText[64];
    sprintf(finalScoreText, "Final Score: %d", game.player.score + baseBonus + scoreBonus);

    // Calculate difficulty increases for next level
    float speedIncrease, widthDecrease;
    CalculateLevelProgression(game.currentLevel, &speedIncrease, &widthDecrease);

    char speedText[64];
    sprintf(speedText, "Max Ball Speed: +%.1f%%", speedIncrease);

    char widthText[64];
    sprintf(widthText, "Paddle Width: -%.1f%%", widthDecrease);

    // This Level Complete breakdown is really long, so I'm commenting just because it feels better
    int baseY = game.screenHeight/2 - BASE_Y_OFFSET * 1.6;

    // Title and Score Section
    DrawText(levelText,
        game.screenWidth/2 - MeasureText(levelText, TITLE_FONT_SIZE)/2,
        baseY,
        TITLE_FONT_SIZE,
        PLAYER_COLOR);

    DrawText(scoreText,
        game.screenWidth/2 - MeasureText(scoreText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING,
        OPTIONS_FONT_SIZE,
        WHITE);

    DrawText(comboText,
        game.screenWidth/2 - MeasureText(comboText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING,
        OPTIONS_FONT_SIZE,
        PLAYER_COLOR);

    // Bonus Section
    DrawText(baseBonusText,
        game.screenWidth/2 - MeasureText(baseBonusText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 2,
        OPTIONS_FONT_SIZE,
        GREEN);

    DrawText(scoreBonusText,
        game.screenWidth/2 - MeasureText(scoreBonusText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 3,
        OPTIONS_FONT_SIZE,
        GREEN);

    DrawText(totalBonusText,
        game.screenWidth/2 - MeasureText(totalBonusText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 4,
        OPTIONS_FONT_SIZE,
        GREEN);

    DrawText(finalScoreText,
        game.screenWidth/2 - MeasureText(finalScoreText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 5,
        OPTIONS_FONT_SIZE,
        WHITE);

    // Difficulty Changes Section
    DrawText(speedText,
        game.screenWidth/2 - MeasureText(speedText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 6 + 50,
        OPTIONS_FONT_SIZE,
        PU_SPEED_COLOR);

    DrawText(widthText,
        game.screenWidth/2 - MeasureText(widthText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 7 + 50,
        OPTIONS_FONT_SIZE,
        PU_GROWTH_COLOR);

    // Continue Text Section
    DrawText(nextText,
        game.screenWidth/2 - MeasureText(nextText, OPTIONS_FONT_SIZE)/2,
        baseY + TITLE_SPACING + NORMAL_SPACING * 10,
        OPTIONS_FONT_SIZE,
        BALL_COLOR);
}

void LoadNextLevel(Game* game)
{
    // Calculate and apply score bonuses
    int currentScore = game->player.score;
    int levelBonus = CalculateLevelBonus(game->currentLevel, currentScore);

    game->lastScoreGained = levelBonus;
    game->lastScoreTimer = SCORE_POPUP_DURATION;

    game->currentLevel++;
    game->player.score = currentScore + levelBonus;

    // Initialize and play =)
    InitializeLevel(game, game->currentLevel);
    game->state = PLAYING;
}

void CalculateLevelProgression(int currentLevel, float* speedIncrease, float* widthDecrease)
{
    float nextLevelFactor = (float)currentLevel;

    // Calculate speed increase
    float nextLevelMaxSpeed = BALL_SPEED_MAX + (BALL_SPEED_MAX_INCREMENT_PER_LEVEL * nextLevelFactor);
    *speedIncrease = ((nextLevelMaxSpeed - BALL_SPEED_MAX) / BALL_SPEED_MAX) * 100.0f;

    // Calculate width decrease
    int nextLevelWidth = fmax(100, PLAYER_BASE_WIDTH - (15 * nextLevelFactor));
    *widthDecrease = ((PLAYER_BASE_WIDTH - nextLevelWidth) / (float)PLAYER_BASE_WIDTH) * 100.0f;
}

void InitializeLevel(Game* game, int level)
{
    // Reset power-ups
    ResetAllPowerUpEffects(game);
    game->powerUpCount = 0;

    for (int i = 0; i < PU_MAX_COUNT; i++)
    {
        game->powerUps[i].active = false;
        game->powerUps[i].wasPickedUp = false;
    }

    // Initialize ball
    float levelFactor = (float)(level - 1);

    game->ball = InitBall((Vector2)
    {
        game->player.position.x + game->player.width / 2,
        game->player.position.y - 20
    });

    game->ball.active = false;
    game->ball.currentMinSpeed = BALL_SPEED_MIN + (BALL_SPEED_INCREMENT_PER_LEVEL * levelFactor);
    game->ball.currentMaxSpeed = BALL_SPEED_MAX + (BALL_SPEED_MAX_INCREMENT_PER_LEVEL * levelFactor);
    game->ball.speed = game->ball.currentMinSpeed;

    // Initialize player
    int widthReduction = 15 * levelFactor;
    game->player.baseWidth = fmax(100, PLAYER_BASE_WIDTH - widthReduction);
    game->player.width = game->player.baseWidth;
    game->combo = 0;

    // Initialize blocks
    game->currentBlockRows = Clamp(MIN_BLOCK_ROWS + (level - 1),
                                MIN_BLOCK_ROWS,
                                MAX_BLOCK_ROWS);

    game->currentBlockColumns = Clamp(MIN_BLOCK_COLUMNS + (level - 1),
                                   MIN_BLOCK_COLUMNS,
                                   MAX_BLOCK_COLUMNS);

    InitBlocks(game->blocks, game->screenWidth, game->screenHeight,
              game->currentBlockRows, game->currentBlockColumns,
              game->isTimewarpActive);

    game->inMenu = false;
}

int CalculateLevelBonus(int level, int currentScore)
{
    int baseBonus = LEVEL_BONUS_MULTIPLIER * level;
    int scoreBonus = currentScore * SCORE_BONUS_MULTIPLIER;

    return baseBonus + scoreBonus;
}