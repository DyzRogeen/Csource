#ifndef _TIMER_H_
#define _TIMER_H_

/// @file Timer.h
/// @defgroup Timer
/// @{
#include "Settings.h"

/// @brief Structure représentant un chronomètre.
typedef struct Timer_s
{
    float m_startTime;
    float m_currentTime;
    float m_previousTime;
    float m_delta;
} Timer;

/// @brief Temps global au programme.
extern Timer *g_time;

/// @brief Crée un nouveau timer.
/// @return Le timer créé.
Timer* Timer_New();

/// @brief Détruit un timer préalablement alloué avec Timer_new().
/// @param timer le timer à détruire.
void Timer_Free(Timer* timer);

/// @brief Lance le timer.
/// @param timer le timer.
void Timer_Start(Timer* timer);

/// @brief Met à jour le timer en fonction du temps actuel.
/// @param timer le timer.
void Timer_Update(Timer* timer);

/// @brief Renvoie l'écart de temps (en secondes) entre les deux derniers appels à la fonction
/// Timer_update().
/// @param timer le timer.
/// @return L'écart de temps entre les deux dernières mises à jour.
float Timer_GetDelta(Timer *timer);

/// @brief Renvoie l'écart de temps (en secondes) entre le lancement du timer avec la fonction
/// Timer_start() et le dernier appel à la fonction Timer_update().
/// @param timer le timer.
/// @return Le nombre de secondes écoulées depuis le lancement du timer et la dernière mise à jour.
float Timer_GetElapsed(Timer *timer);

/// @}

#endif