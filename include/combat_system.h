#ifndef COMBAT_SYSTEM_H
#define COMBAT_SYSTEM_H

#include "types.h"
#include "enemy_types.h"
#include "projectile_types.h"

/**
 * Combat System - Generic combat operations shared between game and showcase
 * 
 * This module provides reusable combat functions that work with any context,
 * eliminating code duplication between the main game and testing environments.
 */

// Combat context - contains only what's needed for combat operations
typedef struct {
    Vector2 playerPosition;       // Current player position (target for enemies)
    Projectile* projectiles;      // Projectile array
    int maxProjectiles;           // Maximum number of projectiles
    float screenWidth;            // Screen width for bounds checking
    float screenHeight;           // Screen height for bounds checking
} CombatContext;

/**
 * Fire enemy projectile using enemy's weapon configuration
 * 
 * @param ctx Combat context with player position and projectile array
 * @param enemy Enemy that is firing
 */
void Combat_FireEnemyProjectile(CombatContext* ctx, EnemyEx* enemy);

/**
 * Update enemy firing timer and fire if ready
 * 
 * @param enemy Enemy to update
 * @param ctx Combat context
 * @param deltaTime Time since last frame
 * @param minX Minimum X position to allow firing
 * @param maxX Maximum X position to allow firing
 * @param minY Minimum Y position to allow firing
 * @param maxY Maximum Y position to allow firing
 * @return true if enemy fired, false otherwise
 */
bool Combat_UpdateEnemyFiring(EnemyEx* enemy, CombatContext* ctx, float deltaTime,
                               float minX, float maxX, float minY, float maxY);

#endif // COMBAT_SYSTEM_H
