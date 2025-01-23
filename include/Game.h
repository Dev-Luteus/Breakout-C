#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "Ball.h"
#include "Block.h"
#include "BlocksManager.h"
#include "PowerUp.h"

typedef enum GameState
{
    TUTORIAL,
    PLAYING,
    GAME_OVER,
    WIN
} GameState;

typedef struct Game
{
    int screenWidth;
    int screenHeight;

    GameState state;
    Player player;
    Ball ball;
    Block blocks[BLOCK_ROWS][BLOCK_COLUMNS];

    int combo;
    int maxCombo;
    int lastScoreGained;
    float lastScoreTimer;
    float dashEffect;

    PowerUp powerUps[10];  // Active powerup array
    int powerUpCount;
    float powerUpSpawnCounter;
    float baseSpawnChance;
} Game;

// Game Basics
Game InitGame(int width, int height);
void UpdateGame(Game* game);
void DrawGame(Game game);
void HandleCollisions(Game* game);
void ResetGame(Game* game);

// Power ups
void HandlePowerUpSpawn(Game* game, Vector2 blockPosition);
void UpdatePowerUps(Game* game);
void DrawPowerUps(Game* game);
void HandlePowerUpCollisions(Game* game);
bool ShouldSpawnPowerUp(Game* game);

#endif // GAME_H
