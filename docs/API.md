# API Reference

## Core Modules

### game.h
Main game state management.

```c
void InitGame(Game* game);
// Initialize all game systems and state

void UpdateGame(Game* game);
// Update game logic, handle input, manage state transitions

void CleanupGame(Game* game);
// Free resources and close files

void UpdateGameSpeed(Game* game);
// Update difficulty/speed based on elapsed time
```

## Entity Modules

### player.h
Player character management.

```c
void InitPlayer(Player* player);
// Initialize player with default values

void UpdatePlayer(Game* game);
// Handle player input and movement

void DrawPlayer(const Player* player, bool showHitbox);
// Render player sprite and optional hitbox
```

### enemy.h
Enemy spawning and AI.

```c
void InitEnemies(Enemy* enemies);
// Initialize enemy array

void SpawnEnemy(Game* game);
// Create new enemy at edge of screen

void UpdateEnemies(Game* game);
// Update all active enemies, handle spawning

void DrawEnemies(const Enemy* enemies, bool showHitbox);
// Render all active enemies

int CountActiveEnemies(const Game* game);
// Return number of active enemies
```

### obstacle.h
Obstacle generation and management.

```c
void InitObstacles(Obstacle* obstacles);
// Initialize obstacle array

void SpawnObstacle(Game* game);
// Create new obstacle with random properties

void UpdateObstacles(Game* game);
// Update all active obstacles

void DrawObstacles(const Obstacle* obstacles, bool showHitbox);
// Render all active obstacles

int CountActiveObstacles(const Game* game);
// Return number of active obstacles
```

## System Modules

### weapon.h
Weapon and projectile management.

```c
void InitBullets(Bullet* bullets);
// Initialize bullet pool

void ShootBullet(Bullet* bullets, Vector2 position);
// Fire bullet from position

void UpdateWeaponHeat(Player* player, float deltaTime);
// Update weapon heat and cooldown

void UpdateBullets(Game* game);
// Update all bullets and handle shooting

void DrawBullets(const Bullet* bullets);
// Render all active bullets

void DrawWeaponHeatBar(const Player* player);
// Draw heat indicator UI
```

### collision.h
Collision detection system.

```c
void CheckCollisions(Game* game);
// Main collision check, calls all sub-checks

void CheckBulletEnemyCollisions(Game* game);
// Handle bullet vs enemy collisions

void CheckBulletObstacleCollisions(Game* game);
// Handle bullet vs obstacle collisions

void CheckPlayerEnemyCollisions(Game* game);
// Handle player vs enemy collisions

void CheckPlayerObstacleCollisions(Game* game);
// Handle player vs obstacle collisions
```

### renderer.h
Rendering and UI system.

```c
void DrawGame(Game* game);
// Main draw function, renders entire frame

void DrawBackground(float backgroundX);
// Draw scrolling star field

void DrawUI(const Game* game);
// Draw HUD elements (score, health, etc.)

void DrawGameOver(const Game* game);
// Draw game over screen

void DrawDebugInfo(const Game* game);
// Draw debug information
```

## Utility Modules

### utils.h
Logging and helper functions.

```c
void InitLogger(Game* game);
// Open log file for collision tracking

void CloseLogger(Game* game);
// Close log file

void LogCollision(Game* game, const char* type, int id, 
                  Rectangle obj1, Rectangle obj2);
// Log collision details

void LogPlayerMovement(Game* game, Vector2 oldPos, Vector2 newPos);
// Log player position changes

void LogEvent(Game* game, const char* format, ...);
// Generic event logging with printf-style formatting
```

## Data Structures

### types.h
Core data structures used throughout the game.

```c
typedef struct Player {
    Vector2 position;     // World position
    Rectangle bounds;     // Collision box
    Color color;         // Render color
    int health;          // Current health (0-3)
    float weaponHeat;    // Current heat (0-100)
    float maxHeat;       // Maximum heat
    bool overheated;     // Overheat state
    float cooldownTime;  // Cooldown remaining
} Player;

typedef struct Bullet {
    Vector2 position;    // World position
    Rectangle bounds;    // Collision box
    bool active;        // Is bullet in use
    float speed;        // Movement speed
} Bullet;

typedef struct Enemy {
    Vector2 position;    // World position
    Rectangle bounds;    // Collision box
    Color color;        // Render color
    bool active;        // Is enemy alive
    float speed;        // Base movement speed
    int health;         // Hit points
    int id;             // Unique identifier
    float radius;       // Visual radius
    float moveTimer;    // Movement pattern timer
    float targetY;      // Target Y for movement
    float speedX;       // X velocity
    float speedY;       // Y velocity
} Enemy;

typedef struct Obstacle {
    Vector2 position;    // World position
    Rectangle bounds;    // Collision box
    Color color;        // Render color
    bool active;        // Is obstacle active
    bool destructible;  // Can be destroyed
    int health;         // Hit points (if destructible)
    float width;        // Visual width
    float height;       // Visual height
    int id;             // Unique identifier
} Obstacle;

typedef struct Game {
    Player player;              // Player instance
    Bullet* bullets;           // Bullet pool
    Enemy* enemies;            // Enemy pool
    Obstacle* obstacles;       // Obstacle pool
    int score;                 // Current score
    int enemySpawnTimer;       // Enemy spawn countdown
    int obstacleSpawnTimer;    // Obstacle spawn countdown
    float backgroundX;         // Background scroll position
    bool gameOver;            // Game state
    float gameTime;           // Elapsed time
    float scrollSpeed;        // Background speed
    float enemySpeed;         // Enemy base speed
    float obstacleSpeed;      // Obstacle speed
    int speedLevel;           // Difficulty level
    char deathCause[256];     // Death reason string
    void* logFile;            // Log file handle
    int nextEnemyId;          // Enemy ID counter
    int nextObstacleId;       // Obstacle ID counter
} Game;
```

## Constants

### constants.h
Game configuration values.

```c
// Screen
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

// Object pools
#define MAX_BULLETS 50
#define MAX_ENEMIES 20
#define MAX_OBSTACLES 15

// Movement speeds
#define PLAYER_SPEED 5.0f
#define BULLET_SPEED 12.0f
#define BASE_ENEMY_SPEED 3.0f
#define BASE_SCROLL_SPEED 2.0f
#define BASE_OBSTACLE_SPEED 2.0f

// Difficulty
#define SPEED_INCREMENT 0.5f
#define SPEED_UPDATE_INTERVAL 20

// Weapon system
#define WEAPON_HEAT_PER_SHOT 8.0f
#define WEAPON_COOLDOWN_RATE 30.0f
#define WEAPON_OVERHEAT_TIME 3.0f
#define WEAPON_FIRE_RATE 0.12f
```
