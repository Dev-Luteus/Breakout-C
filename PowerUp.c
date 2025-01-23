#include "PowerUp.h"
#include "Game.h"
#include "Player.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Here, I want to create a powerUp factory that should be modular and support new power-ups easily!
PowerUp CreatePowerUp(Vector2 position, PowerUpType type)
{
    PowerUp powerUp =
    {
        .position = position,
        .velocity = (Vector2){0, 200},  // Pixel p/s
        .radius = 10,
        .active = true,
        .type = type,
    };

    switch(type)
    {
        case POWERUP_LIFE:
            powerUp.color = RED;
            break;

        case POWERUP_SPEED:
            powerUp.color = YELLOW;
            break;

        default:
            powerUp.color = WHITE;
    }

    return powerUp;
}

void UpdatePowerUp(PowerUp* powerUp, float deltaTime)
{
    if (!powerUp->active)
    {
        return;
    }

    powerUp->position.y += powerUp->velocity.y * deltaTime;
}

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
        break;
    }
}

// Here we check Player/PowerUp collision, and apply effects/handle powerups!
void HandlePowerUpCollisions(Game* game)
{
    Rectangle playerRect = {
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

        // Here we handle our collisions!
        if (CheckPowerUpCollision(&game->powerUps[i], playerRect))
        {
            ApplyPowerUpEffect(&game->powerUps[i], &game->player);
            game->powerUpCount--;
        }
    }
}

bool CheckPowerUpCollision(const PowerUp* powerUp, Rectangle playerRect)
{
    return CheckCollisionCircleRec(
        powerUp->position,
        powerUp->radius,
        playerRect
    );
}

// Here we define our Power Up spawn conditions: Based on Combo and Score!
bool ShouldSpawnPowerUp(Game* game)
{
    float comboBonus = game->combo * 0.05f;  // 5% per combo
    float scoreBonus = (game->player.score / 1000.0f) * 10.0f;  // 10% per 1000

    float finalChance = game->baseSpawnChance + comboBonus + scoreBonus;
    finalChance = fmin(finalChance, 0.5f);  // Cap at 50% chance

    // Here we add a cooldown to prevent powerups from spawning too often!
    game->powerUpSpawnCounter += GetFrameTime();

    printf("Spawn Check - Final Chance: %f, Counter: %f\n", finalChance, game->powerUpSpawnCounter);

    // Finalizing our spawn condition logic and resetting after a powerup has spawned!
    float roll = (float)rand() / RAND_MAX;
    printf("Random Roll: %f vs Chance: %f\n", roll, finalChance);

    if (roll < finalChance)
    {
        printf("Spawn successful\n");
        game->powerUpSpawnCounter = 0.0f;

        return true;
    }

    return false;
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

// When we spawn them in-game in Game C!
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

// The actual graphics that PowerUp C uses
void DrawPowerUp(PowerUp powerUp)
{
    if (!powerUp.active)
    {
        return;
    }

    // Outer circle!
    DrawCircleV(powerUp.position, powerUp.radius, powerUp.color);

    // Inner circle!
    DrawCircleV(powerUp.position, powerUp.radius * 0.6f, WHITE);

    // Here we try to draw an ICON for each type of power up
    switch(powerUp.type)
    {
        case POWERUP_LIFE:
                DrawText("+",
                        powerUp.position.x - powerUp.radius * 0.3f,
                        powerUp.position.y - powerUp.radius * 0.5f,
                        powerUp.radius, RED);
        break;

        case POWERUP_SPEED:
                DrawText("S",
                        powerUp.position.x - powerUp.radius * 0.3f,
                        powerUp.position.y - powerUp.radius * 0.5f,
                        powerUp.radius, YELLOW);
        break;
    }
}