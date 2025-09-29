#include "weapon.h"
#include "constants.h"
#include "utils.h"
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

void UpdateWeaponHeat(Player* player, float deltaTime) {
    if (player->overheated) {
        player->cooldownTime -= deltaTime;
        if (player->cooldownTime <= 0) {
            player->overheated = false;
            player->weaponHeat = 0;
            player->cooldownTime = 0;
        }
    } else {
        // Cool down weapon when not shooting
        if (player->weaponHeat > 0 && !IsKeyDown(KEY_SPACE) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            player->weaponHeat -= WEAPON_COOLDOWN_RATE * deltaTime;
            if (player->weaponHeat < 0) player->weaponHeat = 0;
        }
    }
}

void UpdateBullets(Game* game) {
    Bullet *bullets = game->bullets;
    Player *player = &game->player;
    float deltaTime = GetFrameTime();
    
    // Update weapon heat system
    UpdateWeaponHeat(player, deltaTime);
    
    // Shoot only if not overheated
    static float shootTimer = 0;
    shootTimer -= deltaTime;
    
    if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && 
        !player->overheated && shootTimer <= 0) {
        ShootBullet(bullets, player->position);
        shootTimer = WEAPON_FIRE_RATE;
        
        // Increase heat
        player->weaponHeat += WEAPON_HEAT_PER_SHOT;
        
        // Check for overheat
        if (player->weaponHeat >= player->maxHeat) {
            player->weaponHeat = player->maxHeat;
            player->overheated = true;
            player->cooldownTime = WEAPON_OVERHEAT_TIME;
            
            LogEvent(game, "[%.2f] Weapon overheated! 3 second cooldown initiated", game->gameTime);
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
            DrawRectangle(bullets[i].position.x - 5, bullets[i].position.y - 2, 10, 4, YELLOW);
        }
    }
}

void DrawWeaponHeatBar(const Player* player) {
    DrawText("Weapon Heat:", 10, 85, 20, WHITE);
    float heatBarWidth = 150;
    float heatFillWidth = (player->weaponHeat / player->maxHeat) * heatBarWidth;
    
    // Background bar
    DrawRectangle(120, 85, heatBarWidth, 20, DARKGRAY);
    
    // Heat fill bar - color changes based on heat level
    Color heatColor;
    if (player->overheated) {
        heatColor = PURPLE;  // Overheated
        DrawText("OVERHEATED!", 280, 85, 20, RED);
        DrawText(TextFormat("Cooldown: %.1fs", player->cooldownTime), 280, 105, 18, ORANGE);
    } else if (player->weaponHeat > 75) {
        heatColor = RED;     // Critical
    } else if (player->weaponHeat > 50) {
        heatColor = ORANGE;  // Warning
    } else if (player->weaponHeat > 25) {
        heatColor = YELLOW;  // Warm
    } else {
        heatColor = GREEN;   // Cool
    }
    
    DrawRectangle(120, 85, heatFillWidth, 20, heatColor);
    DrawRectangleLines(120, 85, heatBarWidth, 20, WHITE);
}
