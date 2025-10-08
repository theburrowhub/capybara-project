#ifndef CONSTANTS_H
#define CONSTANTS_H

// Screen dimensions
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

// HUD and play zone dimensions
#define TOP_HUD_HEIGHT 30           // Height of top HUD bar
#define BOTTOM_HUD_HEIGHT 100       // Height of bottom HUD
#define PLAY_ZONE_TOP TOP_HUD_HEIGHT // Top of play zone (30)
#define PLAY_ZONE_BOTTOM (SCREEN_HEIGHT - BOTTOM_HUD_HEIGHT)  // Bottom of play zone (500)
#define PLAY_ZONE_HEIGHT (PLAY_ZONE_BOTTOM - PLAY_ZONE_TOP)  // Effective play zone height (470)

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
#define WEAPON_FIRE_RATE 0.12f

// Weapon overheating (disabled - legacy code remains for compatibility)
#define WEAPON_OVERHEATING false
#define WEAPON_HEAT_PER_SHOT 8.0f
#define WEAPON_COOLDOWN_RATE 30.0f
#define WEAPON_OVERHEAT_TIME 3.0f

// Debug constants
#define DEBUG_LOGGING false          // Set to true to enable game.log file creation
#define DEBUG_INVULNERABILITY false  // Set to true to make player invulnerable
#define DEBUG_START_LEVEL 1          // Level to start at (1 = Level 1, 2 = Level 2, etc.)
#define DEBUG_START_PHASE 0          // Phase to start at (0 = normal start)
                                      // Tank Testing: Phase 3 (35s), 5 (85s), 11 (300s)
                                      // Boss Testing: Phase 12+ (345s+)

#endif // CONSTANTS_H
