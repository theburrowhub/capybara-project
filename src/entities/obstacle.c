#include "obstacle.h"
#include "constants.h"
#include "utils.h"
#include <stdio.h>

void InitObstacles(Obstacle* obstacles) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = false;
    }
}

void SpawnObstacle(Game* game) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!game->obstacles[i].active) {
            game->obstacles[i].position.x = SCREEN_WIDTH + 50;
            // Procedural generation - random position and size
            game->obstacles[i].position.y = GetRandomValue(80, SCREEN_HEIGHT - 80);
            game->obstacles[i].width = GetRandomValue(30, 70);
            game->obstacles[i].height = GetRandomValue(30, 80);
            
            // 60% chance of being destructible
            game->obstacles[i].destructible = (GetRandomValue(1, 10) <= 6);
            
            if (game->obstacles[i].destructible) {
                game->obstacles[i].color = ORANGE;  // Destructible obstacles are orange
                game->obstacles[i].health = 2;      // Takes 2 hits to destroy
            } else {
                game->obstacles[i].color = GRAY;    // Indestructible obstacles are gray
                game->obstacles[i].health = 999;    // Cannot be destroyed
            }
            
            game->obstacles[i].bounds = (Rectangle){
                game->obstacles[i].position.x - game->obstacles[i].width/2,
                game->obstacles[i].position.y - game->obstacles[i].height/2,
                game->obstacles[i].width,
                game->obstacles[i].height
            };
            game->obstacles[i].active = true;
            game->obstacles[i].id = game->nextObstacleId++;
            
            LogEvent(game, "[%.2f] Obstacle spawned - ID:%d Type:%s Pos:(%.0f,%.0f) Size:(%.0fx%.0f)", 
                    game->gameTime, game->obstacles[i].id,
                    game->obstacles[i].destructible ? "Destructible" : "Indestructible",
                    game->obstacles[i].position.x, game->obstacles[i].position.y,
                    game->obstacles[i].width, game->obstacles[i].height);
            break;
        }
    }
}

void UpdateObstacles(Game* game) {
    // Spawn timer - reduce spawn interval as speed increases
    game->obstacleSpawnTimer++;
    int spawnInterval = 90 - (game->speedLevel - 1) * 10;  // Faster spawning at higher speeds
    if (spawnInterval < 30) spawnInterval = 30;  // Minimum spawn interval
    
    if (game->obstacleSpawnTimer >= spawnInterval) {
        SpawnObstacle(game);
        game->obstacleSpawnTimer = 0;
    }
    
    // Update obstacles with dynamic speed
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (game->obstacles[i].active) {
            game->obstacles[i].position.x -= game->obstacleSpeed;
            // Update BOTH x and y bounds to ensure they're properly aligned
            game->obstacles[i].bounds.x = game->obstacles[i].position.x - game->obstacles[i].width/2;
            game->obstacles[i].bounds.y = game->obstacles[i].position.y - game->obstacles[i].height/2;
            
            // Deactivate off-screen obstacles
            if (game->obstacles[i].position.x < -100) {
                game->obstacles[i].active = false;
            }
        }
    }
}

void DrawObstacles(const Obstacle* obstacles, bool showHitbox) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles[i].active) {
            // Draw main obstacle body
            DrawRectangle(
                obstacles[i].position.x - obstacles[i].width/2,
                obstacles[i].position.y - obstacles[i].height/2,
                obstacles[i].width,
                obstacles[i].height,
                obstacles[i].color
            );
            
            // Draw obstacle ID
            DrawText(TextFormat("#%d", obstacles[i].id), 
                    obstacles[i].position.x - 15, obstacles[i].position.y - 5,
                    18, BLACK);
            
            // Draw border to differentiate types
            if (obstacles[i].destructible) {
                // Destructible obstacles have dashed lines pattern
                for (int j = 0; j < 4; j++) {
                    DrawRectangleLines(
                        obstacles[i].position.x - obstacles[i].width/2 + j*2,
                        obstacles[i].position.y - obstacles[i].height/2 + j*2,
                        obstacles[i].width - j*4,
                        obstacles[i].height - j*4,
                        Fade(YELLOW, 0.5f)
                    );
                }
            } else {
                // Indestructible obstacles have solid thick border
                DrawRectangleLines(
                    obstacles[i].position.x - obstacles[i].width/2 - 2,
                    obstacles[i].position.y - obstacles[i].height/2 - 2,
                    obstacles[i].width + 4,
                    obstacles[i].height + 4,
                    DARKGRAY
                );
                DrawRectangleLines(
                    obstacles[i].position.x - obstacles[i].width/2,
                    obstacles[i].position.y - obstacles[i].height/2,
                    obstacles[i].width,
                    obstacles[i].height,
                    BLACK
                );
            }
            
            // Debug: Show hitbox when requested
            if (showHitbox) {
                DrawRectangleLines(
                    obstacles[i].bounds.x, obstacles[i].bounds.y,
                    obstacles[i].bounds.width, obstacles[i].bounds.height,
                    MAGENTA
                );
            }
        }
    }
}

int CountActiveObstacles(const Game* game) {
    int count = 0;
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (game->obstacles[i].active) count++;
    }
    return count;
}
