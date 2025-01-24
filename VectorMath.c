#include "VectorMath.h"
#include <math.h>

/*
 * Creates a 2D vector from x and y components
 * v = (x, y)
 */
Vector2 MyVector2Create(float x, float y)
{
    Vector2 result = { x, y };
    return result;
}

/*
 * Creates a zero vector
 * v = (0, 0)
 */
Vector2 MyVector2Zero(void)
{
    return MyVector2Create(0.0f, 0.0f);
}

/*
 * Creates a unit vector
 * v = (1, 1)
 */
Vector2 MyVector2One(void)
{
    return MyVector2Create(1.0f, 1.0f);
}

/*
 * Vector addition: v3 = v1 + v2
 * v3.x = v1.x + v2.x
 * v3.y = v1.y + v2.y
 */
Vector2 MyVector2Add(Vector2 v1, Vector2 v2)
{
    return MyVector2Create(v1.x + v2.x, v1.y + v2.y);
}

/*
 * Vector subtraction: v3 = v1 - v2
 * v3.x = v1.x - v2.x
 * v3.y = v1.y - v2.y
 */
Vector2 MyVector2Subtract(Vector2 v1, Vector2 v2)
{
    return MyVector2Create(v1.x - v2.x, v1.y - v2.y);
}

/*
 * Vector scalar multiplication: v2 = v1 * scalar
 * v2.x = v1.x * scalar
 * v2.y = v1.y * scalar
 */
Vector2 MyVector2Scale(Vector2 v, float scalar)
{
    return MyVector2Create(v.x * scalar, v.y * scalar);
}

/*
 * Vector length (magnitude): |v| = √(x² + y²)
 * Uses Pythagorean theorem
 */
float MyVector2Length(Vector2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

/*
 * Vector length squared: |v|² = x² + y²
 * Faster than Length() as it avoids square root
 */
float MyVector2LengthSquared(Vector2 v)
{
    return (v.x * v.x + v.y * v.y);
}

/*
 * Vector normalization: û = v/|v|
 * û.x = v.x/|v|
 * û.y = v.y/|v|
 * Creates a unit vector (length = 1) in the same direction
 */
Vector2 MyVector2Normalize(Vector2 v)
{
    float length = MyVector2Length(v);

    if (length > 0)
    {
        float ilength = 1.0f / length;
        return MyVector2Create(v.x * ilength, v.y * ilength);
    }

    return v;
}

/*
 * Dot product: a·b = ax*bx + ay*by
 * Returns scalar product of two vectors
 * Also equals: |a|*|b|*cos(θ) where θ is angle between vectors
 */
float MyVector2DotProduct(Vector2 v1, Vector2 v2)
{
    return (v1.x * v2.x + v1.y * v2.y);
}

/*
 * Distance between two vectors: d = |v2 - v1|
 * d = √((v2.x - v1.x)² + (v2.y - v1.y)²)
 */
float MyVector2Distance(Vector2 v1, Vector2 v2)
{
    return MyVector2Length(MyVector2Subtract(v1, v2));
}

/*
 * Square of distance between vectors
 * d² = (v2.x - v1.x)² + (v2.y - v1.y)²
 * Faster than Distance() as it avoids square root
 */
float MyVector2DistanceSquared(Vector2 v1, Vector2 v2)
{
    Vector2 diff = MyVector2Subtract(v1, v2);
    return MyVector2DotProduct(diff, diff);
}

/*
 * Angle of vector from positive x-axis
 * θ = atan2(y, x)
 * Returns angle in radians in range (-π, π]
 */
float MyVector2Angle(Vector2 v)
{
    return atan2f(v.y, v.x);
}

/*
 * Vector reflection: r = v - 2(v·n)n
 * Where v is incident vector and n is normalized surface normal
 * Returns vector reflected off a surface with given normal
 */
Vector2 MyVector2Reflect(Vector2 v, Vector2 normal)
{
    float dotProduct = MyVector2DotProduct(v, normal);
    Vector2 scaled = MyVector2Scale(normal, 2.0f * dotProduct);

    return MyVector2Subtract(v, scaled);
}

/*
 * Vector rotation by angle θ:
 * x' = x*cos(θ) - y*sin(θ)
 * y' = x*sin(θ) + y*cos(θ)
 * Rotates vector counterclockwise by angle (in radians)
 */
Vector2 MyVector2Rotate(Vector2 v, float angle)
{
    float cosAngle = cosf(angle);
    float sinAngle = sinf(angle);

    return MyVector2Create(
        v.x * cosAngle - v.y * sinAngle,
        v.x * sinAngle + v.y * cosAngle
    );
}

/*
 * Linear interpolation between vectors:
 * v = v1 + (v2 - v1) * t
 * Where t is in range [0,1]
 * t=0 returns v1, t=1 returns v2, t=0.5 returns midpoint
 */
Vector2 MyVector2Lerp(Vector2 v1, Vector2 v2, float amount)
{
    return MyVector2Create(
        v1.x + (v2.x - v1.x) * amount,
        v1.y + (v2.y - v1.y) * amount
    );
}

/*
 * Clamps vector length between min and max values
 * If |v| < min: v = û * min
 * If |v| > max: v = û * max
 * Where û is normalized v
 */
Vector2 MyVector2ClampValue(Vector2 v, float min, float max)
{
    Vector2 result = v;

    float length = MyVector2Length(v);

    if (length < min)
    {
        result = MyVector2Scale(MyVector2Normalize(v), min);
    }
    else if (length > max)
    {
        result = MyVector2Scale(MyVector2Normalize(v), max);
    }

    return result;
}