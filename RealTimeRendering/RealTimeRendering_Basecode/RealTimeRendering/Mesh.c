#include "Mesh.h"
#include "Material.h"
#include "Vector.h"
#include "Tools.h"

#define MAX_NB_POINTS 32

static void Mesh_FillVec3(Vec3 *v, int n)
{
    switch (n)
    {
    case 0: v->x = 0.f;
    case 1: v->y = 0.f;
    case 2: v->z = 0.f;
    }
}

int Buffer_ReadLine(char *buffer, int *offset, int bufferSize, char **line, int *capacity)
{
    int i = *offset;
    int lineSize = 0;
    int lineCapacity = *capacity;
    char *curLine = *line;

    // Passe les caractères blancs
    while (i < bufferSize)
    {
        char c = buffer[i];
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t')
            break;
        i++;
    }

    // Lit la ligne courante
    while (i < bufferSize)
    {
        char c = buffer[i++];

        if (c == '\n' || c == '\r' || c == '\0')
        {
            break;
        }
        else
        {
            if (lineSize >= lineCapacity)
            {
                lineCapacity = lineCapacity << 1;
                char *newLine = (char *)realloc(curLine, lineCapacity * sizeof(char));
                if (!newLine)
                    goto ERROR_LABEL;
                else
                    curLine = newLine;
            }
            curLine[lineSize++] = c;
        }
    }

    // Ajoute '\0' à la fin de la ligne
    if (lineSize >= lineCapacity)
    {
        lineCapacity = lineCapacity << 1;
        char *newLine = (char *)realloc(curLine, lineCapacity * sizeof(char));
        if (!newLine)
            goto ERROR_LABEL;
        else
            curLine = newLine;
    }
    curLine[lineSize++] = '\0';

    *offset = i;
    *line = curLine;
    *capacity = lineCapacity;

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Buffer_ReadLine()\n");
    assert(false);
    return EXIT_FAILURE;
}

char *Buffer_GetFromFile(char *folderPath, char *fileName, long *bufferSize)
{
    char path[1024] = { 0 };
    FILE* file = NULL;
    char *buffer = NULL;

    // Charge le fichier dans un buffer puis ferme le fichier
    strcpy_s(path, 1024, folderPath);
    strcat_s(path, 1024, "/");
    strcat_s(path, 1024, fileName);
    fopen_s(&file, path, "rb");
    if (!file) goto ERROR_LABEL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    buffer = (char *)calloc(size, sizeof(char));
    if (!buffer) goto ERROR_LABEL;

    size_t numRead = fread(buffer, 1, size, file);
    if (numRead != size) goto ERROR_LABEL;

    fclose(file);
    file = NULL;

    *bufferSize = size;

    return buffer;

ERROR_LABEL:
    printf("Buffer_GetFromFile()\n");
    assert(false);
    if (file)
    {
        fclose(file);
    }
    free(buffer);
    return NULL;
}

Mesh *Mesh_LoadOBJ(char *folderPath, char *fileName)
{
    Mesh* mesh       = NULL;
    char *objContent = NULL;
    char *curLine    = NULL;

    int lineCapacity     = 64;
    int vertexCapacity   = 1 << 12;
    int normalCapacity   = 1 << 12;
    int textUVCapacity   = 1 << 12;
    int triangleCapacity = 1 << 12;
    int vertexCount = 0;
    int normalCount = 0;
    int textUVCount = 0;
    int triangleCount = 0;
    int vertexIndices[MAX_NB_POINTS];
    int normalIndices[MAX_NB_POINTS];
    int textUVIndices[MAX_NB_POINTS];
    char pathBuffer[1024] = { 0 };
    int materialIndex = -1;

    // Charge le fichier dans un buffer
    long size = 0;
    objContent = Buffer_GetFromFile(folderPath, fileName, &size);
    if (!objContent) { assert(false); goto ERROR_LABEL; }

    // Alloue la mémoire
    mesh = (Mesh *)calloc(1, sizeof(Mesh));
    if (!mesh) { assert(false); goto ERROR_LABEL; }

    mesh->m_vertices = (Vec3*)calloc(vertexCapacity, sizeof(Vec3));
    if (!mesh->m_vertices) { assert(false); goto ERROR_LABEL; }

    mesh->m_normals = (Vec3 *)calloc(normalCapacity, sizeof(Vec3));
    if (!mesh->m_normals) { assert(false); goto ERROR_LABEL; }

    mesh->m_textUVs = (Vec2 *)calloc(textUVCapacity, sizeof(Vec2));
    if (!mesh->m_textUVs) { assert(false); goto ERROR_LABEL; }

    mesh->m_triangles = (Triangle *)calloc(triangleCapacity, sizeof(Triangle));
    if (!mesh->m_triangles) { assert(false); goto ERROR_LABEL; }

    mesh->m_min = Vec3_Set(+INFINITY, +INFINITY, +INFINITY);
    mesh->m_max = Vec3_Set(-INFINITY, -INFINITY, -INFINITY);

    curLine = (char *)calloc(lineCapacity, sizeof(char));
    if (!curLine) { assert(false); goto ERROR_LABEL; }

    // Parse le buffer
    int offset = 0;
    int lineNb = 0;
    do
    {
        int exitStatus = Buffer_ReadLine(objContent, &offset, size, &curLine, &lineCapacity);
        if (exitStatus == EXIT_FAILURE) { assert(false); goto ERROR_LABEL; }
        if (curLine[0] == '\0') continue;

        lineNb++;

        char* context = NULL;
        char* word = strtok_s(curLine, " ", &context);
        Vec3 vec;

        if (!word || strlen(word) < 1) continue;

        if (word[0] == 'v')
        {
            if (word[1] == '\0')
            {
                //---------------------------------------------------------------------------------
                // Vertex

                int i = 0;
                while ((word = strtok_s(NULL, " ", &context)) && i < 3)
                {
                    vec.data[i++] = (float)atof(word);
                }
                Mesh_FillVec3(&vec, i);

                // Double la capacité si nécesaire
                if (vertexCount >= vertexCapacity)
                {
                    vertexCapacity = vertexCapacity << 1;
                    Vec3 *newVertices = (Vec3 *)realloc(
                        mesh->m_vertices, vertexCapacity * sizeof(Vec3));
                    if (!newVertices) { assert(false); goto ERROR_LABEL; }

                    mesh->m_vertices = newVertices;
                }
                mesh->m_vertices[vertexCount++] = vec;
                mesh->m_min = Vec3_Min(mesh->m_min, vec);
                mesh->m_max = Vec3_Max(mesh->m_max, vec);
            }
            else if (word[1] == 'n')
            {
                if (word[2] != '\0') break;

                //---------------------------------------------------------------------------------
                // Normale

                int i = 0;
                while ((word = strtok_s(NULL, " ", &context)) && i < 3)
                {
                    vec.data[i++] = (float)atof(word);
                }
                Mesh_FillVec3(&vec, i);

                // Double la capacité si nécessaire
                if (normalCount >= normalCapacity)
                {
                    normalCapacity = normalCapacity << 1;
                    Vec3 *newNormals = (Vec3 *)realloc(
                        mesh->m_normals, normalCapacity * sizeof(Vec3));
                    if (!newNormals) { assert(false); goto ERROR_LABEL; }

                    mesh->m_normals = newNormals;
                }

                mesh->m_normals[normalCount++] = vec;
            }
            else if (word[1] == 't')
            {
                if (word[2] != '\0') break;

                //---------------------------------------------------------------------------------
                // Coordonnée de texture
                Vec2 textUV = { 0 };

                if (word = strtok_s(NULL, " ", &context))
                {
                    textUV.x = (float)atof(word);
                }
                if (word = strtok_s(NULL, " ", &context))
                {
                    textUV.y = (float)atof(word);
                }

                // Double la capacité si nécessaire
                if (textUVCount >= textUVCapacity)
                {
                    textUVCapacity = textUVCapacity << 1;
                    Vec2 *newTexCoords = (Vec2 *)realloc(
                        mesh->m_textUVs, textUVCapacity * sizeof(Vec2));
                    if (!newTexCoords) { assert(false); goto ERROR_LABEL; }

                    mesh->m_textUVs = newTexCoords;
                }

                mesh->m_textUVs[textUVCount++] = textUV;
            }
        }
        else if (word[0] == 'f')
        {
            if (word[1] != '\0') break;

            //-------------------------------------------------------------------------------------
            // Faces

            // Parse la face
            int nbPoints = 0;
            while (word = strtok_s(NULL, " ", &context))
            {
                int indices[3] = { 0 };
                if (strstr(word, "//"))
                {
                    // Vertex normal (sans texture)
                    char *contextSlash = NULL;
                    char* data = strtok_s(word, "/", &contextSlash);
                    int i = 0;
                    while (data)
                    {
                        if (i < 3)
                        {
                            indices[i] = atoi(data) - 1;
                        }
                        data = strtok_s(NULL, "/", &contextSlash);
                        i++;
                    }
                    switch (i)
                    {
                    case 2:
                        // [Vertex, normal]
                        if (nbPoints < MAX_NB_POINTS)
                        {
                            vertexIndices[nbPoints] = indices[0];
                            normalIndices[nbPoints] = indices[1];
                            textUVIndices[nbPoints] = -1;
                        }
                        break;
                    default:
                        assert(false);
                        goto ERROR_LABEL;
                        break;
                    }
                }
                else
                {
                    if (strstr(word, "/"))
                    {
                        char *contextSlash = NULL;
                        char* data = strtok_s(word, "/", &contextSlash);
                        int i = 0;
                        while (data)
                        {
                            if (i < 3)
                            {
                                indices[i] = atoi(data) - 1;
                            }
                            data = strtok_s(NULL, "/", &contextSlash);
                            i++;
                        }
                        switch (i)
                        {
                        case 2:
                            // [Vertex, texture]
                            if (nbPoints < MAX_NB_POINTS)
                            {
                                vertexIndices[nbPoints] = indices[0];
                                textUVIndices[nbPoints] = indices[1];
                                normalIndices[nbPoints] = -1;
                            }
                            break;
                        case 3:
                            // [Vertex, texture, normal]
                            if (nbPoints < MAX_NB_POINTS)
                            {
                                vertexIndices[nbPoints] = indices[0];
                                textUVIndices[nbPoints] = indices[1];
                                normalIndices[nbPoints] = indices[2];
                            }
                            break;
                        default:
                            assert(false);
                            goto ERROR_LABEL;
                            break;
                        }
                    }
                    else
                    {
                        // [Vertex]
                        if (nbPoints < MAX_NB_POINTS)
                        {
                            vertexIndices[nbPoints] = atoi(word) - 1;
                            textUVIndices[nbPoints] = -1;
                            normalIndices[nbPoints] = -1;
                        }
                    }
                }
                nbPoints++;
            }

            if (nbPoints > MAX_NB_POINTS)
            {
                printf("WARNING - Mesh_loadObj()\n");
                printf("          Too much points in one face\n");
                nbPoints = MAX_NB_POINTS;
            }

            // Crée les triangles
            for (int i = 1; i < nbPoints - 1; i++)
            {
                // Double la capacité si nécesaire
                if (triangleCount >= triangleCapacity)
                {
                    triangleCapacity = triangleCapacity << 1;
                    Triangle *newTriangles = (Triangle *)realloc(
                        mesh->m_triangles, triangleCapacity * sizeof(Triangle));
                    if (!newTriangles) { assert(false); goto ERROR_LABEL; }

                    mesh->m_triangles = newTriangles;
                }
                Triangle *triangle = &mesh->m_triangles[triangleCount++];

                triangle->m_vertexIndices[0] = vertexIndices[0];
                triangle->m_vertexIndices[1] = vertexIndices[i];
                triangle->m_vertexIndices[2] = vertexIndices[i + 1];

                triangle->m_normalIndices[0] = normalIndices[0];
                triangle->m_normalIndices[1] = normalIndices[i];
                triangle->m_normalIndices[2] = normalIndices[i + 1];

                triangle->m_textUVIndices[0] = textUVIndices[0];
                triangle->m_textUVIndices[1] = textUVIndices[i];
                triangle->m_textUVIndices[2] = textUVIndices[i + 1];

                triangle->m_materialIndex = materialIndex;
            }
        }
        else if (strcmp(word, "mtllib") == 0)
        {
            word = strtok_s(NULL, " ", &context);
            if (!word) continue;

            int materialCount = 0;
            Material *materials = Material_LoadMTL(mesh, folderPath, word, &materialCount);
            if (!materials) { assert(false); goto ERROR_LABEL; }

            mesh->m_materials = materials;
            mesh->m_materialCount = materialCount;
        }
        else if (strcmp(word, "usemtl") == 0)
        {
            word = strtok_s(NULL, " ", &context);
            if (!word) continue;

            materialIndex = -1;
            int materialCount = mesh->m_materialCount;
            for (int i = 0; i < materialCount; ++i)
            {
                char *materialName = mesh->m_materials[i].m_name;
                if (strcmp(materialName, word) == 0)
                {
                    materialIndex = i;
                    break;
                }
            }
        }
    } while (offset < size);

    //---------------------------------------------------------------------------------------------
    // Calcule les normales manquante

    int autoNormalCount = 0;
    for (int i = 0; i < triangleCount; i++)
    {
        Triangle *triangle = &mesh->m_triangles[i];
        for (int j = 0; j < 3; j++)
        {
            if (triangle->m_normalIndices[j] == -1)
            {
                Vec3 *vertices = mesh->m_vertices;
                Vec3 vertex0, vertex1, vertex2, vecA, vecB, normal;
                vertex0 = vertices[triangle->m_vertexIndices[j]];
                vertex1 = vertices[triangle->m_vertexIndices[(j + 1) % 3]];
                vertex2 = vertices[triangle->m_vertexIndices[(j + 2) % 3]];
                vecA    = Vec3_Sub(vertex2, vertex0);
                vecB    = Vec3_Sub(vertex1, vertex0);
                normal  = Vec3_Normalize(Vec3_Cross(vecA, vecB));

                // Double la capacité si nécesaire
                if (normalCount >= normalCapacity)
                {
                    normalCapacity = normalCapacity << 1;
                    Vec3 *newNormals = (Vec3 *)realloc(
                        mesh->m_normals, normalCapacity * sizeof(Vec3));
                    if (!newNormals) { assert(false); goto ERROR_LABEL; }

                    mesh->m_normals = newNormals;
                }

                triangle->m_normalIndices[j] = normalCount;
                mesh->m_normals[normalCount++] = normal;
                ++autoNormalCount;
            }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Vérifie les triangles

    for (int i = 0; i < triangleCount; i++)
    {
        Triangle *triangle = &mesh->m_triangles[i];
        for (int j = 0; j < 3; j++)
        {
            int index = triangle->m_vertexIndices[j];
            if (index < 0 || index >= vertexCount) { assert(false); goto ERROR_LABEL; }

            index = triangle->m_normalIndices[j];
            if (index < 0 || index >= normalCount) { assert(false); goto ERROR_LABEL; }

            index = triangle->m_textUVIndices[j];
            if (index < -1 || index >= textUVCount) { assert(false); goto ERROR_LABEL; }
        }
    }

    //---------------------------------------------------------------------------------------------
    // Réalloue la mémoire

    Vec3 *newVertices = NULL;
    Vec3 *newNormals = NULL;
    Vec2 *newTextUVs = NULL;
    Triangle *newTriangles = NULL;

    if (vertexCount > 0)
    {
        newVertices = (Vec3 *)realloc(mesh->m_vertices, vertexCount * sizeof(Vec3));
        if (!newVertices) { assert(false); goto ERROR_LABEL; }
    }
    else
    {
        printf("WARNING - No vertex\n");
        free(mesh->m_vertices);
    }

    if (normalCount > 0)
    {
        newNormals = (Vec3 *)realloc(mesh->m_normals, normalCount * sizeof(Vec3));
        if (!newNormals) { assert(false); goto ERROR_LABEL; }
    }
    else
    {
        printf("WARNING - No normal\n");
        free(mesh->m_normals);
    }

    if (textUVCount > 0)
    {
        newTextUVs = (Vec2 *)realloc(mesh->m_textUVs, textUVCount * sizeof(Vec2));
        if (!newTextUVs) { assert(false); goto ERROR_LABEL; }
    }
    else
    {
        printf("WARNING - No texture coordinate\n");
        free(mesh->m_textUVs);
    }

    if (triangleCount > 0)
    {
        newTriangles = (Triangle *)realloc(mesh->m_triangles, triangleCount * sizeof(Triangle));
        if (!newTriangles) { assert(false); goto ERROR_LABEL; }
    }
    else
    {
        printf("WARNING - No triangle\n");
        free(mesh->m_triangles);
    }

    mesh->m_vertexCount = vertexCount;
    mesh->m_normalCount = normalCount;
    mesh->m_textUVCount = textUVCount;
    mesh->m_triangleCount = triangleCount;

    mesh->m_vertices = newVertices;
    mesh->m_normals = newNormals;
    mesh->m_textUVs = newTextUVs;
    mesh->m_triangles = newTriangles;

    mesh->m_center = Vec3_Scale(Vec3_Add(mesh->m_min, mesh->m_max), 0.5f);

    free(curLine);
    free(objContent);

    return mesh;

ERROR_LABEL:
    printf("ERROR - Mesh_loadFormFile()\n");
    assert(false);
    free(objContent);
    free(curLine);
    Mesh_Free(mesh);
    return NULL;
}

void Mesh_Free(Mesh *mesh)
{
    if (!mesh) return;

    Material_Free(mesh->m_materials, mesh->m_materialCount);

    free(mesh->m_vertices);
    free(mesh->m_normals);
    free(mesh->m_textUVs);
    free(mesh->m_triangles);

    // Met à zéro la mémoire (sécurité)
    memset(mesh, 0, sizeof(Mesh));

    free(mesh);
}

int Mesh_ComputeTangents(Mesh *mesh)
{
    int vertexCount = mesh->m_vertexCount;
    Vec3 *vertices = mesh->m_vertices;

    int tangentCount = vertexCount;
    Vec3 *tangents = NULL;

    int textUVCount = mesh->m_textUVCount;
    Vec2 *textUVs = mesh->m_textUVs;

    tangents = (Vec3 *)calloc(tangentCount, sizeof(Vec3));
    if (!tangents) goto ERROR_LABEL;

    int triangleCount = mesh->m_triangleCount;
    Triangle *triangles = mesh->m_triangles;

    for (int i = 0; i < triangleCount; ++i)
    {
        Triangle *triangle = triangles + i;
        int vertexIndices[3] = {
            triangle->m_vertexIndices[0],
            triangle->m_vertexIndices[1],
            triangle->m_vertexIndices[2]
        };
        int textUVIndices[3] = {
            triangle->m_textUVIndices[0],
            triangle->m_textUVIndices[1],
            triangle->m_textUVIndices[2]
        };

        if (textUVIndices[0] == -1 ||
            textUVIndices[1] == -1 ||
            textUVIndices[2] == -1)
        {
            continue;
        }

        int j = 0;
        Vec3 v0 = vertices[vertexIndices[0]];
        Vec3 v1 = vertices[vertexIndices[1]];
        Vec3 v2 = vertices[vertexIndices[2]];

        Vec2 uv0 = textUVs[textUVIndices[0]];
        Vec2 uv1 = textUVs[textUVIndices[1]];
        Vec2 uv2 = textUVs[textUVIndices[2]];

        Vec3 deltaP1 = Vec3_Sub(v1, v0);
        Vec3 deltaP2 = Vec3_Sub(v2, v0);

        float deltaU1 = uv1.x - uv0.x;
        float deltaV1 = uv1.y - uv0.y;
        float deltaU2 = uv2.x - uv0.x;
        float deltaV2 = uv2.y - uv0.y;

        float det = deltaU1 * deltaV2 - deltaU2 * deltaV1;
        if (fabsf(det) < 1e-10f)
        {
            det = 1.0f;
        }

        Vec3 tangent = Vec3_Add(
            Vec3_Scale(deltaP1, +deltaV2),
            Vec3_Scale(deltaP2, -deltaV1)
        );
        tangent = Vec3_Scale(tangent, 1.0f / det);
        tangent = Vec3_Normalize(tangent);

        for (int j = 0; j < 3; ++j)
        {
            tangents[vertexIndices[j]] = Vec3_Add(
                tangents[vertexIndices[j]],
                tangent);
        }
    }

    for (int i = 0; i < tangentCount; ++i)
    {
        // Normalise la tangente
        float length = Vec3_Length(tangents[i]);
        if (length < 1E-5f)
        {
            tangents[i] = Vec3_Zero;
        }
        else
        {
            tangents[i] = Vec3_Scale(tangents[i], 1.0f / length);
        }
    }

    mesh->m_tangents = tangents;
    mesh->m_tangentCount = tangentCount;

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - Mesh_ComputeTangents()\n");
    assert(false);
    return EXIT_FAILURE;
}


void Mesh_ReverseNormals(Mesh *mesh)
{
    int nbNormals = mesh->m_normalCount;
    for (int i = 0; i < nbNormals; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            mesh->m_normals[i].data[j] *= -1.0f;
        }
    }
}

void Mesh_ReverseOrientation(Mesh *mesh)
{
    int nbTriangles = mesh->m_triangleCount;
    for (int i = 0; i < nbTriangles; i++)
    {
        Triangle *triangle = &mesh->m_triangles[i];
        int index;

        index = triangle->m_vertexIndices[1];
        triangle->m_vertexIndices[1] = triangle->m_vertexIndices[2];
        triangle->m_vertexIndices[2] = index;

        index = triangle->m_normalIndices[1];
        triangle->m_normalIndices[1] = triangle->m_normalIndices[2];
        triangle->m_normalIndices[2] = index;

        index = triangle->m_textUVIndices[1];
        triangle->m_textUVIndices[1] = triangle->m_textUVIndices[2];
        triangle->m_textUVIndices[2] = index;
    }
}
