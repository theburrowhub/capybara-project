#ifndef CONSTANTS_H
#define CONSTANTS_H

// Screen dimensions
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600

// Game object limits
#define MAX_BULLETS 50
#define MAX_ENEMIES 20
#define MAX_OBSTACLES 15

// Movement and speed constants
#define PLAYER_SPEED 5.0f
#define BULLET_SPEED 12.0f
#define BASE_ENEMY_SPEED 3.0f
#define BASE_SCROLL_SPEED 2.0f
#define BASE_OBSTACLE_SPEED 2.0f
#define SPEED_INCREMENT 0.5f
#define SPEED_UPDATE_INTERVAL 20  // Seconds

// Weapon system
#define WEAPON_HEAT_PER_SHOT 8.0f
#define WEAPON_COOLDOWN_RATE 30.0f
#define WEAPON_OVERHEAT_TIME 3.0f
#define WEAPON_FIRE_RATE 0.12f

#endif // CONSTANTS_H
