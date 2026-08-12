#include "demo_common.h"
#include "weapon.h"
#include "projectile_manager.h"
#include "input_config.h"
#include <stdlib.h>

// ============================================================================
// Camera Initialization
// ============================================================================

Camera2D DemoCommon_InitCamera(float width, float height) {
    Camera2D camera;
    camera.target = (Vector2){ width / 2.0f, height / 2.0f };
    camera.offset = (Vector2){ width / 2.0f, height / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    return camera;
}

// ============================================================================
// Game System Initialization
// ============================================================================

void DemoCommon_InitGameTypes(void) {
    InitEnemyTypes();
    InitProjectileTypes();
}

void DemoCommon_InitBulletsArray(Bullet* bullets, int count) {
    if (!bullets) return;
    
    for (int i = 0; i < count; i++) {
        bullets[i].active = false;
        bullets[i].position = (Vector2){0, 0};
        bullets[i].speed = 12.0f;
        bullets[i].damage = 1.0f;
        bullets[i].velocityY = 0.0f;
        bullets[i].powerLevel = 0;
    }
}

void DemoCommon_InitEnemiesArray(EnemyEx* enemies, int count) {
    if (!enemies) return;
    
    for (int i = 0; i < count; i++) {
        enemies[i].active = false;
    }
}

void DemoCommon_InitProjectilesArray(Projectile* projectiles, int count) {
    if (!projectiles) return;
    
    ProjectileManager mgr = {
        .projectiles = projectiles,
        .maxProjectiles = count
    };
    ProjectileManager_InitAll(&mgr);
}

ExplosionSystem* DemoCommon_CreateExplosionSystem(void) {
    ExplosionSystem* system = (ExplosionSystem*)malloc(sizeof(ExplosionSystem));
    if (system) {
        InitExplosionSystem(system);
    }
    return system;
}

// ============================================================================
// Input System Initialization
// ============================================================================

InputManager* DemoCommon_CreateInputManager(InputConfig** outConfig) {
    // Allocate config
    InputConfig* config = (InputConfig*)malloc(sizeof(InputConfig));
    if (!config) {
        return NULL;
    }
    
    // Initialize config with defaults
    InputConfig_InitDefaults(config);
    
    // Allocate input manager
    InputManager* manager = (InputManager*)malloc(sizeof(InputManager));
    if (!manager) {
        free(config);
        return NULL;
    }
    
    // Initialize input manager with config
    InputManager_Init(manager, config);
    
    // Return both
    if (outConfig) {
        *outConfig = config;
    }
    
    return manager;
}

// ============================================================================
// Starfield Utilities
// ============================================================================

void DemoCommon_InitStarfield(Vector2* starfield, int count, int width, int height) {
    if (!starfield) return;
    
    for (int i = 0; i < count; i++) {
        starfield[i] = (Vector2){
            (float)GetRandomValue(0, width),
            (float)GetRandomValue(0, height)
        };
    }
}

void DemoCommon_DrawStarfield(Vector2* starfield, int count, float scrollSpeed, int width, int height) {
    if (!starfield) return;
    
    for (int i = 0; i < count; i++) {
        // Scroll stars
        starfield[i].x -= scrollSpeed;
        
        // Wrap around
        if (starfield[i].x < 0) {
            starfield[i].x = width;
            starfield[i].y = (float)GetRandomValue(0, height);
        }
        
        // Draw star with varying brightness
        int brightness = 100 + (i % 3) * 50;
        DrawPixel((int)starfield[i].x, (int)starfield[i].y, 
                 (Color){brightness, brightness, brightness, 255});
    }
}

