#ifndef BLOCKS_MANAGER_H
#define BLOCKS_MANAGER_H

#include "Ball.h"
#include "../include/Block.h"

#define BLOCK_ROWS 3
#define BLOCK_COLUMNS 8
#define BLOCK_SPACING 10
#define BLOCK_TOP_OFFSET 0.18f  // 0.15
#define BLOCK_SIDE_OFFSET 0.12f  //0.1

#define BLOCK_COLOR_1 (Color){0x31, 0x14, 0x3F, 0xFF}  // #31143F
#define BLOCK_COLOR_2 (Color){0x55, 0x16, 0x46, 0xFF}  // #551646
#define BLOCK_COLOR_3 (Color){0x79, 0x19, 0x4E, 0xFF}  // #79194E
#define BLOCK_COLOR_4 (Color){0x9D, 0x1C, 0x56, 0xFF}  // #9D1C56
#define BLOCK_COLOR_5 (Color){0xC1, 0x1E, 0x5D, 0xFF}  // #C11E5D
#define BLOCK_COLOR_6 (Color){0xE5, 0x21, 0x65, 0xFF}  // #E52165

void CalculateBlockDimensions(int screenWidth, int screenHeight, float* blockWidth, float* blockHeight);
void InitBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS], int screenWidth, int screenHeight);
void DrawBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS]);

bool CheckBlockCollision(Block* block, Ball* ball);
bool AreAllBlocksDestroyed(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS]);

#endif // BLOCKS_MANAGER_H
