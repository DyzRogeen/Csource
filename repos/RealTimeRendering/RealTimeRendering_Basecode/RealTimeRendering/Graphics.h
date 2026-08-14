#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "Settings.h"
#include "Renderer.h"
#include "Vector.h"
#include "Matrix.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Scene.h"

typedef struct VShaderGlobals_s VShaderGlobals;
typedef struct VShaderIn_s      VShaderIn;
typedef struct VShaderOut_s     VShaderOut;

typedef struct FShaderGlobals_s FShaderGlobals;
typedef struct FShaderIn_s      FShaderIn;

typedef VShaderOut VertexShader(VShaderIn *in, VShaderGlobals *globals);
typedef Vec4     FragmentShader(FShaderIn *in, FShaderGlobals *globals);

/// @brief Calcule le rendu d'un objet.
/// @param renderer le moteur de rendu 2D.
/// @param object l'objet à rendre.
/// @param vertShader le vertex shader.
/// @param fragShader le fragement shader.
void Graphics_RenderObject(
    Renderer *renderer, Object *object,
    VertexShader *vertShader, FragmentShader *fragShader);

/// @brief Calcule le rendu d'un triangle.
/// @param renderer le moteur de rendu 2D.
/// @param vertices tableau contenant les trois sommets du triangle.
/// @param fragShader le fragement shader.
/// @param fragGlobals les données globales au triangle utilisées par le fragment shader.
void Graphics_RenderTriangle(
    Renderer *renderer, VShaderOut *vertices,
    FragmentShader *fragShader, FShaderGlobals *fragGlobals);

#endif
