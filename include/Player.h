#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

#define PLAYER_SPEED_BOOST 1.1
#define PLAYER_COLOR (Color){ 0xE8, 0x67, 0x94, 0xFF }

typedef struct Player {
    Vector2 position;
    float baseSpeed;
    float speed;
    int height;
    int baseWidth;
    int width;
    int lives;
    int score;
} Player;

Player InitPlayer(int width, int height);

#endif //PLAYER_H
