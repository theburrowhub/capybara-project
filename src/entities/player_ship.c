#include "player_ship.h"
#include "constants.h"
#include "weapon.h"
#include "projectile_types.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// Ship configuration constants
static const ShipConfig DEFAULT_SHIP_CONFIG = {
    .baseHealth = 100,
    .baseShield = 50,
    .baseEnergy = 100,
    .baseSpeed = 5.0f,
    .baseFireRate = 0.1f,
    .baseDamage = 10
};

// Ability configurations
static const float ABILITY_COSTS[ABILITY_COUNT] = {
    [ABILITY_BOOST] = 20.0f,
    [ABILITY_SHIELD_BURST] = 40.0f,
    [ABILITY_EMP_BLAST] = 60.0f,
    [ABILITY_OVERDRIVE] = 80.0f
};

static const float ABILITY_COOLDOWNS[ABILITY_COUNT] = {
    [ABILITY_BOOST] = 3.0f,
    [ABILITY_SHIELD_BURST] = 10.0f,
    [ABILITY_EMP_BLAST] = 15.0f,
    [ABILITY_OVERDRIVE] = 20.0f
};

static const float ABILITY_DURATIONS[ABILITY_COUNT] = {
    [ABILITY_BOOST] = 2.0f,
    [ABILITY_SHIELD_BURST] = 3.0f,
    [ABILITY_EMP_BLAST] = 0.5f,
    [ABILITY_OVERDRIVE] = 5.0f
};

void InitPlayerShip(PlayerShip* ship) {
    // Position and physics
    ship->position = (Vector2){150, SCREEN_HEIGHT/2};
    ship->velocity = (Vector2){0, 0};
    ship->bounds = (Rectangle){
        ship->position.x - 25,
        ship->position.y - 20,
        50,
        40
    };
    ship->rotation = 0.0f;
    
    // Health & Shield (reduced for more difficulty)
    ship->maxHealth = DEFAULT_SHIP_CONFIG.baseHealth;
    ship->health = ship->maxHealth;
    ship->maxShield = 20;  // Reduced from 50 to 20 for more difficulty
    ship->shield = ship->maxShield;
    ship->shieldRegenRate = 2.0f;  // Slower regen (was 5.0f)
    ship->shieldRegenDelay = 5.0f; // Longer delay before regen (was 3.0f)
    ship->lastDamageTime = 0.0f;
    
    // Energy system
    ship->maxEnergy = DEFAULT_SHIP_CONFIG.baseEnergy;
    ship->energy = ship->maxEnergy;
    ship->energyRegenRate = 10.0f; // Energy per second
    
    // Weapon system
    ship->weaponMode = WEAPON_MODE_SINGLE;
    ship->weaponHeat = 0.0f;
    ship->maxHeat = 100.0f;
    ship->overheated = false;
    ship->cooldownTime = 0.0f;
    ship->fireTimer = 0.0f;
    ship->chargeLevel = 0.0f;
    ship->isCharging = false;
    
    // Movement
    ship->baseSpeed = DEFAULT_SHIP_CONFIG.baseSpeed;
    ship->currentSpeed = ship->baseSpeed;
    ship->acceleration = 15.0f;
    ship->maxSpeed = ship->baseSpeed * 2.0f;
    ship->boostSpeed = ship->baseSpeed * 3.0f;
    ship->isBoosting = false;
    ship->boostDuration = 0.0f;
    ship->boostCooldown = 0.0f;
    
    // Abilities
    for (int i = 0; i < ABILITY_COUNT; i++) {
        ship->abilityCooldowns[i] = 0.0f;
        ship->abilityActive[i] = false;
        ship->abilityDurations[i] = 0.0f;
    }
    
    // Visual effects
    ship->engineGlow = 0.8f;
    ship->shieldGlow = 0.0f;
    ship->primaryColor = (Color){0, 150, 255, 255};      // Cyan blue
    ship->secondaryColor = (Color){255, 200, 0, 255};    // Gold
    ship->engineColor = (Color){100, 200, 255, 255};     // Light blue
    ship->shieldColor = (Color){0, 255, 200, 100};       // Cyan shield
    
    // Initialize trail
    for (int i = 0; i < 20; i++) {
        ship->trailPositions[i] = ship->position;
        ship->trailAlpha[i] = 0.0f;
    }
    ship->trailIndex = 0;
    
    // Animation
    ship->animTime = 0.0f;
    ship->bankAngle = 0.0f;
    ship->enginePulse = 0.0f;
    
    // Upgrades
    ship->upgrades.weaponLevel = 1;
    ship->upgrades.shieldLevel = 1;
    ship->upgrades.engineLevel = 1;
    ship->upgrades.energyLevel = 1;
    
    // Stats
    ship->score = 0;
    ship->enemiesDestroyed = 0;
    ship->survivalTime = 0.0f;
    
    // Visibility
    ship->isVisible = true;
}

void UpdatePlayerShip(PlayerShip* ship, float deltaTime) {
    // Update survival time
    ship->survivalTime += deltaTime;
    
    // Handle input
    HandlePlayerInput(ship);
    
    // Update physics
    UpdateShipPhysics(ship, deltaTime);
    
    // Update abilities
    UpdateShipAbilities(ship, deltaTime);
    
    // Update visual effects
    UpdateShipEffects(ship, deltaTime);
    
    // Shield regeneration
    if (ship->shield < ship->maxShield) {
        float timeSinceDamage = GetTime() - ship->lastDamageTime;
        if (timeSinceDamage > ship->shieldRegenDelay) {
            ship->shield += ship->shieldRegenRate * deltaTime;
            if (ship->shield > ship->maxShield) {
                ship->shield = ship->maxShield;
            }
            ship->shieldGlow = fminf(1.0f, ship->shield / ship->maxShield);
        }
    }
    
    // Energy regeneration
    if (ship->energy < ship->maxEnergy) {
        ship->energy += ship->energyRegenRate * deltaTime;
        if (ship->energy > ship->maxEnergy) {
            ship->energy = ship->maxEnergy;
        }
    }
    
    // Update animation timers
    ship->animTime += deltaTime;
    ship->enginePulse = (sinf(ship->animTime * 5.0f) + 1.0f) * 0.5f;
}

void HandlePlayerInput(PlayerShip* ship) {
    Vector2 inputDir = {0, 0};
    
    // Movement input
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        inputDir.y = -1;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        inputDir.y = 1;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        inputDir.x = -1;
        ship->bankAngle = fmaxf(-30.0f, ship->bankAngle - 2.0f);
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        inputDir.x = 1;
        ship->bankAngle = fminf(30.0f, ship->bankAngle + 2.0f);
    }
    
    // Normalize diagonal movement
    if (inputDir.x != 0 && inputDir.y != 0) {
        float length = sqrtf(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
        inputDir.x /= length;
        inputDir.y /= length;
    }
    
    // Apply movement
    ship->velocity.x = inputDir.x * ship->currentSpeed;
    ship->velocity.y = inputDir.y * ship->currentSpeed;
    
    // Return banking to neutral
    if (inputDir.x == 0) {
        if (ship->bankAngle > 0) {
            ship->bankAngle = fmaxf(0, ship->bankAngle - 3.0f);
        } else if (ship->bankAngle < 0) {
            ship->bankAngle = fminf(0, ship->bankAngle + 3.0f);
        }
    }
    
    // Boost ability (SHIFT key)
    if (IsKeyDown(KEY_LEFT_SHIFT) && ship->abilityCooldowns[ABILITY_BOOST] <= 0 && ship->energy >= ABILITY_COSTS[ABILITY_BOOST]) {
        ActivateAbility(ship, ABILITY_BOOST);
    }
    
    // Shield burst (E key)
    if (IsKeyPressed(KEY_E) && ship->abilityCooldowns[ABILITY_SHIELD_BURST] <= 0 && ship->energy >= ABILITY_COSTS[ABILITY_SHIELD_BURST]) {
        ActivateAbility(ship, ABILITY_SHIELD_BURST);
    }
    
    // EMP blast (Q key)
    if (IsKeyPressed(KEY_Q) && ship->abilityCooldowns[ABILITY_EMP_BLAST] <= 0 && ship->energy >= ABILITY_COSTS[ABILITY_EMP_BLAST]) {
        ActivateAbility(ship, ABILITY_EMP_BLAST);
    }
    
    // Overdrive (R key)
    if (IsKeyPressed(KEY_R) && ship->abilityCooldowns[ABILITY_OVERDRIVE] <= 0 && ship->energy >= ABILITY_COSTS[ABILITY_OVERDRIVE]) {
        ActivateAbility(ship, ABILITY_OVERDRIVE);
    }
    
    // Weapon mode switching disabled - always use single mode
    // Keep weapon mode as SINGLE always
    ship->weaponMode = WEAPON_MODE_SINGLE;
    
    // Charge beam disabled - no special weapon modes
}

void UpdateShipPhysics(PlayerShip* ship, float deltaTime) {
    // Update position
    ship->position.x += ship->velocity.x;
    ship->position.y += ship->velocity.y;
    
    // Keep ship on screen
    float margin = 30.0f;
    if (ship->position.x < margin) ship->position.x = margin;
    if (ship->position.x > SCREEN_WIDTH - margin) ship->position.x = SCREEN_WIDTH - margin;  // Allow reaching right border
    if (ship->position.y < margin) ship->position.y = margin;
    if (ship->position.y > SCREEN_HEIGHT - margin) ship->position.y = SCREEN_HEIGHT - margin;
    
    // Update bounds
    ship->bounds.x = ship->position.x - 25;
    ship->bounds.y = ship->position.y - 20;
    ship->bounds.width = 50;
    ship->bounds.height = 40;
    
    // Update trail
    ship->trailIndex = (ship->trailIndex + 1) % 20;
    ship->trailPositions[ship->trailIndex] = (Vector2){
        ship->position.x - 30,
        ship->position.y
    };
    
    // Update trail alpha
    for (int i = 0; i < 20; i++) {
        if (ship->isBoosting) {
            ship->trailAlpha[i] = fminf(1.0f, ship->trailAlpha[i] + deltaTime * 5.0f);
        } else {
            ship->trailAlpha[i] = fmaxf(0.0f, ship->trailAlpha[i] - deltaTime * 2.0f);
        }
    }
    
    // Update engine glow based on movement
    float speed = sqrtf(ship->velocity.x * ship->velocity.x + ship->velocity.y * ship->velocity.y);
    ship->engineGlow = 0.5f + (speed / ship->maxSpeed) * 0.5f;
    if (ship->isBoosting) {
        ship->engineGlow = fminf(1.0f, ship->engineGlow + 0.3f);
    }
}

void UpdateShipAbilities(PlayerShip* ship, float deltaTime) {
    // Update ability cooldowns
    for (int i = 0; i < ABILITY_COUNT; i++) {
        if (ship->abilityCooldowns[i] > 0) {
            ship->abilityCooldowns[i] -= deltaTime;
        }
        
        // Update active ability durations
        if (ship->abilityActive[i]) {
            ship->abilityDurations[i] -= deltaTime;
            if (ship->abilityDurations[i] <= 0) {
                ship->abilityActive[i] = false;
                
                // Deactivate ability effects
                switch (i) {
                    case ABILITY_BOOST:
                        ship->isBoosting = false;
                        ship->currentSpeed = ship->baseSpeed;
                        break;
                    case ABILITY_SHIELD_BURST:
                        ship->shieldGlow = 0.0f;
                        break;
                    case ABILITY_OVERDRIVE:
                        // Reset fire rate
                        break;
                }
            }
        }
    }
    
    // Apply active ability effects
    if (ship->abilityActive[ABILITY_BOOST]) {
        ship->currentSpeed = ship->boostSpeed;
        ship->isBoosting = true;
    }
    
    if (ship->abilityActive[ABILITY_SHIELD_BURST]) {
        ship->shieldGlow = (sinf(ship->animTime * 10.0f) + 1.0f) * 0.5f;
    }
}

void UpdateShipEffects(PlayerShip* ship, float deltaTime) {
    // Update shield glow when damaged
    if (ship->shield < ship->maxShield * 0.3f) {
        ship->shieldGlow = (sinf(ship->animTime * 8.0f) + 1.0f) * 0.25f;
    }
    
    // Update weapon heat cooldown
    if (!ship->overheated) {
        ship->weaponHeat = fmaxf(0, ship->weaponHeat - 30.0f * deltaTime);
    } else {
        ship->cooldownTime -= deltaTime;
        if (ship->cooldownTime <= 0) {
            ship->overheated = false;
            ship->weaponHeat = 0;
        }
    }
    
    // Update fire timer
    ship->fireTimer = fmaxf(0, ship->fireTimer - deltaTime);
}

void DrawPlayerShip(const PlayerShip* ship) {
    // Don't draw if ship is not visible (e.g., during boss escape)
    if (!ship->isVisible) {
        return;
    }
    
    // Draw engine trail
    DrawEngineTrail(ship);
    
    // Main ship body - sleek fighter design
    Vector2 shipPoints[7] = {
        // Nose
        {ship->position.x + 25, ship->position.y},
        // Upper wing
        {ship->position.x + 5, ship->position.y - 15},
        {ship->position.x - 10, ship->position.y - 20},
        {ship->position.x - 25, ship->position.y - 10},
        // Lower wing
        {ship->position.x - 25, ship->position.y + 10},
        {ship->position.x - 10, ship->position.y + 20},
        {ship->position.x + 5, ship->position.y + 15}
    };
    
    // Apply banking visual effect (compression/skew, no rotation)
    if (ship->bankAngle != 0) {
        // Banking effect: compress vertically and shift horizontally to simulate tilt
        float bankFactor = ship->bankAngle / 30.0f; // -1 to 1
        for (int i = 0; i < 7; i++) {
            float dy = shipPoints[i].y - ship->position.y;
            // Compress Y axis slightly when banking
            shipPoints[i].y = ship->position.y + dy * (1.0f - fabs(bankFactor) * 0.2f);
            // Slight horizontal shift based on vertical position for skew effect
            shipPoints[i].x += dy * bankFactor * 0.1f;
        }
    }
    
    // Draw main body
    DrawTriangle(shipPoints[0], shipPoints[1], shipPoints[6], ship->primaryColor);
    DrawTriangle(shipPoints[1], shipPoints[2], shipPoints[5], ship->primaryColor);
    DrawTriangle(shipPoints[1], shipPoints[5], shipPoints[6], ship->primaryColor);
    DrawTriangle(shipPoints[2], shipPoints[3], shipPoints[4], Fade(ship->primaryColor, 0.8f));
    DrawTriangle(shipPoints[2], shipPoints[4], shipPoints[5], Fade(ship->primaryColor, 0.8f));
    
    // Draw cockpit
    DrawCircle(ship->position.x + 10, ship->position.y, 5, ship->secondaryColor);
    DrawCircle(ship->position.x + 10, ship->position.y, 3, Fade(WHITE, 0.8f));
    
    // Draw engine glow
    float glowSize = 8.0f + ship->enginePulse * 4.0f;
    Color engineGlow = Fade(ship->engineColor, ship->engineGlow);
    DrawCircle(ship->position.x - 25, ship->position.y - 5, glowSize, engineGlow);
    DrawCircle(ship->position.x - 25, ship->position.y + 5, glowSize, engineGlow);
    DrawCircle(ship->position.x - 20, ship->position.y, glowSize * 0.7f, Fade(engineGlow, 0.5f));
    
    // Single weapon port only (no weapon modes)
    DrawCircle(ship->position.x + 15, ship->position.y, 2, ship->secondaryColor);
    
    // Draw shield effect
    if (ship->shield > 0 || ship->abilityActive[ABILITY_SHIELD_BURST]) {
        DrawShieldEffect(ship);
    }
    
    // Draw EMP blast effect
    if (ship->abilityActive[ABILITY_EMP_BLAST]) {
        float blastRadius = (1.0f - (ship->abilityDurations[ABILITY_EMP_BLAST] / ABILITY_DURATIONS[ABILITY_EMP_BLAST])) * 200.0f;
        DrawCircleLines(ship->position.x, ship->position.y, blastRadius, Fade(SKYBLUE, 0.5f));
        DrawCircleLines(ship->position.x, ship->position.y, blastRadius * 0.8f, Fade(WHITE, 0.3f));
    }
}

void DrawShieldEffect(const PlayerShip* ship) {
    float shieldRadius = 40.0f;
    Color shieldColor = ship->shieldColor;
    
    if (ship->abilityActive[ABILITY_SHIELD_BURST]) {
        // Enhanced shield during burst
        shieldRadius = 45.0f + sinf(ship->animTime * 10.0f) * 5.0f;
        shieldColor = (Color){0, 255, 255, 150};
        DrawCircle(ship->position.x, ship->position.y, shieldRadius, Fade(shieldColor, 0.2f));
    }
    
    // Hexagonal shield pattern
    float angle = ship->animTime * 2.0f;
    for (int i = 0; i < 6; i++) {
        float a1 = angle + (i * PI * 2.0f / 6.0f);
        float a2 = angle + ((i + 1) * PI * 2.0f / 6.0f);
        Vector2 p1 = {
            ship->position.x + cosf(a1) * shieldRadius,
            ship->position.y + sinf(a1) * shieldRadius
        };
        Vector2 p2 = {
            ship->position.x + cosf(a2) * shieldRadius,
            ship->position.y + sinf(a2) * shieldRadius
        };
        DrawLineEx(p1, p2, 2.0f, Fade(shieldColor, ship->shieldGlow * 0.5f));
    }
    
    // Inner shield glow
    if (ship->shield > 0) {
        float alpha = (ship->shield / ship->maxShield) * 0.3f;
        DrawCircle(ship->position.x, ship->position.y, shieldRadius * 0.9f, Fade(shieldColor, alpha));
    }
}

void DrawEngineTrail(const PlayerShip* ship) {
    // Draw particle trail
    for (int i = 0; i < 20; i++) {
        int index = (ship->trailIndex - i + 20) % 20;
        if (ship->trailAlpha[index] > 0) {
            float size = (20 - i) * 0.5f;
            Color trailColor = Fade(ship->engineColor, ship->trailAlpha[index] * (1.0f - i / 20.0f));
            DrawCircle(ship->trailPositions[index].x, ship->trailPositions[index].y - 5, size, trailColor);
            DrawCircle(ship->trailPositions[index].x, ship->trailPositions[index].y + 5, size, trailColor);
        }
    }
    
    // Boost trail effect
    if (ship->isBoosting) {
        for (int i = 0; i < 5; i++) {
            float offset = i * 10.0f;
            float alpha = (1.0f - i / 5.0f) * 0.5f;
            Color boostColor = Fade((Color){255, 150, 0, 255}, alpha);
            DrawRectangle(
                ship->position.x - 30 - offset,
                ship->position.y - 3,
                10,
                6,
                boostColor
            );
        }
    }
}

void DrawShipHUD(const PlayerShip* ship) {
    // Don't draw HUD if ship is not visible
    if (!ship->isVisible) {
        return;
    }
    
    int hudX = 10;
    int hudY = SCREEN_HEIGHT - 120;
    
    // Health bar
    DrawText("HULL", hudX, hudY, 12, WHITE);
    DrawRectangle(hudX + 50, hudY, 200, 15, Fade(RED, 0.3f));
    DrawRectangle(hudX + 50, hudY, (ship->health * 200) / ship->maxHealth, 15, RED);
    DrawRectangleLines(hudX + 50, hudY, 200, 15, WHITE);
    
    // Shield bar
    DrawText("SHIELD", hudX, hudY + 20, 12, WHITE);
    DrawRectangle(hudX + 50, hudY + 20, 200, 15, Fade(SKYBLUE, 0.3f));
    DrawRectangle(hudX + 50, hudY + 20, (ship->shield * 200) / ship->maxShield, 15, SKYBLUE);
    DrawRectangleLines(hudX + 50, hudY + 20, 200, 15, WHITE);
    
    // Energy bar
    DrawText("ENERGY", hudX, hudY + 40, 12, WHITE);
    DrawRectangle(hudX + 50, hudY + 40, 200, 15, Fade(YELLOW, 0.3f));
    DrawRectangle(hudX + 50, hudY + 40, (ship->energy * 200) / ship->maxEnergy, 15, YELLOW);
    DrawRectangleLines(hudX + 50, hudY + 40, 200, 15, WHITE);
    
    // Ability cooldowns
    DrawText("ABILITIES:", hudX, hudY + 65, 12, WHITE);
    const char* abilityNames[ABILITY_COUNT] = {
        "[SHIFT] BOOST", "[E] SHIELD", "[Q] EMP", "[R] OVERDRIVE"
    };
    
    for (int i = 0; i < ABILITY_COUNT; i++) {
        int x = hudX + 90 + i * 70;
        Color abilityColor = (ship->abilityCooldowns[i] <= 0 && ship->energy >= ABILITY_COSTS[i]) ? GREEN : GRAY;
        if (ship->abilityActive[i]) abilityColor = YELLOW;
        
        DrawText(abilityNames[i], x, hudY + 65, 10, abilityColor);
        if (ship->abilityCooldowns[i] > 0) {
            char cooldownText[16];
            snprintf(cooldownText, sizeof(cooldownText), "%.1fs", ship->abilityCooldowns[i]);
            DrawText(cooldownText, x + 10, hudY + 80, 10, RED);
        }
    }
    
    // Score and stats
    char statsText[256];
    snprintf(statsText, sizeof(statsText), 
        "Score: %d | Enemies: %d | Time: %.1fs", 
        ship->score, ship->enemiesDestroyed, ship->survivalTime);
    DrawText(statsText, hudX, hudY + 95, 12, WHITE);
}

void DamagePlayerShip(PlayerShip* ship, int damage) {
    // Check for invulnerability
    if (ship->abilityActive[ABILITY_SHIELD_BURST]) {
        return; // No damage during shield burst
    }
    
    // Apply damage to shield first
    if (ship->shield > 0) {
        float shieldDamage = fminf(damage, ship->shield);
        ship->shield -= shieldDamage;
        damage -= shieldDamage;
        ship->shieldGlow = 1.0f;
    }
    
    // Apply remaining damage to health
    if (damage > 0) {
        ship->health -= damage;
        if (ship->health < 0) {
            ship->health = 0;
        }
    }
    
    ship->lastDamageTime = GetTime();
}

void RepairPlayerShip(PlayerShip* ship, int amount) {
    ship->health = fminf(ship->maxHealth, ship->health + amount);
}

bool ActivateAbility(PlayerShip* ship, PlayerAbility ability) {
    if (ship->abilityCooldowns[ability] > 0 || ship->energy < ABILITY_COSTS[ability]) {
        return false;
    }
    
    ship->energy -= ABILITY_COSTS[ability];
    ship->abilityCooldowns[ability] = ABILITY_COOLDOWNS[ability];
    ship->abilityActive[ability] = true;
    ship->abilityDurations[ability] = ABILITY_DURATIONS[ability];
    
    // Apply immediate effects
    switch (ability) {
        case ABILITY_BOOST:
            ship->isBoosting = true;
            ship->currentSpeed = ship->boostSpeed;
            break;
        case ABILITY_SHIELD_BURST:
            ship->shield = ship->maxShield;
            ship->shieldGlow = 1.0f;
            break;
        case ABILITY_EMP_BLAST:
            // EMP effect handled in game logic
            break;
        case ABILITY_OVERDRIVE:
            ship->weaponHeat = 0;
            ship->overheated = false;
            break;
    }
    
    return true;
}

void SwitchWeaponMode(PlayerShip* ship, WeaponMode mode) {
    ship->weaponMode = mode;
    ship->chargeLevel = 0;
    ship->isCharging = false;
}

void ApplyShipUpgrade(PlayerShip* ship, int upgradeType, int level) {
    level = fminf(5, fmaxf(1, level)); // Clamp between 1-5
    
    switch (upgradeType) {
        case 0: // Weapon upgrade
            ship->upgrades.weaponLevel = level;
            // Increase damage, reduce heat generation
            break;
        case 1: // Shield upgrade
            ship->upgrades.shieldLevel = level;
            ship->maxShield = DEFAULT_SHIP_CONFIG.baseShield * (1.0f + (level - 1) * 0.25f);
            ship->shieldRegenRate = 5.0f * (1.0f + (level - 1) * 0.2f);
            break;
        case 2: // Engine upgrade
            ship->upgrades.engineLevel = level;
            ship->baseSpeed = DEFAULT_SHIP_CONFIG.baseSpeed * (1.0f + (level - 1) * 0.15f);
            ship->boostSpeed = ship->baseSpeed * 3.0f;
            break;
        case 3: // Energy upgrade
            ship->upgrades.energyLevel = level;
            ship->maxEnergy = DEFAULT_SHIP_CONFIG.baseEnergy * (1.0f + (level - 1) * 0.2f);
            ship->energyRegenRate = 10.0f * (1.0f + (level - 1) * 0.25f);
            break;
    }
}

void GetShipStats(const PlayerShip* ship, char* buffer, int bufferSize) {
    snprintf(buffer, bufferSize,
        "=== SHIP STATUS ===\n"
        "Hull: %d/%d\n"
        "Shield: %.0f/%.0f\n"
        "Energy: %.0f/%.0f\n"
        "Weapon: %s\n"
        "Score: %d\n"
        "Enemies Destroyed: %d\n"
        "Survival Time: %.1fs\n"
        "=== UPGRADES ===\n"
        "Weapon: Lv%d\n"
        "Shield: Lv%d\n"
        "Engine: Lv%d\n"
        "Energy: Lv%d\n",
        ship->health, ship->maxHealth,
        ship->shield, ship->maxShield,
        ship->energy, ship->maxEnergy,
        (ship->weaponMode == WEAPON_MODE_SINGLE) ? "Single" :
        (ship->weaponMode == WEAPON_MODE_DOUBLE) ? "Double" :
        (ship->weaponMode == WEAPON_MODE_SPREAD) ? "Spread" :
        (ship->weaponMode == WEAPON_MODE_RAPID) ? "Rapid" :
        (ship->weaponMode == WEAPON_MODE_CHARGE) ? "Charge" : "Wave",
        ship->score,
        ship->enemiesDestroyed,
        ship->survivalTime,
        ship->upgrades.weaponLevel,
        ship->upgrades.shieldLevel,
        ship->upgrades.engineLevel,
        ship->upgrades.energyLevel
    );
}
