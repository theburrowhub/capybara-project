#include "raylib.h"
#include "projectile_types.h"
#include "enemy_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define MAX_PROJECTILES 100

typedef struct {
    Projectile projectiles[MAX_PROJECTILES];
    int projectileCount;
    float globalTimer;
    bool paused;
    bool showTrails;
    bool showInfo;
    int selectedProjectileType;
    int selectedEnemyType;
    Vector2 mouseTarget;
    float autoFireTimer;
    bool autoFire;
    
    // Sprite resources
    Texture2D projectileSpriteSheet;
    Rectangle projectileSpriteRects[PROJECTILE_TYPE_COUNT];
    bool spritesLoaded;
} ShowcaseState;

void InitShowcase(ShowcaseState* state) {
    // Initialize projectile types
    InitProjectileTypes();
    InitEnemyTypes();
    
    // Clear projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        state->projectiles[i].active = false;
    }
    
    state->projectileCount = 0;
    state->globalTimer = 0;
    state->paused = false;
    state->showTrails = true;
    state->showInfo = true;
    state->selectedProjectileType = 0;
    state->selectedEnemyType = 0;
    state->mouseTarget = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    state->autoFireTimer = 0;
    state->autoFire = false;
    
    // Load projectile sprites
    state->spritesLoaded = false;
    if (FileExists("assets/sprites/projectile_spritesheet.png")) {
        state->projectileSpriteSheet = LoadTexture("assets/sprites/projectile_spritesheet.png");
        state->spritesLoaded = true;
        
        // Define sprite rectangles (32x32 sprites, 4 per row)
        for (int i = 0; i < PROJECTILE_TYPE_COUNT; i++) {
            int col = i % 4;
            int row = i / 4;
            state->projectileSpriteRects[i] = (Rectangle){
                col * 32,
                row * 32,
                32,
                32
            };
        }
        printf("Projectile sprites loaded successfully!\n");
    } else {
        printf("Warning: Projectile sprites not found. Using procedural graphics.\n");
        printf("Run 'make generate_projectiles' to create sprites.\n");
    }
}

void FireProjectile(ShowcaseState* state, ProjectileType type, Vector2 position, Vector2 target) {
    // Find inactive projectile slot
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!state->projectiles[i].active) {
            InitializeProjectile(&state->projectiles[i], type, position, target, false);
            state->projectileCount++;
            break;
        }
    }
}

void FireEnemyPattern(ShowcaseState* state, EnemyType enemyType, Vector2 position) {
    const EnemyWeaponConfig* config = GetEnemyWeaponConfig(enemyType);
    
    // Fire burst pattern
    for (int i = 0; i < config->burstCount; i++) {
        float angle = 0;
        
        if (config->spreadAngle > 0 && config->burstCount > 1) {
            // Calculate spread angle for this shot
            if (config->spreadAngle >= 360) {
                // Full circle spread
                angle = (360.0f / config->burstCount) * i;
            } else {
                // Arc spread
                float startAngle = -config->spreadAngle / 2.0f;
                float angleStep = config->spreadAngle / (config->burstCount - 1);
                angle = startAngle + angleStep * i;
            }
        }
        
        // Calculate target position with spread
        Vector2 direction = {
            state->mouseTarget.x - position.x,
            state->mouseTarget.y - position.y
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
            position.x + rotatedDir.x * 1000,
            position.y + rotatedDir.y * 1000
        };
        
        // Alternate between primary and secondary projectiles for variety
        ProjectileType projType = (i % 2 == 0) ? 
            config->primaryProjectile : config->secondaryProjectile;
        
        FireProjectile(state, projType, position, target);
    }
}

void UpdateShowcase(ShowcaseState* state) {
    if (!state->paused) {
        state->globalTimer += GetFrameTime();
        
        // Update all projectiles
        state->projectileCount = 0;
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (state->projectiles[i].active) {
                UpdateProjectile(&state->projectiles[i], GetFrameTime());
                
                // Remove projectiles that go off screen
                if (state->projectiles[i].position.x < -50 || 
                    state->projectiles[i].position.x > SCREEN_WIDTH + 50 ||
                    state->projectiles[i].position.y < -50 || 
                    state->projectiles[i].position.y > SCREEN_HEIGHT + 50) {
                    state->projectiles[i].active = false;
                }
                
                if (state->projectiles[i].active) {
                    state->projectileCount++;
                }
            }
        }
        
        // Auto fire
        if (state->autoFire) {
            state->autoFireTimer += GetFrameTime();
            const EnemyWeaponConfig* config = GetEnemyWeaponConfig(state->selectedEnemyType);
            float fireInterval = 1.0f / config->fireRate;
            
            if (state->autoFireTimer >= fireInterval) {
                state->autoFireTimer = 0;
                
                // Fire from center
                Vector2 firePos = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
                FireEnemyPattern(state, state->selectedEnemyType, firePos);
            }
        }
    }
    
    // Update mouse target
    state->mouseTarget = GetMousePosition();
    
    // Input handling
    if (IsKeyPressed(KEY_SPACE)) {
        state->paused = !state->paused;
    }
    
    if (IsKeyPressed(KEY_T)) {
        state->showTrails = !state->showTrails;
    }
    
    if (IsKeyPressed(KEY_I)) {
        state->showInfo = !state->showInfo;
    }
    
    if (IsKeyPressed(KEY_A)) {
        state->autoFire = !state->autoFire;
        state->autoFireTimer = 0;
    }
    
    if (IsKeyPressed(KEY_C)) {
        // Clear all projectiles
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            state->projectiles[i].active = false;
        }
        state->projectileCount = 0;
    }
    
    // Select projectile type with number keys
    if (IsKeyPressed(KEY_ONE)) state->selectedProjectileType = 0;
    if (IsKeyPressed(KEY_TWO)) state->selectedProjectileType = 1;
    if (IsKeyPressed(KEY_THREE)) state->selectedProjectileType = 2;
    if (IsKeyPressed(KEY_FOUR)) state->selectedProjectileType = 3;
    
    // Cycle through enemy types
    if (IsKeyPressed(KEY_LEFT)) {
        state->selectedEnemyType--;
        if (state->selectedEnemyType < 0) state->selectedEnemyType = ENEMY_TYPE_COUNT - 1;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        state->selectedEnemyType++;
        if (state->selectedEnemyType >= ENEMY_TYPE_COUNT) state->selectedEnemyType = 0;
    }
    
    // Manual fire
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 firePos = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
        FireProjectile(state, state->selectedProjectileType, firePos, state->mouseTarget);
    }
    
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        Vector2 firePos = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
        FireEnemyPattern(state, state->selectedEnemyType, firePos);
    }
}

void DrawShowcase(ShowcaseState* state) {
    BeginDrawing();
    ClearBackground((Color){10, 10, 20, 255});
    
    // Draw grid
    for (int i = 0; i <= SCREEN_WIDTH; i += 50) {
        DrawLine(i, 0, i, SCREEN_HEIGHT, Fade(DARKGRAY, 0.2f));
    }
    for (int i = 0; i <= SCREEN_HEIGHT; i += 50) {
        DrawLine(0, i, SCREEN_WIDTH, i, Fade(DARKGRAY, 0.2f));
    }
    
    // Draw firing point
    DrawCircle(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 20, Fade(WHITE, 0.3f));
    DrawCircleLines(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 20, WHITE);
    DrawText("FIRE POINT", SCREEN_WIDTH/2 - 35, SCREEN_HEIGHT/2 + 25, 10, WHITE);
    
    // Draw target crosshair
    DrawLine(state->mouseTarget.x - 10, state->mouseTarget.y, 
            state->mouseTarget.x + 10, state->mouseTarget.y, RED);
    DrawLine(state->mouseTarget.x, state->mouseTarget.y - 10, 
            state->mouseTarget.x, state->mouseTarget.y + 10, RED);
    DrawCircleLines(state->mouseTarget.x, state->mouseTarget.y, 15, RED);
    
    // Draw all projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (state->projectiles[i].active) {
            if (!state->showTrails) {
                // Clear trail if not showing
                for (int j = 0; j < 10; j++) {
                    state->projectiles[i].trailPositions[j] = state->projectiles[i].position;
                }
            }
            
            // Draw with sprites if available, otherwise procedural
            if (state->spritesLoaded) {
                const Projectile* proj = &state->projectiles[i];
                Rectangle sourceRect = state->projectileSpriteRects[proj->type];
                Vector2 position = {
                    proj->position.x - 16,
                    proj->position.y - 16
                };
                
                // Draw sprite with rotation
                Rectangle destRect = {proj->position.x, proj->position.y, 32, 32};
                Vector2 origin = {16, 16};
                DrawTexturePro(state->projectileSpriteSheet, sourceRect, destRect, 
                              origin, proj->rotation, WHITE);
                
                // Still draw glow effect
                const ProjectileDefinition* def = GetProjectileDefinition(proj->type);
                DrawCircleV(proj->position, def->size * 1.5f, Fade(def->glowColor, 0.2f));
            } else {
                DrawProjectile(&state->projectiles[i]);
            }
        }
    }
    
    // Draw UI
    DrawRectangle(0, 0, SCREEN_WIDTH, 120, Fade(BLACK, 0.7f));
    
    // Title
    DrawText("PROJECTILE & WEAPON SHOWCASE", 20, 10, 24, WHITE);
    
    // Controls
    DrawText("Controls: [LEFT MOUSE] Fire Single | [RIGHT MOUSE] Fire Enemy Pattern | [A] Auto Fire", 
            20, 40, 14, LIGHTGRAY);
    DrawText("[1-4] Select Projectile | [←→] Select Enemy | [SPACE] Pause | [T] Trails | [C] Clear", 
            20, 58, 14, LIGHTGRAY);
    
    // Selected projectile info
    const ProjectileDefinition* projDef = GetProjectileDefinition(state->selectedProjectileType);
    char projInfo[256];
    snprintf(projInfo, sizeof(projInfo), "Selected Projectile: %s (DMG:%d SPD:%.0f)", 
            projDef->name, projDef->damage, projDef->speed);
    DrawText(projInfo, 20, 80, 16, projDef->primaryColor);
    
    // Selected enemy weapon info
    const EnemyTypeDefinition* enemyDef = GetEnemyTypeDefinition(state->selectedEnemyType);
    const EnemyWeaponConfig* weaponConfig = GetEnemyWeaponConfig(state->selectedEnemyType);
    char enemyInfo[256];
    snprintf(enemyInfo, sizeof(enemyInfo), "Enemy: %s | Burst:%d Spread:%.0f° Rate:%.1f/s", 
            enemyDef->name, weaponConfig->burstCount, weaponConfig->spreadAngle, weaponConfig->fireRate);
    DrawText(enemyInfo, 20, 100, 16, enemyDef->primaryColor);
    
    // Projectile type list
    if (state->showInfo) {
        int infoY = 140;
        DrawRectangle(10, infoY, 300, 180, Fade(BLACK, 0.7f));
        DrawText("PROJECTILE TYPES:", 20, infoY + 10, 16, WHITE);
        
        for (int i = 0; i < PROJECTILE_TYPE_COUNT; i++) {
            const ProjectileDefinition* def = GetProjectileDefinition(i);
            char info[128];
            snprintf(info, sizeof(info), "[%d] %s", i+1, def->name);
            Color color = (i == state->selectedProjectileType) ? def->primaryColor : GRAY;
            DrawText(info, 30, infoY + 35 + i * 20, 14, color);
            
            snprintf(info, sizeof(info), "DMG:%d SPD:%.0f %s%s", 
                    def->damage, def->speed,
                    def->explosive ? "EXP " : "",
                    def->piercing ? "PIERCE" : "");
            DrawText(info, 120, infoY + 35 + i * 20, 12, Fade(color, 0.7f));
        }
        
        // Enemy weapon mapping
        DrawRectangle(320, infoY, 350, 180, Fade(BLACK, 0.7f));
        DrawText("ENEMY WEAPONS:", 330, infoY + 10, 16, WHITE);
        
        int enemyListY = infoY + 35;
        for (int i = 0; i < 5; i++) {  // Show first 5 enemies
            const EnemyTypeDefinition* eDef = GetEnemyTypeDefinition(i);
            const EnemyWeaponConfig* wConfig = GetEnemyWeaponConfig(i);
            const ProjectileDefinition* pDef = GetProjectileDefinition(wConfig->primaryProjectile);
            
            char info[128];
            snprintf(info, sizeof(info), "%s: %s", eDef->name, pDef->name);
            Color color = (i == state->selectedEnemyType) ? eDef->primaryColor : GRAY;
            DrawText(info, 340, enemyListY + i * 20, 12, color);
        }
        
        enemyListY = infoY + 35;
        for (int i = 5; i < ENEMY_TYPE_COUNT; i++) {  // Show remaining enemies
            const EnemyTypeDefinition* eDef = GetEnemyTypeDefinition(i);
            const EnemyWeaponConfig* wConfig = GetEnemyWeaponConfig(i);
            const ProjectileDefinition* pDef = GetProjectileDefinition(wConfig->primaryProjectile);
            
            char info[128];
            snprintf(info, sizeof(info), "%s: %s", eDef->name, pDef->name);
            Color color = (i == state->selectedEnemyType) ? eDef->primaryColor : GRAY;
            DrawText(info, 500, enemyListY + (i-5) * 20, 12, color);
        }
    }
    
    // Status indicators
    if (state->paused) {
        DrawText("PAUSED", SCREEN_WIDTH - 100, 20, 20, YELLOW);
    }
    
    if (state->autoFire) {
        DrawText("AUTO FIRE", SCREEN_WIDTH - 100, 50, 20, ORANGE);
    }
    
    // Projectile count
    char countText[64];
    snprintf(countText, sizeof(countText), "Active: %d/%d", state->projectileCount, MAX_PROJECTILES);
    DrawText(countText, SCREEN_WIDTH - 150, SCREEN_HEIGHT - 30, 16, WHITE);
    
    // FPS
    DrawFPS(10, SCREEN_HEIGHT - 30);
    
    EndDrawing();
}

void CleanupShowcase(ShowcaseState* state) {
    if (state->spritesLoaded) {
        UnloadTexture(state->projectileSpriteSheet);
    }
}

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Projectile & Weapon Showcase");
    SetTargetFPS(60);
    
    // Initialize showcase
    ShowcaseState state = {0};
    InitShowcase(&state);
    
    // Main loop
    while (!WindowShouldClose()) {
        UpdateShowcase(&state);
        DrawShowcase(&state);
    }
    
    // Cleanup
    CleanupShowcase(&state);
    CloseWindow();
    
    return 0;
}

