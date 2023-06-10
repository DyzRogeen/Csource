#ifndef _MESH_H_
#define _MESH_H_

/// @file Mesh.h
/// @defgroup Mesh
/// @{

#include "Settings.h"
#include "Vector.h"
#include "Timer.h"

typedef struct Material_s Material;

/// @brief Structure représentant un triangle dans un mesh.
typedef struct Triangle_s
{
    int m_vertexIndices[3];
    int m_normalIndices[3];
    int m_textUVIndices[3];
    int m_materialIndex;
} Triangle;

/// @brief Structure représentant un mesh.
typedef struct Mesh_s
{
    int       m_vertexCount;
    Vec3     *m_vertices;

    int       m_normalCount;
    Vec3     *m_normals;

    int       m_textUVCount;
    Vec2     *m_textUVs;

    int       m_triangleCount;
    Triangle *m_triangles;

    int       m_tangentCount;
    Vec3     *m_tangents;

    Vec3      m_min;
    Vec3      m_max;
    Vec3      m_center;

    int       m_materialCount;
    Material *m_materials;
} Mesh;

/// @brief Crée un mesh et l'initialise à partir d'un fichier objet 3D (d'extension .obj).
/// @param[in] path le chemin vers le ficher obj.
/// @return Le mesh spécifié dans le fichier obj.
Mesh *Mesh_LoadOBJ(char *folderPath, char *fileName);

/// @brief Détruit un mesh préalablement alloué dynamiquement (comme avec Mesh_loadObj).
/// @param[in,out] mesh le mesh à détruire.
void Mesh_Free(Mesh *mesh);

int Mesh_ComputeTangents(Mesh *mesh);

/// @brief Multiplie les normales des sommets du mesh par -1.
/// Cette fonction permet de corriger (éventuellement) les normales calculées automatiquement.
/// @param[in,out] mesh un mesh correctement initialisé.
void Mesh_ReverseNormals(Mesh *mesh);

void Mesh_ReverseOrientation(Mesh *mesh);

/// @}

/// @brief Lit une ligne dans un buffer à partir d'une position donnée.
/// Cette fonction auxiliaire sert à lire un fichier obj/mtl.
/// @param[in] buffer le buffer.
/// @param[in,out] offset en entrée, la position à partir de laquelle la lecture commence.
/// En sortie, la nouvelle position du curseur dans le buffer.
/// @param[in] bufferSize la taille du buffer.
/// @param[in,out] line pointeur vers un tableau de char.
/// Si la ligne à lire est plus grande que la capacité du tableau, il est réalloué 
/// @param[in,out] capacity en entrée, la taille allouée du tableau de char.
/// En sortie, la nouvelle taille.
/// @return EXIT_SUCCESS ou EXIT_FAILURE.
int Buffer_ReadLine(char *buffer, int *offset, int bufferSize, char **line, int *capacity);
char *Buffer_GetFromFile(char *folderPath, char *fileName, long *bufferSize);

#endif