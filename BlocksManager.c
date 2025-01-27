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
                int screenWidth, int screenHeight, int rowCount, int columnCount)
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
            blocks[row][col].color = GetBlockColor(blocks[row][col].lives);
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

bool CheckBlockCollision(Block* block, Ball* ball)
{
    if (!block->active)
    {
        return false;
    }

    // To prevent the ball from going through gaps it shouldn't, we expand the blocsk radius
    Rectangle expandedBlock = {
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
     *
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
            block->color = GetBlockColor(block->lives);
        }

        // Here we calculate a blocks center, to help determine which angle to reflect the ball
        Vector2 blockCenter = MyVector2Create
        (
            block->position.x + block->width/2,
            block->position.y + block->height/2
        );

        /* Here we normalize the distance relative to the block, and determine which side was hit
         * normalizedX / Y in this instance becomes a range beetween: −1, 1
         */
        Vector2 collisionPoint = MyVector2Subtract(ball->position, blockCenter);
        float normalizedX = collisionPoint.x / (block->width/2 + ball->radius);
        float normalizedY = collisionPoint.y / (block->height/2 + ball->radius);

        /* Fabs = Absolute value
         * Here, we determine which collision face is hit, and also attempt to handle corner cases
         * Fabs > 0.8f means that the ball is close to the corner! */
        if (fabs(normalizedX) > 0.8f && fabs(normalizedY) > 0.8f)
        {
            ball->direction.x *= -1;
            ball->direction.y *= -1;
        }
        else if (fabs(normalizedX) > fabs(normalizedY)) // Side collision
        {
            ball->direction.x *= -1;

            // Here, we adjust the vertical position to prevent horizontal locking! (horizontal bounce)
            float verticalAdjust = (GetRandomValue(-10, 10) / 100.0f);
            ball->direction.y += verticalAdjust;
        }
        else // Top Bottom
        {
            ball->direction.y *= -1;

            float horizontalAdjust = (GetRandomValue(-10, 10) / 100.0f);
            ball->direction.x += horizontalAdjust;
        }

        // Prevent near-horizontal or near-vertical trajectories
        const float MIN_COMPONENT = 0.2f;

        if (fabs(ball->direction.x) < MIN_COMPONENT)
        {
            ball->direction.x = (ball->direction.x >= 0) ? MIN_COMPONENT : -MIN_COMPONENT;
            ball->direction = MyVector2Normalize(ball->direction);
        }
        if (fabs(ball->direction.y) < MIN_COMPONENT)
        {
            ball->direction.y = (ball->direction.y >= 0) ? MIN_COMPONENT : -MIN_COMPONENT;
            ball->direction = MyVector2Normalize(ball->direction);
        }

        // We must normalize the direction!
        ball->direction = MyVector2Normalize(ball->direction);
        ball->speed = Clamp(ball->speed * 1.04f, BALL_SPEED_MIN, BALL_SPEED_MAX);

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