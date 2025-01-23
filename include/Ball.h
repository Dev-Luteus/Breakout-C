#ifndef BALL_H
#define BALL_H

#include <Player.h>
#include <raylib.h>
#include <stdbool.h>

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
} Ball;

Ball InitBall(Vector2 position);
void UpdateBall(Ball* ball, float deltaTime, int screenWidth, int screenHeight);
void DrawBall(Ball ball);
void ShootBall(Ball* ball, Vector2 startPos, Vector2 direction, Player player);

#endif