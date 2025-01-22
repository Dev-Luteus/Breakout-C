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
                DrawRectangle(block.Position.x, block.Position.y, block.width, block.height, block.color);

                // Convert our block.lives into a string array (for text)! Make space for null operator
                char lives[2];
                sprintf(lives, "%d", block.lives);

                Vector2 textPos = (Vector2)
                {
                    block.Position.x + block.width/2 - 5,
                    block.Position.y + block.height/2 - 10
                };

                DrawText(lives, textPos.x, textPos.y, 20, BLACK);
            }
        }
    }
}

bool CheckBlockCollision(Block* block, Ball* ball)
{
    if (!block->active) return false;

    // We use ball->radius * 2 to slightly expand its collision
    Rectangle blockRect = {
        block->Position.x - ball->radius,
        block->Position.y - ball->radius,
        block->width + ball->radius * 2,
        block->height + ball->radius * 2
    };

    Vector2 ballPos = ball->position;

    if (CheckCollisionPointRec(ballPos, blockRect))
    {
        block->lives--;

        if (block->lives == 0)
        {
            block->active = false;
        }
        else
        {
            block->color = GetBlockColor(block->lives);
        }

        float overlapLeft = ballPos.x - blockRect.x;
        float overlapRight = blockRect.x + blockRect.width - ballPos.x;
        float overlapTop = ballPos.y - blockRect.y;
        float overlapBottom = blockRect.y + blockRect.height - ballPos.y;

        // Finding smallest overlap in order to fix an unlikely bug that can clear a whole column
        float minOverlap = fminf(fminf(overlapLeft, overlapRight),
                               fminf(overlapTop, overlapBottom));

        if (minOverlap == overlapLeft || minOverlap == overlapRight)
        {
            ball->direction.x *= -1;
        }
        else
        {
            ball->direction.y *= -1;
        }

        // Adding slight randomisation when standing still!
        ball->direction.x += (GetRandomValue(-10, 10) / 100.0f);
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