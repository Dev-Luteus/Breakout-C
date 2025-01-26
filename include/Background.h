#ifndef BACKGROUND_H
#define BACKGROUND_H
#include <raylib.h>

typedef struct Background
{
    float time;
    float scanlinePos;
    Color phosphorColor;
    float screenCurvature;
    float flickerIntensity;
    float vignetteIntensity;
    float scanlineIntensity;

    RenderTexture2D renderTexture;
    RenderTexture2D gameTexture;
    RenderTexture2D finalTexture;
} Background;

// This was a whole of a lot more complicated than I anticipated. We're making layers =)
Background InitBackground(int width, int height);
void UpdateBackground(Background* background, float deltaTime);
void DrawBackgroundEffects(Background* background, int width, int height);
void ComposeDistortedImage(Background* background, int width, int height);
void DrawBackground(Background* background, int width, int height);
void UnloadBackground(Background* background);

#endif //BACKGROUND_H
