#include "Background.h"
#include <math.h>

Background InitBackground(int width, int height)
{
    Background background =
    {
        .time = 0,
        .scanlinePos = 0,
        .phosphorColor = (Color){0, 255, 0, 128},
        .screenCurvature = 0.05f,
        .flickerIntensity = 0.03f,
        .vignetteIntensity = 0.5f,
        .scanlineIntensity = 0.1f,
        .renderTexture = LoadRenderTexture(width, height)
    };

    return background;
}

// Our main update method! Here we adjust our Scanline positions!
void UpdateBackground(Background* background, float deltaTime)
{
    background->time += deltaTime;

    background->scanlinePos += 200.0f * deltaTime;

    if (background->scanlinePos > GetScreenHeight())
    {
        background->scanlinePos = 0;
    }
}

/* I read about Barrel Distortion Effects and their mathematical formula equivalents.
 * I tried using this formula: r' = r * (1 + kr²), but it didn't work as expected.
 * Instead, we use this formula p' = p * (1.0 - k * (p.x² + p.y²)) found here:
 * https://www.geeks3d.com/20140213/glsl-shader-library-fish-eye-and-dome-and-barrel-distortion-post-processing-filters/2/
 *
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

// Our main draw method!
void DrawBackground(Background background, int width, int height)
{
    // This is a RenderTexture method, we want to first render all effects to our the render texture
    BeginTextureMode(background.renderTexture);
    {
        ClearBackground(BLACK);

        // Draw base phosphor layer!
        DrawRectangle(0, 0, width, height,
                     ColorAlpha(background.phosphorColor, 0.05f));

        // Our loop for drawing a grain like effect using DrawPixel!
        for (int i = 0; i < width * height / 100; i++)
        {
            int x = GetRandomValue(0, width - 1);
            int y = GetRandomValue(0, height - 1);
            float noiseIntensity = (float)GetRandomValue(0, 128) / 500.0f;

            DrawPixel(x, y, ColorAlpha(background.phosphorColor, noiseIntensity));
        }

        // A Scanning line effect!
        float scanBrightness = (sinf(background.time * 10) + 1.0f) * 0.5f;
        Color scanColor = ColorAlpha(background.phosphorColor, 0.2f * scanBrightness);
        DrawRectangle(0, background.scanlinePos - 2, width, 4, scanColor);

        // Horizontal scanlines
        for (int y = 0; y < height; y += 2)
        {
            float lineIntensity = background.scanlineIntensity *
                (0.8f + 0.2f * sinf(y * 0.1f + background.time * 2));

            Color lineColor = ColorAlpha(background.phosphorColor, lineIntensity);
            DrawLine(0, y, width, y, lineColor);
        }

        // Screen flicker
        float flicker = 1.0f + sinf(background.time * 60) * background.flickerIntensity;
        Color flickerColor = ColorAlpha(background.phosphorColor, 0.03f * flicker);
        DrawRectangle(0, 0, width, height, flickerColor);
    }
    EndTextureMode();

    // After our effects, we now draw the distorted texture using math that makes me want to die
    Vector2 center = {width/2.0f, height/2.0f};

    /* Thanks to some guides, we're opting to use texture coordinates (UV coordinates)
     * These are always in the range 0.0 to 1.0
     * where (0,0) is top-left and (1,1) is bottom-right of the texture
     *
     * So, for a texture of size 100x100:
     * Position (0,0)    → UV (0.0, 0.0)
     * Position (50,50)  → UV (0.5, 0.5)
     * Position (100,100)→ UV (1.0, 1.0)
     *
     * Our formula here is: UV = position / texture_size */

    // Here, we're drawing a distorted texture using mesh. It's a grid of 4x4 pixel quads
    for(int y = 0; y < height; y += 4)
    {
        for(int x = 0; x < width; x += 4)
        {
            // First, we calculate the distorted positions for each corner of our 4x4 quad
            Vector2 p1 = DistortPoint((Vector2){x, y}, center,
                                    background.screenCurvature, width, height);
            Vector2 p2 = DistortPoint((Vector2){x + 4, y}, center,
                                    background.screenCurvature, width, height);
            Vector2 p3 = DistortPoint((Vector2){x, y + 4}, center,
                                    background.screenCurvature, width, height);
            Vector2 p4 = DistortPoint((Vector2){x + 4, y + 4}, center,
                                    background.screenCurvature, width, height);

            /* UV coordinates must be normalized (0 to 1)
             * u = x / texture_width
             * v = y / texture_height
             * So, for example, if x = 100 and width = 800:
             * u = 100/800 = 0.125 (12.5% across the texture) */

            // Top-left UV coordinate
            Vector2 t1 = {
                (float)x / width,        // u1 = x position / texture width
                (float)y / height        // v1 = y position / texture height
            };

            // Top-right UV coordinate
            Vector2 t2 = {
                (float)(x + 4) / width,  // u2 = (x + quad_size) / texture width
                (float)y / height        // v2 = y position / texture height
            };

            // Bottom-left UV coordinate
            Vector2 t3 = {
                (float)x / width,        // u3 = x position / texture width
                (float)(y + 4) / height  // v3 = (y + quad_size) / texture height
            };

            // Bottom-right UV coordinate
            Vector2 t4 = {
                (float)(x + 4) / width,  // u4 = (x + quad_size) / texture width
                (float)(y + 4) / height  // v4 = (y + quad_size) / texture height
            };

            /* Notes:
             * Here we're using Raylibs DrawTexturePro function. Parameters:
             * source: where to sample from original texture (x, y, width, height)
             * dest: where to draw on screen (x, y, width, height)
             * origin: rotation origin point (0,0 for no rotation)
             * rotation: rotation in degrees (0 for no rotation)
             * tint: color to tint the texture (WHITE for no tint) */

            DrawTexturePro(background.renderTexture.texture,
                (Rectangle){x, y, 4, 4},  // Source rectangle
                (Rectangle){p1.x, p1.y,               // Destination position
                          p2.x - p1.x,                // Distorted width
                          p3.y - p1.y},         // Distorted height
                (Vector2){0, 0},                // Origin (no rotation)
                0,                                    // Rotation angle
                WHITE);                               // No tint
        }
    }

    // A simple Vignette effect!
    float vignetteSize = (float)width * 0.7f;
    DrawCircleGradient(width/2, height/2,
                      vignetteSize,
                      (Color){0, 0, 0, 0},
                      (Color){0, 0, 0, 128 * background.vignetteIntensity});

    // A Phosphor persistence effect, aka ghosting!
    float persistence = (sinf(background.time * 3) + 1.0f) * 0.5f;
    Color persistColor = ColorAlpha(background.phosphorColor, 0.02f * persistence);
    DrawRectangle(0, 0, width, height, persistColor);
}

// Unloading cause otherwise bad (Free memory)
void UnloadBackground(Background* background)
{
    UnloadRenderTexture(background->renderTexture);
}