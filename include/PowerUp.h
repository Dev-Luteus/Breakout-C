#ifndef POWERUP_H
#define POWERUP_H

#include <raylib.h>
#include <stdbool.h>

typedef struct Game Game; // We do this to avoid a circular dependency, when referring to Game.h!

#define PU_DAMAGE_COLOR (Color){0xFF, 0x40, 0x40, 0xFF}    // Bright phosphor red (#FF4040)
#define PU_LIFE_COLOR (Color){0x80, 0x20, 0x20, 0xFF}      // Dark red with slight green (#802020)
#define PU_GROWTH_COLOR (Color){0x16, 0xFF, 0x16, 0xFF}    // Pure phosphor green (#16FF16)
#define PU_SPEED_COLOR (Color){0xFF, 0xFF, 0x40, 0xFF}     // Bright yellow with green tint (#FFFF40)
#define PU_GHOST_COLOR (Color){0x40, 0xFF, 0xFF, 0xFF}     // Bright cyan (#40FFFF)
#define PU_TIMEWARP_COLOR (Color){0xFF, 0x40, 0xFF, 0xFF}  // Bright purple with green tint (#FF40FF)

#define PU_SPEED_DURATION 12.0
#define PU_GROWTH_DURATION 13.0
#define PU_DEFAULT_DURATION 0.0
#define PU_GHOST_DURATION 1.5f
#define PU_TIMEWARP_DURATION 7.0f
#define PU_DAMAGE_DURATION 12.0f

#define PU_MAX_COUNT 10
#define PU_LIFE_AMOUNT 2
#define PU_SPEED_MULTIPLIER 1.25f
#define PU_GROWTH_MULTIPLIER 1.45f
#define PU_TIMEWARP_MULTIPLIER 0.75f
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

    float pulseTimer;
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
void ResetAllPowerUpEffects(Game* game);

// Effects
Color GetActivePowerUpColor(const PowerUp powerUps[], int count);

#endif // POWERUP_H