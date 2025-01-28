#include "Player.h"
#include <raymath.h>
#include <VectorMath.h>

Player InitPlayer(int width, int height)
{
    Player player = {0};  // Zero-initialize all fields
    player.position = MyVector2Create(width / 2 - 25, height - 50);
    player.baseSpeed = 1000.0f;
    player.speed = 1000.0f;
    player.baseWidth = PLAYER_BASE_WIDTH;
    player.width = PLAYER_BASE_WIDTH;
    player.height = 10;
    player.lives = 5;
    player.score = 0;
    player.isDashing = false;
    player.color = PLAYER_COLOR;

    // Initialize trail positions
    for (int i = 0; i < PLAYER_TRAIL_LENGTH; i++)
    {
        player.trail.positions[i] = player.position;
    }

    player.trail.currentIndex = 0;

    return player;
}

void UpdatePlayerMovement(Player* player, float deltaTime, float screenWidth)
{
    Vector2 prevPosition = player->position;
    float moveAmount = 0.0f;
    bool isMoving = false;
    bool wasDashing = player->isDashing;

    // Handle horizontal movement
    if (IsKeyDown(KEY_RIGHT))
    {
        moveAmount = player->speed * deltaTime;
        isMoving = true;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        moveAmount = -player->speed * deltaTime;
        isMoving = true;
    }

    bool shouldDash = isMoving && IsKeyDown(KEY_LEFT_SHIFT);

    if (shouldDash)
    {
        moveAmount *= PLAYER_SPEED_BOOST;
    }

    // Check if we just started dashing
    if (shouldDash && !wasDashing)
    {
        for (int i = 0; i < PLAYER_TRAIL_LENGTH; i++)
        {
            player->trail.positions[i] = player->position;
        }

        player->trail.currentIndex = 0;
    }

    player->isDashing = shouldDash;

    // Update position and trail
    if (isMoving)
    {
        player->position.x += moveAmount;
        player->position.x = Clamp(player->position.x, 0, screenWidth - player->width);

        if (player->isDashing)
        {
            UpdatePlayerTrail(player, prevPosition);
        }
    }
}

void UpdatePlayerTrail(Player* player, Vector2 prevPosition)
{
    player->trail.positions[player->trail.currentIndex] = prevPosition;
    player->trail.currentIndex = (player->trail.currentIndex + 1) % PLAYER_TRAIL_LENGTH;
}

void DrawPlayerWithTrail(const Player* player)
{
    if (player->isDashing)
    {
        Vector2 prevPos = player->position;

        for (int i = 0; i < PLAYER_TRAIL_LENGTH; i++)
        {
            Vector2 trailPos = player->trail.positions
            [
                (player->trail.currentIndex - i + PLAYER_TRAIL_LENGTH) % PLAYER_TRAIL_LENGTH
            ];

            // Fade from 0.4 to 0.0
            float alpha = (float)(PLAYER_TRAIL_LENGTH - i) / PLAYER_TRAIL_LENGTH;
            alpha *= 0.4f; // Maximum opacity

            // Calculate width scale (from 1.0 to 0.4)
            float widthScale = 0.4f + (0.6f * alpha);

            Color trailColor =
            {
                player->color.r,  // Use player's current color
                player->color.g,
                player->color.b,
                (unsigned char)(alpha * 255) // Convert 0-1 to 0-255
            };

            // Calculate centered position for scaled width
            float scaledWidth = player->width * widthScale;
            float xOffset = (player->width - scaledWidth) / 2;

            DrawRectangle
            (
                trailPos.x + xOffset,
                trailPos.y,
                scaledWidth,
                player->height,
                trailColor
            );
        }
    }

    // Draw player
    DrawRectangle
    (
        player->position.x,
        player->position.y,
        player->width,
        player->height,
        player->color
    );
}
void UpdatePlayerColor(Player* player, bool isTimewarpActive)
{
    player->color = isTimewarpActive ? PLAYER_COLOR_PURPLE : PLAYER_COLOR;
}