#ifndef BLOCKS_MANAGER_H
#define BLOCKS_MANAGER_H

#include "Ball.h"
#include "../include/Block.h"

#define BLOCK_ROWS 5
#define BLOCK_COLUMNS 8
#define BLOCK_SPACING 10
#define BLOCK_TOP_OFFSET 0.18f  // 0.15
#define BLOCK_SIDE_OFFSET 0.12f  //0.1

#define BLOCK_COLOR_1 (Color){0x04, 0x31, 0x04, 0xFF}  // #043104 - Deep green (Weakest)
#define BLOCK_COLOR_2 (Color){0x07, 0x5A, 0x07, 0xFF}  // #075A07 - Medium-dark green
#define BLOCK_COLOR_3 (Color){0x0B, 0x84, 0x0B, 0xFF}  // #0B840B - Medium green
#define BLOCK_COLOR_4 (Color){0x0F, 0xAD, 0x0F, 0xFF}  // #0FAD0F - Medium-bright green
#define BLOCK_COLOR_5 (Color){0x12, 0xD6, 0x12, 0xFF}  // #12D612 - Bright green
#define BLOCK_COLOR_6 (Color){0x16, 0xFF, 0x16, 0xFF}  // #16FF16 - Pure phosphor (Strongest)

void CalculateBlockDimensions(int screenWidth, int screenHeight, float* blockWidth, float* blockHeight);
void InitBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS], int screenWidth, int screenHeight);
void DrawBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS]);

bool CheckBlockCollision(Block* block, Ball* ball);
bool AreAllBlocksDestroyed(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS]);

#endif // BLOCKS_MANAGER_H
