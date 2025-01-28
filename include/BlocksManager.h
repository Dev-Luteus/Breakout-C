#ifndef BLOCKS_MANAGER_H
#define BLOCKS_MANAGER_H

#include "Ball.h"
#include "../include/Block.h"

#define MAX_BLOCK_ROWS 6
#define MIN_BLOCK_ROWS 3
#define MIN_BLOCK_COLUMNS 4
#define MAX_BLOCK_COLUMNS 8

#define BLOCK_SPACING 10
#define BLOCK_TOP_OFFSET 0.18f
#define BLOCK_SIDE_OFFSET 0.12f

#define BLOCK_COLOR_1 (Color){0x04, 0x31, 0x04, 0xFF}  // #043104 - Deep green (Weakest)
#define BLOCK_COLOR_2 (Color){0x07, 0x5A, 0x07, 0xFF}  // #075A07 - Medium-dark green
#define BLOCK_COLOR_3 (Color){0x0B, 0x84, 0x0B, 0xFF}  // #0B840B - Medium green
#define BLOCK_COLOR_4 (Color){0x0F, 0xAD, 0x0F, 0xFF}  // #0FAD0F - Medium-bright green
#define BLOCK_COLOR_5 (Color){0x12, 0xD6, 0x12, 0xFF}  // #12D612 - Bright green
#define BLOCK_COLOR_6 (Color){0x16, 0xFF, 0x16, 0xFF}  // #16FF16 - Pure phosphor (Strongest)

#define BLOCK_COLOR_1_PURPLE (Color){0x19, 0x04, 0x31, 0xFF}  // Deep purple (Weakest)
#define BLOCK_COLOR_2_PURPLE (Color){0x2D, 0x07, 0x5A, 0xFF}  // Medium-dark purple
#define BLOCK_COLOR_3_PURPLE (Color){0x42, 0x0B, 0x84, 0xFF}  // Medium purple
#define BLOCK_COLOR_4_PURPLE (Color){0x56, 0x0F, 0xAD, 0xFF}  // Medium-bright purple
#define BLOCK_COLOR_5_PURPLE (Color){0x6B, 0x12, 0xD6, 0xFF}  // Bright purple
#define BLOCK_COLOR_6_PURPLE (Color){0x80, 0x16, 0xFF, 0xFF}  // Pure phosphor purple (Strongest)

void CalculateBlockDimensions(int screenWidth, int screenHeight, float* blockWidth, float* blockHeight, int columnCount);
void InitBlocks(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS],
                int screenWidth, int screenHeight, int rowCount, int columnCount, bool isTimewarpActive);
void DrawBlocks(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS], int rowCount, int columnCount);

bool CheckBlockCollision(Block* block, Ball* ball, bool isTimewarpActive);
bool AreAllBlocksDestroyed(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS], int rowCount, int columnCount);

void UpdateBlockColors(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS],
                      int rowCount, int columnCount, bool isTimewarpActive);
#endif // BLOCKS_MANAGER_H
