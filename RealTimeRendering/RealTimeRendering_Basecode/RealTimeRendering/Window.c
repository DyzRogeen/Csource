#include "window.h"
#include "Renderer.h"

Window* Window_New()
{
    Window* window = NULL;
    Renderer *renderer = NULL;
    SDL_Renderer *rendererSDL = NULL;
    SDL_Window *windowSDL = NULL;

    window = (Window*)calloc(1, sizeof(Window));
    if (!window) goto ERROR_LABEL;

    int flags = 0;
#ifdef _FULLSCREEN
    flags |= SDL_WINDOW_FULLSCREEN;
#endif

    windowSDL = SDL_CreateWindow(
        "Real-Time Rendering",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT, flags);

    if (!windowSDL)
    {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        goto ERROR_LABEL;
    }

    window->m_windowSDL = windowSDL;

    rendererSDL = SDL_CreateRenderer(windowSDL, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!rendererSDL)
    {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        goto ERROR_LABEL;
    }

    renderer = Renderer_New(rendererSDL);
    if (!renderer) goto ERROR_LABEL;

    window->m_renderer = renderer;
    window->m_rendererSDL = rendererSDL;


    int exitStatus = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    if (exitStatus == SDL_FALSE)
    {
        exitStatus = SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    }

    SDL_RenderSetLogicalSize(rendererSDL, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetRenderDrawColor(rendererSDL, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(rendererSDL, SDL_BLENDMODE_BLEND);

    return window;

ERROR_LABEL:
    printf("ERROR - Window_new()\n");
    assert(false);
    Window_Free(window);
    return NULL;
}

void Window_Free(Window* window)
{
    if (!window) return;

    if (window->m_renderer)
    {
        Renderer_Free(window->m_renderer);
    }

    if (window->m_windowSDL)
    {
        SDL_DestroyWindow(window->m_windowSDL);
    }

    // Clear the memory (security)
    memset(window, 0, sizeof(Window));

    free(window);
}

void Window_GetRendererSize(Window* window, int *width, int *height)
{
    SDL_RenderGetLogicalSize(window->m_rendererSDL, width, height);
}

