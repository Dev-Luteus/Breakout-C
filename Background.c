#include "Background.h"
#include <math.h>

Background InitBackground(int width, int height)
{
    Background background =
    {
        .time = 0,
        .scanlinePos = 0,
        .phosphorColor = (Color){0, 255, 0, 128},
        .screenCurvature = 0.03f,
        .flickerIntensity = 0.03f,
        .vignetteIntensity = 0.5f,
        .scanlineIntensity = 0.1f,

        .effectTexture = LoadRenderTexture(width, height),
        .finalTexture = LoadRenderTexture(width, height),

        .quadCache = (DistortedQuad*)MemAlloc(MAX_QUADS * sizeof(DistortedQuad)),
        .distortionNeedsUpdate = true,
        .lastCurvature = 0.03f // We use this to cache our screen curvature values!
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

void DrawBackground(Background* background, int width, int height, Texture2D gameScreen)
{
    // This is a RenderTexture method, we want to first render all effects to our the render texture
    BeginTextureMode(background->effectTexture);
    {
        ClearBackground(BLANK);

        // Draw base phosphor layer!
        DrawRectangle(0, 0, width, height,
                     ColorAlpha(background->phosphorColor, 0.05f));

        // Our loop for drawing a grain like effect using DrawPixel!
        for (int i = 0; i < width * height / 100; i++)
        {
            int x = GetRandomValue(0, width - 1);
            int y = GetRandomValue(0, height - 1);
            float noiseIntensity = (float)GetRandomValue(0, 128) / 500.0f;
            DrawPixel(x, y, ColorAlpha(background->phosphorColor, noiseIntensity));
        }

        // A Scanning line effect!
        float scanBrightness = (sinf(background->time * 10) + 1.0f) * 0.5f;
        Color scanColor = ColorAlpha(background->phosphorColor, 0.2f * scanBrightness);
        DrawRectangle(0, background->scanlinePos - 2, width, 4, scanColor);

        // Horizontal scanlines
        for (int y = 0; y < height; y += 2)
        {
            float lineIntensity = background->scanlineIntensity *
                (0.8f + 0.2f * sinf(y * 0.1f + background->time * 2));
            Color lineColor = ColorAlpha(background->phosphorColor, lineIntensity);
            DrawLine(0, y, width, y, lineColor);
        }

        // Screen flicker
        float flicker = 1.0f + sinf(background->time * 60) * background->flickerIntensity;
        Color flickerColor = ColorAlpha(background->phosphorColor, 0.03f * flicker);
        DrawRectangle(0, 0, width, height, flickerColor);
    }
    EndTextureMode();

    // We now compose and apply the barrel distortion to the final image
    BeginTextureMode(background->finalTexture);
    {
        ClearBackground(BLACK);
        Vector2 center = {width/2.0f, height/2.0f};

        // In order to not flood our memory, here I'm trying to only update distortion if needed
        if (background->screenCurvature != background->lastCurvature)
        {
            background->distortionNeedsUpdate = true;
            background->lastCurvature = background->screenCurvature;
        }

        // Pre-calculate distortion if needed
        if (background->distortionNeedsUpdate)
        {
            int quadIndex = 0;

            for(int y = 0; y < height; y += QUAD_SIZE)
            {
                for(int x = 0; x < width; x += QUAD_SIZE)
                {
                    Vector2 p1 = DistortPoint((Vector2){x, y}, center,
                                          background->screenCurvature, width, height);
                    Vector2 p2 = DistortPoint((Vector2){x + QUAD_SIZE, y}, center,
                                          background->screenCurvature, width, height);
                    Vector2 p3 = DistortPoint((Vector2){x, y + QUAD_SIZE}, center,
                                          background->screenCurvature, width, height);

                    background->quadCache[quadIndex].position = p1;
                    background->quadCache[quadIndex].width = p2.x - p1.x;
                    background->quadCache[quadIndex].height = p3.y - p1.y;
                    quadIndex++;
                }
            }
            background->distortionNeedsUpdate = false;
        }

        // Draw using cached distortion
        int quadIndex = 0;
        for(int y = 0; y < height; y += QUAD_SIZE)
        {
            for(int x = 0; x < width; x += QUAD_SIZE)
            {
                DistortedQuad* quad = &background->quadCache[quadIndex++];

                // Skip if quad is outside screen
                if (quad->position.x > width || (quad->position.x + quad->width) < 0 ||
                    quad->position.y > height || (quad->position.y + quad->height) < 0)
                    continue;

                // Draw the game screen with distortion
                DrawTexturePro(gameScreen,
                    (Rectangle){x, y, QUAD_SIZE, QUAD_SIZE},
                    (Rectangle){quad->position.x, quad->position.y,
                              quad->width, quad->height},
                    (Vector2){0, 0}, 0, WHITE);
            }
        }

        // Overlay the CRT effects
        DrawTexture(background->effectTexture.texture, 0, 0, WHITE);

        // A simple Vignette effect!
        float vignetteSize = (float)width * 0.7f;
        DrawCircleGradient(width/2, height/2,
                          vignetteSize,
                          (Color){0, 0, 0, 0},
                          (Color){0, 0, 0, 128 * background->vignetteIntensity});

        // A Phosphor persistence effect, aka ghosting!
        float persistence = (sinf(background->time * 3) + 1.0f) * 0.5f;
        Color persistColor = ColorAlpha(background->phosphorColor, 0.02f * persistence);
        DrawRectangle(0, 0, width, height, persistColor);
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
}