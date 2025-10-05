#ifndef PLAYER_SHIP_H
#define PLAYER_SHIP_H

#include "raylib.h"
#include "types.h"
#include <stdbool.h>

// Player ship weapon modes
typedef enum {
    WEAPON_MODE_SINGLE,     // Single straight shot
    WEAPON_MODE_DOUBLE,     // Double parallel shots
    WEAPON_MODE_SPREAD,     // 3-way spread shot
    WEAPON_MODE_RAPID,      // Rapid fire mode
    WEAPON_MODE_CHARGE,     // Charge beam
    WEAPON_MODE_DUAL,       // Front and back shots
    WEAPON_MODE_COUNT
} WeaponMode;

// Player ship abilities
typedef enum {
    ABILITY_BOOST,          // Speed boost
    ABILITY_SHIELD_BURST,   // Temporary invulnerability
    ABILITY_EMP_BLAST,      // Clear nearby projectiles
    ABILITY_OVERDRIVE,      // Unlimited fire for short time
    ABILITY_COUNT
} PlayerAbility;

// Energy mode system
typedef enum {
    ENERGY_MODE_OFFENSIVE,   // Boosted weapon damage when full energy
    ENERGY_MODE_DEFENSIVE    // Better shield when full energy
} EnergyMode;

// Ship upgrade levels
typedef struct {
    int weaponLevel;        // 1-5
    int shieldLevel;        // 1-5
    int engineLevel;        // 1-5
    int energyLevel;        // 1-5
} ShipUpgrades;

// Enhanced player ship structure
typedef struct PlayerShip {
    // Core properties
    Vector2 position;
    Vector2 velocity;
    Rectangle bounds;
    float rotation;         // Ship rotation for banking effects
    
    // Health & Shield
    int maxHealth;
    int health;
    float shield;           // Shield points (0-100)
    float maxShield;
    float shieldRegenRate;
    float shieldRegenDelay; // Time before shield starts regenerating
    float lastDamageTime;
    
    // Energy system
    float energy;           // Energy for abilities (0-100)
    float maxEnergy;
    float energyRegenRate;
    
    // Weapon system
    WeaponMode weaponMode;
    float weaponHeat;
    float maxHeat;
    bool overheated;
    float cooldownTime;
    float fireTimer;        // Time since last shot
    float chargeLevel;      // For charge beam (0-100)
    bool isCharging;
    
    // Movement
    float baseSpeed;
    float currentSpeed;
    float acceleration;
    float maxSpeed;
    float boostSpeed;
    bool isBoosting;
    float boostDuration;
    float boostCooldown;
    
    // Abilities
    float abilityCooldowns[ABILITY_COUNT];
    bool abilityActive[ABILITY_COUNT];
    float abilityDurations[ABILITY_COUNT];
    
    // Visual effects
    float engineGlow;       // Engine effect intensity (0-1)
    float shieldGlow;       // Shield effect intensity (0-1)
    Color primaryColor;     // Main ship color
    Color secondaryColor;   // Accent color
    Color engineColor;      // Engine trail color
    Color shieldColor;      // Shield bubble color
    
    // Particle trail
    Vector2 trailPositions[20];
    float trailAlpha[20];
    int trailIndex;
    
    // Animation
    float animTime;         // General animation timer
    float bankAngle;        // Banking angle when turning
    float enginePulse;      // Engine pulse effect
    
    // Upgrades
    ShipUpgrades upgrades;
    
    // Powerup tracking
    int weaponPowerupCount;  // Number of weapon powerups collected (0-3)
    
    // Energy mode system
    EnergyMode energyMode;           // Current mode (offensive/defensive)
    bool specialAbilityActive;       // Whether special ability is currently active
    float specialAbilityTimer;       // Timer for special ability duration
    float specialAbilityHoldTimer;   // Timer for holding CTRL in defensive mode
    bool energyFull;                 // Cache whether energy is at max
    float energyDrainRate;           // Rate at which energy drains during special (per second)
    float lastEnergyDepletionTime;   // Time when energy was last depleted to 0
    float energyRegenDelay;          // Delay before energy starts regenerating after depletion
    
    // Stats
    int score;
    int enemiesDestroyed;
    float survivalTime;
    
    // Visibility (for boss escape sequence)
    bool isVisible;
    
    // Revive tracking (weapon powerup revive system)
    bool justRevived;        // Flag set when a revive happens
    float reviveEffectTimer; // Timer for revive visual effect
} PlayerShip;

// Ship configuration
typedef struct {
    float baseHealth;
    float baseShield;
    float baseEnergy;
    float baseSpeed;
    float baseFireRate;
    float baseDamage;
} ShipConfig;

// Initialize enhanced player ship
void InitPlayerShip(PlayerShip* ship);

// Update player ship
void UpdatePlayerShip(PlayerShip* ship, float deltaTime);

// Handle player input
void HandlePlayerInput(PlayerShip* ship);

// Update ship physics
void UpdateShipPhysics(PlayerShip* ship, float deltaTime);

// Update weapon system
void UpdateShipWeapons(PlayerShip* ship, Game* game, float deltaTime);

// Update abilities
void UpdateShipAbilities(PlayerShip* ship, float deltaTime);

// Update visual effects
void UpdateShipEffects(PlayerShip* ship, float deltaTime);

// Draw the player ship
void DrawPlayerShip(const PlayerShip* ship);

// Draw ship with sprite
void DrawPlayerShipSprite(const PlayerShip* ship, Texture2D texture);

// Draw ship HUD
void DrawShipHUD(const PlayerShip* ship);

// Draw shield effect
void DrawShieldEffect(const PlayerShip* ship);

// Draw engine trail
void DrawEngineTrail(const PlayerShip* ship);

// Apply damage to ship
void DamagePlayerShip(PlayerShip* ship, int damage);

// Heal/repair ship
void RepairPlayerShip(PlayerShip* ship, int amount);

// Activate ability
bool ActivateAbility(PlayerShip* ship, PlayerAbility ability);

// Switch weapon mode
void SwitchWeaponMode(PlayerShip* ship, WeaponMode mode);

// Apply upgrade
void ApplyShipUpgrade(PlayerShip* ship, int upgradeType, int level);

// Get ship stats for display
void GetShipStats(const PlayerShip* ship, char* buffer, int bufferSize);

// Calculate damage per individual bullet based on current state
float CalculateDamagePerShot(const PlayerShip* ship);

#endif // PLAYER_SHIP_H
