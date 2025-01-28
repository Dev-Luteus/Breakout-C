#include "PowerUp.h"
#include "Game.h"
#include "Player.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "BlocksManager.h"

// Initialize our spawn system with balanced default values
PowerUpSpawnSystem InitPowerUpSpawnSystem(void)
{
    return (PowerUpSpawnSystem)
    {
        .baseChance = 0.05f,           // 0.05% base chance
        .comboMultiplier = 0.05f,      // % per combo
        .scoreMultiplier = 0.10f,      // % per 1000
        .maxChance = 0.30f,            // 30% cap
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
    // Update the cooldown timer
    system->cooldownTimer -= deltaTime;

    if (system->cooldownTimer > 0)
    {
        return false;
    }

    float chance = CalculateSpawnChance(system, combo, score);
    float roll = (float)rand() / RAND_MAX;

    // On success, restart cooldown!
    if (roll < chance)
    {
        system->currentChance = system->baseChance;
        system->cooldownTimer = system->cooldownDuration;
        printf("Spawn successful, cooldown started: %.2f seconds\n", system->cooldownDuration);

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
        .velocity = MyVector2Create(0, 800),  // Pixel p/s
        .radius = 28,
        .active = true,
        .type = type,
        .duration = duration,
        .remainingDuration = duration,
        .wasPickedUp = false,
        .pulseTimer = 0
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

        case POWERUP_GHOST:
            powerUp.color = PU_GHOST_COLOR;
            powerUp.duration = PU_GHOST_DURATION;
        break;

        case POWERUP_TIMEWARP:
            powerUp.color = PU_TIMEWARP_COLOR;
            powerUp.duration = PU_TIMEWARP_DURATION;
        break;

        case POWERUP_DAMAGE:
            powerUp.color = PU_DAMAGE_COLOR;
            powerUp.duration = PU_DAMAGE_DURATION;
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
    powerUp->pulseTimer += deltaTime * 5.0f;
}

// Here we apply our powerup effect to our player!
void ApplyPowerUpEffect(PowerUp* powerUp, Player* player, Game* game)
{
    powerUp->wasPickedUp = true;
    powerUp->startTime = GetTime();

    switch(powerUp->type)
    {
        case POWERUP_LIFE:
            game->player.lives += PU_LIFE_AMOUNT;
            powerUp->duration = PU_DEFAULT_DURATION;
            powerUp->active = false;
        break;

        case POWERUP_SPEED:
            player->speed = player->baseSpeed * PU_SPEED_MULTIPLIER;
            powerUp->duration = PU_SPEED_DURATION;
            powerUp->active = true;
        break;

        case POWERUP_GROWTH:
            player->width = player->baseWidth * PU_GROWTH_MULTIPLIER;
            powerUp->duration = PU_GROWTH_DURATION;
            powerUp->active = true;
        break;

        case POWERUP_GHOST:
            game->ball.isGhost = true;
            powerUp->duration = PU_GHOST_DURATION;
            powerUp->active = true;
        break;

        case POWERUP_TIMEWARP:
            game->timeScale = PU_TIMEWARP_MULTIPLIER;
            powerUp->duration = PU_TIMEWARP_DURATION;
            powerUp->active = true;
            UpdateBlockColors(game->blocks, game->currentBlockRows,
                         game->currentBlockColumns, true);
            game->isTimewarpActive = true;
        break;

        case POWERUP_DAMAGE:
            game->ball.damageMultiplier = PU_DAMAGE_MULTIPLIER;
            powerUp->duration = PU_DAMAGE_DURATION;
            powerUp->active = true;
            game->ball.radius += 3;
        break;
    }
}

/* Here, we're creating a method to get the active powerup color. We do this because:
 * I used to have a bug where the ball would reset the colour while another power up was active,
 * if two colour-changing power ups were active at the same time. Now we can give them a priority instead! */
Color GetActivePowerUpColor(const PowerUp powerUps[], int count)
{
    // Priority order: Ghost > Timewarp > Damage > Default
    bool hasGhost = false;
    bool hasTimeWarp = false;
    bool hasDamage = false;

    for (int i = 0; i < count; i++)
    {
        const PowerUp* powerUp = &powerUps[i];

        if (powerUp->active && powerUp->wasPickedUp)
        {
            switch (powerUp->type)
            {
                case POWERUP_GHOST:
                    hasGhost = true;
                break;

                case POWERUP_TIMEWARP:
                    hasTimeWarp = true;
                break;

                case POWERUP_DAMAGE:
                    hasDamage = true;
                break;

                default:
                    break;
            }
        }
    }

    if (hasGhost)
    {
        return PU_GHOST_COLOR;
    }

    if (hasTimeWarp)
    {
        return PU_TIMEWARP_COLOR;
    }

    if (hasDamage)
    {
        return PU_DAMAGE_COLOR;
    }

    return BALL_COLOR;
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
                ApplyPowerUpEffect(powerUp, &game->player, game);
                powerUp->active = true;
                powerUp->wasPickedUp = true;
                game->powerUpCount--;
            }
            else // Skip!
            {
                powerUp->active = false;
                game->powerUpCount--;
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

        // Here, we're adding a validation check to prevent out of bounds power-up types
        if (powerUp->type >= POWERUP_COUNT)
        {
            powerUp->active = false;
            game->powerUpCount = (game->powerUpCount > 0) ? game->powerUpCount - 1 : 0;

            continue;
        }

        if (!powerUp->wasPickedUp)
        {
            UpdatePowerUp(powerUp, deltaTime * game->timeScale);

            // Check for killZone
            if (powerUp->position.y > game->screenHeight)
            {
                powerUp->active = false;
                game->powerUpCount = (game->powerUpCount > 0) ? game->powerUpCount - 1 : 0;
            }
        }
        else  // Power-up is active and was picked up
        {
            double elapsedTime = currentTime - powerUp->startTime;
            powerUp->remainingDuration = powerUp->duration - elapsedTime;

            if (powerUp->remainingDuration <= 0)
            {
                switch(powerUp->type)
                {
                    case POWERUP_SPEED:
                        game->player.speed = game->player.baseSpeed;
                    break;

                    case POWERUP_GROWTH:
                        game->player.width = game->player.baseWidth;
                    break;

                    case POWERUP_GHOST:
                        game->ball.isGhost = false;
                    break;

                    case POWERUP_TIMEWARP:
                        game->timeScale = game->normalTimeScale;
                        game->isTimewarpActive = false;
                        UpdateBlockColors(game->blocks, game->currentBlockRows,
                                     game->currentBlockColumns, false);
                    break;

                    case POWERUP_DAMAGE:
                        game->ball.damageMultiplier = 1;
                        game->ball.radius -= 2;
                    break;
                }

                powerUp->active = false;
                powerUp->wasPickedUp = false;
                game->powerUpCount = (game->powerUpCount > 0) ? game->powerUpCount - 1 : 0;
            }
        }
    }
    game->ball.currentColor = GetActivePowerUpColor(game->powerUps, PU_MAX_COUNT);
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

    float alpha = 0.7f + (sinf(powerUp.pulseTimer) * 0.3f);
    Color pulsingColor = powerUp.color;
    pulsingColor.a = (unsigned char)(255 * alpha);

    // Outer circle!
    DrawCircleV(powerUp.position, powerUp.radius, pulsingColor);

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

        case POWERUP_GHOST:
            text = "¤";
        break;

        case POWERUP_TIMEWARP:
            text = "T";
        break;

        case POWERUP_DAMAGE:
            text = "D";
        break;

        default:
            text = "?";
        break;
    }

    int fontSize = (int)(powerUp.radius * 1.3f);
    int textWidth = MeasureText(text, fontSize);
    int textHeight = fontSize;

    Vector2 textPosition = MyVector2Create
    (
        powerUp.position.x - textWidth / 2,
        powerUp.position.y - textHeight / 2
    );

    DrawText(text, textPosition.x, textPosition.y, fontSize, BLACK);
}

// To display our power ups, we're drawing a timer for each type as an indictator
void DrawPowerUpTimers(Game game)
{
    const int timerHeight = 14;
    const int timerWidth = 120;
    const int padding = 20;
    int activeTimers = 0;

    for (int i = 0; i < PU_MAX_COUNT; i++)
    {
        PowerUp* powerUp = &game.powerUps[i];

        if (powerUp->active && powerUp->wasPickedUp && powerUp->duration > 0)
        {
            // Calculate position for the timer bar
            int x = 15;  // Left margin
            int y = game.screenHeight - 30 - (activeTimers * (timerHeight + padding));  // Bottom margin

            float fillPercent = powerUp->remainingDuration / powerUp->duration;

            DrawRectangle(x, y, timerWidth, timerHeight, GRAY); // Background!
            DrawRectangle(x, y, timerWidth * fillPercent, timerHeight, powerUp->color);  // Timer fill!

            const char* text;

            switch(powerUp->type)
            {
                case POWERUP_SPEED:
                    text = "S";
                break;

                case POWERUP_GROWTH:
                    text = "G";
                break;

                case POWERUP_GHOST:
                    text = "¤";
                break;

                case POWERUP_TIMEWARP:
                    text = "T";
                break;

                case POWERUP_DAMAGE:
                    text = "D";
                break;

                default:
                    text = "?";
                break;
            }

            DrawText(text, x + timerWidth + 5, y - 2, timerHeight + 4, powerUp->color);
            activeTimers++;
        }
    }
}

void ResetAllPowerUpEffects(Game* game)
{
    game->player.speed = game->player.baseSpeed;
    game->player.width = game->player.baseWidth;
    game->ball.isGhost = false;
    game->ball.currentColor = BALL_COLOR;
    game->timeScale = game->normalTimeScale;
    game->ball.damageMultiplier = 1;
    game->ball.radius = BALL_RADIUS;
    game->isTimewarpActive = false;
}