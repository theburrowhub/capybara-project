#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"
#include <stdbool.h>

// Forward declarations
typedef struct Game Game;

// Player structure
typedef struct Player {
    Vector2 position;
    Rectangle bounds;
    Color color;
    Texture2D sprite;     // Player sprite texture
    int health;
    float weaponHeat;     // Current weapon heat (0-100)
    float maxHeat;        // Maximum heat before overheat
    bool overheated;      // Is weapon overheated?
    float cooldownTime;   // Time remaining for cooldown
} Player;

// Bullet structure
typedef struct Bullet {
    Vector2 position;
    Rectangle bounds;
    bool active;
    float speed;
} Bullet;

// Enemy structure
typedef struct Enemy {
    Vector2 position;
    Rectangle bounds;
    Color color;
    bool active;
    float speed;
    int health;
    int id;               // Unique ID
    float radius;         // Circle radius
    float moveTimer;      // For random movement
    float targetY;        // Target Y position for movement
    float speedX;         // Individual X speed
    float speedY;         // Individual Y speed
} Enemy;

// Obstacle structure
typedef struct Obstacle {
    Vector2 position;
    Rectangle bounds;
    Color color;
    bool active;
    bool destructible;    // Can be destroyed by bullets
    int health;
    float width;
    float height;
    int id;               // Unique ID
} Obstacle;

// Main game state structure
struct Game {
    Player player;
    Bullet* bullets;
    Enemy* enemies;
    Obstacle* obstacles;
    int score;
    int enemySpawnTimer;
    int obstacleSpawnTimer;
    float backgroundX;
    bool gameOver;
    float gameTime;       // Time in seconds
    float scrollSpeed;
    float enemySpeed;
    float obstacleSpeed;
    int speedLevel;       // Current speed level (increases every minute)
    // Collision logging
    char deathCause[256];
    void* logFile;        // FILE* (using void* to avoid including stdio.h here)
    int nextEnemyId;
    int nextObstacleId;
};

#endif // TYPES_H
