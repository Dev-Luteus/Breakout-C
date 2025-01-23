#ifndef POWERUP_H
#define POWERUP_H

#include <raylib.h>
#include <stdbool.h>

typedef enum
{
    POWERUP_LIFE,
    POWERUP_SPEED,
    POWERUP_COUNT // Active array!
} PowerUpType;

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float radius;
    bool active;
    PowerUpType type;
    Color color;
} PowerUp;

typedef struct
{
    float baseChance;
    float comboMultiplier;
    float scoreMultiplier;
    float maxChance;
    float cooldownTimer;
    float cooldownDuration;    // How long to wait between spawn attempts
    float currentChance;
} PowerUpSpawnSystem;

// Core
PowerUp CreatePowerUp(Vector2 position, PowerUpType type);
void UpdatePowerUp(PowerUp* powerUp, float deltaTime);
void DrawPowerUp(PowerUp powerUp);
bool CheckPowerUpCollision(const PowerUp* powerUp, Rectangle playerRect);

// Spawn
PowerUpSpawnSystem InitPowerUpSpawnSystem(void);
float CalculateSpawnChance(PowerUpSpawnSystem* system, int combo, int score);
bool CheckPowerUpSpawn(PowerUpSpawnSystem* system, int combo, int score, float deltaTime);

#endif // POWERUP_H