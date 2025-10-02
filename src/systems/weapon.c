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
    }
}

void ShootBullet(Bullet* bullets, Vector2 position) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].position = position;
            bullets[i].position.x += 25;
            bullets[i].bounds = (Rectangle){bullets[i].position.x - 5, bullets[i].position.y - 2, 10, 4};
            bullets[i].active = true;
            break;
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

void UpdateBullets(Game* game) {
    Bullet *bullets = game->bullets;
    PlayerShip *playerShip = game->playerShip;
    float deltaTime = GetFrameTime();
    
    // Update weapon heat system only if enabled
    if (WEAPON_OVERHEATING) {
        UpdateWeaponHeat(playerShip, deltaTime);
    }
    
    // Shoot only if not overheated - requires individual key press
    static float shootTimer = 0;
    shootTimer -= deltaTime;
    
    if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && 
        (!WEAPON_OVERHEATING || !playerShip->overheated) && shootTimer <= 0) {
        // Use original bullet system
        ShootBullet(bullets, playerShip->position);
        shootTimer = WEAPON_FIRE_RATE;
        
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
            bullets[i].bounds.x = bullets[i].position.x - 5;
            
            // Deactivate off-screen bullets
            if (bullets[i].position.x > SCREEN_WIDTH + 50) {
                bullets[i].active = false;
            }
        }
    }
}

void DrawBullets(const Bullet* bullets) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // IMPRESSIVE ENERGY BLAST BULLET
            float x = bullets[i].position.x;
            float y = bullets[i].position.y;
            
            // Energy trail effect
            for (int j = 8; j >= 0; j--) {
                float offset = j * 4.0f;
                float alpha = 1.0f - (j * 0.11f);
                float size = 10.0f - (j * 1.1f);
                
                // Outer energy field
                DrawCircle(x - offset, y, size + 3, Fade((Color){0, 255, 100, 255}, alpha * 0.25f));
                // Middle glow
                DrawCircle(x - offset, y, size + 1.5f, Fade((Color){100, 255, 150, 255}, alpha * 0.35f));
                // Inner trail
                DrawCircle(x - offset, y, size, Fade((Color){200, 255, 230, 255}, alpha * 0.45f));
            }
            
            // Main projectile - ENERGY BLAST
            // Outer glow
            DrawCircle(x, y, 15, Fade((Color){0, 255, 50, 255}, 0.3f));
            
            // Electric green outer ring
            DrawCircle(x, y, 10, Fade((Color){50, 255, 100, 255}, 0.5f));
            DrawCircleLines(x, y, 10, (Color){100, 255, 150, 255});
            
            // Bright green middle layer
            DrawCircle(x, y, 7, (Color){100, 255, 100, 255});
            
            // Hot white-green core
            DrawCircle(x, y, 4, (Color){200, 255, 200, 255});
            DrawCircle(x, y, 2, WHITE);
            
            // Leading edge power burst
            DrawCircle(x + 8, y, 6, Fade((Color){150, 255, 150, 255}, 0.7f));
            DrawCircle(x + 8, y, 4, (Color){200, 255, 200, 255});
            DrawCircle(x + 8, y, 2, WHITE);
            
            // Energy lightning effects around bullet
            float time = GetTime() * 10;
            for (int k = 0; k < 3; k++) {
                float angle = (k * 120 + time) * DEG2RAD;
                float sparkX = x + cosf(angle) * 12;
                float sparkY = y + sinf(angle) * 12;
                DrawLine(x, y, sparkX, sparkY, Fade((Color){150, 255, 150, 255}, 0.4f));
                DrawCircle(sparkX, sparkY, 2, Fade((Color){200, 255, 200, 255}, 0.6f));
            }
            
            // Power particles
            for (int p = 0; p < 5; p++) {
                float pOffset = p * 6;
                DrawPixel(x - pOffset - 10, y + ((p % 2) * 4 - 2), 
                          Fade((Color){150, 255, 150, 255}, 0.8f - p * 0.15f));
            }
            
            // Front shockwave effect
            DrawCircleLines(x + 10, y, 8, Fade((Color){200, 255, 200, 255}, 0.5f));
            DrawCircleLines(x + 10, y, 12, Fade((Color){150, 255, 150, 255}, 0.3f));
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
