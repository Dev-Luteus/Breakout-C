#include <raylib.h>
#include "Ball.h"
#include <Player.h>
#include <raymath.h>

Ball InitBall(Vector2 position)
{
    Ball ball =
    {
        .position = position,
        .direction = (Vector2){0, 0},
        .radius = 10.0f,
        .speed = BALL_SPEED_MIN,
        .active = false,
        .trail = {0} // initialise to default values
    };

    for (int i = 0; i < TRAIL_LENGTH; i++)
    {
        ball.trail.positions[i] = position;
    }

    ball.trail.currentIndex = 0;

    return ball;
}

void UpdateBall(Ball* ball, float deltaTime, int screenWidth, int screenHeight)
{
    if (!ball->active)
    {
        return;
    }

    // Update trail current index before moving the ball
    ball->trail.positions[ball->trail.currentIndex] = ball->position;
    ball->trail.currentIndex = (ball->trail.currentIndex + 1) % TRAIL_LENGTH;

    Vector2 movement = Vector2Scale(ball->direction, ball->speed * deltaTime);
    ball->position = Vector2Add(ball->position, movement);

    // Bounce walls
    if (ball->position.x - ball->radius <= 0)
    {
        ball->position.x = ball->radius;
        ball->direction.x = fabs(ball->direction.x);
        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * 1.05f, BALL_SPEED_MIN, BALL_SPEED_MAX);
    }
    else if (ball->position.x + ball->radius >= screenWidth)
    {
        ball->position.x = screenWidth - ball->radius;
        ball->direction.x = -fabs(ball->direction.x);
        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * 1.05f, BALL_SPEED_MIN, BALL_SPEED_MAX);
    }

    // Bounce ceiling
    if (ball->position.y - ball->radius <= 0)
    {
        ball->position.y = ball->radius;
        ball->direction.y = fabs(ball->direction.y);

        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * 1.05f, BALL_SPEED_MIN, BALL_SPEED_MAX);
    }
}

// Here I'm trying to enforce a minimum vertical component, to prevent a near-horizontal bouncing bug
void AdjustBallDirection(Ball* ball)
{
    const float MIN_VERTICAL = 0.3f;
    const float MIN_HORIZONTAL = 0.2f;

    if (fabs(ball->direction.y) < MIN_VERTICAL)
    {
        ball->direction.y = (ball->direction.y >= 0 ? MIN_VERTICAL : -MIN_VERTICAL);
    }

    if (fabs(ball->direction.x) < MIN_HORIZONTAL)
    {
        ball->direction.x = (ball->direction.x >= 0 ? MIN_HORIZONTAL : -MIN_HORIZONTAL);
    }

    ball->direction = Vector2Normalize(ball->direction);
}

void DrawBall(Ball ball)
{
    if (ball.active)
    {
        Vector2 prevPos = ball.position;

        // Here I'm trying to draw a gradually fading trail like multiple circles
        for (int i = 0; i < TRAIL_LENGTH; i++)
        {
            // Calculate position along the trail
            Vector2 trailPos = Vector2Subtract(prevPos,
                Vector2Scale(ball.direction, i * TRAIL_SPACING));

            // Fade from white to blue!
            float alpha = (float)(TRAIL_LENGTH - i) / TRAIL_LENGTH;
            Color trailColor = {
                255,                                   // R
                255 * alpha,                           // G
                255,                                   // B
                (unsigned char)(alpha * 100)
            };

            DrawCircleV(trailPos, ball.radius * (0.8f + (0.2f * alpha)), trailColor);
            prevPos = trailPos;
        }

        DrawCircleV(ball.position, ball.radius, WHITE);
    }
}

// I want to shoot the ball, and shoot it in the direction the player is moving! Slightly random when still.
void ShootBall(Ball* ball, Vector2 startPosition, Vector2 direction, Player player)
{
    if (!ball->active)
    {
        Vector2 offsetDirection = {0, -1};

        if (IsKeyDown(KEY_RIGHT))
        {
            offsetDirection.x = 0.4f;
            offsetDirection.y = -1.0f;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            offsetDirection.x = -0.4f;
            offsetDirection.y = -1.0f;
        }
        else
        {
            offsetDirection.x = GetRandomValue(-15, 15) / 100.0f;
            offsetDirection.y = -1.0f;
        }

        ball->speed = BALL_SPEED_MIN;
        ball->position = startPosition;
        ball->direction = Vector2Normalize(offsetDirection);
        ball->active = true;
    }
}