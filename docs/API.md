# API Reference

This document provides a comprehensive reference for all modules and their public interfaces.

## Table of Contents
- [Core Modules](#core-modules)
- [Entity Modules](#entity-modules)
- [System Modules](#system-modules)
- [Utility Modules](#utility-modules)
- [Data Structures](#data-structures)
- [Constants](#constants)

---

## Core Modules

### game.h
Main game state management and coordination.

```c
void InitGame(Game* game);
// Initialize all game systems and state
// - Allocates subsystems (player, enemies, powerups, etc.)
// - Loads enemy and projectile type definitions
// - Initializes audio analysis

void UpdateGame(Game* game, float deltaTime);
// Update game logic, handle input, manage state transitions
// - Updates all active systems
// - Checks collisions
// - Manages game state (paused, game over, etc.)

void DrawGame(const Game* game);
// Render entire game frame
// - Calls renderer system to draw all elements
// - Handles HUD rendering

void CleanupGame(Game* game);
// Free resources and cleanup
// - Frees all allocated subsystems
// - Closes log files
// - Releases audio resources

void HandleGameInput(Game* game);
// Process game-level input (pause, debug keys)
```

---

## Entity Modules

### player_ship.h
Player ship management, controls, and HUD.

```c
void InitPlayerShip(PlayerShip* ship);
// Initialize player ship with default values
// - Sets starting position, health, shield, energy
// - Initializes weapon and energy mode

void UpdatePlayerShip(PlayerShip* ship, float deltaTime);
// Handle player input and movement
// - Process WASD/Arrow keys for movement
// - Update shield regeneration
// - Update energy mode effects
// - Constrain to play zone boundaries

void DrawPlayerShip(const PlayerShip* ship);
// Render player ship with visual effects
// - Draw ship body
// - Draw shield effect
// - Draw energy mode indicators
// - Draw revive effect if active

void DrawShieldEffect(const PlayerShip* ship);
// Render shield visual effect around ship
// - Different colors for defensive/offensive mode
// - Enhanced visual when defensive mode active
// - Golden aura when revived

void DrawShipHUD(const PlayerShip* ship);
// Draw ship status HUD in bottom bar
// - Hull, shield, energy bars
// - Energy mode indicator
// - Weapon mode and power level
// - Damage per bullet indicator with multipliers
// - Charge indicator
// - Revive notification

void DamagePlayerShip(PlayerShip* ship, float damage);
// Apply damage to player ship
// - Damages shield first, then hull
// - Triggers revive if weapon powerups available
// - Handles death state

float CalculateDamagePerShot(const PlayerShip* ship);
// Calculate current damage per bullet
// - Base weapon mode damage
// - Weapon powerup multiplier
// - Offensive mode bonus
// Returns: Damage value per individual bullet

void ToggleEnergyMode(PlayerShip* ship);
// Switch between Offensive and Defensive mode
```

### enemy_types.h
Enemy type definitions, behaviors, and AI.

```c
void InitEnemyTypes(void);
// Initialize enemy type system
// - Load enemy data tables
// - Set up behavior functions

void DrawEnemyEx(const EnemyEx* enemy);
// Render enemy with type-specific visuals
// - Abstract geometric design or sprite
// - Visual effects based on type
// - Health bars for strong enemies

void UpdateEnemyBehavior(EnemyEx* enemy, float deltaTime, const Game* game);
// Update enemy AI and movement
// - Type-specific movement patterns
// - Firing behavior
// - State management

Color GetEnemyTypeColor(EnemyType type);
// Get primary color for enemy type
// Returns: Color for rendering

const char* GetEnemyTypeName(EnemyType type);
// Get human-readable name
// Returns: String name like "Grunt", "Boss", etc.

float GetEnemyMaxHealth(EnemyType type);
// Get maximum health for enemy type
// Returns: Health value

float GetEnemySpeed(EnemyType type);
// Get base movement speed
// Returns: Speed in pixels/second

int GetEnemyPower(EnemyType type);
// Get power rating (affects score)
// Returns: Power value
```

---

## System Modules

### weapon.h
Weapon system, firing modes, and heat management.

```c
void InitBullets(Bullet* bullets, int maxBullets);
// Initialize bullet pool
// - Set all bullets to inactive
// - Reset positions

void UpdateBullets(Game* game, float deltaTime);
// Update all active bullets
// - Movement physics
// - Handle firing input
// - Apply weapon power multipliers
// - Update heat management
// - Check boundaries

void DrawBullets(const Bullet* bullets, int maxBullets);
// Render all active bullets
// - Visual scaling based on power level
// - Glow effects
// - Trail effects

void ShootBulletsForMode(Game* game);
// Fire bullets based on current weapon mode
// - SINGLE: 1 bullet straight
// - DOUBLE: 2 bullets slight angle
// - SPREAD: 3 bullets wide angle
// - RAPID: 1 bullet (faster fire rate)
// - DUAL: 2 bullets parallel
// - CHARGE: Hold to charge, release for burst

void CycleWeaponMode(PlayerShip* ship);
// Switch to next weapon mode
// 1→2→3→4→5→6→1

bool CanShoot(const PlayerShip* ship);
// Check if weapon can fire
// - Not overheated
// - Fire rate cooldown elapsed
// Returns: true if can shoot

void UpdateWeaponHeat(PlayerShip* ship, float deltaTime);
// Update heat value and overheat state
// - Increase heat on shot
// - Cool down over time
// - Handle overheat timeout
```

### powerup.h
Powerup system with drop mechanics and collection.

```c
void InitPowerupSystem(PowerupSystem* system);
// Initialize powerup system
// - Clear all powerups
// - Reset counters

void UpdatePowerups(PowerupSystem* system, PlayerShip* player, float deltaTime);
// Update all active powerups
// - Movement and drift
// - Magnetic attraction to player
// - Lifetime and fade-out
// - Remove expired powerups

void DrawPowerups(const PowerupSystem* system);
// Render all active powerups
// - Type-specific visual design
// - Pulse and rotation animations
// - Glow effects
// - Magnet indicator

void DropPowerupFromEnemy(PowerupSystem* system, const EnemyEx* enemy);
// Drop powerup when enemy destroyed
// - Check enemy type drop rates
// - Random roll for powerup type
// - Spawn at enemy position

void CheckPowerupCollisions(PowerupSystem* system, PlayerShip* player, int* score);
// Check if player collected any powerups
// - Rectangle collision detection
// - Apply powerup effect
// - Add bonus score
// - Deactivate collected powerup

void CleanupPowerupSystem(PowerupSystem* system);
// Cleanup powerup system resources
```

### collision.h
Collision detection system.

```c
void CheckCollisions(Game* game);
// Main collision dispatcher
// - Calls all specific collision checks
// - Manages collision context

void CheckBulletEnemyCollisions(CollisionContext* ctx);
// Check player bullets vs enemies
// - Apply damage to enemies
// - Destroy bullets on hit
// - Award score
// - Drop powerups on enemy death
// - Create explosions

void CheckProjectilePlayerCollisions(CollisionContext* ctx);
// Check enemy projectiles vs player
// - Apply damage to player ship
// - Destroy projectiles on hit

void CheckPlayerEnemyCollisions(CollisionContext* ctx);
// Check player ship vs enemies (collision damage)
// - Apply damage to player
// - Apply damage to enemy

void CheckDevastatingAttack(CollisionContext* ctx);
// Check devastating attack projectiles vs enemies
// - High damage special attack
// - Spread pattern
```

### wave_system.h
Dynamic wave spawning and phase progression.

```c
void InitWaveSystem(WaveSystem* system);
// Initialize wave spawning system
// - Set up phase definitions
// - Load wave patterns

void UpdateWaveSystem(WaveSystem* system, Game* game, float deltaTime);
// Update wave system
// - Progress through phases
// - Spawn enemies based on bass intensity
// - Manage wave timers
// - Handle phase transitions

void DrawWaveInfo(const WaveSystem* system, float gameTime);
// Draw wave progress in top HUD
// - Phase name
// - Progress bar
// - Time remaining

const char* GetCurrentPhaseName(const WaveSystem* system);
// Get name of current phase
// Returns: Phase name string

float GetWaveProgress(const WaveSystem* system);
// Get progress through current wave (0.0 to 1.0)
// Returns: Progress percentage
```

### projectile_types.h
Projectile type definitions (Laser, Plasma, Missile, Energy Orb).

```c
void InitProjectileTypes(void);
// Initialize projectile type system
// - Load projectile definitions
// - Set up behavior data

void UpdateProjectiles(Projectile* projectiles, int maxProjectiles, 
                      const EnemyEx* enemies, float deltaTime);
// Update all enemy projectiles
// - Movement and physics
// - Homing behavior
// - Wave motion
// - Lifetime checks

void DrawProjectiles(const Projectile* projectiles, int maxProjectiles);
// Render all active projectiles
// - Type-specific visuals (beam, orb, missile, etc.)
// - Trail effects
// - Glow effects

const char* GetProjectileTypeName(ProjectileType type);
// Get human-readable projectile type name
// Returns: String like "Laser", "Plasma", etc.
```

### explosion.h
Visual explosion effects.

```c
void InitExplosionSystem(ExplosionSystem* system);
// Initialize explosion system
// - Clear explosion pool

void UpdateExplosions(ExplosionSystem* system, float deltaTime);
// Update all active explosions
// - Expand radius
// - Fade out
// - Particle animation

void DrawExplosions(const ExplosionSystem* system);
// Render all active explosions
// - Circle with expanding radius
// - Color based on source
// - Fade effect

void CreateEnemyExplosion(ExplosionSystem* system, Vector2 position, 
                         Color color, float size);
// Create explosion at enemy death position
// - Scales with enemy size
// - Uses enemy color

void CleanupExplosionSystem(ExplosionSystem* system);
// Cleanup explosion system resources
```

### renderer.h
Main rendering pipeline and HUD.

```c
void RenderGame(const Game* game);
// Main render function
// - Coordinates all drawing operations
// - Manages render layers
// - Handles HUD elements

void DrawTopHUD(const Game* game);
// Draw top HUD bar (0-30px)
// - Phase name
// - Wave progress bar
// - Game time
// - Enemy count
// - Debug indicators

void DrawBottomHUD(const Game* game);
// Draw bottom HUD bar (500-600px)
// - Ship status (via player_ship.c)
// - Score (large, centered)
// - Weapon info (via player_ship.c)
// - Controls reference

void DrawBackground(float scrollOffset);
// Draw scrolling star field background
// - Multiple layers for parallax
// - Animated stars

void DrawGameOver(const Game* game);
// Draw game over screen
// - Final score
// - Death cause
// - Restart instructions

void DrawPauseScreen(void);
// Draw pause overlay
// - Semi-transparent overlay
// - "PAUSED" text
```

### combat_system.h
Combat logic and damage calculation.

```c
void InitCombatSystem(CombatSystem* system);
// Initialize combat system

void ApplyDamageToEnemy(EnemyEx* enemy, float damage);
// Apply damage to enemy
// - Reduce health
// - Trigger death if health <= 0

bool IsEnemyDead(const EnemyEx* enemy);
// Check if enemy should be destroyed
// Returns: true if health <= 0
```

---

## Utility Modules

### utils.h / logger.c
Logging and debugging utilities.

```c
void InitLogger(const char* logFilePath);
// Open log file for game events
// - Creates/truncates log file
// - Writes timestamp header

void CloseLogger(void);
// Close log file
// - Flushes buffer
// - Closes file handle

void LogEvent(const char* format, ...);
// Log formatted event message
// - Printf-style formatting
// - Timestamp prefix
// - File and console output

void LogCollision(const char* type, int id1, int id2, 
                 Vector2 pos1, Vector2 pos2);
// Log collision event with details
// - Collision type
// - Entity IDs
// - Positions

void LogEnemySpawn(const char* enemyType, Vector2 position);
// Log enemy spawn event

void LogPowerupDrop(const char* powerupType, Vector2 position);
// Log powerup drop event
```

### audio_analysis.h
Audio analysis and bass detection.

```c
void InitAudioAnalysis(AudioAnalysisData* data, const char* audioFile);
// Initialize audio analysis system
// - Load audio file with BASS
// - Set up analysis parameters
// - Start playback

void UpdateAudioAnalysis(AudioAnalysisData* data);
// Update audio analysis
// - Analyze current audio frame
// - Detect bass intensity
// - Update smoothed values

BassIntensity GetBassIntensity(const AudioAnalysisData* data);
// Get current bass intensity level
// Returns: BASS_NONE, BASS_LOW, BASS_MEDIUM, or BASS_HIGH

float GetBassValue(const AudioAnalysisData* data);
// Get raw bass value (0.0 to 1.0)
// Returns: Bass intensity float

void CleanupAudioAnalysis(AudioAnalysisData* data);
// Cleanup audio resources
// - Stop playback
// - Free BASS resources
```

---

## Data Structures

### types.h
Core data structures used throughout the game.

```c
// Player ship structure
typedef struct PlayerShip {
    Vector2 position;           // World position
    Rectangle bounds;           // Collision box
    float health;              // Current hull (0-100)
    float maxHealth;           // Maximum hull
    float shield;              // Current shield (0-100)
    float maxShield;           // Maximum shield
    float energy;              // Current energy (0-100)
    float maxEnergy;           // Maximum energy
    EnergyMode energyMode;     // OFFENSIVE or DEFENSIVE
    WeaponMode weaponMode;     // Current weapon type
    int weaponPowerupCount;    // Weapon powerup level (0-3)
    float weaponHeat;          // Current heat (0-100)
    bool overheated;           // Overheat state
    float fireRateTimer;       // Cooldown between shots
    float chargeTime;          // Charge weapon timer
    bool justRevived;          // Revive animation flag
    float reviveEffectTimer;   // Revive effect duration
} PlayerShip;

// Enemy structure
typedef struct EnemyEx {
    Vector2 position;          // World position
    Rectangle bounds;          // Collision box
    EnemyType type;            // Enemy type enum
    float health;              // Current health
    float maxHealth;           // Maximum health
    bool active;               // Is enemy alive
    float moveTimer;           // Movement pattern timer
    float fireTimer;           // Shooting cooldown
    int power;                 // Power rating
    Color color;               // Render color
    // ... additional fields for behaviors
} EnemyEx;

// Bullet structure
typedef struct Bullet {
    Vector2 position;          // World position
    Rectangle bounds;          // Collision box
    bool active;               // Is bullet in use
    float damage;              // Damage value
    int powerLevel;            // Visual power level (0-3)
} Bullet;

// Powerup structure
typedef struct Powerup {
    Vector2 position;          // World position
    Rectangle bounds;          // Collision box
    PowerupType type;          // ENERGY, SHIELD, HULL, or WEAPON
    bool active;               // Is powerup spawned
    float lifetime;            // Time until despawn
    float rotation;            // Visual rotation
    float pulsePhase;          // Pulse animation
    int weaponLevel;           // For weapon powerups (1-3)
} Powerup;

// Projectile structure
typedef struct Projectile {
    Vector2 position;          // World position
    Rectangle bounds;          // Collision box
    ProjectileType type;       // LASER, PLASMA, MISSILE, or ENERGY_ORB
    bool active;               // Is projectile in flight
    float speed;               // Movement speed
    Vector2 velocity;          // Direction vector
    float lifetime;            // Time until despawn
    bool fromEnemy;            // Enemy projectile flag
} Projectile;

// Explosion structure
typedef struct Explosion {
    Vector2 position;          // Center position
    Color color;               // Explosion color
    float radius;              // Current radius
    float maxRadius;           // Maximum radius
    float lifetime;            // Time remaining
    bool active;               // Is explosion active
} Explosion;

// Game structure
typedef struct Game {
    PlayerShip* playerShip;         // Player instance
    EnemyEx* enemies;               // Enemy pool
    Bullet* bullets;                // Bullet pool
    PowerupSystem* powerupSystem;   // Powerup manager
    WaveSystem* waveSystem;         // Wave spawner
    ExplosionSystem* explosionSystem; // Explosion effects
    AudioAnalysisData* audioData;   // Audio analysis
    
    int score;                      // Current score
    bool gameOver;                  // Game state
    bool gamePaused;                // Pause state
    float gameTime;                 // Elapsed time
    char deathCause[256];           // Death reason
    FILE* logFile;                  // Debug log
    
    // Debug flags
    bool debugInvulnerable;
    bool debugShowHitboxes;
    bool debugNoEnemyFire;
} Game;
```

### Enums

```c
// Energy modes
typedef enum {
    ENERGY_MODE_OFFENSIVE,   // +100% damage when energy full
    ENERGY_MODE_DEFENSIVE    // +100% shield regen, no damage bonus
} EnergyMode;

// Weapon modes
typedef enum {
    WEAPON_MODE_SINGLE,      // 1 bullet, 3.0 base damage
    WEAPON_MODE_DOUBLE,      // 2 bullets, 1.5 base damage each
    WEAPON_MODE_SPREAD,      // 3 bullets, 1.0 base damage each
    WEAPON_MODE_RAPID,       // 1 bullet, 1.5 damage, 2x fire rate
    WEAPON_MODE_DUAL,        // 2 bullets parallel, 1.5 damage each
    WEAPON_MODE_CHARGE       // Hold to charge, 1.0 base damage
} WeaponMode;

// Enemy types
typedef enum {
    ENEMY_GRUNT,      // Basic enemy
    ENEMY_TANK,       // Slow, high health
    ENEMY_SPEEDER,    // Fast movement
    ENEMY_ZIGZAG,     // Zigzag pattern
    ENEMY_BOMBER,     // Area attacks
    ENEMY_SHIELD,     // Rotating shield
    ENEMY_SWARM,      // Small, numerous
    ENEMY_ELITE,      // Advanced AI
    ENEMY_GHOST,      // Phase in/out
    ENEMY_BOSS        // Boss encounter
} EnemyType;

// Powerup types
typedef enum {
    POWERUP_ENERGY,   // Restore 20% energy
    POWERUP_SHIELD,   // Restore 100% shield
    POWERUP_HULL,     // Restore 20% hull
    POWERUP_WEAPON    // Increase weapon level
} PowerupType;

// Projectile types
typedef enum {
    PROJECTILE_LASER,      // Fast, straight, low damage
    PROJECTILE_PLASMA,     // Medium speed, slight homing
    PROJECTILE_MISSILE,    // Strong homing, explosive
    PROJECTILE_ENERGY_ORB  // Slow, wave motion, piercing
} ProjectileType;

// Bass intensity levels
typedef enum {
    BASS_NONE,
    BASS_LOW,
    BASS_MEDIUM,
    BASS_HIGH
} BassIntensity;
```

---

## Constants

### constants.h
Game configuration values.

```c
// Screen dimensions
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

// HUD layout
#define TOP_HUD_HEIGHT 30
#define BOTTOM_HUD_HEIGHT 100
#define PLAY_ZONE_TOP 30
#define PLAY_ZONE_BOTTOM 500
#define PLAY_ZONE_HEIGHT 470

// Object pools
#define MAX_BULLETS 50
#define MAX_ENEMIES 50
#define MAX_PROJECTILES 100
#define MAX_POWERUPS 20
#define MAX_EXPLOSIONS 50

// Player ship
#define PLAYER_SPEED 300.0f
#define PLAYER_MAX_HEALTH 100.0f
#define PLAYER_MAX_SHIELD 100.0f
#define PLAYER_MAX_ENERGY 100.0f
#define SHIELD_REGEN_RATE 2.0f
#define SHIELD_REGEN_DELAY 3.0f

// Weapon system
#define WEAPON_HEAT_PER_SHOT 8.0f
#define WEAPON_COOLDOWN_RATE 30.0f
#define WEAPON_OVERHEAT_TIME 3.0f
#define WEAPON_FIRE_RATE 0.12f
#define BULLET_SPEED 600.0f

// Powerup system
#define POWERUP_MAX_LIFETIME 15.0f
#define POWERUP_MAGNET_RANGE 80.0f
#define POWERUP_MAGNET_SPEED 200.0f
#define ENERGY_RECOVERY_PERCENT 0.20f
#define HULL_RECOVERY_PERCENT 0.20f

// Wave system
#define WAVE_PHASE_COUNT 9
#define BOSS_SPAWN_TIME 427.0f
```

---

## Usage Examples

### Initialize a New Game
```c
Game game = {0};
InitGame(&game);

while (!WindowShouldClose() && !game.gameOver) {
    float deltaTime = GetFrameTime();
    
    UpdateGame(&game, deltaTime);
    DrawGame(&game);
}

CleanupGame(&game);
```

### Fire Player Weapon
```c
if (IsKeyDown(KEY_SPACE) && CanShoot(playerShip)) {
    ShootBulletsForMode(&game);
}
```

### Spawn Enemy
```c
EnemyEx* enemy = GetInactiveEnemy(game.enemies);
if (enemy) {
    enemy->type = ENEMY_GRUNT;
    enemy->position = (Vector2){SCREEN_WIDTH, PLAY_ZONE_TOP + 50};
    enemy->health = GetEnemyMaxHealth(ENEMY_GRUNT);
    enemy->active = true;
}
```

### Drop Powerup
```c
if (enemy->health <= 0) {
    DropPowerupFromEnemy(game.powerupSystem, enemy);
}
```

---

## See Also

- **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design and data flow
- **[WEAPONS_AND_DAMAGE.md](WEAPONS_AND_DAMAGE.md)** - Weapon and damage mechanics
- **[POWERUPS.md](POWERUPS.md)** - Powerup system details
- **[ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md)** - Enemy behaviors
- **[BUILD.md](BUILD.md)** - Compilation and build system