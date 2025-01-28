#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>
#include <stdbool.h>

#define PLAYER_SPEED_BOOST 1.5f
#define PLAYER_COLOR (Color){0x40, 0xFF, 0x40, 0xFF}  // Bright phosphor green
#define PLAYER_COLOR_PURPLE (Color){0x80, 0x40, 0xFF, 0xFF}  // Bright phosphor purple

#define PLAYER_TRAIL_LENGTH 16
#define PLAYER_TRAIL_SPACING 10

#define PLAYER_BASE_WIDTH 150
typedef struct
{
    Vector2 positions[PLAYER_TRAIL_LENGTH];
    int currentIndex;
} PlayerTrail;

typedef struct Player
{
    Vector2 position;
    float baseSpeed;
    float speed;
    int height;
    int baseWidth;
    int width;
    int lives;
    int score;
    PlayerTrail trail;
    bool isDashing;
    Color color;
    bool isTimewarpActive;
} Player;

// Core initialization
Player InitPlayer(int width, int height);

// Movement functions
void UpdatePlayerMovement(Player* player, float deltaTime, float screenWidth);
void UpdatePlayerTrail(Player* player, Vector2 prevPosition);

// Trail render
void DrawPlayerWithTrail(const Player* player);

// Color
void UpdatePlayerColor(Player* player, bool isTimewarpActive);

#endif //PLAYER_H