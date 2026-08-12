#include "projectile_types.h"
#include <math.h>
#include <string.h>

// Global projectile type definitions
ProjectileDefinition projectileTypes[PROJECTILE_TYPE_COUNT];
EnemyWeaponConfig enemyWeapons[ENEMY_TYPE_COUNT];

void InitProjectileTypes(void) {
    // LASER - Fast, straight, basic projectile
    projectileTypes[PROJECTILE_LASER] = (ProjectileDefinition){
        .type = PROJECTILE_LASER,
        .name = "Laser",
        .speed = 500.0f,
        .damage = 10,
        .size = 12.0f,
        .hitboxRadius = 4.0f,
        .primaryColor = (Color){255, 50, 50, 255},      // Red
        .secondaryColor = (Color){255, 150, 150, 200},  // Light red
        .glowColor = (Color){255, 200, 200, 100},       // White-red glow
        .pattern = PATTERN_STRAIGHT,
        .homingStrength = 0.0f,
        .lifetime = 3.0f,
        .piercing = false,
        .explosive = false,
        .explosionRadius = 0.0f
    };
    
    // PLASMA - Medium speed, slight homing
    projectileTypes[PROJECTILE_PLASMA] = (ProjectileDefinition){
        .type = PROJECTILE_PLASMA,
        .name = "Plasma",
        .speed = 300.0f,
        .damage = 20,
        .size = 16.0f,
        .hitboxRadius = 6.0f,
        .primaryColor = (Color){100, 200, 255, 255},    // Cyan
        .secondaryColor = (Color){150, 220, 255, 200},  // Light cyan
        .glowColor = (Color){200, 240, 255, 150},       // Bright cyan glow
        .pattern = PATTERN_HOMING,
        .homingStrength = 0.3f,
        .lifetime = 4.0f,
        .piercing = false,
        .explosive = false,
        .explosionRadius = 0.0f
    };
    
    // MISSILE - Medium-fast, high damage, strong homing
    projectileTypes[PROJECTILE_MISSILE] = (ProjectileDefinition){
        .type = PROJECTILE_MISSILE,
        .name = "Missile",
        .speed = 280.0f,  // Balanced speed - faster than original 200, slower than 380
        .damage = 50,
        .size = 20.0f,
        .hitboxRadius = 8.0f,
        .primaryColor = (Color){100, 100, 100, 255},    // Gray
        .secondaryColor = (Color){255, 150, 0, 255},    // Orange (exhaust)
        .glowColor = (Color){255, 200, 0, 150},         // Yellow-orange glow
        .pattern = PATTERN_HOMING,
        .homingStrength = 0.7f,
        .lifetime = 999.0f,  // Long lifetime to reach screen edge
        .piercing = false,
        .explosive = true,
        .explosionRadius = 30.0f
    };
    
    // ENERGY ORB - Slow, area damage, infinite range
    projectileTypes[PROJECTILE_ENERGY_ORB] = (ProjectileDefinition){
        .type = PROJECTILE_ENERGY_ORB,
        .name = "Energy Orb",
        .speed = 150.0f,
        .damage = 35,
        .size = 24.0f,
        .hitboxRadius = 10.0f,
        .primaryColor = (Color){200, 50, 255, 255},     // Purple
        .secondaryColor = (Color){150, 100, 255, 200},  // Light purple
        .glowColor = (Color){255, 150, 255, 150},       // Pink glow
        .pattern = PATTERN_WAVE,
        .homingStrength = 0.0f,
        .lifetime = 999.0f,  // Effectively infinite - will despawn at screen edge instead
        .piercing = true,
        .explosive = true,
        .explosionRadius = 40.0f
    };
    
    // PLAYER BULLET - Player's basic projectile with damage = 1
    projectileTypes[PROJECTILE_PLAYER_BULLET] = (ProjectileDefinition){
        .type = PROJECTILE_PLAYER_BULLET,
        .name = "Player Bullet",
        .speed = 600.0f,  // Fast bullets for responsive gameplay
        .damage = 1,      // Exactly 1 damage as required
        .size = 8.0f,     // Small bullet
        .hitboxRadius = 3.0f,
        .primaryColor = (Color){255, 255, 0, 255},      // Yellow
        .secondaryColor = (Color){255, 200, 0, 200},    // Orange-yellow
        .glowColor = (Color){255, 255, 150, 100},       // Light yellow glow
        .pattern = PATTERN_STRAIGHT,
        .homingStrength = 0.0f,
        .lifetime = 2.0f,
        .piercing = false,
        .explosive = false,
        .explosionRadius = 0.0f
    };
    
    // Map projectiles to enemy types
    // GRUNT - Basic laser shots
    enemyWeapons[ENEMY_GRUNT] = (EnemyWeaponConfig){
        .enemyType = ENEMY_GRUNT,
        .primaryProjectile = PROJECTILE_LASER,
        .secondaryProjectile = PROJECTILE_LASER,
        .fireRate = 1.0f,
        .burstCount = 1,
        .spreadAngle = 0.0f
    };
    
    // TANK - Powerful missile barrage
    enemyWeapons[ENEMY_TANK] = (EnemyWeaponConfig){
        .enemyType = ENEMY_TANK,
        .primaryProjectile = PROJECTILE_MISSILE,
        .secondaryProjectile = PROJECTILE_ENERGY_ORB,
        .fireRate = 0.6f,   // Reduced from 0.75 for better balance
        .burstCount = 3,    // Reduced from 4 to 3 missiles (less overwhelming)
        .spreadAngle = 20.0f  // Reduced spread from 25° to 20°
    };
    
    // SPEEDER - Rapid laser fire
    enemyWeapons[ENEMY_SPEEDER] = (EnemyWeaponConfig){
        .enemyType = ENEMY_SPEEDER,
        .primaryProjectile = PROJECTILE_LASER,
        .secondaryProjectile = PROJECTILE_LASER,
        .fireRate = 3.0f,
        .burstCount = 3,
        .spreadAngle = 15.0f
    };
    
    // ZIGZAG - Plasma shots with spread
    enemyWeapons[ENEMY_ZIGZAG] = (EnemyWeaponConfig){
        .enemyType = ENEMY_ZIGZAG,
        .primaryProjectile = PROJECTILE_PLASMA,
        .secondaryProjectile = PROJECTILE_LASER,
        .fireRate = 1.5f,
        .burstCount = 2,
        .spreadAngle = 30.0f
    };
    
    // BOMBER - Energy orbs
    enemyWeapons[ENEMY_BOMBER] = (EnemyWeaponConfig){
        .enemyType = ENEMY_BOMBER,
        .primaryProjectile = PROJECTILE_ENERGY_ORB,
        .secondaryProjectile = PROJECTILE_MISSILE,
        .fireRate = 0.8f,
        .burstCount = 1,
        .spreadAngle = 0.0f
    };
    
    // SHIELD - Defensive plasma bursts
    enemyWeapons[ENEMY_SHIELD] = (EnemyWeaponConfig){
        .enemyType = ENEMY_SHIELD,
        .primaryProjectile = PROJECTILE_PLASMA,
        .secondaryProjectile = PROJECTILE_PLASMA,
        .fireRate = 1.2f,
        .burstCount = 6,  // Fires in all directions
        .spreadAngle = 360.0f
    };
    
    // SWARM - Rapid weak lasers
    enemyWeapons[ENEMY_SWARM] = (EnemyWeaponConfig){
        .enemyType = ENEMY_SWARM,
        .primaryProjectile = PROJECTILE_LASER,
        .secondaryProjectile = PROJECTILE_LASER,
        .fireRate = 2.0f,
        .burstCount = 1,
        .spreadAngle = 0.0f
    };
    
    // ELITE - Mixed arsenal
    enemyWeapons[ENEMY_ELITE] = (EnemyWeaponConfig){
        .enemyType = ENEMY_ELITE,
        .primaryProjectile = PROJECTILE_PLASMA,
        .secondaryProjectile = PROJECTILE_MISSILE,
        .fireRate = 2.0f,
        .burstCount = 4,
        .spreadAngle = 45.0f
    };
    
    // GHOST - Phasing plasma shots
    enemyWeapons[ENEMY_GHOST] = (EnemyWeaponConfig){
        .enemyType = ENEMY_GHOST,
        .primaryProjectile = PROJECTILE_PLASMA,
        .secondaryProjectile = PROJECTILE_ENERGY_ORB,
        .fireRate = 1.0f,
        .burstCount = 2,
        .spreadAngle = 20.0f
    };
    
    // BOSS - Everything!
    enemyWeapons[ENEMY_BOSS] = (EnemyWeaponConfig){
        .enemyType = ENEMY_BOSS,
        .primaryProjectile = PROJECTILE_MISSILE,
        .secondaryProjectile = PROJECTILE_ENERGY_ORB,
        .fireRate = 2.5f,
        .burstCount = 8,
        .spreadAngle = 360.0f
    };
}

const ProjectileDefinition* GetProjectileDefinition(ProjectileType type) {
    if (type < 0 || type >= PROJECTILE_TYPE_COUNT) {
        return &projectileTypes[PROJECTILE_LASER];  // Default to laser
    }
    return &projectileTypes[type];
}

void InitializeProjectile(Projectile* projectile, ProjectileType type, Vector2 position, Vector2 target, bool isPlayer) {
    const ProjectileDefinition* def = GetProjectileDefinition(type);
    
    projectile->type = type;
    projectile->position = position;
    projectile->active = true;
    projectile->lifetime = def->lifetime;
    projectile->animationTimer = 0;
    projectile->damage = def->damage;
    projectile->isPlayerProjectile = isPlayer;
    projectile->target = target;
    
    // Calculate initial velocity based on pattern
    Vector2 direction = {
        target.x - position.x,
        target.y - position.y
    };
    float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }
    
    projectile->velocity.x = direction.x * def->speed;
    projectile->velocity.y = direction.y * def->speed;
    projectile->baseAngle = atan2f(direction.y, direction.x);
    
    // Initialize pattern-specific properties
    projectile->waveOffset = 0;
    projectile->spiralAngle = 0;
    projectile->rotation = projectile->baseAngle * RAD2DEG;
    projectile->scale = 1.0f;
    projectile->tint = WHITE;
    projectile->trailLength = 0;
    
    // Initialize trail
    for (int i = 0; i < 10; i++) {
        projectile->trailPositions[i] = position;
    }
}

void UpdateProjectile(Projectile* projectile, float deltaTime) {
    if (!projectile->active) return;
    
    const ProjectileDefinition* def = GetProjectileDefinition(projectile->type);
    
    // Update lifetime
    projectile->lifetime -= deltaTime;
    if (projectile->lifetime <= 0) {
        projectile->active = false;
        return;
    }
    
    // Update animation timer
    projectile->animationTimer += deltaTime;
    
    // Update trail
    for (int i = 9; i > 0; i--) {
        projectile->trailPositions[i] = projectile->trailPositions[i-1];
    }
    projectile->trailPositions[0] = projectile->position;
    
    // Update movement based on pattern
    switch (def->pattern) {
        case PATTERN_STRAIGHT:
            // Simple straight movement
            projectile->position.x += projectile->velocity.x * deltaTime;
            projectile->position.y += projectile->velocity.y * deltaTime;
            break;
            
        case PATTERN_HOMING:
            // Track target with homing strength
            if (def->homingStrength > 0) {
                Vector2 toTarget = {
                    projectile->target.x - projectile->position.x,
                    projectile->target.y - projectile->position.y
                };
                float dist = sqrtf(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
                if (dist > 0) {
                    toTarget.x /= dist;
                    toTarget.y /= dist;
                    
                    // Blend current velocity with target direction
                    projectile->velocity.x += toTarget.x * def->homingStrength * def->speed * deltaTime;
                    projectile->velocity.y += toTarget.y * def->homingStrength * def->speed * deltaTime;
                    
                    // Normalize velocity to maintain speed
                    float velLength = sqrtf(projectile->velocity.x * projectile->velocity.x + 
                                          projectile->velocity.y * projectile->velocity.y);
                    if (velLength > 0) {
                        projectile->velocity.x = (projectile->velocity.x / velLength) * def->speed;
                        projectile->velocity.y = (projectile->velocity.y / velLength) * def->speed;
                    }
                }
            }
            projectile->position.x += projectile->velocity.x * deltaTime;
            projectile->position.y += projectile->velocity.y * deltaTime;
            projectile->rotation = atan2f(projectile->velocity.y, projectile->velocity.x) * RAD2DEG;
            break;
            
        case PATTERN_WAVE:
            // Sine wave movement
            projectile->waveOffset += deltaTime * 3.0f;
            {
                float waveAmount = sinf(projectile->waveOffset) * 50.0f;
                float perpX = -sinf(projectile->baseAngle);
                float perpY = cosf(projectile->baseAngle);
                
                projectile->position.x += projectile->velocity.x * deltaTime + perpX * waveAmount * deltaTime;
                projectile->position.y += projectile->velocity.y * deltaTime + perpY * waveAmount * deltaTime;
            }
            break;
            
        case PATTERN_SPIRAL:
            // Spiral movement
            projectile->spiralAngle += deltaTime * 5.0f;
            {
                float spiralRadius = projectile->animationTimer * 20.0f;
                float spiralX = cosf(projectile->spiralAngle) * spiralRadius;
                float spiralY = sinf(projectile->spiralAngle) * spiralRadius;
                
                projectile->position.x += projectile->velocity.x * deltaTime + spiralX * deltaTime;
                projectile->position.y += projectile->velocity.y * deltaTime + spiralY * deltaTime;
            }
            break;
            
        case PATTERN_SPREAD:
            // Spreads out over distance
            {
                float spread = projectile->animationTimer * 0.5f;
                projectile->velocity.x *= (1.0f + spread * deltaTime);
                projectile->velocity.y *= (1.0f + spread * deltaTime);
                
                projectile->position.x += projectile->velocity.x * deltaTime;
                projectile->position.y += projectile->velocity.y * deltaTime;
            }
            break;
    }
    
    // Update visual effects
    if (projectile->type == PROJECTILE_MISSILE) {
        // Missile rotation based on velocity
        projectile->rotation = atan2f(projectile->velocity.y, projectile->velocity.x) * RAD2DEG;
    } else if (projectile->type == PROJECTILE_ENERGY_ORB) {
        // Pulsing effect
        projectile->scale = 1.0f + sinf(projectile->animationTimer * 5.0f) * 0.2f;
    }
}

void DrawProjectile(const Projectile* projectile) {
    if (!projectile->active) return;
    
    const ProjectileDefinition* def = GetProjectileDefinition(projectile->type);
    
    // Draw trail
    for (int i = 1; i < 10; i++) {
        float alpha = (1.0f - (i / 10.0f)) * 0.5f;
        Color trailColor = Fade(def->secondaryColor, alpha);
        float trailSize = def->size * 0.5f * (1.0f - (i / 10.0f));
        DrawCircleV(projectile->trailPositions[i], trailSize, trailColor);
    }
    
    // Draw projectile based on type
    switch (projectile->type) {
        case PROJECTILE_LASER:
            // Laser beam appearance
            {
                float beamLength = def->size * 2;
                float beamWidth = def->size * 0.3f;
                Vector2 endPos = {
                    projectile->position.x - cosf(projectile->rotation * DEG2RAD) * beamLength,
                    projectile->position.y - sinf(projectile->rotation * DEG2RAD) * beamLength
                };
                DrawLineEx(projectile->position, endPos, beamWidth, def->primaryColor);
                DrawLineEx(projectile->position, endPos, beamWidth * 0.5f, def->secondaryColor);
                DrawCircleV(projectile->position, def->size * 0.4f, def->glowColor);
            }
            break;
            
        case PROJECTILE_PLASMA:
            // Plasma ball
            DrawCircleV(projectile->position, def->size * projectile->scale, def->primaryColor);
            DrawCircleV(projectile->position, def->size * 0.7f * projectile->scale, def->secondaryColor);
            DrawCircleV(projectile->position, def->size * 0.4f * projectile->scale, def->glowColor);
            // Electric effect
            for (int i = 0; i < 3; i++) {
                float angle = projectile->animationTimer * 10 + i * 120;
                Vector2 sparkEnd = {
                    projectile->position.x + cosf(angle) * def->size * 1.5f,
                    projectile->position.y + sinf(angle) * def->size * 1.5f
                };
                DrawLineEx(projectile->position, sparkEnd, 1, Fade(def->glowColor, 0.5f));
            }
            break;
            
        case PROJECTILE_MISSILE:
            // Missile shape
            {
                // Missile body
                Vector2 front = projectile->position;
                Vector2 back = {
                    projectile->position.x - cosf(projectile->rotation * DEG2RAD) * def->size,
                    projectile->position.y - sinf(projectile->rotation * DEG2RAD) * def->size
                };
                DrawLineEx(front, back, def->size * 0.4f, def->primaryColor);
                
                // Warhead
                DrawCircleV(front, def->size * 0.3f, DARKGRAY);
                
                // Fins
                float finAngle = projectile->rotation * DEG2RAD;
                Vector2 fin1 = {
                    back.x + cosf(finAngle + PI/2) * def->size * 0.3f,
                    back.y + sinf(finAngle + PI/2) * def->size * 0.3f
                };
                Vector2 fin2 = {
                    back.x + cosf(finAngle - PI/2) * def->size * 0.3f,
                    back.y + sinf(finAngle - PI/2) * def->size * 0.3f
                };
                DrawTriangle(back, fin1, front, def->primaryColor);
                DrawTriangle(back, fin2, front, def->primaryColor);
                
                // Exhaust
                DrawCircleV(back, def->size * 0.4f, def->secondaryColor);
                DrawCircleV(back, def->size * 0.3f, def->glowColor);
            }
            break;
            
        case PROJECTILE_ENERGY_ORB:
            // Energy orb with rings
            DrawCircleV(projectile->position, def->size * projectile->scale, def->primaryColor);
            DrawCircleV(projectile->position, def->size * 0.8f * projectile->scale, def->secondaryColor);
            DrawCircleV(projectile->position, def->size * 0.5f * projectile->scale, def->glowColor);
            // Rotating rings
            for (int i = 0; i < 2; i++) {
                float ringAngle = projectile->animationTimer * 2 + i * PI/2;
                DrawRing(projectile->position, 
                        def->size * 0.9f * projectile->scale,
                        def->size * 1.1f * projectile->scale,
                        ringAngle * RAD2DEG, 
                        (ringAngle + PI) * RAD2DEG, 
                        32, Fade(def->glowColor, 0.5f));
            }
            break;
            
        case PROJECTILE_PLAYER_BULLET:
            // Simple circular bullet with glow effect
            DrawCircleV(projectile->position, def->size * projectile->scale, def->primaryColor);
            DrawCircleV(projectile->position, def->size * 0.6f * projectile->scale, def->secondaryColor);
            // Small glow
            DrawCircleV(projectile->position, def->size * 1.5f * projectile->scale, 
                       Fade(def->glowColor, 0.3f));
            break;
    }
    
    // Draw glow effect
    DrawCircleV(projectile->position, def->size * 1.5f, Fade(def->glowColor, 0.2f));
}

const EnemyWeaponConfig* GetEnemyWeaponConfig(EnemyType enemyType) {
    if (enemyType < 0 || enemyType >= ENEMY_TYPE_COUNT) {
        return &enemyWeapons[ENEMY_GRUNT];  // Default to grunt
    }
    return &enemyWeapons[enemyType];
}

ProjectileType GetEnemyPrimaryProjectile(EnemyType enemyType) {
    return GetEnemyWeaponConfig(enemyType)->primaryProjectile;
}

ProjectileType GetEnemySecondaryProjectile(EnemyType enemyType) {
    return GetEnemyWeaponConfig(enemyType)->secondaryProjectile;
}

