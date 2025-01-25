#ifndef POWERUP_H
#define POWERUP_H

#include <raylib.h>
#include <stdbool.h>

#define PU_LIFE_COLOR (Color){0x8B, 0x1B, 0x52, 0xFF}   // #8B1B52
#define PU_SPEED_COLOR (Color){0xEC, 0xED, 0xAB, 0xFF}  // #ECEDAB
#define PU_GROWTH_COLOR (Color){0xAC, 0xED, 0xAB, 0xFF} // #ACEDAB
#define PU_GHOST_COLOR (Color){0x1B, 0x8B, 0x89, 0xFF}    // #1b8b89
#define PU_TIMEWARP_COLOR (Color){0x7E, 0x1B, 0x8B, 0xFF} // #7e1b8b
#define PU_DAMAGE_COLOR (Color){0x8B, 0x1B, 0x1B, 0xFF}  // #8b1b1b

#define PU_SPEED_DURATION 12.0
#define PU_GROWTH_DURATION 13.0
#define PU_DEFAULT_DURATION 0.0
#define PU_GHOST_DURATION 2.0f
#define PU_TIMEWARP_DURATION 6.0f
#define PU_DAMAGE_DURATION 12.0f

#define PU_MAX_COUNT 10
#define PU_LIFE_AMOUNT 2
#define PU_SPEED_MULTIPLIER 1.25f
#define PU_GROWTH_MULTIPLIER 1.45f
#define PU_TIMEWARP_MULTIPLIER 0.80f
#define PU_DAMAGE_MULTIPLIER 3

typedef enum
{
    POWERUP_LIFE,
    POWERUP_SPEED,
    POWERUP_GROWTH,
    POWERUP_GHOST,
    POWERUP_TIMEWARP,
    POWERUP_DAMAGE,
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

    float duration;
    float remainingDuration;
    bool wasPickedUp;
    double startTime; // When!
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
PowerUp CreatePowerUp(Vector2 position, PowerUpType type, float duration);
void UpdatePowerUp(PowerUp* powerUp, float deltaTime);
void DrawPowerUp(PowerUp powerUp);
bool CheckPowerUpCollision(const PowerUp* powerUp, Rectangle playerRect);

// Spawn
PowerUpSpawnSystem InitPowerUpSpawnSystem(void);
float CalculateSpawnChance(PowerUpSpawnSystem* system, int combo, int score);
bool CheckPowerUpSpawn(PowerUpSpawnSystem* system, int combo, int score, float deltaTime);

#endif // POWERUP_H