#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <raylib.h>

#define QUAD_SIZE 12
#define MAX_QUADS ((1920/QUAD_SIZE) * (1080/QUAD_SIZE))

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
} Background;

Background InitBackground(int width, int height);
void UpdateBackground(Background* background, float deltaTime);
void DrawBackground(Background* background, int width, int height, Texture2D sourceTexture);
void UnloadBackground(Background* background);

#endif //BACKGROUND_H