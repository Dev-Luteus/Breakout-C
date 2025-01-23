#ifndef POWERUP_H
#define POWERUP_H

#include <raylib.h>
#include <stdbool.h>

#define LIFE_COLOR (Color){0x8B, 0x1B, 0x52, 0xFF}   // #8B1B52
#define SPEED_COLOR (Color){0xEC, 0xED, 0xAB, 0xFF}  // #ECEDAB
#define GROWTH_COLOR (Color){0xAC, 0xED, 0xAB, 0xFF} // #ACEDAB

typedef enum
{
    POWERUP_LIFE,
    POWERUP_SPEED,
    POWERUP_GROWTH,
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