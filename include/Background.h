#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>

/* Math formula notes to remember:
 * With QUAD_SIZE = 32:
 * Horizontal quads = ceil(1920/32) = ceil(60) = 60
 * Vertical quads = ceil(1080/32) = ceil(33.75) = 34
 * MAX_QUADS = 60 * 34 = 2,040 quads

 * Memory required = 2,040 * sizeof(DistortedQuad)
                = 2,040 * 16 bytes
                = 32,640 bytes (only ~32KB!) */

#define QUAD_SIZE 32
#define MAX_QUADS (((1920 + QUAD_SIZE - 1)/QUAD_SIZE) * ((1080 + QUAD_SIZE - 1)/QUAD_SIZE))

// CRT Quad effect!
typedef struct {
    Vector2 position;
    float width;
    float height;
} DistortedQuad;

// Main background structure
typedef struct Background
{
    float time;
    float scanlinePos;

    Color phosphorColor;
    float screenCurvature;
    float flickerIntensity;
    float vignetteIntensity;
    float scanlineIntensity;

    RenderTexture2D effectTexture;
    RenderTexture2D finalTexture;

    DistortedQuad* quadCache;
    bool distortionNeedsUpdate;
    float lastCurvature;

    RenderTexture2D staticEffects;    // Static effects
    RenderTexture2D uiTexture;
    bool staticEffectsNeedUpdate;
} Background;

Background InitBackground(int width, int height);
void UpdateBackground(Background* background, float deltaTime);
void UpdateStaticEffects(Background* background, int width, int height);
void DrawBackground(Background* background, int width, int height, Texture2D sourceTexture);
void UnloadBackground(Background* background);

#endif //BACKGROUND_H