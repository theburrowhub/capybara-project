#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "types.h"

// Initialize obstacles array
void InitObstacles(Obstacle* obstacles);

// Spawn a new obstacle
void SpawnObstacle(Game* game);

// Update all obstacles
void UpdateObstacles(Game* game);

// Draw all obstacles
void DrawObstacles(const Obstacle* obstacles, bool showHitbox);

// Count active obstacles
int CountActiveObstacles(const Game* game);

#endif // OBSTACLE_H
