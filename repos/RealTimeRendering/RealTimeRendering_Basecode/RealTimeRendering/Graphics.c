#include "Graphics.h"
#include "Tools.h"
#include "Shader.h"

/// @brief Indique si la clipPos d'un point appartient au frustum représentant
/// les objects visibles par la caméra.
/// @param clipPos la clipPos d'un point dans le repère caméra
/// @return true si le point est visible, false sinon.
bool Graphics_Clip(Vec3 clipPos)
{
    return
        (clipPos.x < -1.0f) || (clipPos.x > 1.0f) ||
        (clipPos.y < -1.0f) || (clipPos.y > 1.0f) ||
        (clipPos.z < -1.0f) || (clipPos.z > 1.0f);
}

void Graphics_RenderObject(
    Renderer *renderer, Object *object,
    VertexShader *vertShader, FragmentShader *fragShader)
{
    if (!object->m_mesh)
        return;

    Scene *scene = Object_getScene(object);
    Camera *camera = Scene_GetCamera(scene);
    Mesh *mesh = object->m_mesh;
    bool wireframe = Scene_GetWireframe(scene);

    VShaderGlobals vertGlobals = { 0 };

    Mat4 viewToWorld = Object_GetModelMatrix((Object *)camera);
    Mat4 worldToView = Mat4_Inv(viewToWorld);
    Mat4 objToWorld = Object_GetModelMatrix(object);
    Mat4 objToView = Mat4_MulMM(worldToView, objToWorld);
    
    vertGlobals.cameraPos = Vec3_From4(Mat4_MulMV(viewToWorld, Vec4_ZeroH));
    vertGlobals.viewToWorld = viewToWorld;
    vertGlobals.objToWorld = objToWorld;
    vertGlobals.objToView = objToView;
    vertGlobals.objToClip = Mat4_MulMM(camera->m_projMatrix, objToView);

    int i;
    int triangleCount = mesh->m_triangleCount;

#pragma omp parallel for num_threads(4)
    for (i = 0; i < triangleCount; ++i)
    {
        Triangle *triangle = mesh->m_triangles + i;
        VShaderIn in[3] = { 0 };
        VShaderOut out[3] = { 0 };
        bool clip = true;

        for (int j = 0; j < 3; ++j)
        {
            // Calcule l'entrée du vertex shader
            in[j].vertex = mesh->m_vertices[triangle->m_vertexIndices[j]];
            in[j].normal = mesh->m_normals[triangle->m_normalIndices[j]];
            in[j].tangent = mesh->m_tangents[triangle->m_vertexIndices[j]];
            if (mesh->m_textUVs)
            {
                in[j].textUV = mesh->m_textUVs[triangle->m_textUVIndices[j]];
            }

            // VERTEX SHADER
            out[j] = vertShader(&in[j], &vertGlobals);

            // Clipping
            clip = clip && Graphics_Clip(out[j].clipPos);
        }
        if (clip)
        {
            continue;
        }

        if (!wireframe)
        {
            // Calcule des variables globales du fragment shader
            int materialIndex = triangle->m_materialIndex;
            Material *material = NULL;
            if (materialIndex >= 0)
            {
                material = mesh->m_materials + materialIndex;
            }

            FShaderGlobals fragGlobals = { 0 };
            fragGlobals.material = material;
            fragGlobals.cameraPos = vertGlobals.cameraPos;
            fragGlobals.scene = scene;

            // Calcule le rendu du triangle
            Graphics_RenderTriangle(renderer, out, fragShader, &fragGlobals);
        }
        else
        {
            // Dessine en fil de fer
            Vec4 lineColor = Vec4_Set(1.0f, 1.0f, 1.0f, 1.0f);
            Renderer_DrawLine(renderer, out[0].clipPos, out[1].clipPos, lineColor);
            Renderer_DrawLine(renderer, out[1].clipPos, out[2].clipPos, lineColor);
            Renderer_DrawLine(renderer, out[2].clipPos, out[0].clipPos, lineColor);
        }
    }
}

#define VEC2_INIT_INTERPOLATION(vShaderO, member) \
for (int i = 0; i < 3; ++i) \
{ \
    float invDepth = vShaderO[i].invDepth; \
    vShaderO[i].member.x *= invDepth; \
    vShaderO[i].member.y *= invDepth; \
}

#define VEC3_INIT_INTERPOLATION(vShaderO, member) \
for (int i = 0; i < 3; ++i) \
{ \
    float invDepth = vShaderO[i].invDepth; \
    vShaderO[i].member.x *= invDepth; \
    vShaderO[i].member.y *= invDepth; \
    vShaderO[i].member.z *= invDepth; \
}

#define VEC2_INTERPOLATE(vShaderO, member, result) \
for (int i = 0; i < 2; ++i) \
{ \
    result.data[i] = \
        w[0] * vShaderO[0].member.data[i] + \
        w[1] * vShaderO[1].member.data[i] + \
        w[2] * vShaderO[2].member.data[i]; \
    result.data[i] *= z; \
}

#define VEC3_INTERPOLATE(vShaderO, member, result) \
for (int i = 0; i < 3; ++i) \
{ \
    result.data[i] = \
        w[0] * vShaderO[0].member.data[i] + \
        w[1] * vShaderO[1].member.data[i] + \
        w[2] * vShaderO[2].member.data[i]; \
    result.data[i] *= z; \
}

void Graphics_RenderTriangle(
    Renderer *renderer, VShaderOut *vShaderO,
    FragmentShader *fragShader, FShaderGlobals *fragGlobals)
{
    int w = Renderer_GetWidth(renderer);
    int h = Renderer_GetHeight(renderer);

    // Conversion clip space vers raster space
    Vec2 rasterVertices[3];
    for (int i = 0; i < 3; ++i)
    {
        rasterVertices[i].x = w * (vShaderO[i].clipPos.x + 1.0f) / 2.0f;
        rasterVertices[i].y = h * (vShaderO[i].clipPos.y + 1.0f) / 2.0f;
    }

    float area = Vec2_SignedArea(
        rasterVertices[0], rasterVertices[1], rasterVertices[2]
    );
    if (area < 0.0f)
    {
        // Une aire négative signifie que le triangle est dernière l'objet
        return;
    }

    // Interpolation correcte en perspective
    VEC2_INIT_INTERPOLATION(vShaderO, textUV);
    VEC3_INIT_INTERPOLATION(vShaderO, normal);
    VEC3_INIT_INTERPOLATION(vShaderO, worldPos);

    // Calcule la boîte englobante du triangle
    Vec2 lower = rasterVertices[0];
    Vec2 upper = rasterVertices[0];
    for (int i = 1; i < 3; ++i)
    {
        lower = Vec2_Min(lower, rasterVertices[i]);
        upper = Vec2_Max(upper, rasterVertices[i]);
    }
    int xmin = Int_Clamp((int)lower.x, 0, w - 1);
    int xmax = Int_Clamp((int)upper.x, 0, w - 1);
    int ymin = Int_Clamp((int)lower.y, 0, h - 1);
    int ymax = Int_Clamp((int)upper.y, 0, h - 1);

    float z0 = vShaderO[0].clipPos.z;
    float z1 = vShaderO[1].clipPos.z;
    float z2 = vShaderO[2].clipPos.z;

    for (int x = xmin; x <= xmax; ++x)
    {
        for (int y = ymin; y <= ymax; ++y)
        {
            Vec2 p = Vec2_Set(x + 0.5f, y + 0.5f);
            float w[3];
            bool inside = Vec2_Barycentric(rasterVertices, p, w);
            if (!inside)
            {
                // Le pixel n'appartient pas au triangle
                continue;
            }
            float z = 1.0f / (
                w[0] * vShaderO[0].invDepth +
                w[1] * vShaderO[1].invDepth +
                w[2] * vShaderO[2].invDepth);

            // Interpolation barycentrique
            FShaderIn fShaderI = { 0 };
            VEC3_INTERPOLATE(vShaderO, normal,   fShaderI.normal);
            VEC2_INTERPOLATE(vShaderO, textUV,   fShaderI.textUV);
            VEC3_INTERPOLATE(vShaderO, worldPos,   fShaderI.worldPos);

            // FRAGMENT SHADER
            Vec4 color = fragShader(&fShaderI, fragGlobals);

            // Définit le pixel si sa zValue est inférieure à celle du z-buffer
            float zValue = w[0] * z0 + w[1] * z1 + w[2] * z2;
            Renderer_SetPixel(renderer, x, y, zValue, color, true);
        }
    }
}
