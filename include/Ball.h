#ifndef BALL_H
#define BALL_H

#include <Player.h>
#include <raylib.h>
#include <stdbool.h>

#define BALL_SPEED_MIN 900.0f
#define BALL_SPEED_MAX 1800.0f
#define TRAIL_LENGTH 10
#define TRAIL_SPACING 3

typedef struct
{
    Vector2 positions[TRAIL_LENGTH];
    int currentIndex;
} BallTrail;

typedef struct
{
    Vector2 position;
    Vector2 direction;
    float radius;
    float speed;

    bool active;
    BallTrail trail;

    bool isGhost;
    Color currentColor;
    int damageMultiplier;
} Ball;

Ball InitBall(Vector2 position);
void UpdateBall(Ball* ball, float deltaTime, int screenWidth, int screenHeight);
void DrawBall(Ball ball);
void ShootBall(Ball* ball, Vector2 startPos, Vector2 direction, Player player);
void AdjustBallDirection(Ball* ball);

#endif