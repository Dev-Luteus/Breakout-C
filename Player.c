#include "include/Player.h"

Player InitPlayer(int width, int height)
{
    Player player;
    player.position = (Vector2){ width / 2 - 25, height - 50 };
    player.baseSpeed = 1000.0f;
    player.speed = 1000.0f;
    player.baseWidth = 100;
    player.width = 100;
    player.height = 10;
    player.lives = 5;
    player.score = 0;

    return player;
}
