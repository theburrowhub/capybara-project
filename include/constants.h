#ifndef CONSTANTS_H
#define CONSTANTS_H

// Screen dimensions
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

// Game object limits
#define MAX_BULLETS 50
#define MAX_ENEMIES 30  // Increased for wave system
#define MAX_PROJECTILES 200  // For enemy and player projectiles

// Movement and speed constants
#define PLAYER_SPEED 5.0f
#define BULLET_SPEED 12.0f
#define BASE_ENEMY_SPEED 3.0f
#define BASE_SCROLL_SPEED 2.0f
#define SPEED_INCREMENT 0.5f
#define SPEED_UPDATE_INTERVAL 20  // Seconds

// Weapon system
#define WEAPON_HEAT_PER_SHOT 8.0f
#define WEAPON_COOLDOWN_RATE 30.0f
#define WEAPON_OVERHEAT_TIME 3.0f
#define WEAPON_FIRE_RATE 0.12f
#define WEAPON_OVERHEATING false     // Enable/disable weapon overheating system (default: disabled)

// Boss constants
#define BOSS_SHIELD_HEALTH 100       // Boss shield health
#define BOSS_SHIELD_REGEN_TIME 5.0f  // Time in seconds before shield regenerates
#define BOSS_BASE_HEALTH 500         // Boss base health without shield

// Debug constants
#define DEBUG_LOGGING false          // Set to true to enable game.log file creation
#define DEBUG_INVULNERABILITY false  // Set to true to make player invulnerable
#define DEBUG_START_PHASE 0          // Phase to start at (0 = normal start)
                                      // Tank Testing: Phase 3 (35s), 5 (85s), 11 (300s)
                                      // Boss Testing: Phase 12+ (345s+)

#endif // CONSTANTS_H
