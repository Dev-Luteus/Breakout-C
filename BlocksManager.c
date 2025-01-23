#include "BlocksManager.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

void CalculateBlockDimensions(int screenWidth, int screenHeight, float *blockWidth, float *blockHeight)
{
    float playableWidth = screenWidth * (1.0f - 2 * BLOCK_SIDE_OFFSET);
    float totalWidth = playableWidth - (BLOCK_COLUMNS - 1) * BLOCK_SPACING;

    *blockWidth = totalWidth / BLOCK_COLUMNS;
    *blockHeight = screenHeight * 0.03f;
}

void InitBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS], int screenWidth, int screenHeight)
{
    float blockWidth, blockHeight;
    CalculateBlockDimensions(screenWidth, screenHeight, &blockWidth, &blockHeight);

    float startX = screenWidth * BLOCK_SIDE_OFFSET;
    float startY = screenHeight * BLOCK_TOP_OFFSET;

    for (int row = 0; row < BLOCK_ROWS; row++)
    {
        for (int col = 0; col < BLOCK_COLUMNS; col++)
        {
            float x = startX + col * (blockWidth + BLOCK_SPACING);
            float y = startY + row * (blockHeight + BLOCK_SPACING);

            blocks[row][col] = InitBlock(x, y, blockWidth, blockHeight, row);
        }
    }
}

void DrawBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS])
{
    for (int row = 0; row < BLOCK_ROWS; ++row)
    {
        for (int col = 0; col < BLOCK_COLUMNS; ++col)
        {
            Block block = blocks[row][col];

            if (block.active)
            {
                DrawRectangle(block.position.x, block.position.y, block.width, block.height, block.color);

                // Convert our block.lives into a string array (for text)! Make space for null operator
                char lives[2];
                sprintf(lives, "%d", block.lives);

                Vector2 textPos = (Vector2)
                {
                    block.position.x + block.width/2 - 5,
                    block.position.y + block.height/2 - 10
                };

                DrawText(lives, textPos.x, textPos.y, 20, BLACK);
            }
        }
    }
}

bool CheckBlockCollision(Block* block, Ball* ball)
{
    if (!block->active)
    {
        return false;
    }

    Rectangle blockRect =
    {
        block->position.x,
        block->position.y,
        block->width,
        block->height
    };

    // Check collision between circle (ball) and rectangle (block)
    if (CheckCollisionCircleRec(ball->position, ball->radius, blockRect))
    {
        // Reduce block life
        block->lives--;

        if (block->lives <= 0)
        {
            block->active = false;
        }
        else
        {
            block->color = GetBlockColor(block->lives);
        }

        // Calculate collision side and ball reflection
        float blockCenterX = block->position.x + block->width/2;
        float blockCenterY = block->position.y + block->height/2;

        // Get ball's relative position to block center
        float dx = ball->position.x - blockCenterX;
        float dy = ball->position.y - blockCenterY;

        // Determine dominant axis of collision
        if (fabs(dx/block->width) > fabs(dy/block->height))
        {
            ball->direction.x *= -1;
        }
        else
        {
            ball->direction.y *= -1;
        }

        // Here we're trying to add some slight randomization to prevent straight/horizontal shots
        ball->direction.x += (float)(GetRandomValue(-5, 5)) / 100.0f;
        ball->direction = Vector2Normalize(ball->direction);

        return true;
    }

    return false;
}

bool AreAllBlocksDestroyed(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS])
{
    for (int row = 0; row < BLOCK_ROWS; row++)
    {
        for (int col = 0; col < BLOCK_COLUMNS; col++)
        {
            if (blocks[row][col].active) return false;
        }
    }
    return true;
}