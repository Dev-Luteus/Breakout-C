#include "include/Player.h"

Player InitPlayer(int width, int height)
{
    Player player;
    player.position = (Vector2){ width / 2 - 25, height - 50 };
    player.speed = 1300.0f;
    player.width = 120;
    player.height = 10;
    player.lives = 5;
    player.score = 0;

    return player;
}
