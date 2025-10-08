#include "raylib.h"
#include "player_ship.h"
#include "constants.h"
#include "projectile_types.h"
#include <stdio.h>
#include <math.h>

typedef struct {
    PlayerShip ship;
    Projectile projectiles[MAX_PROJECTILES];
    Texture2D playerSprite;
    Texture2D projectileSprite;
    bool showStats;
    bool showControls;
    float demoTime;
    Vector2 starfield[100];
} ShowcaseState;

void InitShowcase(ShowcaseState* state) {
    // Initialize player ship
    InitPlayerShip(&state->ship);
    state->ship.position = (Vector2){SCREEN_WIDTH/4, SCREEN_HEIGHT/2};
    
    // Initialize projectiles
    InitProjectileTypes();
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        state->projectiles[i].active = false;
    }
    
    // Try to load sprites
    if (FileExists("assets/sprites/player_ship_spritesheet.png")) {
        state->playerSprite = LoadTexture("assets/sprites/player_ship_spritesheet.png");
        printf("Loaded player sprite sheet\n");
    }
    if (FileExists("assets/sprites/projectile_spritesheet.png")) {
        state->projectileSprite = LoadTexture("assets/sprites/projectile_spritesheet.png");
        printf("Loaded projectile sprite sheet\n");
    }
    
    // Initialize starfield
    for (int i = 0; i < 100; i++) {
        state->starfield[i] = (Vector2){
            GetRandomValue(0, SCREEN_WIDTH),
            GetRandomValue(0, SCREEN_HEIGHT)
        };
    }
    
    state->showStats = true;
    state->showControls = true;
    state->demoTime = 0;
}

void FireProjectile(ShowcaseState* state) {
    PlayerShip* ship = &state->ship;
    
    // Don't fire if overheated
    if (ship->overheated) {
        return;
    }
    
    // Check fire rate
    float fireRate = 0.15f;
    if (ship->weaponMode == WEAPON_MODE_RAPID) fireRate = 0.05f;
    if (ship->weaponMode == WEAPON_MODE_CHARGE) fireRate = 0.5f;
    
    if (ship->fireTimer > 0) {
        return;
    }
    
    ship->fireTimer = fireRate;
    
    // Add heat (weapon overheating disabled in main game)
    ship->weaponHeat += 8.0f;
    if (ship->weaponHeat >= ship->maxHeat) {
        ship->overheated = true;
        ship->cooldownTime = 2.0f;
    }
    
    // Find free projectile slots and fire based on weapon mode
    int projectilesFired = 0;
    int maxProjectiles = 1;
    
    switch (ship->weaponMode) {
        case WEAPON_MODE_SINGLE:
            maxProjectiles = 1;
            break;
        case WEAPON_MODE_DOUBLE:
            maxProjectiles = 2;
            break;
        case WEAPON_MODE_SPREAD:
            maxProjectiles = 3;
            break;
        case WEAPON_MODE_RAPID:
            maxProjectiles = 1;
            break;
        case WEAPON_MODE_CHARGE:
            maxProjectiles = 1;
            break;
        case WEAPON_MODE_DUAL:
            maxProjectiles = 2;  // One forward, one backward
            break;
    }
    
    for (int i = 0; i < MAX_PROJECTILES && projectilesFired < maxProjectiles; i++) {
        if (!state->projectiles[i].active) {
            Projectile* p = &state->projectiles[i];
            
            // Initialize projectile
            Vector2 firePos = ship->position;
            Vector2 target = {ship->position.x + 100, ship->position.y};
            
            // Adjust position and direction based on weapon mode
            switch (ship->weaponMode) {
                case WEAPON_MODE_SINGLE:
                    firePos.x += 20;
                    break;
                    
                case WEAPON_MODE_DOUBLE:
                    firePos.x += 15;
                    if (projectilesFired == 0) {
                        firePos.y -= 15;
                    } else {
                        firePos.y += 15;
                    }
                    break;
                    
                case WEAPON_MODE_SPREAD:
                    firePos.x += 15;
                    if (projectilesFired == 0) {
                        target.y -= 30;
                    } else if (projectilesFired == 2) {
                        target.y += 30;
                    }
                    break;
                    
                case WEAPON_MODE_CHARGE:
                    firePos.x += 25;
                    // Larger projectile for charged shot
                    break;
                    
                case WEAPON_MODE_DUAL:
                    // Position projectiles at front and back of ship
                    if (projectilesFired == 0) {
                        firePos.x += 25;  // Forward shot
                    } else {
                        firePos.x -= 25;  // Backward shot
                    }
                    break;
            }
            
            // Set projectile type based on mode
            ProjectileType projType = PROJECTILE_PLAYER_BULLET;
            if (ship->weaponMode == WEAPON_MODE_CHARGE && ship->chargeLevel > 50) {
                projType = PROJECTILE_PLASMA;
            }
            
            InitializeProjectile(p, projType, firePos, target, true);
            
            // Customize projectile properties
            if (ship->weaponMode == WEAPON_MODE_CHARGE) {
                p->damage = 5 + (ship->chargeLevel / 10);
                p->scale = 1.0f + (ship->chargeLevel / 100.0f);
                ship->chargeLevel = 0;
                ship->isCharging = false;
            } else {
                p->damage = 1 * ship->upgrades.weaponLevel;
            }
            
            if (ship->weaponMode == WEAPON_MODE_DUAL) {
                // First projectile goes forward, second goes backward
                if (projectilesFired == 1) {
                    p->velocity.x = 15;  // Forward
                } else {
                    p->velocity.x = -15;  // Backward
                }
                p->velocity.y = 0;
            } else {
                p->velocity.x = 15;
                if (ship->weaponMode == WEAPON_MODE_SPREAD) {
                    if (projectilesFired == 0) p->velocity.y = -3;
                    else if (projectilesFired == 2) p->velocity.y = 3;
                }
            }
            
            projectilesFired++;
        }
    }
}

void UpdateShowcase(ShowcaseState* state) {
    float deltaTime = GetFrameTime();
    state->demoTime += deltaTime;
    
    // Update starfield
    for (int i = 0; i < 100; i++) {
        state->starfield[i].x -= (i % 3 + 1) * 2;
        if (state->starfield[i].x < 0) {
            state->starfield[i].x = SCREEN_WIDTH;
            state->starfield[i].y = GetRandomValue(0, SCREEN_HEIGHT);
        }
    }
    
    // Update player ship
    UpdatePlayerShip(&state->ship, deltaTime);
    
    // Handle firing
    if (IsKeyDown(KEY_SPACE) && state->ship.weaponMode != WEAPON_MODE_CHARGE) {
        FireProjectile(state);
    } else if (IsKeyReleased(KEY_SPACE) && state->ship.weaponMode == WEAPON_MODE_CHARGE && state->ship.chargeLevel > 20) {
        FireProjectile(state);
    }
    
    // Update projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (state->projectiles[i].active) {
            UpdateProjectile(&state->projectiles[i], deltaTime);
            
            // Dual mode projectiles maintain their direction
            // No special velocity updates needed for dual mode
            
            // Deactivate if off screen
            if (state->projectiles[i].position.x > SCREEN_WIDTH || 
                state->projectiles[i].position.x < 0 ||
                state->projectiles[i].position.y > SCREEN_HEIGHT ||
                state->projectiles[i].position.y < 0) {
                state->projectiles[i].active = false;
            }
        }
    }
    
    // Toggle displays
    if (IsKeyPressed(KEY_TAB)) state->showStats = !state->showStats;
    if (IsKeyPressed(KEY_H)) state->showControls = !state->showControls;
    
    // Test damage/repair
    if (IsKeyPressed(KEY_MINUS)) {
        DamagePlayerShip(&state->ship, 10);
        printf("Damaged ship: Health=%d, Shield=%.0f\n", state->ship.health, state->ship.shield);
    }
    if (IsKeyPressed(KEY_EQUAL)) {
        RepairPlayerShip(&state->ship, 10);
        state->ship.shield = fminf(state->ship.maxShield, state->ship.shield + 10);
        printf("Repaired ship: Health=%d, Shield=%.0f\n", state->ship.health, state->ship.shield);
    }
    
    // Test upgrades
    if (IsKeyPressed(KEY_U)) {
        static int upgradeLevel = 1;
        upgradeLevel = (upgradeLevel % 5) + 1;
        ApplyShipUpgrade(&state->ship, GetRandomValue(0, 3), upgradeLevel);
        printf("Applied random upgrade level %d\n", upgradeLevel);
    }
}

void DrawShowcase(ShowcaseState* state) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Draw starfield
    for (int i = 0; i < 100; i++) {
        int brightness = 100 + (i % 3) * 50;
        DrawPixel(state->starfield[i].x, state->starfield[i].y, 
                 (Color){brightness, brightness, brightness, 255});
    }
    
    // Draw projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (state->projectiles[i].active) {
            DrawProjectile(&state->projectiles[i]);
        }
    }
    
    // Draw player ship
    if (state->playerSprite.id > 0) {
        // Use sprite if available
        int frame = ((int)(state->demoTime * 10) % 4);
        // Abilities removed - using normal frames only
        
        Rectangle source = {
            (frame % 4) * 64,
            (frame / 4) * 64,
            64,
            64
        };
        Rectangle dest = {
            state->ship.position.x,
            state->ship.position.y,
            64,
            64
        };
        Vector2 origin = {32, 32};
        
        // Draw without rotation - ship always faces right
        DrawTexturePro(state->playerSprite, source, dest, origin, 
                      0, WHITE);  // 0 rotation - ship always horizontal
    } else {
        // Use procedural drawing
        DrawPlayerShip(&state->ship);
    }
    
    // Draw HUD
    DrawShipHUD(&state->ship);
    
    // Draw stats
    if (state->showStats) {
        char stats[512];
        GetShipStats(&state->ship, stats, sizeof(stats));
        DrawText(stats, SCREEN_WIDTH - 250, 10, 12, GREEN);
    }
    
    // Draw controls
    if (state->showControls) {
        const char* controls = 
            "=== CONTROLS ===\n"
            "Movement: WASD/Arrows\n"
            "Fire: SPACE\n"
            "Boost: LEFT SHIFT\n"
            "Shield Burst: E\n"
            "EMP Blast: Q\n"
            "Overdrive: R\n"
            "Weapons: 1-6 Keys\n"
            " 1: Single\n"
            " 2: Double\n"
            " 3: Spread\n"
            " 4: Rapid\n"
            " 5: Charge\n"
            " 6: Dual (Front+Back)\n"
            "\n=== DEBUG ===\n"
            "Damage: -\n"
            "Repair: =\n"
            "Upgrade: U\n"
            "Toggle Stats: TAB\n"
            "Toggle Help: H\n"
            "Exit: ESC";
        
        DrawText(controls, 10, 10, 12, WHITE);
    }
    
    // Draw title
    DrawText("PLAYER SHIP SHOWCASE", SCREEN_WIDTH/2 - 120, 10, 20, GOLD);
    
    // Draw weapon mode indicator
    const char* weaponNames[] = {
        "SINGLE SHOT", "DOUBLE SHOT", "SPREAD SHOT", 
        "RAPID FIRE", "CHARGE BEAM", "DUAL SHOT"
    };
    DrawText(weaponNames[state->ship.weaponMode], SCREEN_WIDTH/2 - 60, 35, 14, state->ship.secondaryColor);
    
    // Draw charge indicator for charge mode
    if (state->ship.weaponMode == WEAPON_MODE_CHARGE && state->ship.isCharging) {
        DrawRectangle(SCREEN_WIDTH/2 - 100, 55, 200, 10, Fade(GRAY, 0.3f));
        DrawRectangle(SCREEN_WIDTH/2 - 100, 55, state->ship.chargeLevel * 2, 10, YELLOW);
        DrawRectangleLines(SCREEN_WIDTH/2 - 100, 55, 200, 10, WHITE);
        DrawText("CHARGING", SCREEN_WIDTH/2 - 30, 70, 12, YELLOW);
    }
    
    EndDrawing();
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Player Ship Showcase");
    SetTargetFPS(60);
    
    ShowcaseState state = {0};
    InitShowcase(&state);
    
    printf("=== PLAYER SHIP SHOWCASE ===\n");
    printf("This demo showcases the enhanced player ship with:\n");
    printf("- Multiple weapon modes\n");
    printf("- Shield system\n");
    printf("- Special abilities\n");
    printf("- Visual effects\n");
    printf("- Upgrade system\n");
    printf("\nPress H to toggle controls display\n");
    
    while (!WindowShouldClose()) {
        UpdateShowcase(&state);
        DrawShowcase(&state);
    }
    
    // Cleanup
    if (state.playerSprite.id > 0) {
        UnloadTexture(state.playerSprite);
    }
    if (state.projectileSprite.id > 0) {
        UnloadTexture(state.projectileSprite);
    }
    
    CloseWindow();
    return 0;
}
