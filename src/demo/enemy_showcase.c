#include "raylib.h"
#include "enemy_types.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SHOWCASE_WIDTH 1400
#define SHOWCASE_HEIGHT 800
#define ENEMIES_PER_ROW 5
#define ENEMY_SPACING_X 250
#define ENEMY_SPACING_Y 350
#define START_X 150
#define START_Y 150

typedef struct {
    EnemyEx enemies[ENEMY_TYPE_COUNT];
    float globalTimer;
    int selectedEnemy;
    bool showDetails;
    bool animationPaused;
    Camera2D camera;
} ShowcaseState;

void InitShowcase(ShowcaseState* state) {
    // Initialize enemy types
    InitEnemyTypes();
    
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
                    // Pulsing effect (size change simulated by position wobble)
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
        
        // Draw the enemy
        DrawEnemyEx(enemy);
        
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
    DrawRectangle(0, 0, SHOWCASE_WIDTH, 60, Fade(BLACK, 0.7f));
    DrawText("ENEMY SHOWCASE", 20, 10, 30, WHITE);
    DrawText("Controls: [SPACE] Pause Animation | [D] Toggle Details | [R] Reset Camera | Mouse Wheel: Zoom | Arrow Keys: Pan", 
             20, 40, 14, LIGHTGRAY);
    
    // Draw selected enemy details
    if (state->selectedEnemy >= 0) {
        EnemyEx* enemy = &state->enemies[state->selectedEnemy];
        const EnemyTypeDefinition* def = GetEnemyTypeDefinition(enemy->type);
        
        int panelWidth = 300;
        int panelHeight = 200;
        int panelX = SHOWCASE_WIDTH - panelWidth - 20;
        int panelY = 80;
        
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

int main(void) {
    // Initialize window
    InitWindow(SHOWCASE_WIDTH, SHOWCASE_HEIGHT, "Enemy Type Showcase");
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
    CloseWindow();
    
    return 0;
}
