#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "raylib.h"
#include <stdbool.h>

// Maximum number of explosions and particles
#define MAX_EXPLOSIONS 50
#define MAX_PARTICLES_PER_EXPLOSION 30
#define MAX_DEBRIS_PIECES 20

// Explosion types
typedef enum {
    EXPLOSION_SMALL,      // Small enemy destruction
    EXPLOSION_MEDIUM,     // Medium enemy destruction
    EXPLOSION_LARGE,      // Large enemy or boss destruction
    EXPLOSION_PLAYER,     // Player ship destruction
    EXPLOSION_SHOCKWAVE,  // Special shockwave explosion
    EXPLOSION_PLASMA,     // Plasma/energy explosion
} ExplosionType;

// Particle structure for explosion effects
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float size;
    float life;           // Remaining life (0-1)
    float fadeRate;       // How fast it fades
    bool active;
} Particle;

// Debris piece for ship destruction
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotationSpeed;
    float size;
    Color color;
    float life;
    bool active;
} Debris;

// Main explosion structure
typedef struct {
    Vector2 position;
    ExplosionType type;
    float radius;         // Current explosion radius
    float maxRadius;      // Maximum radius
    float expandSpeed;    // How fast it expands
    float life;          // Remaining life (0-1)
    bool active;
    
    // Visual properties
    Color innerColor;
    Color outerColor;
    float intensity;      // Light intensity
    
    // Particles
    Particle particles[MAX_PARTICLES_PER_EXPLOSION];
    int particleCount;
    
    // Debris (for ship explosions)
    Debris debris[MAX_DEBRIS_PIECES];
    int debrisCount;
    
    // Shockwave properties
    bool hasShockwave;
    float shockwaveRadius;
    float shockwaveSpeed;
    float shockwaveAlpha;
    
    // Sound properties (for future use)
    float volume;
    bool playedSound;
} Explosion;

// Explosion system structure
typedef struct ExplosionSystem {
    Explosion explosions[MAX_EXPLOSIONS];
    int activeCount;
    
    // Screen shake for big explosions
    float screenShakeIntensity;
    float screenShakeDuration;
    Vector2 screenShakeOffset;
} ExplosionSystem;

// Function declarations
void InitExplosionSystem(ExplosionSystem* system);
void UpdateExplosionSystem(ExplosionSystem* system, float deltaTime);
void DrawExplosions(const ExplosionSystem* system);

// Create different types of explosions
void CreateExplosion(ExplosionSystem* system, Vector2 position, ExplosionType type);
void CreateEnemyExplosion(ExplosionSystem* system, Vector2 position, Color enemyColor, float size);
void CreatePlayerExplosion(ExplosionSystem* system, Vector2 position);
void CreateShockwaveExplosion(ExplosionSystem* system, Vector2 position, float force);

// Screen shake
Vector2 GetScreenShakeOffset(const ExplosionSystem* system);
void TriggerScreenShake(ExplosionSystem* system, float intensity, float duration);

// Cleanup
void CleanupInactiveExplosions(ExplosionSystem* system);

#endif // EXPLOSION_H
