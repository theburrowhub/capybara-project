#ifndef WEAPON_H
#define WEAPON_H

#include "types.h"

// Initialize bullets array
void InitBullets(Bullet* bullets);

// Shoot a bullet from given position
void ShootBullet(Bullet* bullets, Vector2 position);

// Update weapon heat system
void UpdateWeaponHeat(PlayerShip* playerShip, float deltaTime);

// Update all bullets and handle shooting
void UpdateBullets(Game* game);

// Draw all bullets
void DrawBullets(const Bullet* bullets);

// Draw weapon heat UI
void DrawWeaponHeatBar(const PlayerShip* playerShip);

#endif // WEAPON_H
