#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "Settings.h"
#include "Timer.h"

//#define _FULLSCREEN
//#define _FHD

#ifdef _FHD
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080
#else
#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720
#endif

typedef struct Renderer_s Renderer;

/// @file window.h
/// @defgroup Window
/// @{

/// @brief Structure représentant une fenêtre SDL avec son moteur de rendu.
typedef struct Window_s
{
    SDL_Window *m_windowSDL;
    Renderer   *m_renderer;
    SDL_Renderer *m_rendererSDL;
} Window;

/// @brief Crée une nouvelle fenêtre.
/// @return La fenêtre créée.
Window* Window_New();

/// @brief Détruit une fenêtre préalablement allouée avec Window_new();
/// @param window la fenêtre à détruire.
void Window_Free(Window* window);


INLINE Renderer *Window_getRenderer(Window *window)
{
    return window->m_renderer;
}

/// @brief Renvoie la taille logique du moteur de rendu associé à la fenêtre.
/// Il s'agit de sa résolution interne.
/// La fenêtre peut en réalité être plus grande ou plus petite et dans ce cas,
/// une interpolation est réalisée.
/// @param[in] window la fenêtre.
/// @param[out] width la largeur passée par adresse.
/// @param[out] height la hauteur passée par adresse. 
void Window_GetRendererSize(Window* window, int *width, int *height);

/// @}

#endif