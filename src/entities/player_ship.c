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

// Ability system removed - keeping gameplay simple

void InitPlayerShip(PlayerShip* ship) {
    // Position and physics (centered in play zone, not full screen)
    ship->position = (Vector2){150, PLAY_ZONE_TOP + PLAY_ZONE_HEIGHT/2};
    ship->velocity = (Vector2){0, 0};
    ship->bounds = (Rectangle){
        ship->position.x - 25,
        ship->position.y - 20,
        50,
        40
    };
    ship->rotation = 0.0f;
    
    // Health & Shield
    ship->maxHealth = DEFAULT_SHIP_CONFIG.baseHealth;
    ship->health = ship->maxHealth;
    ship->maxShield = 25;  // Offensive mode starts with 25 shield
    ship->shield = ship->maxShield;
    ship->shieldRegenRate = 2.0f;  // Slower regen (was 5.0f)
    ship->shieldRegenDelay = 5.0f; // Longer delay before regen (was 3.0f)
    ship->lastDamageTime = 0.0f;
    
    // Energy system
    ship->maxEnergy = DEFAULT_SHIP_CONFIG.baseEnergy;
    ship->energy = ship->maxEnergy;
    ship->energyRegenRate = 2.0f; // Very slow energy regen - 2 per second (takes 50 seconds to fully recharge)
    
    // Weapon system (simplified - no heat, no charging)
    ship->weaponMode = WEAPON_MODE_SINGLE;
    ship->fireTimer = 0.0f;
    
    // Movement (simplified - no boost)
    ship->baseSpeed = DEFAULT_SHIP_CONFIG.baseSpeed;
    ship->currentSpeed = ship->baseSpeed;
    ship->acceleration = 15.0f;
    
    // Abilities removed
    
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
    
    // Powerup tracking
    ship->weaponPowerupCount = 0;  // No weapon powerups collected yet
    
    // Energy mode system
    ship->energyMode = ENERGY_MODE_OFFENSIVE;  // Start in offensive mode
    ship->specialAbilityActive = false;
    ship->specialAbilityTimer = 0.0f;
    ship->specialAbilityHoldTimer = 0.0f;
    ship->energyFull = false;
    ship->energyDrainRate = 20.0f;  // Drains 20 energy per second (5 seconds to drain full bar)
    ship->lastEnergyDepletionTime = -10.0f;  // Start ready to use
    ship->energyRegenDelay = 5.0f;  // 5 second delay after depletion
    
    // Stats
    ship->score = 0;
    ship->enemiesDestroyed = 0;
    ship->survivalTime = 0.0f;
    
    // Visibility
    ship->isVisible = true;
    
    // Revive tracking
    ship->justRevived = false;
    ship->reviveEffectTimer = 0.0f;
}

void UpdatePlayerShip(PlayerShip* ship, float deltaTime, const InputManager* inputManager) {
    // Update survival time
    ship->survivalTime += deltaTime;
    
    // Update revive effect timer
    if (ship->reviveEffectTimer > 0.0f) {
        ship->reviveEffectTimer -= deltaTime;
        if (ship->reviveEffectTimer <= 0.0f) {
            ship->reviveEffectTimer = 0.0f;
            ship->justRevived = false;
        }
    }
    
    // Handle input
    HandlePlayerInput(ship, inputManager);
    
    // Update physics
    UpdateShipPhysics(ship, deltaTime);
    
    // Update visual effects
    UpdateShipEffects(ship, deltaTime);
    
    // Check if energy is full
    ship->energyFull = (ship->energy >= ship->maxEnergy);
    
    // Drain energy while special ability is active
    if (ship->specialAbilityActive) {
        // Offensive mode drains energy faster than defensive
        float drainRate = ship->energyDrainRate;
        if (ship->energyMode == ENERGY_MODE_OFFENSIVE) {
            drainRate *= 2.0f;  // Offensive drains 2x faster (40 per second = 2.5 seconds for full bar)
        }
        
        ship->energy -= drainRate * deltaTime;
        if (ship->energy <= 0) {
            ship->energy = 0;
            ship->specialAbilityActive = false;
            ship->lastEnergyDepletionTime = GetTime();  // Mark when energy was depleted
        }
    }
    
    // Shield regeneration (affected by energy mode)
    if (ship->shield < ship->maxShield) {
        float timeSinceDamage = GetTime() - ship->lastDamageTime;
        if (timeSinceDamage > ship->shieldRegenDelay) {
            float regenRate = ship->shieldRegenRate;
            
            // Defensive mode with FULL energy: faster shield regen bonus
            if (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->energyFull) {
                regenRate *= 2.0f;  // 2x faster shield regen bonus only when energy is FULL
            }
            
            ship->shield += regenRate * deltaTime;
            if (ship->shield > ship->maxShield) {
                ship->shield = ship->maxShield;
            }
            ship->shieldGlow = fminf(1.0f, ship->shield / ship->maxShield);
        }
    }
    
    // Energy regeneration with delay after depletion
    if (ship->energy < ship->maxEnergy) {
        float timeSinceDepletion = GetTime() - ship->lastEnergyDepletionTime;
        
        // Only regenerate if 5 seconds have passed since last depletion
        if (timeSinceDepletion > ship->energyRegenDelay) {
            ship->energy += ship->energyRegenRate * deltaTime;
            if (ship->energy > ship->maxEnergy) {
                ship->energy = ship->maxEnergy;
            }
        }
    }
    
    // Update animation timers
    ship->animTime += deltaTime;
    ship->enginePulse = (sinf(ship->animTime * 5.0f) + 1.0f) * 0.5f;
}

void HandlePlayerInput(PlayerShip* ship, const InputManager* inputManager) {
    Vector2 inputDir = {0, 0};
    
    // Safety check - if no input manager, use direct keyboard input as fallback
    if (!inputManager) {
        // Fallback to direct keyboard input
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) inputDir.y = -1;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) inputDir.y = 1;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            inputDir.x = -1;
            ship->bankAngle = fmaxf(-30.0f, ship->bankAngle - 2.0f);
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            inputDir.x = 1;
            ship->bankAngle = fminf(30.0f, ship->bankAngle + 2.0f);
        }
        
        if (inputDir.x != 0 && inputDir.y != 0) {
            float length = sqrtf(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
            inputDir.x /= length;
            inputDir.y /= length;
        }
        
        ship->velocity.x = inputDir.x * ship->currentSpeed;
        ship->velocity.y = inputDir.y * ship->currentSpeed;
        return;
    }
    
    // Weapon mode switching with number keys 1-6
    if (IsKeyPressed(KEY_ONE)) {
        ship->weaponMode = WEAPON_MODE_SINGLE;
    } else if (IsKeyPressed(KEY_TWO)) {
        ship->weaponMode = WEAPON_MODE_DOUBLE;
    } else if (IsKeyPressed(KEY_THREE)) {
        ship->weaponMode = WEAPON_MODE_SPREAD;
    } else if (IsKeyPressed(KEY_FOUR)) {
        ship->weaponMode = WEAPON_MODE_RAPID;
    } else if (IsKeyPressed(KEY_FIVE)) {
        ship->weaponMode = WEAPON_MODE_CHARGE;
    } else if (IsKeyPressed(KEY_SIX)) {
        ship->weaponMode = WEAPON_MODE_DUAL;
    } else if (InputManager_IsActionPressed(inputManager, ACTION_SWITCH_WEAPON_MODE)) {
        // Cycle through weapon modes in order with R key or gamepad button
        ship->weaponMode = (WeaponMode)((ship->weaponMode + 1) % WEAPON_MODE_COUNT);
    }
    
    // Energy mode switching using InputManager
    if (InputManager_IsActionPressed(inputManager, ACTION_SWITCH_ENERGY_MODE)) {
        if (ship->energyMode == ENERGY_MODE_OFFENSIVE) {
            ship->energyMode = ENERGY_MODE_DEFENSIVE;
            // Switch to defensive mode: increase max shield from 25 to 50
            float shieldPercent = ship->shield / ship->maxShield;
            ship->maxShield = 50;
            ship->shield = shieldPercent * ship->maxShield;  // Maintain proportional shield
        } else {
            ship->energyMode = ENERGY_MODE_OFFENSIVE;
            // Switch to offensive mode: decrease max shield from 50 to 25
            float shieldPercent = ship->shield / ship->maxShield;
            ship->maxShield = 25;
            ship->shield = shieldPercent * ship->maxShield;  // Maintain proportional shield
        }
    }
    
    // Special ability activation - use InputManager
    if (InputManager_IsActionDown(inputManager, ACTION_SPECIAL_ABILITY) && ship->energy > 0) {
        // Activate special ability while action is held and energy is available
        ship->specialAbilityActive = true;
    } else {
        // Deactivate when released or energy runs out
        if (ship->specialAbilityActive) {
            ship->specialAbilityActive = false;
        }
    }
    
    // Keyboard/Gamepad movement input using InputManager
    if (InputManager_IsActionDown(inputManager, ACTION_MOVE_UP)) {
        inputDir.y = -1;
    }
    if (InputManager_IsActionDown(inputManager, ACTION_MOVE_DOWN)) {
        inputDir.y = 1;
    }
    if (InputManager_IsActionDown(inputManager, ACTION_MOVE_LEFT)) {
        inputDir.x = -1;
        ship->bankAngle = fmaxf(-30.0f, ship->bankAngle - 2.0f);
    }
    if (InputManager_IsActionDown(inputManager, ACTION_MOVE_RIGHT)) {
        inputDir.x = 1;
        ship->bankAngle = fminf(30.0f, ship->bankAngle + 2.0f);
    }
    
    // Normalize diagonal movement
    if (inputDir.x != 0 && inputDir.y != 0) {
        float length = sqrtf(inputDir.x * inputDir.x + inputDir.y * inputDir.y);
        inputDir.x /= length;
        inputDir.y /= length;
    }
    
    // Return banking to neutral when not moving
    if (inputDir.x == 0) {
        if (ship->bankAngle > 0) {
            ship->bankAngle = fmaxf(0, ship->bankAngle - 3.0f);
        } else if (ship->bankAngle < 0) {
            ship->bankAngle = fminf(0, ship->bankAngle + 3.0f);
        }
    }
    
    // Apply movement
    ship->velocity.x = inputDir.x * ship->currentSpeed;
    ship->velocity.y = inputDir.y * ship->currentSpeed;
}

void UpdateShipPhysics(PlayerShip* ship, float deltaTime) {
    // Update position
    ship->position.x += ship->velocity.x;
    ship->position.y += ship->velocity.y;
    
    // Keep ship on screen - constrained to play zone (above HUD)
    float margin = 30.0f;
    if (ship->position.x < margin) ship->position.x = margin;
    if (ship->position.x > SCREEN_WIDTH - margin) ship->position.x = SCREEN_WIDTH - margin;  // Allow reaching right border
    if (ship->position.y < PLAY_ZONE_TOP + margin) ship->position.y = PLAY_ZONE_TOP + margin;
    if (ship->position.y > PLAY_ZONE_BOTTOM - margin) ship->position.y = PLAY_ZONE_BOTTOM - margin;
    
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
        ship->trailAlpha[i] = fmaxf(0.0f, ship->trailAlpha[i] - deltaTime * 2.0f);
    }
    
    // Update engine glow based on movement
    float speed = sqrtf(ship->velocity.x * ship->velocity.x + ship->velocity.y * ship->velocity.y);
    ship->engineGlow = 0.5f + (speed / ship->baseSpeed) * 0.5f;
}

// Abilities system removed - keeping gameplay simple for now

void UpdateShipEffects(PlayerShip* ship, float deltaTime) {
    // Update shield glow when damaged
    if (ship->shield < ship->maxShield * 0.3f) {
        ship->shieldGlow = (sinf(ship->animTime * 8.0f) + 1.0f) * 0.25f;
    }
    
    // Weapon heat system removed - no overheating
    
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
    
    // Draw shield effect - always show when:
    // 1. Shield is active, or
    // 2. Defensive mode with special ability, or
    // 3. Just revived (shows golden aura)
    if (ship->shield > 0 || 
        (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) ||
        ship->reviveEffectTimer > 0.0f) {
        DrawShieldEffect(ship);
    }
    
    // Ability effects removed
}

void DrawShieldEffect(const PlayerShip* ship) {
    float shieldRadius = 40.0f;
    Color shieldColor = ship->shieldColor;
    
    // REVIVE EFFECT - Special golden aura when just revived
    if (ship->reviveEffectTimer > 0.0f) {
        float reviveIntensity = ship->reviveEffectTimer / 2.0f;  // Fades over 2 seconds
        Color goldColor = (Color){255, 215, 0, 255};  // Golden color
        
        // Expanding rings effect
        float pulse = (sinf(ship->animTime * 10.0f) + 1.0f) * 0.5f;
        float ringRadius1 = 50.0f + pulse * 20.0f;
        float ringRadius2 = 70.0f + pulse * 15.0f;
        
        DrawCircle(ship->position.x, ship->position.y, ringRadius1, 
                  Fade(goldColor, 0.3f * reviveIntensity));
        DrawCircleLines(ship->position.x, ship->position.y, ringRadius1, 
                       Fade(goldColor, 0.8f * reviveIntensity));
        DrawCircleLines(ship->position.x, ship->position.y, ringRadius2, 
                       Fade(goldColor, 0.5f * reviveIntensity));
        
        // Energy particles swirling around
        for (int i = 0; i < 8; i++) {
            float particleAngle = ship->animTime * 3.0f + (i * PI * 2.0f / 8.0f);
            float particleRadius = 45.0f + sinf(ship->animTime * 5.0f + i) * 10.0f;
            Vector2 particlePos = {
                ship->position.x + cosf(particleAngle) * particleRadius,
                ship->position.y + sinf(particleAngle) * particleRadius
            };
            DrawCircle(particlePos.x, particlePos.y, 3.0f, 
                      Fade(goldColor, 0.9f * reviveIntensity));
        }
        
        shieldRadius = 50.0f;  // Larger shield during revive
        shieldColor = (Color){255, 215, 100, 220};  // Golden shield tint
    }
    // Enhanced shield in defensive mode - bigger and brighter
    else if (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) {
        shieldRadius = 60.0f;  // Much bigger shield
        shieldColor = (Color){100, 255, 255, 200};  // Brighter cyan
        
        // Extra outer ring for enhanced shield
        DrawCircleLines(ship->position.x, ship->position.y, shieldRadius + 5, Fade(shieldColor, 0.7f));
        DrawCircleLines(ship->position.x, ship->position.y, shieldRadius + 10, Fade(shieldColor, 0.4f));
        
        // Pulsing effect
        float pulse = (sinf(ship->animTime * 8.0f) + 1.0f) * 0.5f;
        DrawCircle(ship->position.x, ship->position.y, shieldRadius, Fade(shieldColor, 0.2f + pulse * 0.1f));
    }
    
    // Hexagonal shield pattern
    float angle = ship->animTime * 2.0f;
    float lineThickness = (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) ? 3.0f : 2.0f;
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
        float glowAlpha = (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) ? 0.8f : ship->shieldGlow * 0.5f;
        DrawLineEx(p1, p2, lineThickness, Fade(shieldColor, glowAlpha));
    }
    
    // Inner shield glow
    if (ship->shield > 0) {
        float alpha = (ship->shield / ship->maxShield) * 0.3f;
        if (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) {
            alpha = 0.4f;  // More visible in enhanced mode
        }
        DrawCircle(ship->position.x, ship->position.y, shieldRadius * 0.9f, Fade(shieldColor, alpha));
    }
}

void DrawEngineTrail(const PlayerShip* ship) {
    // Draw particle trail (boost effects removed)
    for (int i = 0; i < 20; i++) {
        int index = (ship->trailIndex - i + 20) % 20;
        if (ship->trailAlpha[index] > 0) {
            float size = (20 - i) * 0.5f;
            Color trailColor = Fade(ship->engineColor, ship->trailAlpha[index] * (1.0f - i / 20.0f));
            DrawCircle(ship->trailPositions[index].x, ship->trailPositions[index].y - 5, size, trailColor);
            DrawCircle(ship->trailPositions[index].x, ship->trailPositions[index].y + 5, size, trailColor);
        }
    }
}

void DrawShipHUD(const PlayerShip* ship) {
    // Don't draw HUD if ship is not visible
    if (!ship->isVisible) {
        return;
    }
    
    // Position in bottom HUD area (left side, compact layout)
    int hudX = 10;
    int hudY = PLAY_ZONE_BOTTOM + 8;
    int barWidth = 180;  // Slightly narrower bars
    int barHeight = 12;  // Slightly shorter bars
    int spacing = 18;    // Tighter spacing
    
    // Health bar
    DrawText("HULL", hudX, hudY, 11, WHITE);
    DrawRectangle(hudX + 50, hudY, barWidth, barHeight, Fade(RED, 0.3f));
    DrawRectangle(hudX + 50, hudY, (ship->health * barWidth) / ship->maxHealth, barHeight, RED);
    DrawRectangleLines(hudX + 50, hudY, barWidth, barHeight, WHITE);
    
    // Shield bar
    DrawText("SHIELD", hudX, hudY + spacing, 11, WHITE);
    Color shieldBarColor = SKYBLUE;
    if (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) {
        shieldBarColor = (Color){100, 255, 255, 255};  // Brighter when enhanced
    }
    DrawRectangle(hudX + 50, hudY + spacing, barWidth, barHeight, Fade(shieldBarColor, 0.3f));
    DrawRectangle(hudX + 50, hudY + spacing, (ship->shield * barWidth) / ship->maxShield, barHeight, shieldBarColor);
    DrawRectangleLines(hudX + 50, hudY + spacing, barWidth, barHeight, WHITE);
    
    // Energy bar
    DrawText("ENERGY", hudX, hudY + spacing * 2, 11, WHITE);
    Color energyColor = YELLOW;
    DrawRectangle(hudX + 50, hudY + spacing * 2, barWidth, barHeight, Fade(energyColor, 0.3f));
    DrawRectangle(hudX + 50, hudY + spacing * 2, (ship->energy * barWidth) / ship->maxEnergy, barHeight, energyColor);
    DrawRectangleLines(hudX + 50, hudY + spacing * 2, barWidth, barHeight, WHITE);
    
    // Energy status indicator (compact)
    float timeSinceDepletion = GetTime() - ship->lastEnergyDepletionTime;
    if (ship->energy <= 0 && timeSinceDepletion < ship->energyRegenDelay) {
        float remainingDelay = ship->energyRegenDelay - timeSinceDepletion;
        DrawText(TextFormat("%.1fs", remainingDelay), hudX + 235, hudY + spacing * 2, 10, RED);
    } else if (ship->energyFull) {
        const char* bonusText = (ship->energyMode == ENERGY_MODE_OFFENSIVE) ? "2X" : "2X";
        DrawText(bonusText, hudX + 235, hudY + spacing * 2, 10, GREEN);
    }
    
    // Mode display (compact)
    DrawText("MODE", hudX, hudY + spacing * 3, 11, WHITE);
    const char* modeText = (ship->energyMode == ENERGY_MODE_OFFENSIVE) ? "OFFENSE" : "DEFENSE";
    Color modeColor = (ship->energyMode == ENERGY_MODE_OFFENSIVE) ? ORANGE : SKYBLUE;
    
    if (ship->energyFull) {
        DrawRectangle(hudX + 50, hudY + spacing * 3, 90, barHeight, modeColor);
        DrawText(modeText, hudX + 52, hudY + spacing * 3 + 2, 10, BLACK);
        DrawText("*", hudX + 42, hudY + spacing * 3, 14, modeColor);
    } else {
        DrawRectangle(hudX + 50, hudY + spacing * 3, 90, barHeight, Fade(modeColor, 0.5f));
        DrawText(modeText, hudX + 52, hudY + spacing * 3 + 2, 10, WHITE);
    }
    DrawRectangleLines(hudX + 50, hudY + spacing * 3, 90, barHeight, modeColor);
    
    // Special ability status (compact)
    if (ship->specialAbilityActive) {
        const char* abilityText = (ship->energyMode == ENERGY_MODE_OFFENSIVE) ? "DEVASTATE!" : "ENHANCED!";
        DrawText(abilityText, hudX + 145, hudY + spacing * 3 + 2, 10, GREEN);
    } else if (ship->energy > 0) {
        DrawText("(Press E)", hudX + 145, hudY + spacing * 3 + 2, 9, Fade(WHITE, 0.6f));
    }
    
    // REVIVE NOTIFICATION - Big clear text in center of play zone when revived
    if (ship->reviveEffectTimer > 0.0f) {
        float reviveIntensity = ship->reviveEffectTimer / 2.0f;
        Color mainTextColor = (Color){255, 215, 0, (unsigned char)(255 * reviveIntensity)};
        Color subTextColor = (Color){255, 255, 200, (unsigned char)(255 * reviveIntensity)};
        
        // Pulsing effect
        float pulse = (sinf(GetTime() * 8.0f) + 1.0f) * 0.5f;
        int mainFontSize = 48 + (int)(pulse * 8);
        
        // Main revive message - clearer without emojis
        const char* reviveText = "SHIP REVIVED!";
        int textWidth = MeasureText(reviveText, mainFontSize);
        int centerX = SCREEN_WIDTH / 2 - textWidth / 2;
        int centerY = PLAY_ZONE_TOP + PLAY_ZONE_HEIGHT / 2 - 100;
        
        // Enhanced shadow with outline for maximum readability
        for (int ox = -2; ox <= 2; ox += 2) {
            for (int oy = -2; oy <= 2; oy += 2) {
                if (ox != 0 || oy != 0) {
                    DrawText(reviveText, centerX + ox, centerY + oy, mainFontSize, 
                            Fade(BLACK, reviveIntensity * 0.8f));
                }
            }
        }
        DrawText(reviveText, centerX, centerY, mainFontSize, mainTextColor);
        
        // Clearer sub-text with better formatting
        const char* weaponText = TextFormat("Weapon Power: Level %d", ship->weaponPowerupCount);
        int weaponWidth = MeasureText(weaponText, 22);
        DrawText(weaponText, SCREEN_WIDTH / 2 - weaponWidth / 2 + 1, centerY + 58, 22, 
                Fade(BLACK, reviveIntensity * 0.8f));  // Shadow
        DrawText(weaponText, SCREEN_WIDTH / 2 - weaponWidth / 2, centerY + 57, 22, subTextColor);
        
        const char* hullText = TextFormat("Hull Restored: %d HP", ship->health);
        int hullWidth = MeasureText(hullText, 22);
        DrawText(hullText, SCREEN_WIDTH / 2 - hullWidth / 2 + 1, centerY + 83, 22, 
                Fade(BLACK, reviveIntensity * 0.8f));  // Shadow
        DrawText(hullText, SCREEN_WIDTH / 2 - hullWidth / 2, centerY + 82, 22, subTextColor);
    }
    
    // Compact HUD complete - weapon mode and damage info visible through gameplay
}

void DamagePlayerShip(PlayerShip* ship, int damage) {
    // Abilities removed - no invulnerability check
    
    // Enhanced shield in defensive mode absorbs half damage
    if (ship->energyMode == ENERGY_MODE_DEFENSIVE && ship->specialAbilityActive) {
        damage = damage / 2;  // Absorb half damage
        if (damage < 1) damage = 1;  // At least 1 damage
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
    
    // WEAPON POWERUP REVIVE SYSTEM
    // If health reaches 0 and we have weapon powerups, use one to revive
    if (ship->health <= 0 && ship->weaponPowerupCount > 0) {
        // Consume one weapon powerup level to revive
        ship->weaponPowerupCount--;
        
        // Restore 1/3 of max health
        ship->health = ship->maxHealth / 3;
        
        // Also restore some shield for survival
        ship->shield = ship->maxShield * 0.5f;
        ship->shieldGlow = 1.0f;
        
        // Set revive flags for visual feedback
        ship->justRevived = true;
        ship->reviveEffectTimer = 2.0f;  // 2 second revive effect
    }
    
    ship->lastDamageTime = GetTime();
}

void RepairPlayerShip(PlayerShip* ship, int amount) {
    ship->health = fminf(ship->maxHealth, ship->health + amount);
}

float CalculateDamagePerShot(const PlayerShip* ship) {
    // Calculate base damage per bullet based on weapon mode
    float damagePerBullet = 1.0f;
    
    switch (ship->weaponMode) {
        case WEAPON_MODE_SINGLE:
            damagePerBullet = 3.0f;
            break;
        case WEAPON_MODE_DOUBLE:
            damagePerBullet = 1.5f;
            break;
        case WEAPON_MODE_SPREAD:
            damagePerBullet = 1.0f;
            break;
        case WEAPON_MODE_RAPID:
            damagePerBullet = 1.5f;
            break;
        case WEAPON_MODE_DUAL:
            damagePerBullet = 1.5f;
            break;
        case WEAPON_MODE_CHARGE:
            // Variable based on charge level
            damagePerBullet = 1.0f;
            break;
        default:
            damagePerBullet = 1.0f;
            break;
    }
    
    // Apply weapon powerup multiplier
    float powerMultiplier = 1.0f;
    switch (ship->weaponPowerupCount) {
        case 0: powerMultiplier = 1.0f; break;
        case 1: powerMultiplier = 1.5f; break;
        case 2: powerMultiplier = 2.0f; break;
        case 3: powerMultiplier = 2.5f; break;
        default: powerMultiplier = 1.0f; break;
    }
    damagePerBullet *= powerMultiplier;
    
    // Apply offensive mode bonus if active (applied to each bullet)
    if (ship->energyMode == ENERGY_MODE_OFFENSIVE && ship->energyFull) {
        damagePerBullet *= 2.0f;  // 2x bonus with full energy
    }
    
    // Return damage per individual bullet
    return damagePerBullet;
}

// ActivateAbility function removed - abilities system disabled

// Weapon mode switching removed - always SINGLE mode

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
        (ship->weaponMode == WEAPON_MODE_CHARGE) ? "Charge" : "Dual",
        ship->score,
        ship->enemiesDestroyed,
        ship->survivalTime,
        ship->upgrades.weaponLevel,
        ship->upgrades.shieldLevel,
        ship->upgrades.engineLevel,
        ship->upgrades.energyLevel
    );
}

