﻿#include <raylib.h>
#include "Ball.h"
#include <Player.h>
#include <raymath.h>
#include "VectorMath.h"

Ball InitBall(Vector2 position)
{
    Ball ball = {
        .position = position,
        .direction = MyVector2Zero(),

        .radius = BALL_RADIUS,
        .speed = BALL_SPEED_MIN,
        .currentMinSpeed = BALL_SPEED_MIN,
        .currentMaxSpeed = BALL_SPEED_MAX,

        .active = false,
        .trail = {0}, // initialise to default values
        .damageMultiplier = 1,
        .currentColor = BALL_COLOR
    };

    // Initialize trail positions to starting position
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

    // Movement vector: direction * speed * time
    Vector2 movement = MyVector2Scale(ball->direction, ball->speed * deltaTime);

    // Update position using vector addition
    ball->position = MyVector2Add(ball->position, movement);

    // Bounce walls
    if (ball->position.x - ball->radius <= 0)
    {
        ball->position.x = ball->radius;
        ball->direction.x = fabs(ball->direction.x);

        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * SPEED_INCREASE_FACTOR,
                          ball->currentMinSpeed, ball->currentMaxSpeed);
    }
    else if (ball->position.x + ball->radius >= screenWidth)
    {
        ball->position.x = screenWidth - ball->radius;
        ball->direction.x = -fabs(ball->direction.x);

        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * SPEED_INCREASE_FACTOR,
                          ball->currentMinSpeed, ball->currentMaxSpeed);
    }

    // Bounce ceiling
    if (ball->position.y - ball->radius <= 0)
    {
        ball->position.y = ball->radius;
        ball->direction.y = fabs(ball->direction.y);

        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * SPEED_INCREASE_FACTOR, BALL_SPEED_MIN, BALL_SPEED_MAX);
    }
}

// Here I'm trying to enforce a minimum vertical component, to prevent a near-horizontal bouncing bug
void AdjustBallDirection(Ball* ball)
{
    if (fabs(ball->direction.y) < MIN_VERTICAL_COMPONENT)
    {
        ball->direction.y = (ball->direction.y >= 0 ? MIN_VERTICAL_COMPONENT : -MIN_VERTICAL_COMPONENT);
    }

    if (fabs(ball->direction.x) < MIN_HORIZONTAL_COMPONENT)
    {
        ball->direction.x = (ball->direction.x >= 0 ? MIN_HORIZONTAL_COMPONENT : -MIN_HORIZONTAL_COMPONENT);
    }

    ball->direction = MyVector2Normalize(ball->direction);
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
            Vector2 trailPos = MyVector2Subtract(prevPos,
                MyVector2Scale(ball.direction, i * TRAIL_SPACING));

            float alpha = (float)(TRAIL_LENGTH - i) / TRAIL_LENGTH;

            Color trailColor;

            if (ball.damageMultiplier > 1)
            {
                trailColor = (Color){
                    255,                          // R
                    (unsigned char)(255 * alpha), // G - Fading orange
                    0,                            // B
                    (unsigned char)(alpha * 100)  // A
                };
            }
            else
            {
                // Normal trail color based on ball's current color
                trailColor = ball.currentColor;
                trailColor.a = (unsigned char)(alpha * 100);
            }

            DrawCircleV(trailPos, ball.radius * (0.8f + (0.2f * alpha)), trailColor);
            prevPos = trailPos;
        }

        DrawCircleV(ball.position, ball.radius, ball.currentColor);
    }
}

// I want to shoot the ball, and shoot it in the direction the player is moving! Slightly random when still.
void ShootBall(Ball* ball, Vector2 startPosition, Vector2 direction, Player player)
{
    if (!ball->active)
    {
        Vector2 offsetDirection = MyVector2Create(0, -1);

        if (IsKeyDown(KEY_RIGHT))
        {
            offsetDirection = MyVector2Create(0.5f, -1.0f);
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            offsetDirection = MyVector2Create(-0.5f, -1.0f);
        }
        else
        {
            float randomX = GetRandomValue(-35, 35) / 100.0f;
            offsetDirection = MyVector2Create(randomX, -1.0f);
        }

        ball->speed = BALL_SPEED_MIN;
        ball->position = startPosition;
        ball->direction = MyVector2Normalize(offsetDirection);
        ball->active = true;
    }
}