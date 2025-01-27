#ifndef BALL_H
#define BALL_H

#include <Player.h>
#include <raylib.h>
#include <stdbool.h>

// Ball Properties
#define BALL_RADIUS 13.0f
#define BALL_COLOR (Color){0xB1, 0xFF, 0xB1, 0xFF}
#define BALL_SPEED_MIN 900.0f
#define BALL_SPEED_MAX 1700.0f
#define TRAIL_LENGTH 10
#define TRAIL_SPACING 3

// Ball Collision Properties
#define MIN_VERTICAL_COMPONENT 0.3f
#define MIN_HORIZONTAL_COMPONENT 0.2f
#define SPEED_INCREASE_FACTOR 1.04f

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