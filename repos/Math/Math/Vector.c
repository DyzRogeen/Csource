#include "Vector.h"

const Vec2 Vec2_Right = { +1.0f, +0.0f };
const Vec2 Vec2_Left  = { -1.0f, +0.0f };
const Vec2 Vec2_Up    = { +0.0f, +1.0f };
const Vec2 Vec2_Down  = { +0.0f, -1.0f };
const Vec2 Vec2_Zero  = { +0.0f, +0.0f };
const Vec2 Vec2_One   = { +1.0f, +1.0f };

const Vec3 Vec3_Right = { +1.0f, +0.0f, +0.0f };
const Vec3 Vec3_Left  = { -1.0f, +0.0f, +0.0f };
const Vec3 Vec3_Up    = { +0.0f, +1.0f, +0.0f };
const Vec3 Vec3_Down  = { +0.0f, -1.0f, +0.0f };
const Vec3 Vec3_Front = { +0.0f, +0.0f, +1.0f };
const Vec3 Vec3_Back  = { +0.0f, +0.0f, -1.0f };
const Vec3 Vec3_Zero  = { +0.0f, +0.0f, +0.0f };
const Vec3 Vec3_One   = { +1.0f, +1.0f, +1.0f };

const Vec4 Vec4_RightH = { +1.0f, +0.0f, +0.0f, +1.0f };
const Vec4 Vec4_LeftH  = { -1.0f, +0.0f, +0.0f, +1.0f };
const Vec4 Vec4_UpH    = { +0.0f, +1.0f, +0.0f, +1.0f };
const Vec4 Vec4_DownH  = { +0.0f, -1.0f, +0.0f, +1.0f };
const Vec4 Vec4_FrontH = { +0.0f, +0.0f, +1.0f, +1.0f };
const Vec4 Vec4_BackH  = { +0.0f, +0.0f, -1.0f, +1.0f };
const Vec4 Vec4_ZeroH  = { +0.0f, +0.0f, +0.0f, +1.0f };
const Vec4 Vec4_Zero   = { +0.0f, +0.0f, +0.0f, +0.0f };
const Vec4 Vec4_One    = { +1.0f, +1.0f, +1.0f, +1.0f };

Vec2 Vec2_Add(Vec2 v1, Vec2 v2)
{
    Vec2 v;
    v.x = v1.x + v2.x;
    v.y = v1.y + v2.y;
    return v1;
}

Vec2 Vec2_Sub(Vec2 v1, Vec2 v2)
{
    Vec2 v;
    v.x = v1.x - v2.x;
    v.y = v1.y - v2.y;
    return v1;
}

float Vec2_SignedArea(const Vec2 a, const Vec2 b, const Vec2 c)
{
    return 0.0f;
}

bool Vec2_Barycentric(Vec2 *vertices, Vec2 p, float *bary)
{
    return true;
}

Vec3 Vec3_Add(Vec3 v1, Vec3 v2)
{
    Vec3 v;
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

Vec3 Vec3_Sub(Vec3 v1, Vec3 v2)
{
    Vec3 v;
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

Vec3 Vec3_Scale(Vec3 v, float s)
{
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

float Vec3_Dot(Vec3 v1, Vec3 v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 Vec3_Cross(Vec3 v1, Vec3 v2)
{
    Vec3 v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

float Vec3_Length(Vec3 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 Vec3_Normalize(Vec3 v)
{
    float n = Vec3_Length(v);
    v.x /= n;
    v.y /= n;
    v.z /= n;
    return v;
}

Vec3 Vec3_Mul(Vec3 v1, Vec3 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}

Vec3 Vec3_Max(Vec3 v1, Vec3 v2)
{
    v1.x = fmaxf(v1.x, v2.x);
    v1.y = fmaxf(v1.y, v2.y);
    v1.z = fmaxf(v1.z, v2.z);
    return v1;
}

Vec3 Vec3_Min(Vec3 v1, Vec3 v2)
{
    v1.x = fminf(v1.x, v2.x);
    v1.y = fminf(v1.y, v2.y);
    v1.z = fminf(v1.z, v2.z);
    return v1;
}

Vec3 Vec3_From4(Vec4 v)
{
    Vec3 u;
    if (v.w == 0) {
        u.x = v.x;
        u.y = v.y;
        u.z = v.z;
        return u;
    }
    u.x = v.x / v.w;
    u.y = v.y / v.w;
    u.z = v.z / v.w;
    return u;
}
