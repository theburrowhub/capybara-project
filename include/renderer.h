#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"

// Draw entire game
void DrawGame(Game* game);

// Draw scrolling background
void DrawBackground(float backgroundX);

// Draw UI elements
void DrawUI(const Game* game);

// Draw game over screen
void DrawGameOver(const Game* game);

// Draw debug information
void DrawDebugInfo(const Game* game);

// Draw level complete overlay (semi-transparent)
void DrawLevelCompleteOverlay(const Game* game);

#endif // RENDERER_H
