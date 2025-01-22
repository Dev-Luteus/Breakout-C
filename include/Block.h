#ifndef BLOCK_H
#define BLOCK_H
#include <raylib.h>

typedef struct Block {
    Vector2 Position;
    int width;
    int height;
    Color color;
    int lives;
    bool active;
} Block;

Block InitBlock(int x, int y, int width, int height, int row);
Color GetBlockColor(int lives);

#endif //BLOCK_H