#include "BlocksManager.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "Ball.h"
#include "VectorMath.h"

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
    if (!block->active) return false;

    // Check collision between circle (ball) and rectangle (block)
    float closestX = fmaxf(block->position.x,
                          fminf(ball->position.x, block->position.x + block->width));

    float closestY = fmaxf(block->position.y,
                          fminf(ball->position.y, block->position.y + block->height));

    // Here we try to calculate the distance between the closest point and the circle center
    Vector2 closestPoint = MyVector2Create(closestX, closestY);
    Vector2 ballToClosest = MyVector2Subtract(closestPoint, ball->position);
    float distanceSquared = MyVector2DotProduct(ballToClosest, ballToClosest);

    if (distanceSquared <= (ball->radius * ball->radius))
    {
        // Collision detected
        block->lives--;
        if (block->lives <= 0)
        {
            block->active = false;
        }
        else
        {
            block->color = GetBlockColor(block->lives);
        }

        // Calculate exact collision normal
        Vector2 blockCenter = MyVector2Create(
            block->position.x + block->width/2,
            block->position.y + block->height/2
        );

        Vector2 ballToBlock = MyVector2Subtract(ball->position, blockCenter);
        float dx = fabs(ballToBlock.x) / (block->width/2);
        float dy = fabs(ballToBlock.y) / (block->height/2);

        if (dx > dy)
        {
            ball->direction.x *= -1;
        }
        else
        {
            ball->direction.y *= -1;
        }

        // Add slight randomization
        Vector2 randomOffset = MyVector2Create(
            (float)(GetRandomValue(-5, 5)) / 100.0f,
            0
        );
        ball->direction = MyVector2Add(ball->direction, randomOffset);
        ball->direction = MyVector2Normalize(ball->direction);

        // Speed boost
        ball->speed = Clamp(
            ball->speed * 1.05f,
            BALL_SPEED_MIN,
            BALL_SPEED_MAX
        );

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