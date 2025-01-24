#ifndef VECTORMATH_H
#define VECTORMATH_H

#include <raylib.h>

// Having my own Vector2 (same as raylib) causes a conflict with raylib's Vector2.
//typedef struct Vector2 {
//    float x;
//    float y;
//} Vector2;

// Basic Functions
Vector2 MyVector2Create(float x, float y);
Vector2 MyVector2Add(Vector2 v1, Vector2 v2);
Vector2 MyVector2Subtract(Vector2 v1, Vector2 v2);
Vector2 MyVector2Scale(Vector2 v, float scalar);
Vector2 MyVector2Normalize(Vector2 v);
Vector2 MyVector2Zero(void);
Vector2 MyVector2One(void);

// Measure / Calculate Functions
float MyVector2Length(Vector2 v);
float MyVector2LengthSquared(Vector2 v);
float MyVector2Distance(Vector2 v1, Vector2 v2);
float MyVector2DistanceSquared(Vector2 v1, Vector2 v2);
float MyVector2DotProduct(Vector2 v1, Vector2 v2);
float MyVector2Angle(Vector2 v);

// Extra Useful Functions
Vector2 MyVector2Reflect(Vector2 v, Vector2 normal);
Vector2 MyVector2Rotate(Vector2 v, float angle);
Vector2 MyVector2Lerp(Vector2 v1, Vector2 v2, float amount);
Vector2 MyVector2ClampValue(Vector2 v, float min, float max);

#endif