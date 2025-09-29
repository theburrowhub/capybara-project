#include "game.h"
#include "constants.h"
#include "player.h"
#include "enemy.h"
#include "obstacle.h"
#include "weapon.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

void InitGame(Game* game) {
    // Initialize logger
    InitLogger(game);
    
    // Allocate memory for arrays
    game->bullets = (Bullet*)malloc(MAX_BULLETS * sizeof(Bullet));
    game->enemies = (Enemy*)malloc(MAX_ENEMIES * sizeof(Enemy));
    game->obstacles = (Obstacle*)malloc(MAX_OBSTACLES * sizeof(Obstacle));
    
    // Initialize player
    InitPlayer(&game->player);
    
    // Initialize bullets
    InitBullets(game->bullets);
    
    // Initialize enemies
    InitEnemies(game->enemies);
    
    // Initialize obstacles
    InitObstacles(game->obstacles);
    
    // Initialize game state
    game->score = 0;
    game->enemySpawnTimer = 0;
    game->obstacleSpawnTimer = 0;
    game->backgroundX = 0;
    game->gameOver = false;
    game->gameTime = 0.0f;
    game->scrollSpeed = BASE_SCROLL_SPEED;
    game->enemySpeed = BASE_ENEMY_SPEED;
    game->obstacleSpeed = BASE_OBSTACLE_SPEED;
    game->speedLevel = 1;
    game->nextEnemyId = 1;
    game->nextObstacleId = 1;
    strcpy(game->deathCause, "Alive");
}

void UpdateGameSpeed(Game* game) {
    int newSpeedLevel = (int)(game->gameTime / SPEED_UPDATE_INTERVAL) + 1;
    if (newSpeedLevel != game->speedLevel) {
        game->speedLevel = newSpeedLevel;
        game->scrollSpeed = BASE_SCROLL_SPEED + (SPEED_INCREMENT * (game->speedLevel - 1));
        game->enemySpeed = BASE_ENEMY_SPEED + (SPEED_INCREMENT * (game->speedLevel - 1));
        game->obstacleSpeed = BASE_OBSTACLE_SPEED + (SPEED_INCREMENT * (game->speedLevel - 1));
        
        // Update existing enemies speed
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (game->enemies[i].active) {
                game->enemies[i].speed = game->enemySpeed;
            }
        }
    }
}

void UpdateGame(Game* game) {
    if (!game->gameOver) {
        // Update game time
        float deltaTime = GetFrameTime();
        game->gameTime += deltaTime;
        
        // Update speed levels
        UpdateGameSpeed(game);
        
        // Update game components
        UpdatePlayer(game);
        UpdateBullets(game);
        UpdateEnemies(game);
        UpdateObstacles(game);
        
        // Background scroll with dynamic speed
        game->backgroundX -= game->scrollSpeed;
        if (game->backgroundX <= -SCREEN_WIDTH) {
            game->backgroundX = 0;
        }
        
        // Game over check
        if (game->player.health <= 0) {
            game->gameOver = true;
        }
    } else {
        // Restart game on R key press
        if (IsKeyPressed(KEY_R)) {
            CleanupGame(game);
            InitGame(game);
        }
    }
}

void CleanupGame(Game* game) {
    CloseLogger(game);
    
    // Free allocated memory
    if (game->bullets) {
        free(game->bullets);
        game->bullets = NULL;
    }
    if (game->enemies) {
        free(game->enemies);
        game->enemies = NULL;
    }
    if (game->obstacles) {
        free(game->obstacles);
        game->obstacles = NULL;
    }
}
