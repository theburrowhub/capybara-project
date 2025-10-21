#include "weapon.h"
#include "constants.h"
#include "projectile_types.h"
#include "player_ship.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

void InitBullets(Bullet* bullets) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
        bullets[i].speed = BULLET_SPEED;
        bullets[i].damage = 1.0f;
        bullets[i].velocityY = 0.0f;
        bullets[i].powerLevel = 0;
    }
}

// Fire bullets based on weapon mode
void ShootBullet(Bullet* bullets, Vector2 position) {
    // This is a legacy function - not used anymore
    // Keeping for compatibility but should use ShootBulletsForMode instead
}

// Get base damage per bullet for a weapon mode (single source of truth)
float GetBaseDamagePerBullet(WeaponMode mode) {
    float baseDamage;
    switch (mode) {
        case WEAPON_MODE_SINGLE:
            baseDamage = 1.0f;  // 1 shot × 1.0 = 1.0 total (100%)
            break;
        case WEAPON_MODE_SPREAD:
            baseDamage = 0.1667f;  // 3 shots × 0.1667 = 0.5 total (50%)
            break;
        case WEAPON_MODE_DUAL:
            baseDamage = 0.375f;  // 2 shots × 0.375 = 0.75 total (75%)
            break;
        default:
            baseDamage = 1.0f;
            break;
    }
    return baseDamage * WEAPON_DAMAGE_MULTIPLIER;
}

// Get fire rate for a weapon mode (single source of truth)
float GetFireRateForMode(WeaponMode mode) {
    float baseFireRate;
    switch (mode) {
        case WEAPON_MODE_SINGLE:
            baseFireRate = 0.12f;  // Fastest
            break;
        case WEAPON_MODE_DUAL:
            baseFireRate = 0.15f;  // Medium
            break;
        case WEAPON_MODE_SPREAD:
            baseFireRate = 0.20f;  // Slowest
            break;
        default:
            baseFireRate = 0.12f;
            break;
    }
    return baseFireRate * WEAPON_FIRE_RATE_MULTIPLIER;
}

// Get weapon mode name (single source of truth)
const char* GetWeaponModeName(WeaponMode mode) {
    switch (mode) {
        case WEAPON_MODE_SINGLE:
            return "SINGLE";
        case WEAPON_MODE_SPREAD:
            return "SPREAD";
        case WEAPON_MODE_DUAL:
            return "DUAL";
        default:
            return "UNKNOWN";
    }
}

// Fire bullets according to weapon mode with balanced damage
void ShootBulletsForMode(Bullet* bullets, PlayerShip* playerShip) {
    Vector2 position = playerShip->position;
    WeaponMode mode = playerShip->weaponMode;
    
    // Determine number of bullets and damage per bullet based on mode
    int bulletCount = 1;
    float angleSpread = 0.0f;  // degrees
    
    // Get base damage from centralized function
    float damagePerBullet = GetBaseDamagePerBullet(mode);
    
    switch (mode) {
        case WEAPON_MODE_SINGLE:
            bulletCount = 1;
            break;
        case WEAPON_MODE_SPREAD:
            bulletCount = 3;
            angleSpread = 20.0f;     // Spread angle
            break;
        case WEAPON_MODE_DUAL:
            bulletCount = 2;
            break;
        default:
            bulletCount = 1;
            break;
    }
    
    // Apply weapon powerup damage multipliers
    // Level 0 (standard): 1x, Level 1 (+1): 1.5x, Level 2 (+2): 2x, Level 3 (+3): 2.5x
    float powerMultiplier = 1.0f;
    int powerLevel = playerShip->weaponPowerupCount;
    switch (powerLevel) {
        case 0: powerMultiplier = 1.0f; break;
        case 1: powerMultiplier = 1.5f; break;
        case 2: powerMultiplier = 2.0f; break;
        case 3: powerMultiplier = 2.5f; break;
        default: powerMultiplier = 1.0f; break;
    }
    damagePerBullet *= powerMultiplier;
    
    // Fire the bullets
    int bulletsFired = 0;
    for (int i = 0; i < MAX_BULLETS && bulletsFired < bulletCount; i++) {
        if (!bullets[i].active) {
            bullets[i].position = position;
            bullets[i].damage = damagePerBullet;
            bullets[i].speed = BULLET_SPEED;
            bullets[i].velocityY = 0.0f;
            bullets[i].powerLevel = powerLevel;
            
            // Position and velocity based on weapon mode
            switch (mode) {
                case WEAPON_MODE_SINGLE:
                    bullets[i].position.x += 25;
                    break;
                    
                case WEAPON_MODE_SPREAD:
                    bullets[i].position.x += 20;
                    if (bulletsFired == 0) {
                        // Up shot
                        bullets[i].velocityY = -tanf(angleSpread * DEG2RAD) * BULLET_SPEED;
                    } else if (bulletsFired == 1) {
                        // Center shot
                        bullets[i].velocityY = 0;
                    } else {
                        // Down shot
                        bullets[i].velocityY = tanf(angleSpread * DEG2RAD) * BULLET_SPEED;
                    }
                    break;
                    
                case WEAPON_MODE_DUAL:
                    if (bulletsFired == 0) {
                        // Front shot
                        bullets[i].position.x += 25;
                    } else {
                        // Back shot
                        bullets[i].position.x -= 25;
                        bullets[i].speed = -BULLET_SPEED;  // Negative speed for backward
                    }
                    break;
                    
                default:
                    bullets[i].position.x += 25;
                    break;
            }
            
            bullets[i].bounds = (Rectangle){
                bullets[i].position.x - 5,
                bullets[i].position.y - 2,
                10, 4
            };
            bullets[i].active = true;
            bulletsFired++;
        }
    }
}

void UpdateWeaponHeat(PlayerShip* playerShip, float deltaTime) {
    // Skip heat management if overheating is disabled
    if (!WEAPON_OVERHEATING) {
        playerShip->weaponHeat = 0;
        playerShip->overheated = false;
        playerShip->cooldownTime = 0;
        return;
    }
    
    if (playerShip->overheated) {
        playerShip->cooldownTime -= deltaTime;
        if (playerShip->cooldownTime <= 0) {
            playerShip->overheated = false;
            playerShip->weaponHeat = 0;
            playerShip->cooldownTime = 0;
        }
    } else {
        // Cool down weapon continuously
        if (playerShip->weaponHeat > 0) {
            playerShip->weaponHeat -= WEAPON_COOLDOWN_RATE * deltaTime;
            if (playerShip->weaponHeat < 0) playerShip->weaponHeat = 0;
        }
    }
}

// Helper function to safely check if fire action is active (with NULL check)
static bool IsFireActionDown(Game* game) {
    if (game->inputManager) {
        return InputManager_IsActionDown(game->inputManager, ACTION_FIRE);
    }
    // Fallback to direct input if inputManager is NULL
    return IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON);
}

void UpdateBullets(Game* game) {
    Bullet *bullets = game->bullets;
    PlayerShip *playerShip = game->playerShip;
    float deltaTime = GetFrameTime();
    
    // Update weapon heat system only if enabled
    if (WEAPON_OVERHEATING) {
        UpdateWeaponHeat(playerShip, deltaTime);
    }
    
    // Log when devastating attack is activated
    static bool wasSpecialActive = false;
    if (playerShip->energyMode == ENERGY_MODE_OFFENSIVE && 
        playerShip->specialAbilityActive && !wasSpecialActive) {
        LogEvent(game, "[%.2f] DEVASTATING ATTACK ACTIVATED! Duration: 2 seconds", game->gameTime);
    }
    wasSpecialActive = playerShip->specialAbilityActive;
    
    // Fire continuously during devastating attack in offensive mode
    if (playerShip->energyMode == ENERGY_MODE_OFFENSIVE && playerShip->specialAbilityActive) {
        // Fire continuous spread of bullets during the 2 second duration
        static float devastatingFireTimer = 0;
        devastatingFireTimer -= deltaTime;
        
        if (devastatingFireTimer <= 0) {
            devastatingFireTimer = 0.05f;  // Fire every 0.05 seconds
            
            // Fire bullets in spread pattern
            int bulletsFired = 0;
            
            // Calculate power multiplier for devastating attack
            int powerLevel = playerShip->weaponPowerupCount;
            float powerMultiplier = 1.0f;
            switch (powerLevel) {
                case 0: powerMultiplier = 1.0f; break;
                case 1: powerMultiplier = 1.5f; break;
                case 2: powerMultiplier = 2.0f; break;
                case 3: powerMultiplier = 2.5f; break;
                default: powerMultiplier = 1.0f; break;
            }
            
            for (int angle = -60; angle <= 60; angle += 20) {
                for (int i = 0; i < MAX_BULLETS && bulletsFired < 7; i++) {
                    if (!bullets[i].active) {
                        bullets[i].position = playerShip->position;
                        bullets[i].position.x += 25;
                        
                        // Calculate direction based on angle
                        float rad = angle * DEG2RAD;
                        float speed = BULLET_SPEED * 1.5f;  // Faster bullets
                        bullets[i].speed = speed * cosf(rad);  // X component
                        
                        // Spread bullets vertically
                        bullets[i].position.y += sinf(rad) * 5.0f;
                        
                        // Apply power level
                        bullets[i].damage = 1.0f * powerMultiplier;
                        bullets[i].powerLevel = powerLevel;
                        bullets[i].velocityY = 0.0f;
                        
                        bullets[i].bounds = (Rectangle){bullets[i].position.x - 5, bullets[i].position.y - 2, 10, 4};
                        bullets[i].active = true;
                        bulletsFired++;
                        break;
                    }
                }
            }
        }
    }
    
    // Normal firing modes
    static float shootTimer = 0;
    shootTimer -= deltaTime;
    
    // Get fire rate based on weapon mode
    float fireRate = GetFireRateForMode(playerShip->weaponMode);
    
    // Can't fire when defensive special shield is active
    bool canFire = !(playerShip->energyMode == ENERGY_MODE_DEFENSIVE && playerShip->specialAbilityActive);
    
    if (IsFireActionDown(game) && 
        (!WEAPON_OVERHEATING || !playerShip->overheated) && shootTimer <= 0 && canFire) {
        // Use new weapon mode system
        ShootBulletsForMode(bullets, playerShip);
        shootTimer = fireRate;
        
        // Offensive mode with full energy: more damage (handled in collision system)
        // The damage multiplier is applied in CheckBulletEnemyCollisions
        
        // Only manage heat if overheating is enabled
        if (WEAPON_OVERHEATING) {
            // Increase heat
            playerShip->weaponHeat += WEAPON_HEAT_PER_SHOT;
            
            // Check for overheat
            if (playerShip->weaponHeat >= playerShip->maxHeat) {
                playerShip->weaponHeat = playerShip->maxHeat;
                playerShip->overheated = true;
                playerShip->cooldownTime = WEAPON_OVERHEAT_TIME;
                
                LogEvent(game, "[%.2f] Weapon overheated! 3 second cooldown initiated", game->gameTime);
            }
        }
    }
    
    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].speed;
            bullets[i].position.y += bullets[i].velocityY;
            bullets[i].bounds.x = bullets[i].position.x - 5;
            bullets[i].bounds.y = bullets[i].position.y - 2;
            
            // Deactivate off-screen bullets
            if (bullets[i].position.x > SCREEN_WIDTH + 50 || 
                bullets[i].position.x < -50 ||
                bullets[i].position.y > SCREEN_HEIGHT + 50 ||
                bullets[i].position.y < -50) {
                bullets[i].active = false;
            }
        }
    }
}

void DrawBullets(const Bullet* bullets) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            float x = bullets[i].position.x;
            float y = bullets[i].position.y;
            float damage = bullets[i].damage;
            int powerLevel = bullets[i].powerLevel; // Keep for trail/lightning effects
            
            // Scale factor based on actual damage (using square root for smoother scaling)
            float sizeScale = 0.5f + sqrtf(damage) * 0.5f;
            
            // Number of trail segments and lightning bolts increase with level
            int trailSegments = 8 + powerLevel * 2;
            int lightningBolts = 3 + powerLevel;
            
            // Define base colors for each power level
            // Level 0: Blue, Level 1: Yellow, Level 2: Green, Level 3: White
            Color baseColor, midColor, brightColor;
            switch (powerLevel) {
                case 0: // Blue
                    baseColor = (Color){0, 100, 255, 255};
                    midColor = (Color){50, 150, 255, 255};
                    brightColor = (Color){150, 200, 255, 255};
                    break;
                case 1: // Yellow
                    baseColor = (Color){255, 200, 0, 255};
                    midColor = (Color){255, 220, 50, 255};
                    brightColor = (Color){255, 240, 150, 255};
                    break;
                case 2: // Green
                    baseColor = (Color){0, 255, 100, 255};
                    midColor = (Color){100, 255, 150, 255};
                    brightColor = (Color){200, 255, 230, 255};
                    break;
                case 3: // White
                    baseColor = (Color){200, 200, 255, 255};
                    midColor = (Color){230, 230, 255, 255};
                    brightColor = (Color){255, 255, 255, 255};
                    break;
                default:
                    baseColor = (Color){0, 255, 100, 255};
                    midColor = (Color){100, 255, 150, 255};
                    brightColor = (Color){200, 255, 230, 255};
                    break;
            }
            
            // Energy trail effect - more intense at higher levels
            for (int j = trailSegments; j >= 0; j--) {
                float offset = j * 4.0f;
                float alpha = 1.0f - (j * (0.9f / trailSegments));
                float size = (10.0f - (j * 1.1f)) * sizeScale;
                
                // Outer energy field
                DrawCircle(x - offset, y, size + 3 * sizeScale, 
                          Fade(baseColor, alpha * 0.25f));
                // Middle glow
                DrawCircle(x - offset, y, size + 1.5f * sizeScale, 
                          Fade(midColor, alpha * 0.35f));
                // Inner trail
                DrawCircle(x - offset, y, size, 
                          Fade(brightColor, alpha * 0.45f));
            }
            
            // Main projectile - ENERGY BLAST (scaled by power level)
            // Outer glow - larger and brighter at higher levels
            DrawCircle(x, y, 15 * sizeScale, 
                      Fade(baseColor, 0.3f + powerLevel * 0.05f));
            
            // Outer ring
            DrawCircle(x, y, 10 * sizeScale, 
                      Fade(midColor, 0.5f + powerLevel * 0.05f));
            DrawCircleLines(x, y, 10 * sizeScale, midColor);
            
            // Middle layer
            DrawCircle(x, y, 7 * sizeScale, midColor);
            
            // Bright core
            DrawCircle(x, y, 4 * sizeScale, brightColor);
            DrawCircle(x, y, 2 * sizeScale, WHITE);
            
            // Leading edge power burst - scaled
            DrawCircle(x + 8 * sizeScale, y, 6 * sizeScale, 
                      Fade(midColor, 0.7f));
            DrawCircle(x + 8 * sizeScale, y, 4 * sizeScale, brightColor);
            DrawCircle(x + 8 * sizeScale, y, 2 * sizeScale, WHITE);
            
            // Energy lightning effects - more bolts at higher levels
            float time = GetTime() * 10;
            float lightningRadius = 12 * sizeScale;
            for (int k = 0; k < lightningBolts; k++) {
                float angle = (k * (360.0f / lightningBolts) + time) * DEG2RAD;
                float sparkX = x + cosf(angle) * lightningRadius;
                float sparkY = y + sinf(angle) * lightningRadius;
                DrawLine(x, y, sparkX, sparkY, 
                        Fade(midColor, 0.4f + powerLevel * 0.05f));
                DrawCircle(sparkX, sparkY, 2 * sizeScale, 
                          Fade(brightColor, 0.6f));
            }
            
            // Power particles - more at higher levels
            int particleCount = 5 + powerLevel * 2;
            for (int p = 0; p < particleCount; p++) {
                float pOffset = p * 6;
                DrawPixel(x - pOffset - 10, y + ((p % 2) * 4 - 2), 
                          Fade(midColor, 0.8f - p * (0.8f / particleCount)));
            }
            
            // Front shockwave effect - larger at higher levels
            DrawCircleLines(x + 10 * sizeScale, y, 8 * sizeScale, 
                           Fade(brightColor, 0.5f + powerLevel * 0.05f));
            DrawCircleLines(x + 10 * sizeScale, y, 12 * sizeScale, 
                           Fade(midColor, 0.3f));
            
            // Extra effects for higher power levels
            if (powerLevel >= 2) {
                // Additional outer shockwave ring
                DrawCircleLines(x, y, 18 * sizeScale, 
                               Fade(midColor, 0.4f));
            }
            if (powerLevel >= 3) {
                // Maximum power: add pulsing aura
                float pulse = sinf(time * 0.5f) * 0.3f + 0.7f;
                DrawCircle(x, y, 20 * sizeScale, 
                          Fade(baseColor, 0.2f * pulse));
            }
        }
    }
}

void DrawWeaponHeatBar(const PlayerShip* playerShip) {
    // Don't draw heat bar if overheating is disabled
    if (!WEAPON_OVERHEATING) {
        return;
    }
    
    DrawText("Weapon Heat:", 10, 85, 20, WHITE);
    float heatBarWidth = 150;
    float heatFillWidth = (playerShip->weaponHeat / playerShip->maxHeat) * heatBarWidth;
    
    // Background bar
    DrawRectangle(120, 85, heatBarWidth, 20, DARKGRAY);
    
    // Heat fill bar - color changes based on heat level
    Color heatColor;
    if (playerShip->overheated) {
        heatColor = PURPLE;  // Overheated
        DrawText("OVERHEATED!", 280, 85, 20, RED);
        DrawText(TextFormat("Cooldown: %.1fs", playerShip->cooldownTime), 280, 105, 18, ORANGE);
    } else if (playerShip->weaponHeat > 75) {
        heatColor = RED;     // Critical
    } else if (playerShip->weaponHeat > 50) {
        heatColor = ORANGE;  // Warning
    } else if (playerShip->weaponHeat > 25) {
        heatColor = YELLOW;  // Warm
    } else {
        heatColor = GREEN;   // Cool
    }
    
    DrawRectangle(120, 85, heatFillWidth, 20, heatColor);
    DrawRectangleLines(120, 85, heatBarWidth, 20, WHITE);
}
