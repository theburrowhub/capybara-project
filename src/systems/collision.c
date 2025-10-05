#include "collision.h"
#include "constants.h"
#include "enemy_types.h"
#include "projectile_types.h"
#include "player_ship.h"
#include "explosion.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

void CheckBulletEnemyCollisions(Game* game) {
    // Use generic collision system
    CollisionContext ctx = {
        .bullets = game->bullets,
        .maxBullets = MAX_BULLETS,
        .enemies = game->enemies,
        .maxEnemies = MAX_ENEMIES,
        .explosionSystem = game->explosionSystem,
        .score = &game->score,
        .enemiesKilled = NULL,  // Game doesn't track this separately
        .logContext = game,
        .onEnemyHit = NULL,
        .onEnemyDestroyed = NULL
    };
    
    Collision_CheckBulletEnemyGeneric(&ctx);
}

void Collision_CheckBulletEnemyGeneric(CollisionContext* ctx) {
    for (int b = 0; b < ctx->maxBullets; b++) {
        if (ctx->bullets[b].active) {
            for (int e = 0; e < ctx->maxEnemies; e++) {
                if (ctx->enemies[e].active) {
                    // Skip ghost enemies when they're not visible
                    if (ctx->enemies[e].type == ENEMY_GHOST && !ctx->enemies[e].isVisible) {
                        continue;
                    }
                    
                    if (CheckCollisionRecs(ctx->bullets[b].bounds, ctx->enemies[e].bounds)) {
                        // Calculate damage with resistance
                        float baseDamage = 1.0f;
                        float effectiveDamage = baseDamage * (1.0f - ctx->enemies[e].resistance);
                        int damageDealt = (int)fmaxf(1.0f, effectiveDamage);  // At least 1 damage
                        
                        // Deactivate bullet
                        ctx->bullets[b].active = false;
                        
                        // Boss shield handling (if applicable)
                        if (ctx->enemies[e].type == ENEMY_BOSS && ctx->enemies[e].shieldAngle > 0) {
                            ctx->enemies[e].shieldAngle -= damageDealt;
                            ctx->enemies[e].specialTimer = 0;  // Reset shield regen timer
                            
                            if (ctx->enemies[e].shieldAngle <= 0) {
                                ctx->enemies[e].shieldAngle = 0;
                                // Log if logging is enabled
                                if (ctx->logContext) {
                                    Game* game = (Game*)ctx->logContext;
                                    LogEvent(game, "[%.2f] Boss shield BROKEN!", game->gameTime);
                                }
                            }
                        } else {
                            // Apply damage to health
                            ctx->enemies[e].health -= damageDealt;
                        }
                        
                        ctx->enemies[e].hitsTaken++;
                        
                        // Call hit callback if provided
                        if (ctx->onEnemyHit) {
                            ctx->onEnemyHit(ctx->logContext, &ctx->enemies[e], damageDealt);
                        }
                        
                        // Log hit if logging is enabled
                        if (ctx->logContext) {
                            Game* game = (Game*)ctx->logContext;
                            LogEvent(game, "[%.2f] Enemy hit - Type:%s ID:%d Health:%d/%d Hits:%d Damage:%d",
                                    game->gameTime, GetEnemyTypeName(ctx->enemies[e].type),
                                    ctx->enemies[e].id, ctx->enemies[e].health, ctx->enemies[e].maxHealth,
                                    ctx->enemies[e].hitsTaken, damageDealt);
                        }
                        
                        // Check if enemy is destroyed
                        if (ctx->enemies[e].health <= 0) {
                            // Create explosion if system is provided
                            if (ctx->explosionSystem) {
                                Color enemyColor = GetEnemyTypeColor(ctx->enemies[e].type);
                                float enemySize = ctx->enemies[e].bounds.width;
                                CreateEnemyExplosion(ctx->explosionSystem, ctx->enemies[e].position, 
                                                   enemyColor, enemySize);
                            }
                            
                            // Add score if score pointer is provided
                            if (ctx->score) {
                                *ctx->score += ctx->enemies[e].power * 2;
                            }
                            
                            // Track kills if counter is provided
                            if (ctx->enemiesKilled) {
                                (*ctx->enemiesKilled)++;
                            }
                            
                            // Deactivate enemy
                            ctx->enemies[e].active = false;
                            
                            // Call destroyed callback if provided
                            if (ctx->onEnemyDestroyed) {
                                ctx->onEnemyDestroyed(ctx->logContext, &ctx->enemies[e]);
                            }
                          
                            // Log destruction if logging is enabled
                            if (ctx->logContext) {
                                Game* game = (Game*)ctx->logContext;
                                
                                // Clear boss tracking if this was the boss
                                if (ctx->enemies[e].type == ENEMY_BOSS && game->bossEnemyIndex == e) {
                                    game->bossEnemyIndex = -1;
                                    LogEvent(game, "[%.2f] BOSS DEFEATED!", game->gameTime);
                                }
                                
                                LogEvent(game, "[%.2f] Enemy DESTROYED - Type:%s ID:%d TotalHits:%d Score:+%d",
                                        game->gameTime, GetEnemyTypeName(ctx->enemies[e].type),
                                        ctx->enemies[e].id, ctx->enemies[e].hitsTaken, 
                                        ctx->enemies[e].power * 2);
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
            // Skip ghost enemies when they're not visible
            if (game->enemies[e].type == ENEMY_GHOST && !game->enemies[e].isVisible) {
                continue;
            }
            
            if (CheckCollisionRecs(game->playerShip->bounds, game->enemies[e].bounds)) {
                LogCollision(game, "Enemy", game->enemies[e].id, 
                           game->playerShip->bounds, game->enemies[e].bounds);
                
                snprintf(game->deathCause, sizeof(game->deathCause), 
                        "Collision with %s Enemy #%d at position (%.0f, %.0f)", 
                        GetEnemyTypeName(game->enemies[e].type),
                        game->enemies[e].id, 
                        game->enemies[e].position.x, 
                        game->enemies[e].position.y);
                
                // Apply damage only if not invulnerable or shield burst is active
                if (!DEBUG_INVULNERABILITY && !game->playerShip->abilityActive[ABILITY_SHIELD_BURST]) {
                    // Damage based on enemy power
                    int damage = (game->enemies[e].power / 20) + 1;  // 1-3 damage based on power
                    DamagePlayerShip(game->playerShip, damage * 10);  // Scale up damage
                }
                
                // Destroy enemy on collision (except bosses)
                if (game->enemies[e].type != ENEMY_BOSS) {
                    // Create explosion for destroyed enemy
                    Color enemyColor = GetEnemyTypeColor(game->enemies[e].type);
                    float enemySize = game->enemies[e].bounds.width;
                    CreateEnemyExplosion(game->explosionSystem, game->enemies[e].position, enemyColor, enemySize);
                    
                    game->enemies[e].active = false;
                } else {
                    // Boss takes damage from collision
                    game->enemies[e].health -= 2;
                    if (game->enemies[e].health <= 0) {
                        // Large explosion for boss
                        CreateExplosion(game->explosionSystem, game->enemies[e].position, EXPLOSION_LARGE);
                        
                        game->enemies[e].active = false;
                        game->score += game->enemies[e].power * 10;  // Big score for boss
                    }
                }
            }
        }
    }
}


void CheckProjectilePlayerCollision(Game* game) {
    Projectile* projectiles = (Projectile*)game->projectiles;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active && !projectiles[i].isPlayerProjectile) {
            const ProjectileDefinition* def = GetProjectileDefinition(projectiles[i].type);
            
            // Check collision with player using circular collision
            float dx = projectiles[i].position.x - game->playerShip->position.x;
            float dy = projectiles[i].position.y - game->playerShip->position.y;
            float distance = sqrtf(dx * dx + dy * dy);
            float playerRadius = 25.0f;  // Approximate player ship radius
            
            if (distance < (def->hitboxRadius + playerRadius)) {
                // Apply damage only if not invulnerable or shield burst is active
                if (!DEBUG_INVULNERABILITY && !game->playerShip->abilityActive[ABILITY_SHIELD_BURST]) {
                    // Hit the player
                    int damage = fmaxf(1, def->damage / 2);  // Scale damage
                    DamagePlayerShip(game->playerShip, damage);
                }
                
                snprintf(game->deathCause, sizeof(game->deathCause), 
                        "Hit by %s projectile from Enemy #%d", 
                        def->name, projectiles[i].enemyId);
                
                LogEvent(game, "[%.2f] Player hit by %s - Damage:%d Health:%d",
                        game->gameTime, def->name, def->damage / 10, game->playerShip->health);
                
                // Destroy projectile unless it's piercing
                if (!def->piercing) {
                    projectiles[i].active = false;
                }
                
                // Handle explosion
                if (def->explosive && def->explosionRadius > 0) {
                    // Create plasma/energy explosion for explosive projectiles
                    CreateExplosion(game->explosionSystem, projectiles[i].position, EXPLOSION_PLASMA);
                }
            }
        }
    }
}

void CheckProjectileEnemyCollision(Game* game) {
    Projectile* projectiles = (Projectile*)game->projectiles;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active && projectiles[i].isPlayerProjectile) {
            const ProjectileDefinition* def = GetProjectileDefinition(projectiles[i].type);
            
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (game->enemies[e].active) {
                    // Skip ghost enemies when not visible
                    if (game->enemies[e].type == ENEMY_GHOST && !game->enemies[e].isVisible) {
                        continue;
                    }
                    
                    // Check circular collision
                    float dx = projectiles[i].position.x - game->enemies[e].position.x;
                    float dy = projectiles[i].position.y - game->enemies[e].position.y;
                    float distance = sqrtf(dx * dx + dy * dy);
                    
                    if (distance < (def->hitboxRadius + game->enemies[e].radius)) {
                        // Apply damage with resistance - always at least 1 damage
                        float baseDamage = (float)def->damage;
                        float effectiveDamage = baseDamage * (1.0f - game->enemies[e].resistance);
                        int damageDealt = (int)fmaxf(1.0f, effectiveDamage);  // Always deal at least 1 damage
                        
                        // Boss shield handling
                        if (game->enemies[e].type == ENEMY_BOSS && game->enemies[e].shieldAngle > 0) {
                            game->enemies[e].shieldAngle -= damageDealt;
                            game->enemies[e].specialTimer = 0;  // Reset shield regen timer
                            if (game->enemies[e].shieldAngle <= 0) {
                                game->enemies[e].shieldAngle = 0;
                                LogEvent(game, "[%.2f] Boss shield BROKEN!", game->gameTime);
                            }
                        } else {
                            game->enemies[e].health -= damageDealt;
                        }
                        game->enemies[e].hitsTaken++;
                        
                        LogEvent(game, "[%.2f] Enemy hit by %s - Type:%s ID:%d Health:%d/%d Hits:%d Damage:%d",
                                game->gameTime, def->name, GetEnemyTypeName(game->enemies[e].type),
                                game->enemies[e].id, game->enemies[e].health, game->enemies[e].maxHealth,
                                game->enemies[e].hitsTaken, damageDealt);
                        
                        if (game->enemies[e].health <= 0) {
                            // Create explosion effect based on enemy type
                            Color enemyColor = GetEnemyTypeColor(game->enemies[e].type);
                            float enemySize = game->enemies[e].bounds.width;
                            CreateEnemyExplosion(game->explosionSystem, game->enemies[e].position, enemyColor, enemySize);
                            
                            game->enemies[e].active = false;
                            game->score += game->enemies[e].power * 2;
                            
                            // Clear boss tracking if this was the boss
                            if (game->enemies[e].type == ENEMY_BOSS && game->bossEnemyIndex == e) {
                                game->bossEnemyIndex = -1;
                                LogEvent(game, "[%.2f] BOSS DEFEATED!", game->gameTime);
                            }
                            
                            LogEvent(game, "[%.2f] Enemy DESTROYED by %s - Type:%s ID:%d TotalHits:%d Score:+%d",
                                    game->gameTime, def->name, GetEnemyTypeName(game->enemies[e].type),
                                    game->enemies[e].id, game->enemies[e].hitsTaken, game->enemies[e].power * 2);
                        }
                        
                        // Destroy projectile unless piercing
                        if (!def->piercing) {
                            projectiles[i].active = false;
                            break;  // Stop checking other enemies for this projectile
                        }
                    }
                }
            }
        }
    }
}

void CheckCollisions(Game* game) {
    CheckBulletEnemyCollisions(game);
    CheckProjectileEnemyCollision(game);
    CheckProjectilePlayerCollision(game);
    CheckPlayerEnemyCollisions(game);
}
