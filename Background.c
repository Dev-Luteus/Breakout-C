﻿#include "Background.h"
#include <math.h>
#include <raymath.h>

Background InitBackground(int width, int height)
{
    Background background =
    {
        .time = 0,
        .scanlinePos = 0,
        .phosphorColor = (Color){0, 255, 0, 255},
        .colorTransition = 0.0f,
        .screenCurvature = 0.05f, // Current Curvature!
        .flickerIntensity = 0.4f,
        .vignetteIntensity = 0.3f,
        .scanlineIntensity = 1.0f,

        .effectTexture = LoadRenderTexture(width, height),
        .finalTexture = LoadRenderTexture(width, height),

        // Allocate memory for all possible quads (size of struct)
        .quadCache = (DistortedQuad*)MemAlloc(MAX_QUADS * sizeof(DistortedQuad)),
        .distortionNeedsUpdate = true,
        .lastCurvature = 0.05f, // Curvature Cache! We use this to cache our screen curvature values!

        .staticEffects = LoadRenderTexture(width, height),
        .uiTexture = LoadRenderTexture(width, height),
        .staticEffectsNeedUpdate = true, // Here, we force our first calculation of static effects
    };

    UpdateStaticEffects(&background, width, height);
    return background;
}

// Due to our effects tanking FPS, I've refactored some of them to be static! Hence this method.
void UpdateStaticEffects(Background* background, int width, int height)
{
    if (!background->staticEffectsNeedUpdate)
    {
        return;
    }

    BeginTextureMode(background->staticEffects);
    {
        ClearBackground(BLANK);

        // Base phosphor!
        DrawRectangle(0, 0, width, height,
                     ColorAlpha(background->phosphorColor, 0.3f));

        // Scanline effect!
        for (int y = 0; y < height; y += 4)
        {
            DrawRectangle(0, y, width, 2,
                         ColorAlpha(background->phosphorColor, 0.4f));
        }

        // Vignette effect!
        float vignetteSize = (float)width * 0.8f;

        DrawCircleGradient(width/2, height/2,
                          vignetteSize,
                          (Color){0, 0, 0, 0},
                          (Color){0, 0, 0, 180 * background->vignetteIntensity});
    }
    EndTextureMode();

    background->staticEffectsNeedUpdate = false;
}

// Our main update method! Here we adjust our Scanline positions!
void UpdateBackground(Background* background, float deltaTime, bool isTimewarpActive)
{
    background->time += deltaTime;
    background->scanlinePos += 100.0f * deltaTime;

    if (background->scanlinePos > GetScreenHeight())
    {
        background->scanlinePos = 0;
    }

    // Here, I'm trying to implemenet a smoooth transition to the Timewarp colour! We do this by using Lerp.
    const float TRANSITION_SPEED = 4.0f;

    if (isTimewarpActive && background->colorTransition < 1.0f)
    {
        background->colorTransition += deltaTime * TRANSITION_SPEED;

        if (background->colorTransition > 1.0f)
        {
            background->colorTransition = 1.0f;
        }
    }
    else if (!isTimewarpActive && background->colorTransition > 0.0f)
    {
        background->colorTransition -= deltaTime * TRANSITION_SPEED;

        if (background->colorTransition < 0.0f)
        {
            background->colorTransition = 0.0f;
        }
    }

    // Here, we actually lerp between the two colours =)
    Color normalColor = (Color){0, 255, 0, 255};
    Color purpleColor = BACKGROUND_PURPLE;

    // Set background colour to a new colour!
    background->phosphorColor = (Color)
    {
        (unsigned char)Lerp(normalColor.r, purpleColor.r, background->colorTransition),
        (unsigned char)Lerp(normalColor.g, purpleColor.g, background->colorTransition),
        (unsigned char)Lerp(normalColor.b, purpleColor.b, background->colorTransition),
        255
    };

    background->staticEffectsNeedUpdate = true;
}

/* I read about Barrel Distortion Effects and their mathematical formula equivalents.
 * I tried using this formula: r' = r * (1 + kr²), but it didn't work as expected.
 * Instead, we use this formula p' = p * (1.0 - k * (p.x² + p.y²)) found here:
 * https://www.geeks3d.com/20140213/glsl-shader-library-fish-eye-and-dome-and-barrel-distortion-post-processing-filters/2/
 */
Vector2 DistortPoint(Vector2 point, Vector2 center, float curveAmount, int width, int height)
{
    // Convert to normalized device coordinates (-1 to 1)
    Vector2 p =
    {
        (point.x - center.x) / (width * 0.5f),
        (point.y - center.y) / (height * 0.5f)
    };

    /* Here, we calculate the normalized distance from the center,
     * Using a provided formula I found:
     * p' = p * (1.0 - k * (p.x² + p.y²)),
     * where k is our curve amount */
    float distSqr = p.x * p.x + p.y * p.y;
    float scale = 1.0f - curveAmount * distSqr;

    // Apply the distortion
    p.x *= scale;
    p.y *= scale;

    // Convert back to screen coordinates
    return (Vector2)
    {
        center.x + p.x * width * 0.5f,
        center.y + p.y * height * 0.5f
    };
}

// Our main draw method! This is supposed to compose the final image after all effects!
void DrawBackground(Background* background, int width, int height, Texture2D gameScreen)
{
    // Update effects if needed ( we do this in Init too )
    UpdateStaticEffects(background, width, height); // background->staticEffects

    // Drawing our dynamic animated effects!
    BeginTextureMode(background->effectTexture); // background->effectTexture
    {
        ClearBackground(BLANK);

        // Moving scanline effect!
        float scanBrightness = (sinf(background->time * 5) + 1.0f) * 0.5f;
        Color scanColor = ColorAlpha(background->phosphorColor, 0.4f * scanBrightness);
        DrawRectangle(0, (int)background->scanlinePos - 2, width, 3, scanColor);

        // Screen flicker effect!
        float flicker = 1.0f + sinf(background->time * 40) * background->flickerIntensity;
        Color flickerColor = ColorAlpha(background->phosphorColor, 0.1f * flicker);
        DrawRectangle(0, 0, width, height, flickerColor);
    }
    EndTextureMode();

    // We now compose and apply the barrel distortion to the final image
    BeginTextureMode(background->finalTexture); // background->finalTexture
    {
        ClearBackground(BLACK);
        Vector2 center = {width/2.0f, height/2.0f};

        // In order to not flood our memory, here I'm trying to only update distortion if needed
        if (background->screenCurvature != background->lastCurvature)
        {
            /* Curvature changed: recalculate distortion.
             * In our current implementation, we never change our screen curvature. But I put this here:
             * Because if in the future I wanted to make this a game-play mechanic, I am now able to!
             * Example: Pulsing screen curvature?? Extreme curvature during a special powerup?? Boss??
             */
            background->distortionNeedsUpdate = true;
            background->lastCurvature = background->screenCurvature;
        }

        // These are our grid dimensions!
        int horizontalQuads = (width + QUAD_SIZE - 1) / QUAD_SIZE;
        int verticalQuads = (height + QUAD_SIZE - 1) / QUAD_SIZE;

        // Here we draw our distorted game screen
        if (background->distortionNeedsUpdate)
        {
            int quadIndex = 0;

            for(int y = 0; y < verticalQuads; y++)
            {
                float screenY = y * QUAD_SIZE; // Y Pos

                for(int x = 0; x < horizontalQuads; x++)
                {
                    float screenX = x * QUAD_SIZE; // X Pos

                    // To optimize this code, we try to only calculate distortion for the visible area
                    float distFromCenter = sqrtf
                    (
                        powf((screenX - center.x) / width, 2) +
                        powf((screenY - center.y) / height, 2)
                    );

                    // Here, we use larger quads for the edges of the screen
                    int currentQuadSize = distFromCenter > 0.7f ? QUAD_SIZE * 2 : QUAD_SIZE;

                    Vector2 p1 = DistortPoint((Vector2){screenX, screenY}, center,
                                      background->screenCurvature, width, height);
                    Vector2 p2 = DistortPoint((Vector2){screenX + QUAD_SIZE, screenY}, center,
                                          background->screenCurvature, width, height);
                    Vector2 p3 = DistortPoint((Vector2){screenX, screenY + QUAD_SIZE}, center,
                                          background->screenCurvature, width, height);

                    /* Here, we cache all of our quad properties, to draw them later (saving CPU cycles)
                     * We get a specific quad from our array, and define it!
                     */
                    background->quadCache[quadIndex].position = p1;
                    background->quadCache[quadIndex].width = p2.x - p1.x;
                    background->quadCache[quadIndex].height = p3.y - p1.y;
                    quadIndex++;
                }
            }
            background->distortionNeedsUpdate = false;
        }

        /* Just for clarity, this is the distortion of our game screen!
         * This is also where we re-use our previously cached quads, to draw them withour recalculating.
         */
        int quadIndex = 0;

        for(int y = 0; y < verticalQuads; y++)
        {
            float screenY = y * QUAD_SIZE;

            for(int x = 0; x < horizontalQuads; x++)
            {
                float screenX = x * QUAD_SIZE;

                /* Here, we first make our array of distorted quad structs
                 * Since our quads and their positions are pre-calculated, we only need their positions here!
                 * Here, we get the memory adress of our quad, and we do this for every quad.
                 * So, we allocate memory for all quads in our array, and then draw them
                 */
                DistortedQuad* quad = &background->quadCache[quadIndex++];

                // Skip if quad is outside screen
                if (quad->position.x + quad->width < 0 || quad->position.x > width ||
                quad->position.y + quad->height < 0 || quad->position.y > height)
                {
                    continue;
                }

                // Draw the game screen with distortion!
                DrawTexturePro(gameScreen,
                    // Original
                    (Rectangle){screenX, screenY, QUAD_SIZE, QUAD_SIZE}, //
                    // Destination rectangle: Our Distorted Quads
                    (Rectangle){quad->position.x, quad->position.y, //
                          quad->width, quad->height}, //
                    (Vector2){0, 0}, 0, WHITE); //
            }
        }

        // Drawing our static effects!
        DrawTexture(background->staticEffects.texture, 0, 0, WHITE);

        // Then, we must draw to overlay our dynamic effects
        DrawTexture(background->effectTexture.texture, 0, 0, WHITE);
    }
    EndTextureMode();

    // Draw the final result
    DrawTexture(background->finalTexture.texture, 0, 0, WHITE);
}

// Unloading cause otherwise bad (Free memory)
void UnloadBackground(Background* background)
{
    UnloadRenderTexture(background->effectTexture);
    UnloadRenderTexture(background->finalTexture);
    UnloadRenderTexture(background->staticEffects);
    UnloadRenderTexture(background->uiTexture);
    MemFree(background->quadCache);
}