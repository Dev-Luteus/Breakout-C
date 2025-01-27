#ifndef GAME_H
#define GAME_H

#include "Background.h"
#include "Player.h"
#include "Ball.h"
#include "Block.h"
#include "BlocksManager.h"
#include "PowerUp.h"
#include "Core.h"
#include "Leaderboard.h"

#define PADDING_TOP 40
#define PADDING_SIDE 60
#define FONT_SIZE 28

#define TITLE_FONT_SIZE 72
#define OPTIONS_FONT_SIZE 36

typedef struct Game
{
    int screenWidth;
    int screenHeight;

    RenderTexture2D gameTexture; // before background!
    Background background;

    GameState state;
    MenuOption selectedOption;
    bool inMenu;
    bool shouldClose;
    float menuArrowTimer;

    Player player;
    Ball ball;
    Block blocks[MAX_BLOCK_ROWS][BLOCK_COLUMNS];

    int combo;
    int maxCombo;
    int lastScoreGained;
    float lastScoreTimer;
    float dashEffect;

    int powerUpCount;
    PowerUp powerUps[10];
    PowerUpSpawnSystem spawnSystem;

    float timeScale;
    float normalTimeScale;

    Leaderboard leaderboard;

    float uiUpdateTimer;
    const float UI_UPDATE_INTERVAL;

    int currentLevel;
    const int maxLevels;
    int currentBlockRows;
} Game;

// Core!
Game InitGame(int width, int height);
void UpdateGame(Game* game);
void DrawGame(Game game);
void HandleCollisions(Game* game);
void ResetGame(Game* game);

// Power ups!
void HandlePowerUpCollisions(Game* game);
void UpdatePowerUps(Game* game);
void DrawPowerUps(Game* game);
void DrawPowerUpTimers(Game game);

// UI!
void DrawUI(Game* game);
void TransitionToMenu(Game* game);

// Progression!
void LoadNextLevel(Game* game);
int CalculateLevelBonus(int level, int currentScore);

#endif // GAME_H