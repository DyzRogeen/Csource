#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "Settings.h"
#include "Vector.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 // pi
#endif

#define FLOAT_EPSILON 1e-5f;

INLINE int Int_Max(int a, int b)
{
    return (a > b) ? a : b;
}

INLINE int Int_Min(int a, int b)
{
    return (a < b) ? a : b;
}

INLINE int Int_Clamp(int value, int a, int b)
{
    return Int_Max(a, Int_Min(value, b));
}

INLINE float Float_Clamp(float value, float a, float b)
{
    return fmaxf(a, fminf(value, b));
}

INLINE float Float_Clamp01(float value)
{
    return Float_Clamp(value, 0.0f, 1.0f);
}

INLINE float Float_Lerp(float a, float b, float t)
{
    return (1.0f - t) * a + t * b;
}

INLINE float Float_LerpClamped(float a, float b, float t)
{
    t = Float_Clamp01(t);
    return (1.0f - t) * a + t * b;
}

float Float_SmoothDamp(
    float current, float target, float *currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime);

INLINE bool Float_EpsEq(float value1, float value2)
{
    return fabsf(value1 - value2) < FLOAT_EPSILON;
}

INLINE float Float_Sign(float value)
{
    return value < 0.0f ? -1.0f : 1.0f;
}

INLINE float Float_Frac(float value)
{
    return value - floorf(value);
}

Vec3 Vec3_Clamp01(Vec3 v);
Vec3 Vec3_Frac(Vec3 v);
Vec3 Vec3_Abs(Vec3 v);

#endif