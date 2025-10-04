#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <stdbool.h>

// Forward declarations
typedef struct Game Game;
typedef struct WaveSystem WaveSystem;
typedef struct EnemyEx EnemyEx;
typedef struct PlayerShip PlayerShip;
typedef struct ExplosionSystem ExplosionSystem;

// Bullet structure
typedef struct Bullet {
    Vector2 position;
    Rectangle bounds;
    bool active;
    float speed;
} Bullet;

// Note: Enemy structure has been replaced by EnemyEx in enemy_types.h

// Starfield for background
typedef struct {
    Vector2 position;
    float speed;
    int brightness;
} Star;

// Main game state structure
struct Game {
    PlayerShip* playerShip;   // Enhanced player ship
    Bullet* bullets;
    void* projectiles;        // Projectile* (using void* to avoid circular dependency)
    EnemyEx* enemies;         // Updated to use new enemy structure
    int score;
    float backgroundX;
    bool gameOver;
    bool gamePaused;          // Pause state
    float gameTime;           // Time in seconds
    float scrollSpeed;
    int speedLevel;           // Current speed level (increases every minute)
    // Wave system
    WaveSystem* waveSystem;
    // Explosion system
    ExplosionSystem* explosionSystem;
    // Starfield
    Star* stars;
    int numStars;
    // Collision logging
    char deathCause[256];
    void* logFile;            // FILE* (using void* to avoid including stdio.h here)
    int nextEnemyId;
    int nextProjectileId;
    // Audio
    Music backgroundMusic;
    bool musicLoaded;
};

#endif // TYPES_H
