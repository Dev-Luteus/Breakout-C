#ifndef VECTORMATH_H
#define VECTORMATH_H

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// Basic
Vector2 MyVector2Create(float x, float y);
Vector2 MyVector2Add(Vector2 v1, Vector2 v2);
Vector2 MyVector2Subtract(Vector2 v1, Vector2 v2);
Vector2 MyVector2Scale(Vector2 v, float scalar);
Vector2 MyVector2Normalize(Vector2 v);

// Utility methods
float MyVector2Length(Vector2 v);
float MyVector2DotProduct(Vector2 v1, Vector2 v2);
float MyVector2Angle(Vector2 v);

#endif