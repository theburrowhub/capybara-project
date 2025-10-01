#ifndef COLLISION_H
#define COLLISION_H

#include "types.h"

// Check all collisions in the game
void CheckCollisions(Game* game);

// Check bullet-enemy collisions
void CheckBulletEnemyCollisions(Game* game);

// Check bullet-obstacle collisions
void CheckBulletObstacleCollisions(Game* game);

// Check player-enemy collisions
void CheckPlayerEnemyCollisions(Game* game);

// Check player-obstacle collisions
void CheckPlayerObstacleCollisions(Game* game);

#endif // COLLISION_H
