#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

// Initialize player
void InitPlayer(Player* player);

// Update player movement and state
void UpdatePlayer(Game* game);

// Draw player on screen
void DrawPlayer(const Player* player, bool showHitbox);

#endif // PLAYER_H
