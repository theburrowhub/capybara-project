#include "raylib.h"
#include "powerup.h"
#include "player_ship.h"
#include "constants.h"
#include "enemy_types.h"
#include "input_config.h"
#include "input_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SPAWN_INTERVAL 3.0f

int main(void) {
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Powerup System Showcase");
    SetTargetFPS(60);
    
    // Initialize input system
    InputConfig inputConfig;
    InputConfig_InitDefaults(&inputConfig);
    InputManager inputManager;
    InputManager_Init(&inputManager, &inputConfig);
    
    // Initialize systems
    PowerupSystem powerupSystem;
    InitPowerupSystem(&powerupSystem);
    
    PlayerShip player;
    InitPlayerShip(&player);
    
    // Test variables
    float spawnTimer = 0.0f;
    int currentPowerupType = 0;
    bool autoSpawn = true;
    int score = 0;
    
    // Statistics
    int totalSpawned = 0;
    int totalCollected = 0;
    
    // Instructions
    const char* instructions[] = {
        "POWERUP SHOWCASE",
        "",
        "CONTROLS:",
        "  Arrow Keys - Move ship",
        "  SPACE - Spawn random powerup",
        "  1-6 - Spawn specific powerup type",
        "  A - Toggle auto-spawn",
        "  R - Reset player stats",
        "",
        "POWERUP TYPES:",
        "  1: Energy (Yellow) - Restores 20% energy",
        "  2: Shield (Cyan) - Restores full shield",
        "  3: Hull (Red) - Repairs 20% hull",
        "  4: Weapon (Orange) - Accumulative upgrade",
    };
    
    bool showInstructions = true;
    
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        
        // Update input manager
        InputManager_Update(&inputManager);
        
        // Handle input
        HandlePlayerInput(&player, &inputManager);
        
        // Toggle instructions
        if (IsKeyPressed(KEY_I)) {
            showInstructions = !showInstructions;
        }
        
        // Toggle auto-spawn
        if (IsKeyPressed(KEY_A)) {
            autoSpawn = !autoSpawn;
        }
        
        // Reset player stats
        if (IsKeyPressed(KEY_R)) {
            player.health = player.maxHealth / 2;
            player.shield = player.maxShield / 2;
            player.energy = player.maxEnergy / 2;
            player.upgrades.weaponLevel = 1;
        }
        
        // Manual spawn specific powerup types
        if (IsKeyPressed(KEY_ONE)) {
            SpawnPowerup(&powerupSystem, POWERUP_ENERGY, 
                        (Vector2){SCREEN_WIDTH - 100, GetRandomValue(100, SCREEN_HEIGHT - 100)});
            totalSpawned++;
        }
        if (IsKeyPressed(KEY_TWO)) {
            SpawnPowerup(&powerupSystem, POWERUP_SHIELD,
                        (Vector2){SCREEN_WIDTH - 100, GetRandomValue(100, SCREEN_HEIGHT - 100)});
            totalSpawned++;
        }
        if (IsKeyPressed(KEY_THREE)) {
            SpawnPowerup(&powerupSystem, POWERUP_HULL,
                        (Vector2){SCREEN_WIDTH - 100, GetRandomValue(100, SCREEN_HEIGHT - 100)});
            totalSpawned++;
        }
        if (IsKeyPressed(KEY_FOUR)) {
            SpawnPowerup(&powerupSystem, POWERUP_WEAPON,
                        (Vector2){SCREEN_WIDTH - 100, GetRandomValue(100, SCREEN_HEIGHT - 100)});
            totalSpawned++;
        }
        
        // Spawn random powerup
        if (IsKeyPressed(KEY_SPACE)) {
            PowerupType type = GetRandomValue(0, POWERUP_TYPE_COUNT - 1);
            SpawnPowerup(&powerupSystem, type,
                        (Vector2){SCREEN_WIDTH - 100, GetRandomValue(100, SCREEN_HEIGHT - 100)});
            totalSpawned++;
        }
        
        // Auto-spawn powerups
        if (autoSpawn) {
            spawnTimer += deltaTime;
            if (spawnTimer >= SPAWN_INTERVAL) {
                spawnTimer = 0.0f;
                currentPowerupType = (currentPowerupType + 1) % POWERUP_TYPE_COUNT;
                SpawnPowerup(&powerupSystem, currentPowerupType,
                            (Vector2){SCREEN_WIDTH - 100, GetRandomValue(100, SCREEN_HEIGHT - 100)});
                totalSpawned++;
            }
        }
        
        // Update systems
        UpdateShipPhysics(&player, deltaTime);
        
        // Track collected before checking collisions
        int beforeCollected = powerupSystem.totalPowerupsCollected;
        
        UpdatePowerups(&powerupSystem, &player, deltaTime);
        CheckPowerupCollisions(&powerupSystem, &player, &score);
        
        // Update collected count
        if (powerupSystem.totalPowerupsCollected > beforeCollected) {
            totalCollected++;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground((Color){10, 10, 30, 255});
        
        // Draw stars background
        for (int i = 0; i < 100; i++) {
            int x = (i * 37) % SCREEN_WIDTH;
            int y = (i * 53) % SCREEN_HEIGHT;
            int brightness = 100 + (i * 17) % 155;
            DrawPixel(x, y, (Color){brightness, brightness, brightness, 255});
        }
        
        // Draw powerups
        DrawPowerups(&powerupSystem);
        
        // Draw player
        DrawPlayerShip(&player);
        
        // Draw HUD
        DrawShipHUD(&player);
        
        // Draw showcase info
        int infoX = SCREEN_WIDTH - 350;
        int infoY = 10;
        
        DrawRectangle(infoX - 10, infoY - 10, 340, 180, Fade(BLACK, 0.7f));
        DrawRectangleLines(infoX - 10, infoY - 10, 340, 180, DARKGRAY);
        
        DrawText("POWERUP SHOWCASE", infoX, infoY, 20, YELLOW);
        infoY += 30;
        
        DrawText(TextFormat("Active Powerups: %d / %d", 
                           powerupSystem.activePowerupCount, MAX_POWERUPS), 
                infoX, infoY, 16, WHITE);
        infoY += 20;
        
        DrawText(TextFormat("Total Spawned: %d", totalSpawned), 
                infoX, infoY, 16, WHITE);
        infoY += 20;
        
        DrawText(TextFormat("Total Collected: %d", totalCollected), 
                infoX, infoY, 16, GREEN);
        infoY += 20;
        
        DrawText(TextFormat("Collection Rate: %.1f%%", 
                           totalSpawned > 0 ? (totalCollected * 100.0f / totalSpawned) : 0.0f), 
                infoX, infoY, 16, SKYBLUE);
        infoY += 20;
        
        DrawText(TextFormat("Score: %d", score), infoX, infoY, 16, GOLD);
        infoY += 20;
        
        DrawText(TextFormat("Auto-Spawn: %s (A)", autoSpawn ? "ON" : "OFF"), 
                infoX, infoY, 16, autoSpawn ? GREEN : RED);
        infoY += 20;
        
        DrawText("Press SPACE for random", infoX, infoY, 14, GRAY);
        infoY += 18;
        DrawText("Press I for instructions", infoX, infoY, 14, GRAY);
        
        // Draw drop rate table
        int tableX = 10;
        int tableY = 150;
        
        DrawRectangle(tableX - 5, tableY - 5, 360, 400, Fade(BLACK, 0.8f));
        DrawRectangleLines(tableX - 5, tableY - 5, 360, 400, DARKGRAY);
        
        DrawText("DROP RATE TABLE", tableX, tableY, 18, YELLOW);
        tableY += 25;
        
        DrawText("Enemy Type    Energy Shield  Hull  Weapon", tableX, tableY, 12, LIGHTGRAY);
        tableY += 18;
        
        // Draw drop rates for each enemy type
        EnemyType types[] = {ENEMY_GRUNT, ENEMY_SWARM, ENEMY_SPEEDER, ENEMY_ZIGZAG, 
                            ENEMY_BOMBER, ENEMY_SHIELD, ENEMY_TANK, ENEMY_GHOST, 
                            ENEMY_ELITE, ENEMY_BOSS};
        
        for (int i = 0; i < 10; i++) {
            PowerupDropRate rates = GetDropRateForEnemy(types[i]);
            const char* name = GetEnemyTypeName(types[i]);
            Color typeColor = GetEnemyTypeColor(types[i]);
            
            char line[128];
            snprintf(line, sizeof(line), "%-12s  %4.1f%%  %4.1f%%  %3.1f%%  %4.1f%%",
                    name, rates.energyChance, rates.shieldChance, 
                    rates.hullChance, rates.weaponChance);
            
            DrawText(line, tableX, tableY, 12, typeColor);
            tableY += 16;
        }
        
        tableY += 10;
        DrawText("Legend:", tableX, tableY, 14, WHITE);
        tableY += 18;
        DrawCircle(tableX + 10, tableY + 5, 5, YELLOW);
        DrawText("Energy - Common, frequent drops", tableX + 25, tableY, 12, WHITE);
        tableY += 18;
        DrawCircle(tableX + 10, tableY + 5, 5, SKYBLUE);
        DrawText("Shield - Uncommon, instant restore", tableX + 25, tableY, 12, WHITE);
        tableY += 18;
        DrawCircle(tableX + 10, tableY + 5, 5, RED);
        DrawText("Hull - Very rare, critical repair", tableX + 25, tableY, 12, WHITE);
        tableY += 18;
        DrawCircle(tableX + 10, tableY + 5, 5, ORANGE);
        DrawText("Weapon - Uncommon, power upgrade", tableX + 25, tableY, 12, WHITE);
        
        // Draw instructions overlay
        if (showInstructions) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.85f));
            
            int instY = 50;
            for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
                int textWidth = MeasureText(instructions[i], 20);
                Color color = WHITE;
                
                if (i == 0) color = YELLOW;  // Title
                else if (strstr(instructions[i], "CONTROLS:") || 
                        strstr(instructions[i], "POWERUP TYPES:")) {
                    color = SKYBLUE;  // Section headers
                }
                
                DrawText(instructions[i], SCREEN_WIDTH/2 - textWidth/2, instY, 20, color);
                instY += 28;
            }
            
            int msgWidth = MeasureText("Press I to close", 20);
            DrawText("Press I to close", SCREEN_WIDTH/2 - msgWidth/2, 
                    SCREEN_HEIGHT - 50, 20, GRAY);
        }
        
        EndDrawing();
    }
    
    // Cleanup
    CleanupPowerupSystem(&powerupSystem);
    CloseWindow();
    
    return 0;
}
