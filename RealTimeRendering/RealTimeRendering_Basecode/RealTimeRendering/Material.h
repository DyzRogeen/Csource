#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Settings.h"
#include "Mesh.h"

#define MATERIAL_NAME_SIZE 128

typedef union Color_s
{
    struct {
        Uint8 r, g, b, a;
    };
    Uint8 data[4];
} Color;

typedef struct MeshTexture_s
{
    Color **m_pixels;
    int m_width;
    int m_height;
} MeshTexture;

MeshTexture *MeshTexture_Load(char *path);
void MeshTexture_Free(MeshTexture *meshTexture);

Vec3 MeshTexture_GetColorVec3(MeshTexture *meshTexture, Vec2 textUV);

typedef struct Material_s
{
    char m_name[MATERIAL_NAME_SIZE];
    MeshTexture *m_albedoMap;
    MeshTexture *m_normalMap;
} Material;

Material *Material_LoadMTL(Mesh *mesh, char *path, char *fileName, int *count);
void Material_Free(Material *materials, int count);

INLINE MeshTexture *Material_GetAlbedo(Material *material)
{
    return material->m_albedoMap;
}

INLINE MeshTexture *Material_GetNormalMap(Material *material)
{
    return material->m_normalMap;
}

#endif
