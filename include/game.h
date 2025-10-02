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

// Enemy management functions
void UpdateEnemies(Game* game);
void DrawEnemies(const Game* game, bool showHitbox);
int CountActiveEnemies(const Game* game);
void FireEnemyProjectile(Game* game, struct EnemyEx* enemy);

// Projectile management functions  
void UpdateProjectiles(Game* game);

#endif // GAME_H
