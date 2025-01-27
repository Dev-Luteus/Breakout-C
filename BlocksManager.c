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

void InitBlocks(Block blocks[MAX_BLOCK_ROWS][BLOCK_COLUMNS], int screenWidth, int screenHeight, int rowCount)
{
    float blockWidth, blockHeight;
    CalculateBlockDimensions(screenWidth, screenHeight, &blockWidth, &blockHeight);

    float startX = screenWidth * BLOCK_SIDE_OFFSET;
    float startY = screenHeight * BLOCK_TOP_OFFSET;

    // First we initialize all blocks to inactive
    for (int row = 0; row < MAX_BLOCK_ROWS; row++)
    {
        for (int col = 0; col < BLOCK_COLUMNS; col++)
        {
            blocks[row][col].position = (Vector2){0, 0};
            blocks[row][col].width = 0;
            blocks[row][col].height = 0;
            blocks[row][col].lives = 0;
            blocks[row][col].color = BLACK;
            blocks[row][col].active = false;
        }
    }

    // Clamp rowCount to valid range
    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);

    // Then initialize only the active rows
    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < BLOCK_COLUMNS; col++)
        {
            float x = startX + col * (blockWidth + BLOCK_SPACING);
            float y = startY + row * (blockHeight + BLOCK_SPACING);

            blocks[row][col].position = (Vector2){x, y};
            blocks[row][col].width = blockWidth;
            blocks[row][col].height = blockHeight;
            blocks[row][col].lives = rowCount - row;
            blocks[row][col].color = GetBlockColor(blocks[row][col].lives);
            blocks[row][col].active = true;
        }
    }
}

void DrawBlocks(Block blocks[BLOCK_ROWS][BLOCK_COLUMNS], int rowCount)
{
    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);

    for (int row = 0; row < rowCount; ++row)
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

                Vector2 textPos = MyVector2Create
                (
                    block.position.x + block.width/2 - 5,
                    block.position.y + block.height/2 - 10
                );

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

    // Damage but don't collide!
    if (ball->isGhost)
    {
        Rectangle blockRect = {
            block->position.x - ball->radius,
            block->position.y - ball->radius,
            block->width + (ball->radius * 2),
            block->height + (ball->radius * 2)
        };

        if (CheckCollisionCircleRec(ball->position, ball->radius, blockRect))
        {
            block->lives--;

            if (block->lives <= 0)
            {
                block->active = false;
            }

            return true;
        }
        return false;
    }

    // To prevent the ball from going through gaps it shouldn't, we expand the blocsk radius
    Rectangle expandedBlock = {
        block->position.x - ball->radius,
        block->position.y - ball->radius,
        block->width + (ball->radius * 2),
        block->height + (ball->radius * 2)
    };

    /* Here we find the closest point on our block, relevant to the balls center
     * This closest points, then determines where the ball collides with the block.
     */
    float closestX = fmaxf(expandedBlock.x,
                          fminf(ball->position.x, expandedBlock.x + expandedBlock.width));

    float closestY = fmaxf(expandedBlock.y,
                          fminf(ball->position.y, expandedBlock.y + expandedBlock.height));

    /* Here, we then calculate the distance between the closest point, and our ball's center
     * We do this to get the squared distance between the two points, which we then use to:
     * Compare the squared distance to the squared radius of the ball, to determine if a collision occurred
     */
    Vector2 closestPoint = MyVector2Create(closestX, closestY);
    Vector2 ballToClosest = MyVector2Subtract(closestPoint, ball->position);
    float distanceSquared = MyVector2DotProduct(ballToClosest, ballToClosest);

    // Check if ball/block collision occurred (distance² ≤ radius²)
    if (distanceSquared <= (ball->radius * ball->radius))
    {
        // Reduce block life
        block->lives -= ball->damageMultiplier;

        if (block->lives <= 0)
        {
            block->active = false;
        }
        else
        {
            block->color = GetBlockColor(block->lives);
        }

        // Here we calculate a blocks center, to help determine which angle to reflect the ball
        Vector2 blockCenter = MyVector2Create(
            block->position.x + block->width/2,
            block->position.y + block->height/2
        );

        // Here we normalize the distance relative to the block, and determine which side was hit
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

        // Add randomization to prevent repetitive patterns
        Vector2 randomOffset = MyVector2Create
        (
            (float)(GetRandomValue(-5, 5)) / 100.0f,
            0
        );
        ball->direction = MyVector2Add(ball->direction, randomOffset);
        ball->direction = MyVector2Normalize(ball->direction);

        ball->speed = Clamp
        (
            ball->speed * 1.05f,
            BALL_SPEED_MIN,
            BALL_SPEED_MAX
        );

        return true;
    }

    return false;
}

bool AreAllBlocksDestroyed(Block blocks[MAX_BLOCK_ROWS][BLOCK_COLUMNS], int rowCount)
{
    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);

    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < BLOCK_COLUMNS; col++)
        {
            if (blocks[row][col].active)
            {
                return false;
            }
        }
    }
    return true;
}