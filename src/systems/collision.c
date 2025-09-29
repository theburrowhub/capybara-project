#include "collision.h"
#include "constants.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void CheckBulletEnemyCollisions(Game* game) {
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (game->bullets[b].active) {
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (game->enemies[e].active) {
                    if (CheckCollisionRecs(game->bullets[b].bounds, game->enemies[e].bounds)) {
                        game->bullets[b].active = false;
                        game->enemies[e].health--;
                        
                        if (game->enemies[e].health <= 0) {
                            game->enemies[e].active = false;
                            game->score += 100;
                        }
                    }
                }
            }
        }
    }
}

void CheckBulletObstacleCollisions(Game* game) {
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (game->bullets[b].active) {
            for (int o = 0; o < MAX_OBSTACLES; o++) {
                if (game->obstacles[o].active) {
                    if (CheckCollisionRecs(game->bullets[b].bounds, game->obstacles[o].bounds)) {
                        game->bullets[b].active = false;
                        
                        // Only damage destructible obstacles
                        if (game->obstacles[o].destructible) {
                            game->obstacles[o].health--;
                            
                            // Change color when damaged
                            if (game->obstacles[o].health == 1) {
                                game->obstacles[o].color = Fade(ORANGE, 0.5f);
                            }
                            
                            if (game->obstacles[o].health <= 0) {
                                game->obstacles[o].active = false;
                                game->score += 50;  // Bonus points for destroying obstacles
                            }
                        }
                    }
                }
            }
        }
    }
}

void CheckPlayerEnemyCollisions(Game* game) {
    for (int e = 0; e < MAX_ENEMIES; e++) {
        if (game->enemies[e].active) {
            if (CheckCollisionRecs(game->player.bounds, game->enemies[e].bounds)) {
                LogCollision(game, "Enemy", game->enemies[e].id, 
                           game->player.bounds, game->enemies[e].bounds);
                
                snprintf(game->deathCause, sizeof(game->deathCause), 
                        "Collision with Enemy #%d at position (%.0f, %.0f)", 
                        game->enemies[e].id, 
                        game->enemies[e].position.x, 
                        game->enemies[e].position.y);
                
                game->enemies[e].active = false;
                game->player.health--;
            }
        }
    }
}

void CheckPlayerObstacleCollisions(Game* game) {
    for (int o = 0; o < MAX_OBSTACLES; o++) {
        if (game->obstacles[o].active) {
            // Add a small margin to avoid false positives
            Rectangle playerHitbox = {
                game->player.bounds.x + 3,
                game->player.bounds.y + 3,
                game->player.bounds.width - 6,
                game->player.bounds.height - 6
            };
            Rectangle obstacleHitbox = {
                game->obstacles[o].bounds.x + 3,
                game->obstacles[o].bounds.y + 3,
                game->obstacles[o].bounds.width - 6,
                game->obstacles[o].bounds.height - 6
            };
            
            // Log near misses for debugging
            float distX = fabs(game->player.position.x - game->obstacles[o].position.x);
            float distY = fabs(game->player.position.y - game->obstacles[o].position.y);
            
            if (distX < 200 && distY < 200 && game->logFile) {
                LogEvent(game, "[%.2f] NEAR MISS - Obstacle #%d Distance: X=%.1f Y=%.1f",
                        game->gameTime, game->obstacles[o].id, distX, distY);
            }
            
            if (CheckCollisionRecs(playerHitbox, obstacleHitbox)) {
                LogCollision(game, "Obstacle", game->obstacles[o].id, 
                           playerHitbox, obstacleHitbox);
                
                snprintf(game->deathCause, sizeof(game->deathCause), 
                        "Collision with %s Obstacle #%d at position (%.0f, %.0f) - Size: %.0fx%.0f",
                        game->obstacles[o].destructible ? "Destructible" : "Indestructible",
                        game->obstacles[o].id, 
                        game->obstacles[o].position.x, 
                        game->obstacles[o].position.y,
                        game->obstacles[o].width, 
                        game->obstacles[o].height);
                
                game->player.health = 0;  // Instant death
                game->gameOver = true;
            }
        }
    }
}

void CheckCollisions(Game* game) {
    CheckBulletEnemyCollisions(game);
    CheckBulletObstacleCollisions(game);
    CheckPlayerEnemyCollisions(game);
    CheckPlayerObstacleCollisions(game);
}
