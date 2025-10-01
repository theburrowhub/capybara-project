#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"

// Initialize enemies array
void InitEnemies(Enemy* enemies);

// Spawn a new enemy
void SpawnEnemy(Game* game);

// Update all enemies
void UpdateEnemies(Game* game);

// Draw all enemies
void DrawEnemies(const Enemy* enemies, bool showHitbox);

// Count active enemies
int CountActiveEnemies(const Game* game);

#endif // ENEMY_H
