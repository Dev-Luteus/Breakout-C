#include "BlocksManager.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "Ball.h"
#include "VectorMath.h"

void CalculateBlockDimensions(int screenWidth, int screenHeight, float *blockWidth, float *blockHeight, int columnCount)
{
    float playableWidth = screenWidth * (1.0f - 2 * BLOCK_SIDE_OFFSET);
    float totalWidth = playableWidth - (columnCount - 1) * BLOCK_SPACING;  // Need to pass columnCount parameter

    *blockWidth = totalWidth / columnCount;
    *blockHeight = screenHeight * 0.03f;
}

void InitBlocks(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS],
                int screenWidth, int screenHeight, int rowCount, int columnCount, bool isTimewarpActive)
{
    float blockWidth, blockHeight;
    CalculateBlockDimensions(screenWidth, screenHeight, &blockWidth, &blockHeight, columnCount);

    float startX = screenWidth * BLOCK_SIDE_OFFSET;
    float startY = screenHeight * BLOCK_TOP_OFFSET;

    // Initialize all blocks to inactive first
    for (int row = 0; row < MAX_BLOCK_ROWS; row++)
    {
        for (int col = 0; col < MAX_BLOCK_COLUMNS; col++)
        {
            blocks[row][col].position = (Vector2){0, 0};
            blocks[row][col].width = 0;
            blocks[row][col].height = 0;
            blocks[row][col].lives = 0;
            blocks[row][col].color = BLACK;
            blocks[row][col].active = false;
        }
    }

    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);
    columnCount = Clamp(columnCount, MIN_BLOCK_COLUMNS, MAX_BLOCK_COLUMNS);

    // Here, we have to calculate a new spacing in order for us to maintain centered blocks
    float totalWidth = screenWidth * (1.0f - 2 * BLOCK_SIDE_OFFSET);
    float blockAndSpacing = (totalWidth - BLOCK_SPACING) / columnCount;
    blockWidth = blockAndSpacing - BLOCK_SPACING;

    // Initialize active blocks
    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < columnCount; col++)
        {
            float x = startX + col * (blockWidth + BLOCK_SPACING);
            float y = startY + row * (blockHeight + BLOCK_SPACING);

            blocks[row][col].position = (Vector2){x, y};
            blocks[row][col].width = blockWidth;
            blocks[row][col].height = blockHeight;
            blocks[row][col].lives = rowCount - row;
            blocks[row][col].color = GetBlockColor(blocks[row][col].lives, isTimewarpActive);
            blocks[row][col].active = true;
        }
    }
}

void DrawBlocks(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS], int rowCount, int columnCount)
{
    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);
    columnCount = Clamp(columnCount, MIN_BLOCK_COLUMNS, MAX_BLOCK_COLUMNS);

    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < columnCount; ++col)
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

bool CheckBlockCollision(Block* block, Ball* ball, bool isTimewarpActive)
{
    if (!block->active)
    {
        return false;
    }

    // To prevent the ball from going through gaps it shouldn't, we expand the blocsk radius
    Rectangle expandedBlock =
    {
        block->position.x - ball->radius,
        block->position.y - ball->radius,
        block->width + (ball->radius * 2),
        block->height + (ball->radius * 2)
    };

    // Damage but don't collide!
    if (ball->isGhost)
    {
        if (CheckCollisionCircleRec(ball->position, ball->radius, expandedBlock))
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
     * Pythagoras:
     * distanceSquared = (Cx - Px)² + (Cy - Py), aka:
     * distanceSquared = (closestX−ball.centerX)^2 + (closestY−ball.centerY)^2 (pythagoras)
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
            block->color = GetBlockColor(block->lives, isTimewarpActive);
        }

        // Here, we make calculate the actual blocks width for precise reflection ( minus the ball radius )
        Rectangle actualBlock =
        {
            block->position.x,
            block->position.y,
            block->width,
            block->height
        };

        // I revised the algorithm to use *depth calculation* instead of distance calculation
        float leftDepth = ball->position.x + ball->radius - actualBlock.x;
        float rightDepth = actualBlock.x + actualBlock.width - (ball->position.x - ball->radius);
        float topDepth = ball->position.y + ball->radius - actualBlock.y;
        float bottomDepth = actualBlock.y + actualBlock.height - (ball->position.y - ball->radius);

        float minDepth = leftDepth;
        int collisionSide = 0; // 0: left, 1: right, 2: top, 3: bottom

        if (rightDepth < minDepth)
        {
            minDepth = rightDepth;
            collisionSide = 1;
        }
        if (topDepth < minDepth)
        {
            minDepth = topDepth;
            collisionSide = 2;
        }
        if (bottomDepth < minDepth)
        {
            minDepth = bottomDepth;
            collisionSide = 3;
        }

        // This is where we actually apply the reflection based on the side of the block!
        switch (collisionSide)
        {
            case 0: // Left
            case 1: // Right
                ball->direction.x *= -1;
                ball->direction.y += (GetRandomValue(-5, 5) / 100.0f);
            break;

            case 2: // Top
            case 3: // Bottom
                ball->direction.y *= -1;
                ball->direction.x += (GetRandomValue(-5, 5) / 100.0f);
            break;
        }
        
        // Normalizing our direction vector!
        AdjustBallDirection(ball);
        ball->speed = Clamp(ball->speed * SPEED_INCREASE_FACTOR, BALL_SPEED_MIN, BALL_SPEED_MAX);
        return true;
    }

    return false;
}

bool AreAllBlocksDestroyed(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS], int rowCount, int columnCount)
{
    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);
    columnCount = Clamp(columnCount, MIN_BLOCK_COLUMNS, MAX_BLOCK_COLUMNS);

    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < columnCount; col++)
        {
            if (blocks[row][col].active)
            {
                return false;
            }
        }
    }
    return true;
}

void UpdateBlockColors(Block blocks[MAX_BLOCK_ROWS][MAX_BLOCK_COLUMNS],
                      int rowCount, int columnCount,
                      bool isTimewarpActive)
{
    rowCount = Clamp(rowCount, MIN_BLOCK_ROWS, MAX_BLOCK_ROWS);
    columnCount = Clamp(columnCount, MIN_BLOCK_COLUMNS, MAX_BLOCK_COLUMNS);

    for (int row = 0; row < rowCount; row++)
    {
        for (int col = 0; col < columnCount; col++)
        {
            if (blocks[row][col].active)
            {
                blocks[row][col].color = GetBlockColor(blocks[row][col].lives, isTimewarpActive);
            }
        }
    }
}