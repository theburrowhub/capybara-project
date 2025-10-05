#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include "projectile_types.h"

/**
 * Projectile Manager - Generic projectile lifecycle management
 * 
 * This module provides reusable functions for managing projectiles,
 * eliminating duplication between game and showcase.
 */

// Projectile manager configuration
typedef struct {
    Projectile* projectiles;      // Projectile array
    int maxProjectiles;           // Maximum projectiles
    float minX, maxX;             // X bounds for culling
    float minY, maxY;             // Y bounds for culling
} ProjectileManager;

/**
 * Update all active projectiles and cull off-screen ones
 * 
 * @param mgr Projectile manager configuration
 * @param deltaTime Time since last frame
 */
void ProjectileManager_UpdateAll(ProjectileManager* mgr, float deltaTime);

/**
 * Draw all active projectiles
 * 
 * @param mgr Projectile manager configuration
 */
void ProjectileManager_DrawAll(const ProjectileManager* mgr);

/**
 * Count active projectiles
 * 
 * @param mgr Projectile manager configuration
 * @return Number of active projectiles
 */
int ProjectileManager_CountActive(const ProjectileManager* mgr);

/**
 * Initialize all projectiles to inactive
 * 
 * @param mgr Projectile manager configuration
 */
void ProjectileManager_InitAll(ProjectileManager* mgr);

#endif // PROJECTILE_MANAGER_H
