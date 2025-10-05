#ifndef COLLISION_H
#define COLLISION_H

#include "types.h"
#include "enemy_types.h"
#include "explosion.h"

// Check all collisions in the game
void CheckCollisions(Game* game);

// Check bullet-enemy collisions
void CheckBulletEnemyCollisions(Game* game);

// Check player-enemy collisions
void CheckPlayerEnemyCollisions(Game* game);

/**
 * Generic Bullet-Enemy Collision System
 * 
 * This context allows both game and showcase to use the same collision logic
 * with optional features like explosions, score, and kill tracking.
 */
typedef struct {
    Bullet* bullets;                 // Bullet array
    int maxBullets;                  // Maximum bullets
    EnemyEx* enemies;                // Enemy array
    int maxEnemies;                  // Maximum enemies
    
    // Optional features (set to NULL if not used)
    ExplosionSystem* explosionSystem;  // For creating explosions (game only)
    int* score;                        // For adding score (game only)
    int* enemiesKilled;                // For tracking kills (showcase only)
    void* logContext;                  // For logging (game only)
    
    // Callbacks (optional)
    void (*onEnemyHit)(void* context, EnemyEx* enemy, int damage);
    void (*onEnemyDestroyed)(void* context, EnemyEx* enemy);
} CollisionContext;

/**
 * Check bullet-enemy collisions using generic context
 * Works for both game and showcase with different feature sets
 * 
 * @param ctx Collision context with bullets, enemies, and optional features
 */
void Collision_CheckBulletEnemyGeneric(CollisionContext* ctx);

#endif // COLLISION_H
