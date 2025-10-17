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
typedef struct PowerupSystem PowerupSystem;
typedef struct LevelManager LevelManager;
typedef struct InputManager InputManager;

// Bullet structure
typedef struct Bullet {
    Vector2 position;
    Rectangle bounds;
    bool active;
    float speed;
    float damage;       // Damage dealt by this bullet
    float velocityY;    // Y velocity for spread/dual shots
    int powerLevel;     // Weapon power level 0-3 (standard, +1, +2, +3)
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
    InputManager* inputManager; // Input manager for controls
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
    // Level system
    LevelManager* levelManager;
    // Wave system
    WaveSystem* waveSystem;
    // Explosion system
    ExplosionSystem* explosionSystem;
    // Powerup system
    PowerupSystem* powerupSystem;
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
    float musicVolume;
    // Boss tracking
    int bossEnemyIndex;       // Index of boss enemy in enemies array (-1 if none)
    float bossSpawnTime;      // Time when boss spawned (for countdown calculation)
    bool bossEscapeTriggered; // Whether boss escape sequence has been triggered
    float bossEscapeTimer;    // Timer for dramatic escape sequence
    int bossEscapePhase;      // Phase of escape: 0=not started, 1=destroying everything, 2=boss escaping, 3=show game over
    // Interlevel transition system
    bool showingLevelComplete; // Whether we're showing level complete overlay
    float levelCompleteTimer;  // Time since level complete overlay appeared
    bool transitioningToNextLevel; // Whether we're in the middle of transitioning
    float levelStartTime;      // Game time when current level started (for display)
    // Input debouncing
    bool justStarted;          // Prevent input on first frame after starting game
};

#endif // TYPES_H
