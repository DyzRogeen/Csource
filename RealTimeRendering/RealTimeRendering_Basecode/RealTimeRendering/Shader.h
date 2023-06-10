#ifndef _SHADER_H_
#define _SHADER_H_

#include "Settings.h"
#include "Vector.h"
#include "Matrix.h"
#include "Mesh.h"
#include "Material.h"

typedef struct Scene_s Scene;

/// @brief Structure représentant les données globales fournies au vertex shader.
typedef struct VShaderGlobals_s
{
    Vec3 cameraPos;
    Mat4 objToWorld;
    Mat4 viewToWorld;
    Mat4 objToView;
    Mat4 objToClip;
} VShaderGlobals;

/// @brief Structure représentant les données d'un sommet fournies au vertex shader.
typedef struct VShaderIn_s
{
    /// @brief Position du sommet dans le référentiel objet.
    Vec3 vertex;

    /// @brief Position de la normale associée au sommet dans le référentiel objet.
    Vec3 normal;

    /// @brief Position de la tangente associée au sommet dans le référentiel objet.
    Vec3 tangent;

    /// @brief Coordonnées uv associées au sommet.
    Vec2 textUV;
} VShaderIn;

/// @brief Structure représentant la sortie du vertex shader
typedef struct VShaderOut_s
{
    /// @brief Position du sommet après la projection en perspective.
    /// Les coordonnées des points visibles sont entre -1.0f et 1.0f (sur chaque axe).
    Vec3  clipPos;

    /// @brief Inverse de la coordonnée z du sommet dans le référentiel caméra.
    float invDepth;

    /// @brief Normale associée au sommet exprimée dans le référentiel monde.
    Vec3  normal;

    /// @brief Coordonnées uv associées au sommet.
    Vec2  textUV;

    // TODO
    // Pour les bonus

    /// @brief Position du sommet dans le référentiel monde.
    Vec3  worldPos;

    /// @brief Tangente associée au sommet exprimée dans le référentiel monde.
    //Vec3  tangent;

} VShaderOut;

/// @brief Structure représentant les données globales fournies au fragment shader.
typedef struct FShaderGlobals_s
{
    /// @brief Pointeur vers la scène.
    Scene *scene;

    /// @brief Matériau utilisé pour le triangle.
    Material *material;

    /// @brief Position de la caméra.
    Vec3 cameraPos;
} FShaderGlobals;

/// @brief Structure représentant les données associée à un pixel (fragment)
/// fournies au fragment shader.
/// Elle sont obtenues par interpolation des sorties du vertex shader.
typedef struct FShaderIn_s
{
    /// @brief Normale associée au pixel exprimée dans le référentiel monde.
    /// Le vecteur est obtenu par interpolation, donc n'est pas nécessairement unitaire.
    Vec3 normal;

    /// @brief Coordonnées uv associées au pixel.
    Vec2 textUV;

    // TODO
    // Pour les bonus

    /// @brief Position associée au pixel exprimée dans le référentiel monde.
    /// Nécessaire pour la lumière spéculaire de Blinn-Phong
    Vec3 worldPos;

    /// @brief Tangente associée au pixel exprimée dans le référentiel monde.
    /// Nécessaire pour l'utilisation d'une "normal map".
    /// Le vecteur est obtenu par interpolation, donc n'est pas nécessairement unitaire
    /// ou perpendiculaire à la normale.
    Vec3 tangent;
} FShaderIn;

typedef VShaderOut VertexShader(VShaderIn *in, VShaderGlobals *globals);
typedef Vec4     FragmentShader(FShaderIn *in, FShaderGlobals *globals);

VShaderOut VertexShader_Base(VShaderIn *in, VShaderGlobals *globals);

Vec4 FragmentShader_Base(FShaderIn *in, FShaderGlobals *globals);

#endif
