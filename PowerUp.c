#include "PowerUp.h"
#include "Game.h"
#include "Player.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Initialize our spawn system with balanced default values
PowerUpSpawnSystem InitPowerUpSpawnSystem(void)
{
    return (PowerUpSpawnSystem)
    {
        .baseChance = 0.0f,
        .comboMultiplier = 0.05f,      // 5% per combo
        .scoreMultiplier = 0.05f,      // 5% per 1000
        .maxChance = 0.20f,            // 20% cap
        .cooldownTimer = 0.0f,
        .cooldownDuration = 3.0f,      // Second cooldown
        .currentChance = 0.0f
    };
}

// Calculate current spawn chance based on player performance
float CalculateSpawnChance(PowerUpSpawnSystem* system, int combo, int score)
{
    float comboBonus = combo * system->comboMultiplier;
    float scoreBonus = (score / 1000.0f) * system->scoreMultiplier;

    system->currentChance = system->baseChance + comboBonus + scoreBonus;
    system->currentChance = fminf(system->currentChance, system->maxChance);

    return system->currentChance;
}

// Determine if a powerup should spawn based on current conditions
bool CheckPowerUpSpawn(PowerUpSpawnSystem* system, int combo, int score, float deltaTime)
{
    float chance = CalculateSpawnChance(system, combo, score);
    float roll = (float)rand() / RAND_MAX;

    printf("Spawn Check - Chance: %f, Roll: %f, Combo: %d, Score: %d\n",
           chance, roll, combo, score);

    if (roll < chance)
    {
        system->currentChance = system->baseChance; // Reset
        printf("Spawn successful - Chance reset\n");

        return true;
    }
    return false;
}

// Here, I want to create a powerUp factory that should be modular and support new power-ups easily!
PowerUp CreatePowerUp(Vector2 position, PowerUpType type)
{
    PowerUp powerUp =
    {
        .position = position,
        .velocity = (Vector2){0, 200},  // Pixel p/s
        .radius = 24,
        .active = true,
        .type = type,
    };

    // Assign color based on powerup type
    switch(type)
    {
        case POWERUP_LIFE:
            powerUp.color = LIFE_COLOR;
            break;

        case POWERUP_SPEED:
            powerUp.color = SPEED_COLOR;
            break;

        case POWERUP_GROWTH:
            powerUp.color = GROWTH_COLOR;
            break;

        default:
            powerUp.color = WHITE;
    }

    return powerUp;
}

// Update powerup position and state
void UpdatePowerUp(PowerUp* powerUp, float deltaTime)
{
    if (!powerUp->active)
    {
        return;
    }

    powerUp->position.y += powerUp->velocity.y * deltaTime;
}

// Here we apply our powerup effect to our player
void ApplyPowerUpEffect(PowerUp* powerUp, Player* player)
{
    switch(powerUp->type)
    {
        case POWERUP_LIFE:
            player->lives++;
            powerUp->active = false;
        break;

        case POWERUP_SPEED:
            player->speed *= 1.1f;
            powerUp->active = false;
        break;

        case POWERUP_GROWTH:
            player->width *= 1.1f;
            powerUp->active = false;
        break;
    }
}

// Here we check Player/PowerUp collision, and apply effects/handle powerups!
void HandlePowerUpCollisions(Game* game)
{
    Rectangle playerRect =
    {
        game->player.position.x,
        game->player.position.y,
        game->player.width,
        game->player.height
    };

    for (int i = 0; i < 10; i++)
    {
        if (!game->powerUps[i].active)
        {
            continue;
        }

        if (CheckPowerUpCollision(&game->powerUps[i], playerRect))
        {
            ApplyPowerUpEffect(&game->powerUps[i], &game->player);
            game->powerUpCount--;
        }
    }
}

// Check collision between powerup and player rectangle
bool CheckPowerUpCollision(const PowerUp* powerUp, Rectangle playerRect)
{
    return CheckCollisionCircleRec(
        powerUp->position,
        powerUp->radius,
        playerRect
    );
}

// Our general update method. We also make sure to remove power-ups if the player misses them in the killZone!
void UpdatePowerUps(Game* game)
{
    float deltaTime = GetFrameTime();

    for (int i = 0; i < 10; i++)
    {
        if (!game->powerUps[i].active)
        {
            continue;
        }

        UpdatePowerUp(&game->powerUps[i], deltaTime);

        if (game->powerUps[i].position.y > game->screenHeight)
        {
            game->powerUps[i].active = false;
            game->powerUpCount--;
        }
    }
}

// Draw all active powerups in Game C!
void DrawPowerUps(Game* game)
{
    for (int i = 0; i < 10; i++)
    {
        if (game->powerUps[i].active)
        {
            DrawPowerUp(game->powerUps[i]);
        }
    }
}

// Draw individual powerup with appropriate icon
void DrawPowerUp(PowerUp powerUp)
{
    if (!powerUp.active)
    {
        return;
    }

    // Outer circle!
    DrawCircleV(powerUp.position, powerUp.radius, powerUp.color);

    // Inner circle!
    DrawCircleV(powerUp.position, powerUp.radius * 0.55f, WHITE);

    // Here we try to draw an ICON for each type of power up
    switch(powerUp.type)
    {
        case POWERUP_LIFE:
            DrawText("+",
                    powerUp.position.x - powerUp.radius * 0.3f,
                    powerUp.position.y - powerUp.radius * 0.5f,
                    powerUp.radius, BLACK);
            break;

        case POWERUP_SPEED:
            DrawText("S",
                    powerUp.position.x - powerUp.radius * 0.3f,
                    powerUp.position.y - powerUp.radius * 0.5f,
                    powerUp.radius, BLACK);
            break;

        case POWERUP_GROWTH:
            DrawText("G",
                    powerUp.position.x - powerUp.radius * 0.3f,
                    powerUp.position.y - powerUp.radius * 0.5f,
                    powerUp.radius, BLACK);
        break;
    }
}
