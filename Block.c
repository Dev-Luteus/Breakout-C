#include "Block.h"
#include "BlocksManager.h"

Color GetBlockColor(int lives, bool isTimewarpActive)
{
    if (!isTimewarpActive)
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
    else
    {
        switch(lives)
        {
            case 6: return BLOCK_COLOR_1_PURPLE;
            case 5: return BLOCK_COLOR_2_PURPLE;
            case 4: return BLOCK_COLOR_3_PURPLE;
            case 3: return BLOCK_COLOR_4_PURPLE;
            case 2: return BLOCK_COLOR_5_PURPLE;
            case 1: return BLOCK_COLOR_6_PURPLE;
            default: return GRAY;
        }
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
    block.lives = MAX_BLOCK_ROWS - row;
    block.color = GetBlockColor(block.lives, false);
    block.active = true;

    return block;
}