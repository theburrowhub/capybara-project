#include "raylib.h"
#include "enemy_types.h"
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
#define SPRITE_SIZE 64

typedef enum {
    SPRITE_SET_ABSTRACT,
    SPRITE_SET_SPACESHIP,
    SPRITE_SET_COUNT
} SpriteSet;

typedef struct {
    EnemyEx enemies[ENEMY_TYPE_COUNT];
    float globalTimer;
    int selectedEnemy;
    bool showDetails;
    bool animationPaused;
    bool useSprites;
    Camera2D camera;
    Texture2D spriteSheet;
    Texture2D spaceshipSpriteSheet;
    Texture2D individualSprites[ENEMY_TYPE_COUNT];
    Texture2D spaceshipSprites[ENEMY_TYPE_COUNT];
    bool spritesLoaded;
    bool spaceshipSpritesLoaded;
    Rectangle spriteRects[ENEMY_TYPE_COUNT];
    SpriteSet currentSpriteSet;
} ShowcaseState;

void InitShowcase(ShowcaseState* state) {
    // Initialize enemy types
    InitEnemyTypes();
    
    // Try to load sprites
    state->spritesLoaded = false;
    state->spaceshipSpritesLoaded = false;
    state->useSprites = true;
    state->currentSpriteSet = SPRITE_SET_SPACESHIP;  // Default to spaceship sprites
    
    // Try to load abstract sprite sheet
    if (FileExists("assets/sprites/enemy_spritesheet.png")) {
        state->spriteSheet = LoadTexture("assets/sprites/enemy_spritesheet.png");
        state->spritesLoaded = true;
        
        // Define sprite rectangles in the sprite sheet
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            int col = i % 5;  // 5 sprites per row
            int row = i / 5;
            state->spriteRects[i] = (Rectangle){
                col * SPRITE_SIZE,
                row * SPRITE_SIZE,
                SPRITE_SIZE,
                SPRITE_SIZE
            };
        }
        printf("Abstract sprite sheet loaded successfully!\n");
    }
    
    // Try to load spaceship sprite sheet
    if (FileExists("assets/sprites/spaceship_spritesheet.png")) {
        state->spaceshipSpriteSheet = LoadTexture("assets/sprites/spaceship_spritesheet.png");
        state->spaceshipSpritesLoaded = true;
        printf("Spaceship sprite sheet loaded successfully!\n");
    }
    
    // Fallback to individual sprites if sheets not found
    if (!state->spritesLoaded) {
        const char* enemyNames[] = {
            "grunt", "tank", "speeder", "zigzag", "bomber",
            "shield", "swarm", "elite", "ghost", "boss"
        };
        
        bool anyLoaded = false;
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "assets/sprites/enemies/enemy_%s.png", enemyNames[i]);
            
            if (FileExists(filepath)) {
                state->individualSprites[i] = LoadTexture(filepath);
                anyLoaded = true;
            }
        }
        
        if (anyLoaded) {
            state->spritesLoaded = true;
            printf("Individual abstract sprites loaded.\n");
        }
    }
    
    // Load individual spaceship sprites if sheet not found
    if (!state->spaceshipSpritesLoaded) {
        const char* enemyNames[] = {
            "grunt", "tank", "speeder", "zigzag", "bomber",
            "shield", "swarm", "elite", "ghost", "boss"
        };
        
        bool anyLoaded = false;
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "assets/sprites/spaceships/spaceship_%s.png", enemyNames[i]);
            
            if (FileExists(filepath)) {
                state->spaceshipSprites[i] = LoadTexture(filepath);
                anyLoaded = true;
            }
        }
        
        if (anyLoaded) {
            state->spaceshipSpritesLoaded = true;
            printf("Individual spaceship sprites loaded.\n");
        }
    }
    
    // Check if any sprites are loaded
    if (!state->spritesLoaded && !state->spaceshipSpritesLoaded) {
        printf("Warning: No sprites found!\n");
        printf("Run 'make generate_sprites' for abstract sprites\n");
        printf("Run 'make generate_spaceships' for spaceship sprites\n");
        state->useSprites = false;
    }
    
    // Create one enemy of each type
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
    state->showDetails = true;
    state->animationPaused = false;
}

void UpdateShowcase(ShowcaseState* state) {
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
                    // Rotate shield
                    enemy->shieldAngle += 60 * GetFrameTime();
                    if (enemy->shieldAngle > 360) enemy->shieldAngle -= 360;
                    break;
                    
                case ENEMY_GHOST:
                    // Phase in and out
                    enemy->specialTimer += GetFrameTime();
                    if (enemy->specialTimer > 2.0f) {
                        enemy->isVisible = !enemy->isVisible;
                        enemy->specialTimer = 0;
                    }
                    break;
                    
                case ENEMY_ZIGZAG:
                    // Zigzag movement
                    enemy->position.x += sinf(enemy->animationTimer * 3) * 30 * GetFrameTime();
                    enemy->position.y += cosf(enemy->animationTimer * 2) * 20 * GetFrameTime();
                    break;
                    
                case ENEMY_SPEEDER:
                    // Quick horizontal movements
                    enemy->position.x += sinf(enemy->animationTimer * 5) * 50 * GetFrameTime();
                    break;
                    
                case ENEMY_SWARM:
                    {
                        // Circular movement
                        float angle = enemy->animationTimer * 2;
                        enemy->position.x += cosf(angle) * 15 * GetFrameTime();
                        enemy->position.y += sinf(angle) * 15 * GetFrameTime();
                    }
                    break;
                    
                case ENEMY_BOMBER:
                    // Pulsing effect
                    enemy->position.y += sinf(enemy->animationTimer * 2) * 10 * GetFrameTime();
                    break;
                    
                default:
                    // Gentle floating for others
                    enemy->position.y += sinf(enemy->animationTimer) * 5 * GetFrameTime();
                    break;
            }
            
            // Update bounds
            enemy->bounds.x = enemy->position.x - enemy->radius;
            enemy->bounds.y = enemy->position.y - enemy->radius;
        }
    }
    
    // Handle input
    if (IsKeyPressed(KEY_SPACE)) {
        state->animationPaused = !state->animationPaused;
    }
    
    if (IsKeyPressed(KEY_D)) {
        state->showDetails = !state->showDetails;
    }
    
    if (IsKeyPressed(KEY_S)) {
        state->useSprites = !state->useSprites;
        if (state->useSprites && !state->spritesLoaded && !state->spaceshipSpritesLoaded) {
            printf("No sprites loaded. Using procedural graphics.\n");
            state->useSprites = false;
        }
    }
    
    if (IsKeyPressed(KEY_T)) {
        // Toggle between sprite sets
        if (state->useSprites) {
            state->currentSpriteSet = (state->currentSpriteSet + 1) % SPRITE_SET_COUNT;
            
            // Check if the selected sprite set is available
            if (state->currentSpriteSet == SPRITE_SET_ABSTRACT && !state->spritesLoaded) {
                state->currentSpriteSet = SPRITE_SET_SPACESHIP;
            }
            if (state->currentSpriteSet == SPRITE_SET_SPACESHIP && !state->spaceshipSpritesLoaded) {
                state->currentSpriteSet = SPRITE_SET_ABSTRACT;
            }
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
    
    // Camera controls
    if (IsKeyDown(KEY_RIGHT)) state->camera.target.x += 5;
    if (IsKeyDown(KEY_LEFT)) state->camera.target.x -= 5;
    if (IsKeyDown(KEY_DOWN)) state->camera.target.y += 5;
    if (IsKeyDown(KEY_UP)) state->camera.target.y -= 5;
    
    // Zoom controls
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        state->camera.zoom += wheel * 0.1f;
        if (state->camera.zoom < 0.5f) state->camera.zoom = 0.5f;
        if (state->camera.zoom > 2.0f) state->camera.zoom = 2.0f;
    }
    
    if (IsKeyPressed(KEY_R)) {
        // Reset camera
        state->camera.target = (Vector2){ SHOWCASE_WIDTH/2.0f, SHOWCASE_HEIGHT/2.0f };
        state->camera.zoom = 1.0f;
    }
}

void DrawEnemyWithSprite(const ShowcaseState* state, const EnemyEx* enemy, int index) {
    if (!enemy->active) return;
    
    float scale = (enemy->radius * 2.0f) / SPRITE_SIZE;
    
    // Apply ghost transparency
    Color tint = WHITE;
    if (enemy->type == ENEMY_GHOST && !enemy->isVisible) {
        tint = Fade(WHITE, 0.3f);
    }
    
    // Draw sprite from appropriate sprite sheet or individual sprites
    Vector2 position = {
        enemy->position.x - (SPRITE_SIZE * scale) / 2,
        enemy->position.y - (SPRITE_SIZE * scale) / 2
    };
    
    // Choose sprite based on current set
    if (state->currentSpriteSet == SPRITE_SET_SPACESHIP) {
        if (state->spaceshipSpriteSheet.id > 0) {
            DrawTextureRec(state->spaceshipSpriteSheet, state->spriteRects[index], position, tint);
        } else if (state->spaceshipSprites[index].id > 0) {
            DrawTextureEx(state->spaceshipSprites[index], position, 0, scale, tint);
        }
    } else {
        if (state->spriteSheet.id > 0) {
            DrawTextureRec(state->spriteSheet, state->spriteRects[index], position, tint);
        } else if (state->individualSprites[index].id > 0) {
            DrawTextureEx(state->individualSprites[index], position, 0, scale, tint);
        }
    }
    
    // Add special effects on top
    switch (enemy->type) {
        case ENEMY_SHIELD:
            // Draw rotating shield orbs
            for (int i = 0; i < 6; i++) {
                float angle = enemy->shieldAngle + (i * 60);
                float x = enemy->position.x + cosf(angle * DEG2RAD) * (enemy->radius + 10);
                float y = enemy->position.y + sinf(angle * DEG2RAD) * (enemy->radius + 10);
                DrawCircle(x, y, 5, Fade(enemy->glowColor, 0.7f));
            }
            break;
            
        case ENEMY_SPEEDER:
            // Draw speed lines
            for (int i = 0; i < 3; i++) {
                DrawLine(
                    enemy->position.x + enemy->radius + i * 5,
                    enemy->position.y - 5 + i * 5,
                    enemy->position.x + enemy->radius + 20 + i * 5,
                    enemy->position.y - 5 + i * 5,
                    Fade(enemy->glowColor, 0.7f - i * 0.2f)
                );
            }
            break;
            
        case ENEMY_BOMBER:
            // Danger indicator
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                          enemy->radius + 5 + sinf(enemy->animationTimer * 5) * 3, 
                          Fade(RED, 0.5f));
            break;
            
        case ENEMY_BOSS:
            // Multiple rings
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                          enemy->radius + 10, Fade(RED, 0.7f));
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                          enemy->radius + 20, Fade(MAROON, 0.5f));
            break;
            
        default:
            break;
    }
    
    // Health bar (if damaged)
    if (enemy->health < enemy->maxHealth && enemy->maxHealth > 1) {
        float barWidth = enemy->radius * 2;
        float barHeight = 4;
        float healthPercent = (float)enemy->health / (float)enemy->maxHealth;
        
        DrawRectangle(enemy->position.x - enemy->radius, 
                     enemy->position.y - enemy->radius - 10,
                     barWidth, barHeight, Fade(RED, 0.3f));
        DrawRectangle(enemy->position.x - enemy->radius, 
                     enemy->position.y - enemy->radius - 10,
                     barWidth * healthPercent, barHeight, Fade(GREEN, 0.8f));
    }
}

void DrawShowcase(ShowcaseState* state) {
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
        EnemyEx* enemy = &state->enemies[i];
        
        // Highlight selected enemy
        if (state->selectedEnemy == i) {
            DrawCircle(enemy->position.x, enemy->position.y, 
                      enemy->radius + 20, Fade(WHITE, 0.2f));
        }
        
        // Draw the enemy (sprite or procedural)
        if (state->useSprites && state->spritesLoaded) {
            DrawEnemyWithSprite(state, enemy, i);
        } else {
            DrawEnemyEx(enemy);
        }
        
        // Draw enemy info
        if (state->showDetails) {
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
            
            // Size and resistance
            snprintf(stats, sizeof(stats), "Size:%.1fx Resist:%.0f%%", 
                    def->size, def->resistance * 100);
            statsWidth = MeasureText(stats, fontSize);
            DrawText(stats,
                    enemy->position.x - statsWidth/2,
                    enemy->position.y + enemy->radius + 52,
                    fontSize, LIGHTGRAY);
        }
    }
    
    EndMode2D();
    
    // Draw UI overlay
    DrawRectangle(0, 0, SHOWCASE_WIDTH, 80, Fade(BLACK, 0.7f));
    DrawText("ENEMY SHOWCASE (SPRITE VERSION)", 20, 10, 30, WHITE);
    DrawText("Controls: [SPACE] Pause | [D] Details | [S] Toggle Sprites | [T] Switch Sprite Set | [R] Reset Camera", 
             20, 40, 14, LIGHTGRAY);
    
    // Show sprite status
    char spriteStatus[256];
    if (state->useSprites) {
        const char* setName = state->currentSpriteSet == SPRITE_SET_SPACESHIP ? "Spaceship" : "Abstract";
        snprintf(spriteStatus, sizeof(spriteStatus), "Using %s Sprites (Press T to switch, S for procedural)", setName);
    } else {
        snprintf(spriteStatus, sizeof(spriteStatus), "Using Procedural Graphics (Press S for sprites)");
    }
    DrawText(spriteStatus, 20, 60, 12, state->useSprites ? GREEN : YELLOW);
    
    // Draw selected enemy details
    if (state->selectedEnemy >= 0) {
        EnemyEx* enemy = &state->enemies[state->selectedEnemy];
        const EnemyTypeDefinition* def = GetEnemyTypeDefinition(enemy->type);
        
        int panelWidth = 300;
        int panelHeight = 200;
        int panelX = SHOWCASE_WIDTH - panelWidth - 20;
        int panelY = 100;
        
        DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade(BLACK, 0.8f));
        DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, def->primaryColor);
        
        int textY = panelY + 10;
        DrawText(def->name, panelX + 10, textY, 24, def->primaryColor);
        textY += 35;
        
        char info[256];
        snprintf(info, sizeof(info), "Health: %d", def->health);
        DrawText(info, panelX + 10, textY, 16, WHITE);
        textY += 20;
        
        snprintf(info, sizeof(info), "Power: %d", def->power);
        DrawText(info, panelX + 10, textY, 16, WHITE);
        textY += 20;
        
        snprintf(info, sizeof(info), "Speed: %.1fx", def->speed);
        DrawText(info, panelX + 10, textY, 16, WHITE);
        textY += 20;
        
        snprintf(info, sizeof(info), "Size: %.1fx", def->size);
        DrawText(info, panelX + 10, textY, 16, WHITE);
        textY += 20;
        
        snprintf(info, sizeof(info), "Damage Resistance: %.0f%%", def->resistance * 100);
        DrawText(info, panelX + 10, textY, 16, WHITE);
        textY += 20;
        
        snprintf(info, sizeof(info), "Symbol: %s", def->symbol);
        DrawText(info, panelX + 10, textY, 16, WHITE);
    }
    
    // Animation status
    if (state->animationPaused) {
        DrawText("PAUSED", SHOWCASE_WIDTH - 100, SHOWCASE_HEIGHT - 30, 20, YELLOW);
    }
    
    // FPS
    DrawFPS(10, SHOWCASE_HEIGHT - 30);
    
    EndDrawing();
}

void CleanupShowcase(ShowcaseState* state) {
    if (state->spritesLoaded) {
        if (state->spriteSheet.id > 0) {
            UnloadTexture(state->spriteSheet);
        }
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            if (state->individualSprites[i].id > 0) {
                UnloadTexture(state->individualSprites[i]);
            }
        }
    }
    if (state->spaceshipSpritesLoaded) {
        if (state->spaceshipSpriteSheet.id > 0) {
            UnloadTexture(state->spaceshipSpriteSheet);
        }
        for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
            if (state->spaceshipSprites[i].id > 0) {
                UnloadTexture(state->spaceshipSprites[i]);
            }
        }
    }
}

int main(void) {
    // Initialize window
    InitWindow(SHOWCASE_WIDTH, SHOWCASE_HEIGHT, "Enemy Showcase - Sprite Version");
    SetTargetFPS(60);
    
    // Initialize showcase state
    ShowcaseState state = {0};
    InitShowcase(&state);
    
    // Main game loop
    while (!WindowShouldClose()) {
        UpdateShowcase(&state);
        DrawShowcase(&state);
    }
    
    // Cleanup
    CleanupShowcase(&state);
    CloseWindow();
    
    return 0;
}
