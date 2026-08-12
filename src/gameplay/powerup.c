#include "powerup.h"
#include "constants.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// Powerup configuration constants
#define POWERUP_SIZE 12.0f          // Reduced from 20 to 12
#define POWERUP_FALL_SPEED 120.0f   // Increased from 50 to 120 (faster movement)
#define POWERUP_MAX_LIFETIME 10.0f  // Reduced from 15 to 10 seconds (faster despawn)
#define POWERUP_MAGNET_RANGE 80.0f
#define POWERUP_MAGNET_SPEED 200.0f
#define POWERUP_COLLECT_SCORE 50    // Bonus score for collecting powerup

// Recovery amounts
#define ENERGY_RECOVERY_PERCENT 0.20f   // 20%
#define HULL_RECOVERY_PERCENT 0.20f     // 20%
#define SHIELD_RECOVERY_FULL 1.0f       // 100%

// Drop rate tables for each enemy type
static const PowerupDropRate DROP_RATES[] = {
    // ENEMY_GRUNT - Common weak enemy: high weapon drop for progression
    [ENEMY_GRUNT] = {
        .energyChance = 10.0f,       // Reduced to make room for weapons
        .shieldChance = 4.0f,
        .hullChance = 0.5f,
        .weaponChance = 12.0f,       // Increased from 5% to 12% (2.4x increase!)
        .nothingChance = 73.5f
    },
    
    // ENEMY_TANK - Tough enemy: good drop rates for all items
    [ENEMY_TANK] = {
        .energyChance = 20.0f,
        .shieldChance = 20.0f,
        .hullChance = 8.0f,
        .weaponChance = 20.0f,       // Increased from 12% to 20%
        .nothingChance = 32.0f
    },
    
    // ENEMY_SPEEDER - Fast enemy: good weapon drops
    [ENEMY_SPEEDER] = {
        .energyChance = 8.0f,        // Reduced to make room for weapons
        .shieldChance = 7.0f,
        .hullChance = 1.0f,
        .weaponChance = 15.0f,       // Increased from 9% to 15% (1.67x increase!)
        .nothingChance = 69.0f
    },
    
    // ENEMY_ZIGZAG - Medium enemy: balanced drops
    [ENEMY_ZIGZAG] = {
        .energyChance = 12.0f,
        .shieldChance = 10.0f,
        .hullChance = 2.5f,
        .weaponChance = 12.0f,       // Increased from 6.5% to 12%
        .nothingChance = 63.5f
    },
    
    // ENEMY_BOMBER - Large enemy: good shield and weapon drops
    [ENEMY_BOMBER] = {
        .energyChance = 15.0f,
        .shieldChance = 15.0f,
        .hullChance = 5.0f,
        .weaponChance = 18.0f,       // Increased from 12% to 18%
        .nothingChance = 47.0f
    },
    
    // ENEMY_SHIELD - Defensive enemy: high shield drop chance
    [ENEMY_SHIELD] = {
        .energyChance = 10.0f,
        .shieldChance = 30.0f,
        .hullChance = 4.0f,
        .weaponChance = 14.0f,       // Increased from 8% to 14%
        .nothingChance = 42.0f
    },
    
    // ENEMY_SWARM - Small enemy: better weapon drops for progression
    [ENEMY_SWARM] = {
        .energyChance = 6.0f,        // Reduced to make room for weapons
        .shieldChance = 3.0f,
        .hullChance = 0.2f,
        .weaponChance = 8.0f,        // Increased from 3% to 8% (2.67x increase!)
        .nothingChance = 82.8f
    },
    
    // ENEMY_ELITE - Elite enemy: high drop rates for valuable items
    [ENEMY_ELITE] = {
        .energyChance = 20.0f,
        .shieldChance = 25.0f,
        .hullChance = 12.0f,
        .weaponChance = 30.0f,       // Increased from 20% to 30%
        .nothingChance = 13.0f
    },
    
    // ENEMY_GHOST - Special enemy: moderate drops
    [ENEMY_GHOST] = {
        .energyChance = 15.0f,
        .shieldChance = 15.0f,
        .hullChance = 4.0f,
        .weaponChance = 18.0f,       // Increased from 10% to 18%
        .nothingChance = 48.0f
    },
    
    // ENEMY_BOSS - Boss enemy: guaranteed multiple drops for recovery
    [ENEMY_BOSS] = {
        .energyChance = 10.0f,       // Lower individual chances since boss drops multiple items
        .shieldChance = 25.0f,
        .hullChance = 20.0f,
        .weaponChance = 35.0f,       // High weapon chance for boss
        .nothingChance = 10.0f       // Still high chance to drop something
    }
};

void InitPowerupSystem(PowerupSystem* system) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        system->powerups[i].active = false;
    }
    system->activePowerupCount = 0;
    system->totalPowerupsSpawned = 0;
    system->totalPowerupsCollected = 0;
}

void SpawnPowerup(PowerupSystem* system, PowerupType type, Vector2 position) {
    // Find an inactive powerup slot
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!system->powerups[i].active) {
            Powerup* powerup = &system->powerups[i];
            
            powerup->position = position;
            powerup->velocity = (Vector2){-POWERUP_FALL_SPEED, 0};  // Drift left at full speed
            powerup->type = type;
            powerup->active = true;
            powerup->lifetime = 0.0f;
            powerup->animationTime = 0.0f;
            powerup->pulsePhase = (float)rand() / RAND_MAX * 2.0f * PI;
            powerup->rotationAngle = 0.0f;
            powerup->magnetRange = POWERUP_MAGNET_RANGE;
            powerup->isBeingMagneted = false;
            
            // Set size
            float size = POWERUP_SIZE;
            powerup->bounds = (Rectangle){
                position.x - size/2,
                position.y - size/2,
                size,
                size
            };
            
            // Set colors based on type
            switch (type) {
                case POWERUP_ENERGY:
            powerup->primaryColor = YELLOW;
            powerup->glowColor = (Color){255, 255, 150, 150};
            break;
        case POWERUP_SHIELD:
            powerup->primaryColor = SKYBLUE;
            powerup->glowColor = (Color){135, 206, 250, 150};
            break;
        case POWERUP_HULL:
            powerup->primaryColor = RED;
            powerup->glowColor = (Color){255, 150, 150, 150};
            break;
        case POWERUP_WEAPON:
            powerup->primaryColor = ORANGE;
            powerup->glowColor = (Color){255, 200, 100, 150};
            break;
                default:
                    powerup->primaryColor = WHITE;
                    powerup->glowColor = (Color){200, 200, 200, 150};
                    break;
            }
            
            system->activePowerupCount++;
            system->totalPowerupsSpawned++;
            return;
        }
    }
}

PowerupDropRate GetDropRateForEnemy(EnemyType enemyType) {
    if (enemyType >= 0 && enemyType < ENEMY_TYPE_COUNT) {
        return DROP_RATES[enemyType];
    }
    // Default: no drops
    return (PowerupDropRate){0, 0, 0, 0, 100.0f};
}

PowerupType GetRandomPowerupFromRates(PowerupDropRate rates) {
    float roll = (float)rand() / RAND_MAX * 100.0f;
    float cumulative = 0.0f;
    
    // Check each powerup type in order
    cumulative += rates.energyChance;
    if (roll < cumulative) return POWERUP_ENERGY;
    
    cumulative += rates.shieldChance;
    if (roll < cumulative) return POWERUP_SHIELD;
    
    cumulative += rates.hullChance;
    if (roll < cumulative) return POWERUP_HULL;
    
    cumulative += rates.weaponChance;
    if (roll < cumulative) return POWERUP_WEAPON;  // Just one weapon type now!
    
    // Return energy as fallback (should not reach here normally)
    return POWERUP_ENERGY;
}

void DropPowerupFromEnemy(PowerupSystem* system, const EnemyEx* enemy) {
    if (!enemy || !enemy->active) return;
    
    // Special handling for BOSS - drop multiple powerups!
    if (enemy->type == ENEMY_BOSS) {
        // Boss drops 4-5 powerups in a spread pattern for recovery
        int numDrops = 4 + (rand() % 2);  // 4 or 5 powerups
        PowerupDropRate rates = GetDropRateForEnemy(enemy->type);
        
        for (int i = 0; i < numDrops; i++) {
            // Spread powerups in a circular pattern around boss death position
            float angle = (2.0f * PI * i) / numDrops;
            float spreadRadius = 40.0f + (rand() % 40);  // 40-80 pixels spread
            Vector2 dropPos = {
                enemy->position.x + cosf(angle) * spreadRadius,
                enemy->position.y + sinf(angle) * spreadRadius
            };
            
            // Each drop has a chance based on boss drop rates
            float dropRoll = (float)rand() / RAND_MAX * 100.0f;
            if (dropRoll < rates.nothingChance) {
                continue;  // This particular slot drops nothing
            }
            
            PowerupType type = GetRandomPowerupFromRates(rates);
            SpawnPowerup(system, type, dropPos);
        }
        return;
    }
    
    // Normal enemy drop logic
    PowerupDropRate rates = GetDropRateForEnemy(enemy->type);
    
    // Roll to see if anything drops
    float dropRoll = (float)rand() / RAND_MAX * 100.0f;
    if (dropRoll < rates.nothingChance) {
        return;  // No drop
    }
    
    // Determine what powerup to drop
    PowerupType type = GetRandomPowerupFromRates(rates);
    
    // Spawn the powerup at enemy position
    SpawnPowerup(system, type, enemy->position);
}

void UpdatePowerups(PowerupSystem* system, PlayerShip* player, float deltaTime) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!system->powerups[i].active) continue;
        
        Powerup* powerup = &system->powerups[i];
        
        // Update lifetime
        powerup->lifetime += deltaTime;
        if (powerup->lifetime > POWERUP_MAX_LIFETIME) {
            powerup->active = false;
            system->activePowerupCount--;
            continue;
        }
        
        // Update animation
        powerup->animationTime += deltaTime;
        powerup->pulsePhase += deltaTime * 3.0f;
        powerup->rotationAngle += deltaTime * 90.0f;  // Rotate 90 degrees per second
        
        // Check distance to player for magnet effect
        if (player && player->isVisible) {
            float dx = player->position.x - powerup->position.x;
            float dy = player->position.y - powerup->position.y;
            float distance = sqrtf(dx*dx + dy*dy);
            
            if (distance < powerup->magnetRange) {
                // Magnet effect - pull towards player
                powerup->isBeingMagneted = true;
                float magnetStrength = 1.0f - (distance / powerup->magnetRange);
                powerup->velocity.x = (dx / distance) * POWERUP_MAGNET_SPEED * magnetStrength;
                powerup->velocity.y = (dy / distance) * POWERUP_MAGNET_SPEED * magnetStrength;
            } else {
                // Normal drift
                powerup->isBeingMagneted = false;
                powerup->velocity.x = -POWERUP_FALL_SPEED;
                powerup->velocity.y = 0;
            }
        }
        
        // Update position
        powerup->position.x += powerup->velocity.x * deltaTime;
        powerup->position.y += powerup->velocity.y * deltaTime;
        
        // Update bounds
        float size = POWERUP_SIZE;
        powerup->bounds.x = powerup->position.x - size/2;
        powerup->bounds.y = powerup->position.y - size/2;
        
        // Remove if off screen (left side)
        if (powerup->position.x < -50) {
            powerup->active = false;
            system->activePowerupCount--;
        }
    }
}

void DrawPowerup(const Powerup* powerup) {
    if (!powerup || !powerup->active) return;
    
    Vector2 pos = powerup->position;
    float pulse = sinf(powerup->pulsePhase) * 0.2f + 0.8f;  // 0.6 to 1.0 (less variation)
    
    // Fade out in last 2 seconds
    float alpha = 1.0f;
    if (powerup->lifetime > POWERUP_MAX_LIFETIME - 2.0f) {
        alpha = (POWERUP_MAX_LIFETIME - powerup->lifetime) / 2.0f;
    }
    
    // Smaller glow effect
    Color glowColor = Fade(powerup->glowColor, alpha * pulse * 0.4f);
    DrawCircle(pos.x, pos.y, POWERUP_SIZE * 1.3f * pulse, glowColor);
    
    // Draw magnet indicator
    if (powerup->isBeingMagneted) {
        DrawCircleLines(pos.x, pos.y, POWERUP_SIZE * 1.8f, Fade(WHITE, alpha * 0.4f));
    }
    
    // Draw powerup based on type
    Color mainColor = Fade(powerup->primaryColor, alpha);
    
    switch (powerup->type) {
        case POWERUP_ENERGY: {
            // Energy: Lightning bolt shape (no circle)
            float s = POWERUP_SIZE * 0.7f;
            
            // Main lightning bolt shape
            Vector2 bolt[] = {
                {pos.x, pos.y - s*1.2f},        // Top point
                {pos.x - s*0.4f, pos.y - s*0.2f}, // Top left
                {pos.x + s*0.2f, pos.y - s*0.2f}, // Top right inner
                {pos.x - s*0.2f, pos.y + s*0.2f}, // Bottom left inner
                {pos.x + s*0.4f, pos.y + s*0.2f}, // Bottom right
                {pos.x, pos.y + s*1.2f}          // Bottom point
            };
            
            // Draw filled lightning bolt
            DrawTriangle(bolt[0], bolt[1], bolt[2], mainColor);
            DrawTriangle(bolt[1], bolt[3], bolt[2], mainColor);
            DrawTriangle(bolt[2], bolt[3], bolt[4], Fade(YELLOW, alpha * 0.9f));
            DrawTriangle(bolt[3], bolt[4], bolt[5], Fade(YELLOW, alpha * 0.9f));
            
            // Outline
            for (int i = 0; i < 6; i++) {
                DrawLineEx(bolt[i], bolt[(i+1)%6], 1.0f, Fade(WHITE, alpha * 0.6f));
            }
            break;
        }
        
        case POWERUP_SHIELD: {
            // Shield: Diamond/Rhombus shape
            float s = POWERUP_SIZE * 0.85f;
            
            // Diamond points
            Vector2 diamond[] = {
                {pos.x, pos.y - s*1.3f},       // Top
                {pos.x + s*0.9f, pos.y},       // Right
                {pos.x, pos.y + s*1.3f},       // Bottom
                {pos.x - s*0.9f, pos.y}        // Left
            };
            
            // Draw filled diamond
            DrawTriangle(diamond[0], diamond[1], diamond[3], mainColor);
            DrawTriangle(diamond[1], diamond[2], diamond[3], Fade(SKYBLUE, alpha * 0.8f));
            
            // Inner smaller diamond for depth
            float innerScale = 0.5f;
            Vector2 innerDiamond[] = {
                {pos.x, pos.y - s*1.3f*innerScale},
                {pos.x + s*0.9f*innerScale, pos.y},
                {pos.x, pos.y + s*1.3f*innerScale},
                {pos.x - s*0.9f*innerScale, pos.y}
            };
            DrawTriangle(innerDiamond[0], innerDiamond[1], innerDiamond[3], Fade(WHITE, alpha * 0.4f));
            DrawTriangle(innerDiamond[1], innerDiamond[2], innerDiamond[3], Fade(WHITE, alpha * 0.4f));
            
            // Outline
            for (int i = 0; i < 4; i++) {
                DrawLineEx(diamond[i], diamond[(i+1)%4], 1.0f, Fade(WHITE, alpha * 0.7f));
            }
            break;
        }
        
        case POWERUP_HULL: {
            // Hull: Medical cross shape (no circle)
            float s = POWERUP_SIZE * 0.75f;
            float thick = s * 0.4f;
            
            // Vertical bar of cross
            DrawRectangle(pos.x - thick/2, pos.y - s*1.2f, thick, s*2.4f, mainColor);
            
            // Horizontal bar of cross
            DrawRectangle(pos.x - s*1.2f, pos.y - thick/2, s*2.4f, thick, mainColor);
            
            // Lighter inner cross for depth
            float innerThick = thick * 0.5f;
            DrawRectangle(pos.x - innerThick/2, pos.y - s*1.0f, innerThick, s*2.0f, Fade(WHITE, alpha * 0.5f));
            DrawRectangle(pos.x - s*1.0f, pos.y - innerThick/2, s*2.0f, innerThick, Fade(WHITE, alpha * 0.5f));
            
            // Outline rectangles
            DrawRectangleLines(pos.x - thick/2, pos.y - s*1.2f, thick, s*2.4f, Fade(WHITE, alpha * 0.7f));
            DrawRectangleLines(pos.x - s*1.2f, pos.y - thick/2, s*2.4f, thick, Fade(WHITE, alpha * 0.7f));
            break;
        }
        
        case POWERUP_WEAPON: {
            // Weapon: Single chevron arrow pointing up
            float s = POWERUP_SIZE * 0.8f;
            
            // Arrow/chevron shape pointing up
            Vector2 arrow[] = {
                {pos.x, pos.y - s*0.9f},           // Top point
                {pos.x + s*0.8f, pos.y + s*0.4f},  // Right outer
                {pos.x + s*0.4f, pos.y + s*0.4f},  // Right inner
                {pos.x, pos.y - s*0.2f},           // Center inner
                {pos.x - s*0.4f, pos.y + s*0.4f},  // Left inner
                {pos.x - s*0.8f, pos.y + s*0.4f}   // Left outer
            };
            
            // Draw filled arrow
            DrawTriangle(arrow[0], arrow[1], arrow[5], mainColor);
            DrawTriangle(arrow[0], arrow[2], arrow[1], mainColor);
            DrawTriangle(arrow[0], arrow[5], arrow[4], mainColor);
            
            // Inner highlight for depth
            DrawTriangle(arrow[0], arrow[3], arrow[2], Fade(YELLOW, alpha * 0.7f));
            DrawTriangle(arrow[0], arrow[4], arrow[3], Fade(YELLOW, alpha * 0.7f));
            
            // Outline
            for (int i = 0; i < 6; i++) {
                DrawLineEx(arrow[i], arrow[(i+1)%6], 1.5f, Fade(WHITE, alpha * 0.7f));
            }
            break;
        }
        
        default:
            // Fallback: small square
            DrawRectangle(pos.x - POWERUP_SIZE/2, pos.y - POWERUP_SIZE/2, 
                         POWERUP_SIZE, POWERUP_SIZE, mainColor);
            DrawRectangleLines(pos.x - POWERUP_SIZE/2, pos.y - POWERUP_SIZE/2,
                             POWERUP_SIZE, POWERUP_SIZE, Fade(WHITE, alpha * 0.8f));
            break;
    }
}

void DrawPowerups(const PowerupSystem* system) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (system->powerups[i].active) {
            DrawPowerup(&system->powerups[i]);
        }
    }
}

void ApplyPowerupEffect(PlayerShip* player, PowerupType type, int* score) {
    if (!player) return;
    
    switch (type) {
        case POWERUP_ENERGY:
            // Restore 20% of max energy
            player->energy += player->maxEnergy * ENERGY_RECOVERY_PERCENT;
            if (player->energy > player->maxEnergy) {
                player->energy = player->maxEnergy;
            }
            break;
            
        case POWERUP_SHIELD:
            // Restore full shield
            player->shield = player->maxShield;
            player->shieldGlow = 1.0f;
            break;
            
        case POWERUP_HULL:
            // Restore 20% of max health
            player->health += (int)(player->maxHealth * HULL_RECOVERY_PERCENT);
            if (player->health > player->maxHealth) {
                player->health = player->maxHealth;
            }
            break;
            
        case POWERUP_WEAPON: {
            // Increment weapon powerup counter (max 3) - accumulative!
            if (player->weaponPowerupCount < 3) {
                player->weaponPowerupCount++;
                // Note: The weapon behavior changes based on this counter
                // but we don't modify weaponLevel here - that's for upgrades
            }
            break;
        }
            
        default:
            break;
    }
    
    // Add bonus score
    if (score) {
        *score += POWERUP_COLLECT_SCORE;
    }
}

void CheckPowerupCollisions(PowerupSystem* system, PlayerShip* player, int* score) {
    if (!player || !player->isVisible) return;
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!system->powerups[i].active) continue;
        
        Powerup* powerup = &system->powerups[i];
        
        // Check collision with player
        if (CheckCollisionRecs(player->bounds, powerup->bounds)) {
            // Apply powerup effect
            ApplyPowerupEffect(player, powerup->type, score);
            
            // Deactivate powerup
            powerup->active = false;
            system->activePowerupCount--;
            system->totalPowerupsCollected++;
        }
    }
}

const char* GetPowerupName(PowerupType type) {
    switch (type) {
        case POWERUP_ENERGY: return "Energy Cell";
        case POWERUP_SHIELD: return "Shield Booster";
        case POWERUP_HULL: return "Repair Kit";
        case POWERUP_WEAPON: return "Weapon Upgrade";
        default: return "Unknown";
    }
}

const char* GetPowerupDescription(PowerupType type) {
    switch (type) {
        case POWERUP_ENERGY: return "Restores 20% energy";
        case POWERUP_SHIELD: return "Fully restores shields";
        case POWERUP_HULL: return "Repairs 20% hull damage";
        case POWERUP_WEAPON: return "Weapon upgrade (accumulative)";
        default: return "";
    }
}

void CleanupPowerupSystem(PowerupSystem* system) {
    // Nothing to cleanup - powerups are stack-allocated within the struct
    // Just mark all as inactive
    if (system) {
        for (int i = 0; i < MAX_POWERUPS; i++) {
            system->powerups[i].active = false;
        }
    }
}
