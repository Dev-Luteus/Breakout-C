#include "Block.h"
#include "BlocksManager.h"

Color GetBlockColor(int lives)
{
    switch(lives)
    {
        case 6: return BLOCK_COLOR_1;
        case 5: return BLOCK_COLOR_2;
        case 4: return BLOCK_COLOR_3;
        case 3: return BLOCK_COLOR_4;
        case 2: return BLOCK_COLOR_5;
        case 1: return BLOCK_COLOR_6;
        default: return GRAY;
    }
}

// I want to initialise blocks with different lives, and colours that correspond to them!
// I also want to make the rows determine the lives of the blocks in a descending order!
Block InitBlock(int x, int y, int width, int height, int row)
{
    Block block;
    block.position = (Vector2) {x, y};
    block.width = width;
    block.height = height;
    block.lives = BLOCK_ROWS+1 - row;
    block.color = GetBlockColor(block.lives);
    block.active = true;

    return block;
}