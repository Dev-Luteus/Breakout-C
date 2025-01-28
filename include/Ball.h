#ifndef BALL_H
#define BALL_H

#include <Player.h>
#include <raylib.h>
#include <stdbool.h>

// Ball Properties
#define BALL_RADIUS 13.0f
#define BALL_COLOR (Color){0xB1, 0xFF, 0xB1, 0xFF}
#define BALL_SPEED_MIN 900.0f
#define BALL_SPEED_MAX 1400.0f
#define TRAIL_LENGTH 10
#define TRAIL_SPACING 3

// Ball Collision Properties
#define MIN_VERTICAL_COMPONENT 0.3f
#define MIN_HORIZONTAL_COMPONENT 0.2f
#define SPEED_INCREASE_FACTOR 1.04f

// Level Bonus Properties
#define BALL_SPEED_INCREMENT_PER_LEVEL 25.0f
#define BALL_SPEED_MAX_INCREMENT_PER_LEVEL 75.0f

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

    float currentMinSpeed;
    float currentMaxSpeed;
} Ball;

Ball InitBall(Vector2 position);
void UpdateBall(Ball* ball, float deltaTime, int screenWidth, int screenHeight);
void DrawBall(Ball ball);
void ShootBall(Ball* ball, Vector2 startPos, Vector2 direction, Player player);
void AdjustBallDirection(Ball* ball);

#endif