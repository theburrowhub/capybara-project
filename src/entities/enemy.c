#include "enemy.h"
#include "constants.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

void InitEnemies(Enemy* enemies) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].speed = BASE_ENEMY_SPEED;
    }
}

void SpawnEnemy(Game* game) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) {
            game->enemies[i].position.x = SCREEN_WIDTH + 50;
            game->enemies[i].position.y = GetRandomValue(50, SCREEN_HEIGHT - 50);
            game->enemies[i].radius = GetRandomValue(15, 25);  // Random size
            game->enemies[i].bounds = (Rectangle){
                game->enemies[i].position.x - game->enemies[i].radius,
                game->enemies[i].position.y - game->enemies[i].radius,
                game->enemies[i].radius * 2,
                game->enemies[i].radius * 2
            };
            game->enemies[i].color = RED;
            game->enemies[i].active = true;
            game->enemies[i].health = 1;
            game->enemies[i].id = game->nextEnemyId++;
            game->enemies[i].speed = game->enemySpeed;
            game->enemies[i].moveTimer = 0;
            game->enemies[i].targetY = game->enemies[i].position.y;
            game->enemies[i].speedX = game->enemySpeed + GetRandomValue(-10, 10) * 0.1f;
            game->enemies[i].speedY = 0;
            
            LogEvent(game, "[%.2f] Enemy spawned - ID:%d Pos:(%.0f,%.0f) Radius:%.0f", 
                    game->gameTime, game->enemies[i].id, 
                    game->enemies[i].position.x, game->enemies[i].position.y,
                    game->enemies[i].radius);
            break;
        }
    }
}

void UpdateEnemies(Game* game) {
    // Spawn timer - reduce spawn interval as speed increases
    game->enemySpawnTimer++;
    int spawnInterval = 60 - (game->speedLevel - 1) * 5;  // Faster spawning at higher speeds
    if (spawnInterval < 20) spawnInterval = 20;  // Minimum spawn interval
    
    if (game->enemySpawnTimer >= spawnInterval) {
        SpawnEnemy(game);
        game->enemySpawnTimer = 0;
    }
    
    // Update enemies with random movement
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            // Update position
            game->enemies[i].position.x -= game->enemies[i].speedX;
            
            // Random movement pattern
            game->enemies[i].moveTimer += GetFrameTime();
            
            // Change direction randomly every 0.5-2 seconds
            if (game->enemies[i].moveTimer > GetRandomValue(5, 20) * 0.1f) {
                game->enemies[i].moveTimer = 0;
                game->enemies[i].targetY = GetRandomValue(50, SCREEN_HEIGHT - 50);
                
                // Occasionally do zigzag or spiral
                int pattern = GetRandomValue(0, 3);
                if (pattern == 0) {
                    // Zigzag
                    game->enemies[i].speedY = GetRandomValue(-3, 3);
                } else if (pattern == 1) {
                    // Dive towards player
                    float diffY = game->player.position.y - game->enemies[i].position.y;
                    game->enemies[i].speedY = (diffY > 0 ? 2 : -2);
                } else {
                    // Smooth movement to target
                    game->enemies[i].speedY = 0;
                }
            }
            
            // Apply vertical movement
            float diffY = game->enemies[i].targetY - game->enemies[i].position.y;
            if (fabs(diffY) > 5) {
                game->enemies[i].position.y += (diffY > 0 ? 1 : -1) * 2 + game->enemies[i].speedY;
            }
            
            // Add some wobble
            game->enemies[i].position.y += sinf(game->gameTime * 5 + i) * 1.5f;
            
            // Keep in bounds
            if (game->enemies[i].position.y < 30) game->enemies[i].position.y = 30;
            if (game->enemies[i].position.y > SCREEN_HEIGHT - 30) game->enemies[i].position.y = SCREEN_HEIGHT - 30;
            
            // Update bounds
            game->enemies[i].bounds.x = game->enemies[i].position.x - game->enemies[i].radius;
            game->enemies[i].bounds.y = game->enemies[i].position.y - game->enemies[i].radius;
            
            // Deactivate off-screen enemies
            if (game->enemies[i].position.x < -50) {
                game->enemies[i].active = false;
            }
        }
    }
}

void DrawEnemies(const Enemy* enemies, bool showHitbox) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Draw enemy as circle with gradient effect
            DrawCircle(enemies[i].position.x, enemies[i].position.y, 
                      enemies[i].radius, enemies[i].color);
            DrawCircle(enemies[i].position.x, enemies[i].position.y, 
                      enemies[i].radius * 0.7f, Fade(MAROON, 0.7f));
            DrawCircle(enemies[i].position.x - enemies[i].radius * 0.3f, 
                      enemies[i].position.y - enemies[i].radius * 0.3f, 
                      enemies[i].radius * 0.3f, Fade(WHITE, 0.3f));
            
            // Draw enemy ID
            DrawText(TextFormat("%d", enemies[i].id), 
                    enemies[i].position.x - 8, enemies[i].position.y - 8, 
                    15, WHITE);
            
            // Debug: Show hitbox when requested
            if (showHitbox) {
                DrawRectangleLines(
                    enemies[i].bounds.x, enemies[i].bounds.y,
                    enemies[i].bounds.width, enemies[i].bounds.height,
                    PINK
                );
            }
        }
    }
}

int CountActiveEnemies(const Game* game) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) count++;
    }
    return count;
}
