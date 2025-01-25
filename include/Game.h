﻿#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Ball.h"
#include "Block.h"
#include "BlocksManager.h"
#include "PowerUp.h"
#include "Core.h"
#include "Leaderboard.h"

typedef struct Game
{
    int screenWidth;
    int screenHeight;

    GameState state;
    MenuOption selectedOption;
    bool inMenu;
    bool shouldClose;
    float menuArrowTimer;

    Player player;
    Ball ball;
    Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];

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

#endif // GAME_H