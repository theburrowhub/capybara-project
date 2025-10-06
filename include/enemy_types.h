#ifndef ENEMY_TYPES_H
#define ENEMY_TYPES_H

#include "types.h"

// Enemy type enumeration
typedef enum {
    ENEMY_GRUNT,        // Basic enemy - small, weak, fast
    ENEMY_TANK,         // Large, slow, high health
    ENEMY_SPEEDER,      // Very fast, low health
    ENEMY_ZIGZAG,       // Medium stats, zigzag movement
    ENEMY_BOMBER,       // Large, drops bombs
    ENEMY_SHIELD,       // Has a protective shield
    ENEMY_SWARM,        // Small, appears in groups
    ENEMY_ELITE,        // Strong all-around enemy
    ENEMY_GHOST,        // Phases in and out of visibility
    ENEMY_BOSS,         // Boss enemy - very large and powerful
    ENEMY_TYPE_COUNT
} EnemyType;

// Boss-specific constants (each boss type will have its own in the future)
#define BOSS_SHIELD_HEALTH 100       // Boss shield health
#define BOSS_SHIELD_REGEN_TIME 5.0f  // Time in seconds before shield regenerates
#define BOSS_BASE_HEALTH 500         // Boss base health without shield

// Enemy type definition structure
typedef struct {
    EnemyType type;
    const char* name;
    float size;           // Radius multiplier (1.0 = normal)
    int health;           // Hit points
    float speed;          // Speed multiplier
    int power;            // Damage dealt
    Color primaryColor;   // Main color
    Color secondaryColor; // Accent color
    Color glowColor;      // Glow/highlight color
    float resistance;     // Damage reduction (0.0 = none, 1.0 = immune)
    const char* symbol;   // Visual identifier symbol
} EnemyTypeDefinition;

// Extended enemy structure with type information
typedef struct EnemyEx {
    Vector2 position;
    Rectangle bounds;
    Color color;
    bool active;
    float speed;
    int health;
    int maxHealth;
    int id;
    float radius;
    float moveTimer;
    float targetY;
    float speedX;
    float speedY;
    
    // Type-specific properties
    EnemyType type;
    int power;
    float resistance;
    Color secondaryColor;
    Color glowColor;
    const char* symbol;
    
    // Special behavior properties
    float animationTimer;
    float specialTimer;
    bool isVisible;       // For ghost enemy
    float shieldAngle;    // For shield enemy rotation
    int swarmGroup;       // For swarm enemies
    
    // Firing properties
    float fireTimer;      // Timer for firing projectiles
    float nextFireTime;   // When to fire next
    bool can_fire;        // Whether this enemy is allowed to fire
    
    // Hit tracking
    int hitsTaken;        // Number of hits taken before dying
    
    // Boss-specific escape behavior
    bool isEscaping;      // Boss is escaping off screen
} EnemyEx;

// Function declarations
void InitEnemyTypes(void);
const EnemyTypeDefinition* GetEnemyTypeDefinition(EnemyType type);
void InitializeEnemyFromType(EnemyEx* enemy, EnemyType type, float x, float y);
void DrawEnemyEx(const EnemyEx* enemy);
const char* GetEnemyTypeName(EnemyType type);
Color GetEnemyTypeColor(EnemyType type);

// Global enemy type definitions
extern EnemyTypeDefinition enemyTypes[ENEMY_TYPE_COUNT];

#endif // ENEMY_TYPES_H
