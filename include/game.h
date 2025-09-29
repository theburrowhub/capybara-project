#ifndef GAME_H
#define GAME_H

#include "types.h"

// Initialize game state
void InitGame(Game* game);

// Update game logic
void UpdateGame(Game* game);

// Clean up game resources
void CleanupGame(Game* game);

// Update game speed based on time
void UpdateGameSpeed(Game* game);

#endif // GAME_H
