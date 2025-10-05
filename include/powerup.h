#ifndef POWERUP_H
#define POWERUP_H

#include "raylib.h"
#include "types.h"
#include "enemy_types.h"
#include "player_ship.h"
#include <stdbool.h>

// Maximum number of active powerups
#define MAX_POWERUPS 20

// Powerup types
typedef enum {
    POWERUP_ENERGY,         // 20% energy recovery - COMMON
    POWERUP_SHIELD,         // Full shield recovery - UNCOMMON
    POWERUP_HULL,          // 20% hull recovery - VERY RARE
    POWERUP_WEAPON,        // Weapon upgrade (accumulative) - UNCOMMON
    POWERUP_TYPE_COUNT
} PowerupType;

// Powerup structure
typedef struct Powerup {
    Vector2 position;
    Vector2 velocity;
    Rectangle bounds;
    PowerupType type;
    bool active;
    float lifetime;         // Time since spawn
    float animationTime;    // Animation timer
    float pulsePhase;       // Pulse animation phase
    float rotationAngle;    // Rotation for visual effect
    Color primaryColor;
    Color glowColor;
    float magnetRange;      // Range at which player attracts powerup
    bool isBeingMagneted;   // Is currently being pulled to player
} Powerup;

// Powerup drop rates per enemy type (in percentage)
typedef struct {
    float energyChance;     // Chance to drop energy powerup
    float shieldChance;     // Chance to drop shield powerup
    float hullChance;       // Chance to drop hull powerup
    float weaponChance;     // Chance to drop weapon powerup
    float nothingChance;    // Chance to drop nothing
} PowerupDropRate;

// Powerup system
struct PowerupSystem {
    Powerup powerups[MAX_POWERUPS];
    int activePowerupCount;
    float totalPowerupsSpawned;
    float totalPowerupsCollected;
};

// Initialize powerup system
void InitPowerupSystem(PowerupSystem* system);

// Update all powerups
void UpdatePowerups(PowerupSystem* system, PlayerShip* player, float deltaTime);

// Draw all powerups
void DrawPowerups(const PowerupSystem* system);

// Spawn a powerup at a position
void SpawnPowerup(PowerupSystem* system, PowerupType type, Vector2 position);

// Drop a powerup from a defeated enemy
void DropPowerupFromEnemy(PowerupSystem* system, const EnemyEx* enemy);

// Check collision between player and powerups
void CheckPowerupCollisions(PowerupSystem* system, PlayerShip* player, int* score);

// Apply powerup effect to player
void ApplyPowerupEffect(PlayerShip* player, PowerupType type, int* score);

// Get drop rates for an enemy type
PowerupDropRate GetDropRateForEnemy(EnemyType enemyType);

// Get a random powerup type based on drop rates
PowerupType GetRandomPowerupFromRates(PowerupDropRate rates);

// Draw a single powerup
void DrawPowerup(const Powerup* powerup);

// Get powerup name
const char* GetPowerupName(PowerupType type);

// Get powerup description
const char* GetPowerupDescription(PowerupType type);

// Cleanup powerup system
void CleanupPowerupSystem(PowerupSystem* system);

#endif // POWERUP_H
