#ifndef WEAPON_H
#define WEAPON_H

#include "types.h"
#include "player_ship.h"

// Initialize bullets array
void InitBullets(Bullet* bullets);

// Shoot a bullet from given position (legacy - use ShootBulletsForMode)
void ShootBullet(Bullet* bullets, Vector2 position);

// Get base damage per bullet for a weapon mode (before multipliers)
float GetBaseDamagePerBullet(WeaponMode mode);

// Get fire rate (time between shots) for a weapon mode
float GetFireRateForMode(WeaponMode mode);

// Get the name of a weapon mode
const char* GetWeaponModeName(WeaponMode mode);

// Fire bullets according to weapon mode with balanced damage
void ShootBulletsForMode(Bullet* bullets, PlayerShip* playerShip);

// Update weapon heat system
void UpdateWeaponHeat(PlayerShip* playerShip, float deltaTime);

// Update all bullets and handle shooting
void UpdateBullets(Game* game);

// Draw all bullets
void DrawBullets(const Bullet* bullets);

// Draw weapon heat UI
void DrawWeaponHeatBar(const PlayerShip* playerShip);

#endif // WEAPON_H
