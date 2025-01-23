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

    if (roll < chance)
    {
        system->currentChance = system->baseChance; // Reset
        printf("Spawn successful\n");

        return true;
    }
    return false;
}

// Here, I want to create a powerUp factory that should be modular and support new power-ups easily!
PowerUp CreatePowerUp(Vector2 position, PowerUpType type, float duration)
{
    PowerUp powerUp =
    {
        .position = position,
        .velocity = (Vector2){0, 300},  // Pixel p/s
        .radius = 24,
        .active = true,
        .type = type,
        .duration = duration,
        .remainingDuration = duration,
        .wasPickedUp = false
    };

    // Assign color based on powerup type
    switch(type)
    {
        case POWERUP_LIFE:
            powerUp.color = PU_LIFE_COLOR;
        break;

        case POWERUP_SPEED:
            powerUp.color = PU_SPEED_COLOR;
        break;

        case POWERUP_GROWTH:
            powerUp.color = PU_GROWTH_COLOR;
        break;

        default:
            powerUp.color = WHITE;
        break;
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
    powerUp->wasPickedUp = true;
    powerUp->startTime = GetTime();

    switch(powerUp->type)
    {
        case POWERUP_LIFE:
            player->lives++;
            powerUp->duration = PU_DEFAULT_DURATION;
            powerUp->active = false;  // We do this for an immediate effect!
        break;

        case POWERUP_SPEED:
            player->speed = player->baseSpeed * PU_SPEED_MULTIPLIER;
            powerUp->duration = PU_SPEED_DURATION;
            powerUp->active = true;  // Keep active to track duration
            printf("Speed increased to %.2f for %.1f seconds\n",
               player->speed, powerUp->duration);
        break;

        case POWERUP_GROWTH:
            player->width = player->baseWidth * PU_GROWTH_MULTIPLIER;
            powerUp->duration = PU_GROWTH_DURATION;
            powerUp->active = true;  // Keep active to track duration
            printf("Width increased to %d for %.1f seconds\n",
            player->width, powerUp->duration);
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

    for (int i = 0; i < PU_MAX_COUNT; i++)
    {
        PowerUp* powerUp = &game->powerUps[i];

        if (!powerUp->active || powerUp->wasPickedUp)
        {
            // Skip if already picked up
            continue;
        }

        if (CheckPowerUpCollision(powerUp, playerRect))
        {
            bool alreadyActive = false;

            for (int j = 0; j < PU_MAX_COUNT; j++)
            {
                if (i != j && game->powerUps[j].active &&
                    game->powerUps[j].wasPickedUp &&
                    game->powerUps[j].type == powerUp->type)
                {
                    alreadyActive = true;
                    break;
                }
            }

            if (!alreadyActive)
            {
                ApplyPowerUpEffect(powerUp, &game->player);
                powerUp->active = true;      // Keep the power-up active for effect
                powerUp->wasPickedUp = true; // Mark as picked up
                game->powerUpCount--;
                printf("Applied power-up of type %d\n", powerUp->type);
            }
            else
            {
                powerUp->active = false;
                game->powerUpCount--;
                printf("Power-up of type %d already active, skipping\n", powerUp->type);
            }
        }
    }
}

// Check collision between powerup and player rectangle
bool CheckPowerUpCollision(const PowerUp* powerUp, Rectangle playerRect)
{
    return CheckCollisionCircleRec
    (
        powerUp->position,
        powerUp->radius,
        playerRect
    );
}

// Our general update method. We also make sure to remove power-ups if the player misses them in the killZone!
void UpdatePowerUps(Game* game)
{
    double currentTime = GetTime();
    float deltaTime = GetFrameTime();

    for (int i = 0; i < PU_MAX_COUNT; i++)
    {
        PowerUp* powerUp = &game->powerUps[i];

        if (!powerUp->active)
        {
            continue;
        }

        if (!powerUp->wasPickedUp)
        {
            UpdatePowerUp(powerUp, deltaTime); // Update falling

            // Check for killZone
            if (powerUp->position.y > game->screenHeight)
            {
                powerUp->active = false;
                game->powerUpCount--;
            }
        }
        else  // Power-up is active and was picked up
        {
            double elapsedTime = currentTime - powerUp->startTime;
            powerUp->remainingDuration = powerUp->duration - elapsedTime;

            printf("PowerUp type %d: %.2f seconds remaining\n",
                   powerUp->type, powerUp->remainingDuration);

            if (powerUp->remainingDuration <= 0)
            {
                switch(powerUp->type)
                {
                    case POWERUP_SPEED:
                        game->player.speed = game->player.baseSpeed;
                        printf("Speed power-up expired, reset to: %.2f\n",
                           game->player.baseSpeed);
                    break;

                    case POWERUP_GROWTH:
                        game->player.width = game->player.baseWidth;
                        printf("Growth power-up expired, reset to: %d\n",
                           game->player.baseWidth);
                    break;
                }

                powerUp->active = false;
                powerUp->wasPickedUp = false;
                game->powerUpCount--;
            }
        }
    }
}

// Draw all active powerups in Game C!
void DrawPowerUps(Game* game)
{
    for (int i = 0; i < PU_MAX_COUNT; i++)
    {
        if (game->powerUps[i].active && !game->powerUps[i].wasPickedUp)
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
    const char* text;

    switch(powerUp.type)
    {
        case POWERUP_LIFE:
            text = "+";
            break;

        case POWERUP_SPEED:
            text = "S";
            break;

        case POWERUP_GROWTH:
            text = "G";
            break;

        default:
            text = "?";
            break;
    }

    int fontSize = powerUp.radius;
    int textWidth = MeasureText(text, fontSize);
    int textHeight = fontSize;

    Vector2 textPosition =
    {
        powerUp.position.x - textWidth / 2,
        powerUp.position.y - textHeight / 2
    };

    DrawText(text, textPosition.x, textPosition.y, fontSize, BLACK);
}

