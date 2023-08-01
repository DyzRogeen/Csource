#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "Settings.h"

/// @file Vector.h
/// @defgroup Vector
/// @{

typedef union Vec2_u {
    struct {
        float x;
        float y;
    };
    float data[2];
} Vec2;

/// @brief Raccourci pour [1,0].
/// Vecteur unitaire dirigé vers la droite.
extern const Vec2 Vec2_Right;

/// @brief Raccourci pour [-1,0].
/// Vecteur unitaire dirigé vers la gauche.
extern const Vec2 Vec2_Left;

/// @brief Raccourci pour [0,1].
/// Vecteur unitaire dirigé vers le haut.
extern const Vec2 Vec2_Up;

/// @brief Raccourci pour [0,-1].
/// Vecteur unitaire dirigé vers le bas.
extern const Vec2 Vec2_Down;

/// @brief Raccourci pour [0,0].
/// Vecteur nul.
extern const Vec2 Vec2_Zero;

/// @brief Raccourci pour [1,1].
/// Vecteur dont toutes les composantes valent 1.0f.
extern const Vec2 Vec2_One;

/// @brief Construit un vecteur à partir de ses composantes.
/// @param x la composante x.
/// @param y la composante y.
/// @return Le vecteur ayant les composantes données.
INLINE Vec2 Vec2_Set(float x, float y)
{
    Vec2 res = { x, y };
    return res;
}

/// @brief Calcule le minimum composante par composante entre deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le vecteur dont la i-ème composante est le minimum des i-èmes composantes de v1 et v2.
INLINE Vec2 Vec2_Min(Vec2 v1, Vec2 v2)
{
    v1.x = fminf(v1.x, v2.x);
    v1.y = fminf(v1.y, v2.y);
    return v1;
}

/// @brief Calcule le maximum composante par composante entre deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le vecteur dont la i-ème composante est le maximum des i-èmes composantes de v1 et v2.
INLINE Vec2 Vec2_Max(Vec2 v1, Vec2 v2)
{
    v1.x = fmaxf(v1.x, v2.x);
    v1.y = fmaxf(v1.y, v2.y);
    return v1;
}

/// @brief Aditionne deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return La somme de v1 et v2.
Vec2 Vec2_Add(Vec2 v1, Vec2 v2);

/// @brief Soustrait deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return La différence de v1 par v2.
Vec2 Vec2_Sub(Vec2 v1, Vec2 v2);

/// @brief Renvoie l'aire algébrique du parallélogramme définit par AB et AC.
/// Il s'agit également du double de l'aire algébrique du triangle ABC.
/// @param[in] a le sommet A.
/// @param[in] b le sommet B.
/// @param[in] c le sommet C.
/// @return L'aire algébrique du parallélogramme définit par AB et AC.
float Vec2_SignedArea(const Vec2 a, const Vec2 b, const Vec2 c);

/// @brief Calcule les coordonées barycentrique d'un point dans un triangle
/// @param[in] vertices tableau contenant les trois sommets du triangle.
/// @param[in] p coordonnées du point.
/// @param[out] bary tableau dans lequel écrire les trois coordonnées barycentrique du point p.
/// @return true si le point p appartient au triangle, false sinon.
bool Vec2_Barycentric(Vec2 *vertices, Vec2 p, float *bary);

//-------------------------------------------------------------------------------------------------
// Structure Vec3

/// @brief Structure représentant un vecteur à 3 dimensions (coordonées cartésiennes).
/// Il est possible d'accéder aux données via le tableau data ou via les champs x, y et z.
typedef union Vec3_u {
    struct {
        float x;
        float y;
        float z;
    };
    float data[3];
} Vec3;

/// @brief Vecteur unitaire dirigé vers la droite.
extern const Vec3 Vec3_Right;

/// @brief Vecteur unitaire dirigé vers la gauche.
extern const Vec3 Vec3_Left;

/// @brief Vecteur unitaire dirigé vers le haut.
extern const Vec3 Vec3_Up;

/// @brief Vecteur unitaire dirigé vers le bas.
extern const Vec3 Vec3_Down;

/// @brief Vecteur unitaire dirigé vers l'avant.
extern const Vec3 Vec3_Front;

/// @brief Vecteur unitaire dirigé vers l'arrière.
extern const Vec3 Vec3_Back;

/// @brief Vecteur nul.
extern const Vec3 Vec3_Zero;

/// @brief Vecteur dont toutes les composantes valent 1.0f.
extern const Vec3 Vec3_One;

/// @brief Construit un vecteur à partir de ses composantes.
/// @param x la composante x.
/// @param y la composante y.
/// @param z la composante z.
/// @return Le vecteur ayant les composantes données.
INLINE Vec3 Vec3_Set(float x, float y, float z)
{
    Vec3 res = { x, y, z};
    return res;
}

/// @brief Aditionne deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return La somme de v1 et v2.
Vec3 Vec3_Add(Vec3 v1, Vec3 v2);

/// @brief Soustrait deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return La différence de v1 par v2.
Vec3 Vec3_Sub(Vec3 v1, Vec3 v2);

/// @brief Multiplie un vecteur par un scalaire.
/// @param v le vecteur.
/// @param s le scalaire.
/// @return Le produit de s et v.
Vec3 Vec3_Scale(Vec3 v, float s);

/// @brief Renvoie l'opposé d'un vecteur.
/// @param v le vecteur.
/// @return L'opposé du vecteur v.
INLINE Vec3 Vec3_Neg(Vec3 v)
{
    return Vec3_Scale(v, -1.0f);
}

/// @brief Calcule le produit scalaire usuel de deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le produit scalaire de v1 et v2.
float Vec3_Dot(Vec3 v1, Vec3 v2);

/// @brief Calcule le produit vectoriel de deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le produit vectoriel de v1 et v2.
Vec3 Vec3_Cross(Vec3 v1, Vec3 v2);

/// @brief Calcule la norme d'un vecteur.
/// @param v le vecteur.
/// @return La norme euclidienne de v.
float Vec3_Length(Vec3 v);

/// @brief Normalise un vecteur.
/// @param v le vecteur.
/// @return Le vecteur unitaire de même direction.
Vec3 Vec3_Normalize(Vec3 v);

/// @brief Multiplie deux vecteurs composante par composante.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le produit composante par composante de v1 et v2.
Vec3 Vec3_Mul(Vec3 v1, Vec3 v2);

/// @brief Calcule le minimum composante par composante entre deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le vecteur dont la i-ème composante est le minimum des i-èmes composantes de v1 et v2.
Vec3 Vec3_Min(Vec3 v1, Vec3 v2);

/// @brief Calcule le maximum composante par composante entre deux vecteurs.
/// @param v1 le premier vecteur.
/// @param v2 le second vecteur.
/// @return Le vecteur dont la i-ème composante est le maximum des i-èmes composantes de v1 et v2.
Vec3 Vec3_Max(Vec3 v1, Vec3 v2);


//-------------------------------------------------------------------------------------------------
// Structure Vec4

/// @brief Structure représentant un vecteur à 4 dimensions (coordonées homogènes).
/// Il est possible d'accéder aux données via le tableau data ou via les champs x, y, z et w.
typedef union Vec4_u {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    float data[4];
} Vec4;

/// @brief Vecteur unitaire dirigé vers la droite en coordonnées homogènes.
extern const Vec4 Vec4_RightH;

/// @brief Vecteur unitaire dirigé vers la gauche en coordonnées homogènes.
extern const Vec4 Vec4_LeftH;

/// @brief Vecteur unitaire dirigé vers le haut en coordonnées homogènes.
extern const Vec4 Vec4_UpH;

/// @brief Vecteur unitaire dirigé vers le bas en coordonnées homogènes.
extern const Vec4 Vec4_DownH;

/// @brief Vecteur unitaire dirigé vers l'avant en coordonnées homogènes.
extern const Vec4 Vec4_FrontH;

/// @brief Vecteur unitaire dirigé vers l'arrière en coordonnées homogènes.
extern const Vec4 Vec4_BackH;

/// @brief Vecteur nul en coordonnées homogènes.
extern const Vec4 Vec4_ZeroH;

/// @brief Vecteur nul.
extern const Vec4 Vec4_Zero;

/// @brief Vecteur dont toutes les composantes valent 1.0f.
extern const Vec4 Vec4_One;

/// @brief Construit un vecteur à partir de ses composantes.
/// @param x la composante x.
/// @param y la composante y.
/// @param z la composante z.
/// @param w la composante w.
/// @return Le vecteur ayant les composantes données.
INLINE Vec4 Vec4_Set(float x, float y, float z, float w)
{
    Vec4 res = { x, y, z, w };
    return res;
}

//-------------------------------------------------------------------------------------------------
// Fonctions de conversion

/// @brief Construit un vecteur à 3 dimensions dont toutes les composantes sont identiques.
/// @param value la valeur des composantes.
/// @return Le vecteur à 3 dimensions associé.
INLINE Vec3 Vec3_FromFloat(float value)
{
    Vec3 res = { value, value, value };
    return res;
}

/// @brief Construit un vecteur à 3 dimensions à partir d'un vecteur à 2 dimensions.
/// @param v un vecteur à 2 dimension.
/// @param z la composante z.
/// @return Le vecteur à 3 dimensions associé.
INLINE Vec3 Vec3_From2(Vec2 v, float z)
{
    Vec3 res = { v.x, v.y, z };
    return res;
}

/// @brief Transforme un vecteur des coordonnées homogènes vers cartésiennes.
/// Si le vecteur désigne un point [x, y, z, w] (avec w non nul),
/// cette fonction renvoie (x/w, y/w, z/w).
/// Si le vecteur désigne une direction [x, y, z, 0], cette fonction renvoie (x, y, z).
/// @param vector le vecteur.
/// @return Le vecteur en coordonnées cartésiennes.
Vec3 Vec3_From4(Vec4 vector);

/// @brief Construit un vecteur à 4 dimensions dont toutes les composantes sont identiques.
/// @param value la valeur des composantes.
/// @return Le vecteur à 4 dimensions associé.
INLINE Vec4 Vec4_FromFloat(float value)
{
    Vec4 res = { value, value, value, value };
    return res;
}

/// @brief Construit un vecteur à 4 dimensions à partir d'un vecteur à 2 dimensions.
/// @param v un vecteur à 2 dimension.
/// @param z la composante z.
/// @param w la composante w.
/// @return Le vecteur à 4 dimensions associé.
INLINE Vec4 Vec4_From2(Vec2 v, float z, float w)
{
    Vec4 res = { v.x, v.y, z, w };
    return res;
}

/// @brief Construit un vecteur à 4 dimensions à partir d'un vecteur à 3 dimensions.
/// @param v un vecteur à 3 dimension.
/// @param w la composante w.
/// @return Le vecteur à 4 dimensions associé.
INLINE Vec4 Vec4_From3(Vec3 v, float w)
{
    Vec4 res = { v.x, v.y, v.z, w };
    return res;
}

/// @}

#endif