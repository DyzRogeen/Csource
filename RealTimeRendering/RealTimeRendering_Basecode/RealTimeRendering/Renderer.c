#include "Renderer.h"
#include "camera.h"
#include "Tools.h"

Renderer *Renderer_New(SDL_Renderer *rendererSDL)
{
    Renderer *renderer = NULL;
    int width = 0;
    int height = 0;

    renderer = (Renderer *)calloc(1, sizeof(Renderer));
    if (!renderer) goto ERROR_LABEL;

    int exitStatus = SDL_GetRendererOutputSize(rendererSDL, &width, &height);
    if (exitStatus < 0)
    {
        printf("ERROR - RE_Renderer_setLogicalSize()\n");
        printf("      - %s\n", SDL_GetError());
        goto ERROR_LABEL;
    }

    renderer->m_width = width;
    renderer->m_height = height;
    renderer->m_rendererSDL = rendererSDL;

    renderer->m_zBuffer = (float **)calloc(width, sizeof(float *));
    if (!renderer->m_zBuffer) goto ERROR_LABEL;

    for (int x = 0; x < width; ++x)
    {
        renderer->m_zBuffer[x] = (float *)calloc(height, sizeof(float));
        if (!renderer->m_zBuffer[x]) goto ERROR_LABEL;
    }

    renderer->m_streamTex = SDL_CreateTexture(
        rendererSDL, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        width, height);
    if (!renderer->m_streamTex) goto ERROR_LABEL;

    renderer->m_pixels = (Uint32 *)calloc((size_t)width * (size_t)height, sizeof(Uint32));
    if (!renderer->m_pixels) goto ERROR_LABEL;

    return renderer;

ERROR_LABEL:
    printf("ERROR - Renderer_new()\n");
    assert(false);
    return NULL;
}

void Renderer_Free(Renderer *renderer)
{
    if (!renderer) return;

    if (renderer->m_streamTex)
    {
        SDL_DestroyTexture(renderer->m_streamTex);
    }
    if (renderer->m_rendererSDL)
    {
        SDL_DestroyRenderer(renderer->m_rendererSDL);
    }

    if (renderer->m_zBuffer)
    {
        int width = renderer->m_width;
        float **zBuffer = renderer->m_zBuffer;
        for (int x = 0; x < renderer->m_width; ++x)
        {
            if (zBuffer[x])
            {
                free(zBuffer[x]);
            }
        }
        free(zBuffer);
    }
    free(renderer->m_pixels);

    // Met à zéro la mémoire (sécurité)
    memset(renderer, 0, sizeof(Renderer));

    free(renderer);
}

void Renderer_SetPixel(Renderer *renderer, int x, int y, float zValue, Vec4 color, bool zWrite)
{
    if (x < 0 || x >= renderer->m_width ||
        y < 0 || y >= renderer->m_height)
        return;

    y = renderer->m_height - 1 - y;
//#pragma omp critical
    {
        if (zValue <= renderer->m_zBuffer[x][y])
        {
            SDL_Renderer *rendererSDL = renderer->m_rendererSDL;
            int r = Int_Clamp((int)(255.f * color.x), 0, 255);
            int g = Int_Clamp((int)(255.f * color.y), 0, 255);
            int b = Int_Clamp((int)(255.f * color.z), 0, 255);
            int a = Int_Clamp((int)(255.f * color.w), 0, 255);

            renderer->m_pixels[y * renderer->m_width + x] =
                ((Uint32)r << 24) |
                ((Uint32)g << 16) |
                ((Uint32)b <<  8) |
                ((Uint32)a <<  0);

            if (zWrite)
            {
                renderer->m_zBuffer[x][y] = zValue;
            }
        }
    }
}

void Renderer_DrawLine(Renderer *renderer, Vec3 p0, Vec3 p1, Vec4 color)
{
    // Algorithme de Bresenham (cf wikipedia anglais)
    int w = Renderer_GetWidth(renderer);
    int h = Renderer_GetHeight(renderer);
    int x0 = (int)(w * (p0.x + 1.0f) / 2.0f);
    int y0 = (int)(h * (p0.y + 1.0f) / 2.0f);
    int x1 = (int)(w * (p1.x + 1.0f) / 2.0f);
    int y1 = (int)(h * (p1.y + 1.0f) / 2.0f);
    int dx = +abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        // Dessine toujours le pixel (zValue = -2.0f)
        Renderer_SetPixel(renderer, x0, y0, -2.f, color, true);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) // e_xy + e_x > 0
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) // e_xy + e_y < 0
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer_ResetDepthBuffer(Renderer *renderer)
{
    // Les points entres les plans near et far ont une profondeur dans [-1.0f, 1.0f]
    int width = Renderer_GetWidth(renderer);
    int height = Renderer_GetHeight(renderer);
    float **zBuffer = renderer->m_zBuffer;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            zBuffer[x][y] = 2.f;
        }
    }
}

void Renderer_Fill(Renderer *renderer, Vec4 color)
{
    SDL_Renderer *rendererSDL = renderer->m_rendererSDL;
    Uint8 r = (Uint8)(Int_Clamp((int)(255.f * color.x), 0, 255));
    Uint8 g = (Uint8)(Int_Clamp((int)(255.f * color.y), 0, 255));
    Uint8 b = (Uint8)(Int_Clamp((int)(255.f * color.z), 0, 255));
    Uint8 a = (Uint8)(Int_Clamp((int)(255.f * color.w), 0, 255));
    Uint32 val = ((Uint32)r << 24) | ((Uint32)g << 16) | ((Uint32)b << 8) | ((Uint32)a << 0);

    int size = renderer->m_height * renderer->m_width;
    for (int i = 0; i < size; ++i)
    {
        renderer->m_pixels[i] = val;
    }
}

void Renderer_Update(Renderer *renderer)
{
    SDL_Texture *texture = renderer->m_streamTex;
    Uint32 *pixels = renderer->m_pixels;

    SDL_UpdateTexture(texture, NULL, pixels, renderer->m_width * sizeof (Uint32));
    SDL_RenderCopy(renderer->m_rendererSDL, texture, NULL, NULL);
    SDL_RenderPresent(renderer->m_rendererSDL);
}
