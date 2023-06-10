#include "Tools.h"

float Float_SmoothDamp(
    float current, float target, float *currentVelocity,
    float smoothTime, float maxSpeed, float deltaTime)
{
    // Implémentation de Unity
    // Voir Game Programming Gems 4 chapitre 1.10 page 99
    smoothTime = fmaxf(0.0001f, smoothTime);
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float targetCopy = target;
    float maxChange = maxSpeed * smoothTime;
    change = Float_Clamp(change, -maxChange, maxChange);
    target = current - change;
    float temp = (*currentVelocity + omega * change) * deltaTime;
    *currentVelocity = (*currentVelocity - omega * temp) * exp;
    float res = target + (change + temp) * exp;
    if (targetCopy - current > 0.0f == res > targetCopy)
    {
        res = targetCopy;
        *currentVelocity = (res - targetCopy) / deltaTime;
    }
    return res;
}

Vec3 Vec3_Clamp01(Vec3 v)
{
    v.x = Float_Clamp01(v.x);
    v.y = Float_Clamp01(v.y);
    v.z = Float_Clamp01(v.z);
    return v;
}

Vec3 Vec3_Frac(Vec3 v)
{
    v.x = Float_Frac(v.x);
    v.y = Float_Frac(v.y);
    v.z = Float_Frac(v.z);
    return v;
}

Vec3 Vec3_Abs(Vec3 v)
{
    v.x = fabsf(v.x);
    v.y = fabsf(v.y);
    v.z = fabsf(v.z);
    return v;
}
