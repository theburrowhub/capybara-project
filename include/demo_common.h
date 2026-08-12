#ifndef DEMO_COMMON_H
#define DEMO_COMMON_H

#include "raylib.h"
#include "types.h"
#include "enemy_types.h"
#include "explosion.h"
#include "projectile_types.h"
#include "input_manager.h"
#include <stdbool.h>

/**
 * Common utilities for demo programs to reduce code duplication
 */

// ============================================================================
// Camera Initialization
// ============================================================================

/**
 * Initialize a camera for demo/showcase displays
 * @param width Screen/showcase width
 * @param height Screen/showcase height
 * @return Initialized Camera2D structure
 */
Camera2D DemoCommon_InitCamera(float width, float height);

// ============================================================================
// Game System Initialization
// ============================================================================

/**
 * Initialize game type systems (enemy types, projectile types)
 * Call this once at the start of any demo that uses enemies or projectiles
 */
void DemoCommon_InitGameTypes(void);

/**
 * Initialize a bullets array for player weapons
 * @param bullets Pointer to bullets array
 * @param count Number of bullets in array (typically MAX_BULLETS)
 */
void DemoCommon_InitBulletsArray(Bullet* bullets, int count);

/**
 * Initialize an enemies array
 * @param enemies Pointer to enemies array
 * @param count Number of enemies in array (typically MAX_ENEMIES)
 */
void DemoCommon_InitEnemiesArray(EnemyEx* enemies, int count);

/**
 * Initialize a projectiles array using ProjectileManager
 * @param projectiles Pointer to projectiles array
 * @param count Number of projectiles in array (typically MAX_PROJECTILES)
 */
void DemoCommon_InitProjectilesArray(Projectile* projectiles, int count);

/**
 * Allocate and initialize an explosion system
 * @return Pointer to allocated and initialized ExplosionSystem
 */
ExplosionSystem* DemoCommon_CreateExplosionSystem(void);

// ============================================================================
// Input System Initialization
// ============================================================================

/**
 * Allocate and initialize an input manager with default configuration
 * @return Pointer to allocated and initialized InputManager with its config
 * Note: Caller must free both the config and the manager when done
 */
InputManager* DemoCommon_CreateInputManager(InputConfig** outConfig);

// ============================================================================
// Starfield Utilities
// ============================================================================

/**
 * Initialize a starfield array for background effects
 * @param starfield Pointer to Vector2 array for star positions
 * @param count Number of stars
 * @param width Screen width
 * @param height Screen height
 */
void DemoCommon_InitStarfield(Vector2* starfield, int count, int width, int height);

/**
 * Update and draw a simple starfield
 * @param starfield Pointer to Vector2 array for star positions
 * @param count Number of stars
 * @param scrollSpeed Horizontal scroll speed
 * @param width Screen width
 * @param height Screen height
 */
void DemoCommon_DrawStarfield(Vector2* starfield, int count, float scrollSpeed, int width, int height);

#endif // DEMO_COMMON_H

