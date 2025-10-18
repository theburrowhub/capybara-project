#include "combat_system.h"
#include "constants.h"
#include <math.h>

void Combat_FireEnemyProjectile(CombatContext* ctx, EnemyEx* enemy) {
    const EnemyWeaponConfig* config = GetEnemyWeaponConfig(enemy->type);
    Vector2 playerPos = ctx->playerPosition;
    Projectile* projectiles = ctx->projectiles;
    
    // Fire burst pattern
    for (int burst = 0; burst < config->burstCount; burst++) {
        float angle = 0;
        
        if (config->spreadAngle > 0 && config->burstCount > 1) {
            if (config->spreadAngle >= 360) {
                // Full circle spread
                angle = (360.0f / config->burstCount) * burst;
            } else {
                // Arc spread
                float startAngle = -config->spreadAngle / 2.0f;
                float angleStep = config->spreadAngle / (config->burstCount - 1);
                angle = startAngle + angleStep * burst;
            }
        }
        
        // Calculate target direction
        Vector2 direction = {
            playerPos.x - enemy->position.x,
            playerPos.y - enemy->position.y
        };
        float dist = sqrtf(direction.x * direction.x + direction.y * direction.y);
        if (dist > 0) {
            direction.x /= dist;
            direction.y /= dist;
        }
        
        // Apply angle rotation
        float radAngle = angle * DEG2RAD;
        float cosA = cosf(radAngle);
        float sinA = sinf(radAngle);
        Vector2 rotatedDir = {
            direction.x * cosA - direction.y * sinA,
            direction.x * sinA + direction.y * cosA
        };
        
        Vector2 target = {
            enemy->position.x + rotatedDir.x * 1000,
            enemy->position.y + rotatedDir.y * 1000
        };
        
        // Find free projectile slot
        for (int i = 0; i < ctx->maxProjectiles; i++) {
            if (!projectiles[i].active) {
                // Alternate between primary and secondary projectiles
                ProjectileType projType = (burst % 2 == 0) ? 
                    config->primaryProjectile : config->secondaryProjectile;
                
                InitializeProjectile(&projectiles[i], projType, 
                                   enemy->position, target, false);
                projectiles[i].enemyId = enemy->id;
                break;
            }
        }
    }
}

bool Combat_UpdateEnemyFiring(EnemyEx* enemy, CombatContext* ctx, float deltaTime,
                               float minX, float maxX, float minY, float maxY) {
    // Update firing timer
    enemy->fireTimer += deltaTime;
    
    // Check if enemy should fire
    const EnemyWeaponConfig* config = GetEnemyWeaponConfig(enemy->type);
    float fireInterval = 1.0f / config->fireRate;
    
    if (enemy->fireTimer >= fireInterval && enemy->can_fire) {
        // Only fire if enemy is on screen and in bounds
        if (enemy->position.x > minX && enemy->position.x < maxX &&
            enemy->position.y > minY && enemy->position.y < maxY) {
            
            // Ghost enemies only fire when visible
            if (enemy->type != ENEMY_GHOST || enemy->isVisible) {
                Combat_FireEnemyProjectile(ctx, enemy);
                enemy->fireTimer = 0;
                return true;
            }
        }
    }
    
    return false;
}
