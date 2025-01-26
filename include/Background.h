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
} Background;

Background InitBackground(int width, int height);
void UpdateBackground(Background* background, float deltaTime);
void DrawBackground(Background background, int width, int height);

#endif //BACKGROUND_H
