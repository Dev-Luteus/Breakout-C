#include "VectorMath.h"
#include <math.h>

Vector2 MyVector2Create(float x, float y)
{
    Vector2 result = { x, y };
    return result;
}

Vector2 MyVector2Add(Vector2 v1, Vector2 v2)
{
    return MyVector2Create(v1.x + v2.x, v1.y + v2.y);
}

Vector2 MyVector2Subtract(Vector2 v1, Vector2 v2)
{
    return MyVector2Create(v1.x - v2.x, v1.y - v2.y);
}

Vector2 MyVector2Scale(Vector2 v, float scalar)
{
    return MyVector2Create(v.x * scalar, v.y * scalar);
}

float MyVector2Length(Vector2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vector2 MyVector2Normalize(Vector2 v)
{
    float length = MyVector2Length(v);

    if (length > 0)
    {
        return MyVector2Create(v.x / length, v.y / length);
    }

    return v;
}

float MyVector2DotProduct(Vector2 v1, Vector2 v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float MyVector2Angle(Vector2 v)
{
    return atan2f(v.y, v.x);
}