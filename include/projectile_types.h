#ifndef PROJECTILE_TYPES_H
#define PROJECTILE_TYPES_H

#include "raylib.h"
#include "enemy_types.h"
#include <stdbool.h>

// Projectile type enumeration
typedef enum {
    PROJECTILE_LASER,       // Fast, straight line, low damage
    PROJECTILE_PLASMA,      // Medium speed, medium damage, slight homing
    PROJECTILE_MISSILE,     // Slow, high damage, strong homing
    PROJECTILE_ENERGY_ORB,  // Slow, area damage, no homing
    PROJECTILE_PLAYER_BULLET, // Player's basic bullet, damage = 1
    PROJECTILE_TYPE_COUNT
} ProjectileType;

// Projectile movement pattern
typedef enum {
    PATTERN_STRAIGHT,       // Moves in straight line
    PATTERN_HOMING,         // Tracks target
    PATTERN_WAVE,           // Sine wave pattern
    PATTERN_SPIRAL,         // Spiral pattern
    PATTERN_SPREAD          // Spreads out over distance
} MovementPattern;

// Projectile definition structure
typedef struct {
    ProjectileType type;
    const char* name;
    float speed;            // Base speed
    int damage;             // Damage dealt
    float size;             // Visual size
    float hitboxRadius;     // Collision radius
    Color primaryColor;     // Main color
    Color secondaryColor;   // Trail/glow color
    Color glowColor;        // Outer glow
    MovementPattern pattern;
    float homingStrength;   // 0 = no homing, 1 = strong homing
    float lifetime;         // Seconds before despawn
    bool piercing;          // Can hit multiple targets
    bool explosive;         // Explodes on impact
    float explosionRadius;  // Area of effect radius
} ProjectileDefinition;

// Individual projectile instance
typedef struct {
    Vector2 position;
    Vector2 velocity;
    ProjectileType type;
    bool active;
    float lifetime;
    float animationTimer;
    int damage;
    
    // Movement properties
    Vector2 target;         // For homing projectiles
    float baseAngle;        // Initial angle
    float waveOffset;       // For wave pattern
    float spiralAngle;      // For spiral pattern
    
    // Visual properties
    float rotation;
    float scale;
    Color tint;
    float trailLength;
    Vector2 trailPositions[10];  // Trail effect
    
    // Ownership
    bool isPlayerProjectile;
    int enemyId;            // Which enemy fired it
} Projectile;

// Enemy to projectile mapping
typedef struct {
    EnemyType enemyType;
    ProjectileType primaryProjectile;
    ProjectileType secondaryProjectile;  // Some enemies have two attack types
    float fireRate;         // Shots per second
    int burstCount;         // Number of shots in a burst
    float spreadAngle;      // Spread for multi-shot
} EnemyWeaponConfig;

// Function declarations
void InitProjectileTypes(void);
const ProjectileDefinition* GetProjectileDefinition(ProjectileType type);
void InitializeProjectile(Projectile* projectile, ProjectileType type, Vector2 position, Vector2 target, bool isPlayer);
void UpdateProjectile(Projectile* projectile, float deltaTime);
void DrawProjectile(const Projectile* projectile);
void DrawProjectileSprite(const Projectile* projectile, Texture2D spriteSheet, Rectangle sourceRect);

// Weapon configuration
const EnemyWeaponConfig* GetEnemyWeaponConfig(EnemyType enemyType);
ProjectileType GetEnemyPrimaryProjectile(EnemyType enemyType);
ProjectileType GetEnemySecondaryProjectile(EnemyType enemyType);

// Global projectile definitions
extern ProjectileDefinition projectileTypes[PROJECTILE_TYPE_COUNT];
extern EnemyWeaponConfig enemyWeapons[ENEMY_TYPE_COUNT];

#endif // PROJECTILE_TYPES_H

