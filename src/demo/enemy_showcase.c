#include "raylib.h"
#include "enemy_types.h"
#include "projectile_types.h"
#include "player_ship.h"
#include "weapon.h"
#include "collision.h"
#include "wave_system.h"
#include "combat_system.h"
#include "projectile_manager.h"
#include "explosion.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SHOWCASE_WIDTH 1400
#define SHOWCASE_HEIGHT 800
#define ENEMIES_PER_ROW 5
#define ENEMY_SPACING_X 250
#define ENEMY_SPACING_Y 350
#define START_X 150
#define START_Y 150

// Arena play area (below HUD)
#define ARENA_HUD_HEIGHT 100
#define ARENA_PLAY_TOP ARENA_HUD_HEIGHT
#define ARENA_PLAY_BOTTOM SHOWCASE_HEIGHT

// Showcase modes
typedef enum {
    MODE_GRID_SELECT,   // Grid selection screen
    MODE_TEST_ARENA     // Interactive test arena
} ShowcaseMode;

// Grid selection state
typedef struct {
    EnemyEx enemies[ENEMY_TYPE_COUNT];
    float globalTimer;
    int selectedEnemy;
    bool animationPaused;
    Camera2D camera;
} GridState;

// Test arena state
typedef struct {
    EnemyType testingEnemyType;
    PlayerShip* playerShip;
    Bullet* bullets;
    EnemyEx* enemies;
    void* projectiles;        // Projectile* (enemy projectiles)
    ExplosionSystem* explosionSystem;  // Shared explosion system
    int activeEnemyCount;
    float spawnTimer;
    float spawnDelay;
    int enemiesKilled;
    int enemiesSpawned;
} ArenaState;

// Main showcase state
typedef struct {
    ShowcaseMode mode;
    GridState gridState;
    ArenaState arenaState;
} ShowcaseState;

// Initialize grid selection mode
void InitGridState(GridState* state) {
    // Initialize enemy types and projectile types
    InitEnemyTypes();
    InitProjectileTypes();
    
    // Create one enemy of each type for display
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        int row = i / ENEMIES_PER_ROW;
        int col = i % ENEMIES_PER_ROW;
        
        float x = START_X + col * ENEMY_SPACING_X;
        float y = START_Y + row * ENEMY_SPACING_Y;
        
        InitializeEnemyFromType(&state->enemies[i], (EnemyType)i, x, y);
        state->enemies[i].id = i;
    }
    
    // Initialize camera
    state->camera.target = (Vector2){ SHOWCASE_WIDTH/2.0f, SHOWCASE_HEIGHT/2.0f };
    state->camera.offset = (Vector2){ SHOWCASE_WIDTH/2.0f, SHOWCASE_HEIGHT/2.0f };
    state->camera.rotation = 0.0f;
    state->camera.zoom = 1.0f;
    
    state->globalTimer = 0;
    state->selectedEnemy = -1;
    state->animationPaused = false;
}

// Initialize test arena mode
void InitArenaState(ArenaState* state, EnemyType enemyType) {
    state->testingEnemyType = enemyType;
    
    // Initialize player ship (position in playable area below HUD)
    state->playerShip = (PlayerShip*)malloc(sizeof(PlayerShip));
    InitPlayerShip(state->playerShip);
    float playAreaCenter = ARENA_PLAY_TOP + (ARENA_PLAY_BOTTOM - ARENA_PLAY_TOP) / 2;
    state->playerShip->position = (Vector2){ 150, playAreaCenter };
    
    // Initialize bullets array
    state->bullets = (Bullet*)malloc(sizeof(Bullet) * MAX_BULLETS);
    InitBullets(state->bullets);
    
    // Initialize enemies array
    state->enemies = (EnemyEx*)malloc(sizeof(EnemyEx) * MAX_ENEMIES);
    for (int i = 0; i < MAX_ENEMIES; i++) {
        state->enemies[i].active = false;
    }
    
    // Initialize projectiles array (enemy projectiles)
    state->projectiles = malloc(sizeof(Projectile) * MAX_PROJECTILES);
    ProjectileManager mgr = {
        .projectiles = (Projectile*)state->projectiles,
        .maxProjectiles = MAX_PROJECTILES
    };
    ProjectileManager_InitAll(&mgr);
    
    // Initialize explosion system (reused from main game)
    state->explosionSystem = (ExplosionSystem*)malloc(sizeof(ExplosionSystem));
    InitExplosionSystem(state->explosionSystem);
    
    state->activeEnemyCount = 0;
    state->spawnTimer = 0.0f;
    state->spawnDelay = 0.5f; // Spawn first enemy quickly
    state->enemiesKilled = 0;
    state->enemiesSpawned = 0;
}

// Cleanup arena state
void CleanupArenaState(ArenaState* state) {
    if (state->playerShip) {
        free(state->playerShip);
        state->playerShip = NULL;
    }
    if (state->bullets) {
        free(state->bullets);
        state->bullets = NULL;
    }
    if (state->enemies) {
        free(state->enemies);
        state->enemies = NULL;
    }
    if (state->projectiles) {
        free(state->projectiles);
        state->projectiles = NULL;
    }
    if (state->explosionSystem) {
        free(state->explosionSystem);
        state->explosionSystem = NULL;
    }
}

// Fire enemy projectile (uses generic combat system)
void FireEnemyProjectileArena(ArenaState* state, EnemyEx* enemy) {
    CombatContext ctx = {
        .playerPosition = state->playerShip->position,
        .projectiles = (Projectile*)state->projectiles,
        .maxProjectiles = MAX_PROJECTILES,
        .screenWidth = SHOWCASE_WIDTH,
        .screenHeight = SHOWCASE_HEIGHT
    };
    
    Combat_FireEnemyProjectile(&ctx, enemy);
}

// Spawn an enemy in the arena with standard attack pattern
void SpawnArenaEnemy(ArenaState* state) {
    // Find an inactive enemy slot
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!state->enemies[i].active) {
            // Spawn at random height on the right side (within playable area)
            float spawnX = SHOWCASE_WIDTH + 50;
            float spawnY = GetRandomValue(ARENA_PLAY_TOP + 50, ARENA_PLAY_BOTTOM - 50);
            
            InitializeEnemyFromType(&state->enemies[i], state->testingEnemyType, spawnX, spawnY);
            state->enemies[i].id = state->enemiesSpawned++;
            state->enemies[i].can_fire = true;
            
            // Apply standard attack pattern based on enemy type
            const char* pattern = NULL;
            switch (state->testingEnemyType) {
                case ENEMY_GRUNT:
                    pattern = "straight";
                    break;
                case ENEMY_TANK:
                    pattern = "tank_assault";
                    break;
                case ENEMY_SPEEDER:
                    pattern = "rush";
                    break;
                case ENEMY_ZIGZAG:
                    pattern = "zigzag";
                    break;
                case ENEMY_BOMBER:
                    pattern = "slow_advance";
                    break;
                case ENEMY_SHIELD:
                    pattern = "hover";
                    break;
                case ENEMY_SWARM:
                    pattern = "erratic";
                    break;
                case ENEMY_ELITE:
                    pattern = "elite_retreat";
                    break;
                case ENEMY_GHOST:
                    pattern = "phasing";
                    break;
                case ENEMY_BOSS:
                    pattern = "boss";
                    spawnY = SHOWCASE_HEIGHT/2; // Boss spawns in center
                    break;
                default:
                    pattern = "straight";
                    break;
            }
            
            ApplyMovementPattern(&state->enemies[i], pattern);
            state->activeEnemyCount++;
            break;
        }
    }
}

// Update grid selection mode
void UpdateGridState(GridState* state) {
    if (!state->animationPaused) {
        state->globalTimer += GetFrameTime();
    }
    
    // Update animations for each enemy
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        EnemyEx* enemy = &state->enemies[i];
        
        if (!state->animationPaused) {
            enemy->animationTimer += GetFrameTime();
            
            // Update special animations based on type
            switch (enemy->type) {
                case ENEMY_SHIELD:
                    enemy->shieldAngle += 60 * GetFrameTime();
                    if (enemy->shieldAngle > 360) enemy->shieldAngle -= 360;
                    break;
                    
                case ENEMY_GHOST:
                    enemy->specialTimer += GetFrameTime();
                    if (enemy->specialTimer > 2.0f) {
                        enemy->isVisible = !enemy->isVisible;
                        enemy->specialTimer = 0;
                    }
                    break;
                    
                case ENEMY_ZIGZAG:
                    enemy->position.x += sinf(enemy->animationTimer * 3) * 30 * GetFrameTime();
                    enemy->position.y += cosf(enemy->animationTimer * 2) * 20 * GetFrameTime();
                    break;
                    
                case ENEMY_SPEEDER:
                    enemy->position.x += sinf(enemy->animationTimer * 5) * 50 * GetFrameTime();
                    break;
                    
                case ENEMY_SWARM:
                    {
                        float angle = enemy->animationTimer * 2;
                        enemy->position.x += cosf(angle) * 15 * GetFrameTime();
                        enemy->position.y += sinf(angle) * 15 * GetFrameTime();
                    }
                    break;
                    
                case ENEMY_BOMBER:
                    enemy->position.y += sinf(enemy->animationTimer * 2) * 10 * GetFrameTime();
                    break;
                    
                default:
                    enemy->position.y += sinf(enemy->animationTimer) * 5 * GetFrameTime();
                    break;
            }
            
            // Update bounds
            enemy->bounds.x = enemy->position.x - enemy->radius;
            enemy->bounds.y = enemy->position.y - enemy->radius;
        }
    }
    
    // Mouse selection
    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), state->camera);
    state->selectedEnemy = -1;
    
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        EnemyEx* enemy = &state->enemies[i];
        float dx = mousePos.x - enemy->position.x;
        float dy = mousePos.y - enemy->position.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist <= enemy->radius * 1.5f) {
            state->selectedEnemy = i;
            break;
        }
    }
}

// Update test arena mode
void UpdateArenaState(ArenaState* state) {
    float deltaTime = GetFrameTime();
    
    // Update player ship (invincible)
    HandlePlayerInput(state->playerShip);
    UpdateShipPhysics(state->playerShip, deltaTime);
    UpdateShipEffects(state->playerShip, deltaTime);
    
    // Constrain player to playable area (below HUD, above bottom)
    if (state->playerShip->position.y < ARENA_PLAY_TOP + 30) {
        state->playerShip->position.y = ARENA_PLAY_TOP + 30;
    }
    if (state->playerShip->position.y > ARENA_PLAY_BOTTOM - 30) {
        state->playerShip->position.y = ARENA_PLAY_BOTTOM - 30;
    }
    
    // Handle player shooting - same as main game (press to shoot, not hold)
    static float shootTimer = 0;
    shootTimer -= deltaTime;
    
    if ((IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && shootTimer <= 0) {
        ShootBullet(state->bullets, state->playerShip->position);
        shootTimer = WEAPON_FIRE_RATE;  // Same as main game (0.12f)
    }
    
    // Update bullets (same as main game - per frame, not deltaTime)
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (state->bullets[i].active) {
            state->bullets[i].position.x += state->bullets[i].speed;  // Uses BULLET_SPEED (12.0f per frame)
            state->bullets[i].bounds.x = state->bullets[i].position.x - 5;
            state->bullets[i].bounds.y = state->bullets[i].position.y - 2;
            
            // Deactivate if off screen
            if (state->bullets[i].position.x > SHOWCASE_WIDTH + 50) {
                state->bullets[i].active = false;
            }
        }
    }
    
    // Update enemies (using generic combat system)
    state->activeEnemyCount = 0;
    
    // Create combat context
    CombatContext ctx = {
        .playerPosition = state->playerShip->position,
        .projectiles = (Projectile*)state->projectiles,
        .maxProjectiles = MAX_PROJECTILES,
        .screenWidth = SHOWCASE_WIDTH,
        .screenHeight = SHOWCASE_HEIGHT
    };
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state->enemies[i].active) {
            EnemyEx* enemy = &state->enemies[i];
            
            // Update movement
            UpdateEnemyMovement(enemy, deltaTime);
            state->activeEnemyCount++;
            
            // Update firing using generic combat system
            Combat_UpdateEnemyFiring(enemy, &ctx, deltaTime,
                                     0, SHOWCASE_WIDTH,
                                     ARENA_PLAY_TOP, ARENA_PLAY_BOTTOM);
            
            // Check if enemy is off screen (left side or after retreat)
            if (enemy->position.x < -100 || !enemy->active) {
                enemy->active = false;
                state->activeEnemyCount--;
            }
        }
    }
    
    // Update projectiles using projectile manager
    ProjectileManager projMgr = {
        .projectiles = (Projectile*)state->projectiles,
        .maxProjectiles = MAX_PROJECTILES,
        .minX = -50, .maxX = SHOWCASE_WIDTH + 50,
        .minY = -50, .maxY = SHOWCASE_HEIGHT + 50
    };
    ProjectileManager_UpdateAll(&projMgr, deltaTime);
    
    // Update explosion system (reused from main game)
    UpdateExplosionSystem(state->explosionSystem, deltaTime);
    
    // Check bullet-enemy collisions using generic collision system
    CollisionContext collisionCtx = {
        .bullets = state->bullets,
        .maxBullets = MAX_BULLETS,
        .enemies = state->enemies,
        .maxEnemies = MAX_ENEMIES,
        .explosionSystem = state->explosionSystem,  // ✅ Enable explosions (reused from game)
        .score = NULL,                              // No score in showcase
        .enemiesKilled = &state->enemiesKilled,     // Track kills
        .logContext = NULL,                         // No logging in showcase
        .onEnemyHit = NULL,
        .onEnemyDestroyed = NULL
    };
    Collision_CheckBulletEnemyGeneric(&collisionCtx);
    
    // Update active enemy count after collisions
    state->activeEnemyCount = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state->enemies[i].active) {
            state->activeEnemyCount++;
        }
    }
    
    // Player is invincible - projectiles pass through
    // (No player-projectile collision needed)
    
    // Spawn new enemy if none active
    state->spawnTimer += deltaTime;
    if (state->activeEnemyCount == 0 && state->spawnTimer >= state->spawnDelay) {
        SpawnArenaEnemy(state);
        state->spawnTimer = 0.0f;
        state->spawnDelay = 1.0f; // Delay between spawns
    }
}

// Draw grid selection mode
void DrawGridState(const GridState* state) {
    BeginDrawing();
    ClearBackground((Color){20, 20, 30, 255});
    
    BeginMode2D(state->camera);
    
    // Draw grid background
    for (int i = 0; i <= SHOWCASE_WIDTH; i += 50) {
        DrawLine(i, 0, i, SHOWCASE_HEIGHT * 2, Fade(DARKGRAY, 0.2f));
    }
    for (int i = 0; i <= SHOWCASE_HEIGHT * 2; i += 50) {
        DrawLine(0, i, SHOWCASE_WIDTH, i, Fade(DARKGRAY, 0.2f));
    }
    
    // Draw all enemies
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        const EnemyEx* enemy = &state->enemies[i];
        
        // Highlight selected enemy
        if (state->selectedEnemy == i) {
            DrawCircle(enemy->position.x, enemy->position.y, 
                      enemy->radius + 20, Fade(YELLOW, 0.3f));
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                           enemy->radius + 25, YELLOW);
        }
        
        // Draw the enemy
        DrawEnemyEx(enemy);
        
        // Draw enemy info
        const EnemyTypeDefinition* def = GetEnemyTypeDefinition(enemy->type);
        
        // Name
        int fontSize = 20;
        int nameWidth = MeasureText(def->name, fontSize);
        DrawText(def->name, 
                enemy->position.x - nameWidth/2, 
                enemy->position.y + enemy->radius + 10,
                fontSize, WHITE);
        
        // Stats
        char stats[256];
        snprintf(stats, sizeof(stats), "HP:%d PWR:%d SPD:%.1fx", 
                def->health, def->power, def->speed);
        fontSize = 14;
        int statsWidth = MeasureText(stats, fontSize);
        DrawText(stats,
                enemy->position.x - statsWidth/2,
                enemy->position.y + enemy->radius + 35,
                fontSize, LIGHTGRAY);
    }
    
    EndMode2D();
    
    // Draw UI overlay
    DrawRectangle(0, 0, SHOWCASE_WIDTH, 80, Fade(BLACK, 0.7f));
    DrawText("ENEMY SHOWCASE - TEST ARENA", 20, 10, 30, WHITE);
    DrawText("Click on an enemy to test its behavior in combat", 20, 45, 18, LIGHTGRAY);
    DrawText("ESC: Exit", SHOWCASE_WIDTH - 120, 50, 16, YELLOW);
    
    // Draw selected enemy hint
    if (state->selectedEnemy >= 0) {
        const EnemyEx* enemy = &state->enemies[state->selectedEnemy];
        const EnemyTypeDefinition* def = GetEnemyTypeDefinition(enemy->type);
        
        DrawText("Click to test!", SHOWCASE_WIDTH/2 - 60, SHOWCASE_HEIGHT - 40, 20, YELLOW);
    }
    
    EndDrawing();
}

// Draw test arena mode
void DrawArenaState(const ArenaState* state) {
    BeginDrawing();
    ClearBackground((Color){15, 15, 25, 255});
    
    // Draw grid background (only in playable area)
    for (int i = 0; i <= SHOWCASE_WIDTH; i += 100) {
        DrawLine(i, ARENA_PLAY_TOP, i, ARENA_PLAY_BOTTOM, Fade(DARKGRAY, 0.1f));
    }
    for (int i = ARENA_PLAY_TOP; i <= ARENA_PLAY_BOTTOM; i += 100) {
        DrawLine(0, i, SHOWCASE_WIDTH, i, Fade(DARKGRAY, 0.1f));
    }
    
    // Draw HUD separator line
    DrawLine(0, ARENA_PLAY_TOP, SHOWCASE_WIDTH, ARENA_PLAY_TOP, Fade(YELLOW, 0.3f));
    
    // Draw spawn zone indicator (right side, playable area only)
    DrawRectangle(SHOWCASE_WIDTH - 150, ARENA_PLAY_TOP, 150, ARENA_PLAY_BOTTOM - ARENA_PLAY_TOP, Fade(RED, 0.05f));
    DrawText("SPAWN", SHOWCASE_WIDTH - 120, ARENA_PLAY_TOP + 20, 20, Fade(RED, 0.3f));
    
    // Draw player zone indicator (left side, playable area only)
    DrawRectangle(0, ARENA_PLAY_TOP, 200, ARENA_PLAY_BOTTOM - ARENA_PLAY_TOP, Fade(BLUE, 0.05f));
    
    // Draw player ship
    DrawPlayerShip(state->playerShip);
    DrawEngineTrail(state->playerShip);
    
    // Draw bullets
    DrawBullets(state->bullets);
    
    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (state->enemies[i].active) {
            DrawEnemyEx(&state->enemies[i]);
        }
    }
    
    // Draw enemy projectiles using projectile manager
    ProjectileManager projMgr = {
        .projectiles = (Projectile*)state->projectiles,
        .maxProjectiles = MAX_PROJECTILES
    };
    ProjectileManager_DrawAll(&projMgr);
    
    // Draw explosions (reused from main game)
    DrawExplosions(state->explosionSystem);
    
    // Draw HUD
    DrawRectangle(0, 0, SHOWCASE_WIDTH, 100, Fade(BLACK, 0.7f));
    
    const EnemyTypeDefinition* def = GetEnemyTypeDefinition(state->testingEnemyType);
    char title[256];
    snprintf(title, sizeof(title), "Testing: %s", def->name);
    DrawText(title, 20, 10, 28, def->primaryColor);
    
    char stats[256];
    snprintf(stats, sizeof(stats), "Spawned: %d | Destroyed: %d | Active: %d", 
            state->enemiesSpawned, state->enemiesKilled, state->activeEnemyCount);
    DrawText(stats, 20, 45, 18, LIGHTGRAY);
    
    DrawText("Controls: WASD/Arrows - Move | SPACE/LMB - Shoot", 20, 70, 14, GRAY);
    DrawText("ESC: Back to Grid", SHOWCASE_WIDTH - 180, 70, 16, YELLOW);
    
    // Draw player invincibility indicator
    DrawText("INVINCIBLE", state->playerShip->position.x - 40, 
             state->playerShip->position.y - 50, 14, Fade(YELLOW, 0.7f));
    
    EndDrawing();
}

// Initialize showcase
void InitShowcase(ShowcaseState* state) {
    state->mode = MODE_GRID_SELECT;
    InitGridState(&state->gridState);
    
    // Arena state will be initialized when entering arena mode
    memset(&state->arenaState, 0, sizeof(ArenaState));
}

// Update showcase
void UpdateShowcase(ShowcaseState* state) {
    if (state->mode == MODE_GRID_SELECT) {
        UpdateGridState(&state->gridState);
        
        // Check for enemy selection (mouse click)
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && state->gridState.selectedEnemy >= 0) {
            // Enter arena mode with selected enemy
            EnemyType selectedType = (EnemyType)state->gridState.selectedEnemy;
            InitArenaState(&state->arenaState, selectedType);
            state->mode = MODE_TEST_ARENA;
        }
        
    } else if (state->mode == MODE_TEST_ARENA) {
        UpdateArenaState(&state->arenaState);
        
        // Check for ESC to return to grid
        if (IsKeyPressed(KEY_ESCAPE)) {
            CleanupArenaState(&state->arenaState);
            state->mode = MODE_GRID_SELECT;
            // Reset grid state animations
            state->gridState.globalTimer = 0;
        }
    }
}

// Draw showcase
void DrawShowcase(const ShowcaseState* state) {
    if (state->mode == MODE_GRID_SELECT) {
        DrawGridState(&state->gridState);
    } else if (state->mode == MODE_TEST_ARENA) {
        DrawArenaState(&state->arenaState);
    }
}

int main(void) {
    // Initialize window
    InitWindow(SHOWCASE_WIDTH, SHOWCASE_HEIGHT, "Enemy Test Arena - Interactive Showcase");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);  // Disable automatic ESC to close window
    
    // Initialize showcase state
    ShowcaseState state = {0};
    InitShowcase(&state);
    
    bool shouldExit = false;
    
    // Main game loop
    while (!shouldExit && !WindowShouldClose()) {
        // Handle ESC key based on current mode
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (state.mode == MODE_GRID_SELECT) {
                // In grid mode: exit program
                shouldExit = true;
            }
            // In arena mode: ESC is handled in UpdateShowcase() to return to grid
        }
        
        UpdateShowcase(&state);
        DrawShowcase(&state);
    }
    
    // Cleanup
    if (state.mode == MODE_TEST_ARENA) {
        CleanupArenaState(&state.arenaState);
    }
    CloseWindow();
    
    return 0;
}