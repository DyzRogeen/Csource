#include "Mesh.h"
#include "Material.h"
#include "Vector.h"
#include "Tools.h"

Material *Material_LoadMTL(Mesh *mesh, char *path, char *fileName, int *count)
{
    char *fileBuffer = NULL;
    char *curLine = NULL;
    int lineCapacity = 64;
    Material *materials = NULL;
    int materialCount = 0;
    int materialCapacity = 64;
    char filePathBuffer[1024] = { 0 };

    // Définit le nombre de matériaux à 0 par sécurité
    *count = 0;

    // Charge le fichier dans un buffer
    long size = 0;
    fileBuffer = Buffer_GetFromFile(path, fileName, &size);
    if (!fileBuffer) { assert(false); goto ERROR_LABEL; }

    // Alloue la ligne
    curLine = (char *)calloc(lineCapacity, sizeof(char));
    if (!curLine) goto ERROR_LABEL;

    materials = (Material *)calloc(materialCapacity, sizeof(Material));
    if (!materials) goto ERROR_LABEL;

    // Parse le buffer
    int offset = 0;
    int lineNb = 0;
    int index = -1;
    do
    {
        int exitStatus = Buffer_ReadLine(fileBuffer, &offset, size, &curLine, &lineCapacity);
        if (exitStatus == EXIT_FAILURE) goto ERROR_LABEL;

        if (curLine[0] == '\0') continue;

        lineNb++;

        char *context = NULL;
        char *word = strtok_s(curLine, " ", &context);

        if (!word || strlen(word) < 1) continue;

        if (strcmp(word, "newmtl") == 0)
        {
            word = strtok_s(NULL, " ", &context);
            if (!word) continue;

            materialCount++;
            index++;
            strcpy_s(materials[index].m_name, MATERIAL_NAME_SIZE, word);
            printf("new material %s\n", materials[index].m_name);
        }
        else if (strcmp(word, "map_Ka") == 0 || strcmp(word, "map_Kd") == 0)
        {
            word = strtok_s(NULL, " ", &context);
            if (!word) continue;

            strcpy_s(filePathBuffer, 1024, path);
            strcat_s(filePathBuffer, 1024, "/");
            strcat_s(filePathBuffer, 1024, word);
            if (materials[index].m_albedoMap != NULL) continue;

            MeshTexture *texture = MeshTexture_Load(filePathBuffer);
            if (!texture) goto ERROR_LABEL;
            
            materials[index].m_albedoMap = texture;
        }
        else if (strcmp(word, "map_Nrm") == 0)
        {
            word = strtok_s(NULL, " ", &context);
            if (!word) continue;

            strcpy_s(filePathBuffer, 1024, path);
            strcat_s(filePathBuffer, 1024, "/");
            strcat_s(filePathBuffer, 1024, word);
            if (materials[index].m_normalMap != NULL) continue;

            MeshTexture *texture = MeshTexture_Load(filePathBuffer);
            if (!texture) goto ERROR_LABEL;

            materials[index].m_normalMap = texture;
        }

    } while (offset < size);

    *count = materialCount;

    return materials;

ERROR_LABEL:
    printf("ERROR - Material_LoadMTL()\n");
    assert(false);
    return NULL;
}

void Material_Free(Material *materials, int count)
{
    if (!materials) return;

    for (int i = 0; i < count; ++i)
    {
        MeshTexture_Free(materials[i].m_albedoMap);
        MeshTexture_Free(materials[i].m_normalMap);
    }

    free(materials);
}

MeshTexture *MeshTexture_Load(char *path)
{
    MeshTexture *texture = NULL;
    SDL_Surface *surface = NULL;

    texture = (MeshTexture *)calloc(1, sizeof(MeshTexture));
    if (!texture) goto ERROR_LABEL;

    surface = IMG_Load(path);
    if (!surface) goto ERROR_LABEL;

    // Vérification du format
    SDL_PixelFormat *format = surface->format;
    if (format->format != SDL_PIXELFORMAT_RGBA32)
    {
        SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
        SDL_Surface *output = SDL_ConvertSurface(surface, format, 0);
        SDL_FreeFormat(format);
        SDL_FreeSurface(surface);

        if (!output) goto ERROR_LABEL;

        surface = output;
    }
    int pitch = surface->pitch;
    int width = surface->w;
    int height = surface->h;

    texture->m_width = width;
    texture->m_height = height;
    texture->m_pixels = (Color **)calloc(width, sizeof(Color *));
    if (!texture->m_pixels) goto ERROR_LABEL;

    for (int x = 0; x < width; ++x)
    {
        texture->m_pixels[x] = (Color *)calloc(height, sizeof(Color));
        if (!texture->m_pixels[x]) goto ERROR_LABEL;

        for (int y = 0; y < height; ++y)
        {
            uint32_t *pixelRow = (uint32_t *)&((uint8_t *)surface->pixels)[y * pitch];
            texture->m_pixels[x][y] = ((Color *)pixelRow)[x];
        }
    }

    SDL_FreeSurface(surface);
    surface = NULL;

    return texture;

ERROR_LABEL:
    printf("ERROR - MeshTexture_Load()");
    assert(false);
    if (surface)
    {
        SDL_FreeSurface(surface);
    }
    return NULL;
}

void MeshTexture_Free(MeshTexture *meshTexture)
{
    if (!meshTexture) return;

    if (meshTexture->m_pixels)
    {
        for (int x = 0; x < meshTexture->m_width; ++x)
        {
            free(meshTexture->m_pixels[x]);
        }
        free(meshTexture->m_pixels);
    }

    free(meshTexture);
}

Vec3 MeshTexture_GetColorVec3(MeshTexture *texture, Vec2 textUV)
{
    int textureW = texture->m_width;
    int textureH = texture->m_height;

    float u = textUV.x - floorf(textUV.x);
    float v = textUV.y - floorf(textUV.y);
    v = 1.0f - v;

    int x = (int)(u * textureW);
    int y = (int)(v * textureH);

    x = Int_Clamp(x, 0, textureW - 1);
    y = Int_Clamp(y, 0, textureH - 1);

    Color color = texture->m_pixels[x][y];
    return Vec3_Set(color.r / 255.f, color.g / 255.f, color.b / 255.f);
}

