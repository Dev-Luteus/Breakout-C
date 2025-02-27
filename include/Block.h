﻿#ifndef BLOCK_H
#define BLOCK_H

#include "VectorMath.h"

typedef struct Block {
    Vector2 position;
    int width;
    int height;
    Color color;
    int lives;
    bool active;
} Block;

Block InitBlock(int x, int y, int width, int height, int row);
Color GetBlockColor(int lives, bool isTimewarpActive);

#endif //BLOCK_H