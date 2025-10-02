#include "raylib.h"
#include "enemy_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SPRITE_SIZE 64
#define SPRITES_PER_ROW 5
#define SHEET_WIDTH (SPRITE_SIZE * SPRITES_PER_ROW)
#define SHEET_HEIGHT (SPRITE_SIZE * 2)  // 2 rows for 10 enemies

// Helper function to draw enemy sprite at specific position
void DrawEnemySprite(RenderTexture2D* target, EnemyType type, int gridX, int gridY) {
    BeginTextureMode(*target);
    
    const EnemyTypeDefinition* def = GetEnemyTypeDefinition(type);
    
    // Calculate center position in the grid cell
    float centerX = gridX * SPRITE_SIZE + SPRITE_SIZE / 2.0f;
    float centerY = gridY * SPRITE_SIZE + SPRITE_SIZE / 2.0f;
    
    // Base radius scaled to fit in sprite
    float baseRadius = (SPRITE_SIZE * 0.35f) * def->size;
    if (baseRadius > SPRITE_SIZE * 0.45f) baseRadius = SPRITE_SIZE * 0.45f;
    
    // Draw based on enemy type
    switch (type) {
        case ENEMY_GRUNT:
            // Triangle shape
            DrawTriangle(
                (Vector2){centerX, centerY - baseRadius},
                (Vector2){centerX - baseRadius * 0.866f, centerY + baseRadius * 0.5f},
                (Vector2){centerX + baseRadius * 0.866f, centerY + baseRadius * 0.5f},
                def->primaryColor
            );
            DrawTriangle(
                (Vector2){centerX, centerY - baseRadius * 0.7f},
                (Vector2){centerX - baseRadius * 0.606f, centerY + baseRadius * 0.35f},
                (Vector2){centerX + baseRadius * 0.606f, centerY + baseRadius * 0.35f},
                def->secondaryColor
            );
            // Eye
            DrawCircle(centerX, centerY - baseRadius * 0.2f, baseRadius * 0.15f, WHITE);
            DrawCircle(centerX, centerY - baseRadius * 0.2f, baseRadius * 0.08f, BLACK);
            break;
            
        case ENEMY_TANK:
            // Square with armor
            DrawRectangle(centerX - baseRadius, centerY - baseRadius, 
                         baseRadius * 2, baseRadius * 2, def->primaryColor);
            DrawRectangle(centerX - baseRadius * 0.7f, centerY - baseRadius * 0.7f, 
                         baseRadius * 1.4f, baseRadius * 1.4f, def->secondaryColor);
            // Armor plates
            DrawRectangleLines(centerX - baseRadius * 0.9f, centerY - baseRadius * 0.9f,
                              baseRadius * 1.8f, baseRadius * 1.8f, DARKGRAY);
            DrawRectangleLines(centerX - baseRadius * 0.95f, centerY - baseRadius * 0.95f,
                              baseRadius * 1.9f, baseRadius * 1.9f, GRAY);
            // Cannon
            DrawRectangle(centerX - baseRadius * 0.1f, centerY - baseRadius * 1.2f,
                         baseRadius * 0.2f, baseRadius * 0.4f, DARKGRAY);
            break;
            
        case ENEMY_SPEEDER:
            // Arrow/chevron shape
            for (int i = 0; i < 3; i++) {
                float offset = i * baseRadius * 0.3f;
                Color color = Fade(def->primaryColor, 1.0f - i * 0.3f);
                DrawTriangle(
                    (Vector2){centerX + offset, centerY},
                    (Vector2){centerX - baseRadius * 0.7f + offset, centerY - baseRadius * 0.5f},
                    (Vector2){centerX - baseRadius * 0.7f + offset, centerY + baseRadius * 0.5f},
                    color
                );
            }
            // Engine glow
            DrawCircle(centerX + baseRadius * 0.8f, centerY, baseRadius * 0.3f, 
                      Fade(ORANGE, 0.7f));
            DrawCircle(centerX + baseRadius * 0.8f, centerY, baseRadius * 0.2f, YELLOW);
            break;
            
        case ENEMY_ZIGZAG:
            // Diamond with zigzag pattern
            DrawPoly((Vector2){centerX, centerY}, 4, baseRadius, 45, def->primaryColor);
            DrawPoly((Vector2){centerX, centerY}, 4, baseRadius * 0.7f, 45, def->secondaryColor);
            // Zigzag lines
            for (int i = -1; i <= 1; i++) {
                float y = centerY + i * baseRadius * 0.3f;
                DrawLineEx(
                    (Vector2){centerX - baseRadius * 0.5f, y},
                    (Vector2){centerX, y - baseRadius * 0.15f},
                    2, def->glowColor
                );
                DrawLineEx(
                    (Vector2){centerX, y - baseRadius * 0.15f},
                    (Vector2){centerX + baseRadius * 0.5f, y},
                    2, def->glowColor
                );
            }
            break;
            
        case ENEMY_BOMBER:
            // Circle with danger markings
            DrawCircle(centerX, centerY, baseRadius, def->primaryColor);
            DrawCircle(centerX, centerY, baseRadius * 0.8f, def->secondaryColor);
            // Danger symbol
            DrawRing((Vector2){centerX, centerY}, baseRadius * 0.9f, baseRadius * 1.1f, 0, 360, 32, RED);
            // Bomb icon
            DrawCircle(centerX, centerY + baseRadius * 0.2f, baseRadius * 0.25f, BLACK);
            DrawRectangle(centerX - baseRadius * 0.05f, centerY - baseRadius * 0.2f,
                         baseRadius * 0.1f, baseRadius * 0.4f, BLACK);
            // Fuse
            DrawLineEx(
                (Vector2){centerX, centerY - baseRadius * 0.2f},
                (Vector2){centerX + baseRadius * 0.2f, centerY - baseRadius * 0.4f},
                2, ORANGE
            );
            break;
            
        case ENEMY_SHIELD:
            // Hexagon with shield elements
            DrawPoly((Vector2){centerX, centerY}, 6, baseRadius, 0, def->primaryColor);
            DrawPoly((Vector2){centerX, centerY}, 6, baseRadius * 0.7f, 0, def->secondaryColor);
            // Shield orbs
            for (int i = 0; i < 6; i++) {
                float angle = (60 * i) * DEG2RAD;
                float x = centerX + cosf(angle) * baseRadius * 0.9f;
                float y = centerY + sinf(angle) * baseRadius * 0.9f;
                DrawCircle(x, y, baseRadius * 0.15f, def->glowColor);
                DrawCircle(x, y, baseRadius * 0.1f, Fade(WHITE, 0.8f));
            }
            // Center core
            DrawCircle(centerX, centerY, baseRadius * 0.2f, Fade(SKYBLUE, 0.8f));
            break;
            
        case ENEMY_SWARM:
            // Multiple small circles in formation
            float swarmRadius = baseRadius * 0.3f;
            // Center
            DrawCircle(centerX, centerY, swarmRadius, def->primaryColor);
            // Surrounding swarm
            for (int i = 0; i < 6; i++) {
                float angle = (60 * i) * DEG2RAD;
                float x = centerX + cosf(angle) * baseRadius * 0.6f;
                float y = centerY + sinf(angle) * baseRadius * 0.6f;
                DrawCircle(x, y, swarmRadius * 0.7f, def->secondaryColor);
                // Connection lines
                DrawLineEx(
                    (Vector2){centerX, centerY},
                    (Vector2){x, y},
                    1, Fade(def->glowColor, 0.5f)
                );
            }
            break;
            
        case ENEMY_ELITE:
            // Star shape with decorations
            // Outer star
            for (int i = 0; i < 5; i++) {
                float angle1 = (72 * i - 90) * DEG2RAD;
                float angle2 = (72 * (i + 1) - 90) * DEG2RAD;
                float angle3 = (72 * i - 90 + 36) * DEG2RAD;
                
                Vector2 p1 = {centerX + cosf(angle1) * baseRadius, centerY + sinf(angle1) * baseRadius};
                Vector2 p2 = {centerX + cosf(angle2) * baseRadius, centerY + sinf(angle2) * baseRadius};
                Vector2 p3 = {centerX + cosf(angle3) * baseRadius * 0.5f, centerY + sinf(angle3) * baseRadius * 0.5f};
                
                DrawTriangle(p1, p3, (Vector2){centerX, centerY}, def->primaryColor);
                DrawTriangle(p2, p3, (Vector2){centerX, centerY}, def->primaryColor);
            }
            // Inner star
            DrawPoly((Vector2){centerX, centerY}, 5, baseRadius * 0.6f, -90, def->secondaryColor);
            // Center gem
            DrawPoly((Vector2){centerX, centerY}, 6, baseRadius * 0.3f, 0, GOLD);
            DrawPoly((Vector2){centerX, centerY}, 6, baseRadius * 0.2f, 0, YELLOW);
            break;
            
        case ENEMY_GHOST:
            // Semi-transparent wispy shape
            // Main body (fading circles)
            for (int i = 3; i >= 0; i--) {
                float factor = 1.0f - i * 0.2f;
                float radius = baseRadius * factor;
                Color color = def->primaryColor;
                color.a = (unsigned char)(100 + i * 30);
                DrawCircle(centerX, centerY - i * 2, radius, color);
            }
            // Wispy tail
            for (int i = 0; i < 3; i++) {
                float offset = i * baseRadius * 0.3f;
                float radius = baseRadius * (0.6f - i * 0.15f);
                Color color = def->secondaryColor;
                color.a = (unsigned char)(150 - i * 40);
                DrawCircle(centerX - offset * 0.3f, centerY + offset, radius, color);
                DrawCircle(centerX + offset * 0.3f, centerY + offset, radius, color);
            }
            // Eyes
            DrawCircle(centerX - baseRadius * 0.25f, centerY - baseRadius * 0.2f, 
                      baseRadius * 0.12f, Fade(WHITE, 0.9f));
            DrawCircle(centerX + baseRadius * 0.25f, centerY - baseRadius * 0.2f, 
                      baseRadius * 0.12f, Fade(WHITE, 0.9f));
            DrawCircle(centerX - baseRadius * 0.25f, centerY - baseRadius * 0.2f, 
                      baseRadius * 0.06f, BLACK);
            DrawCircle(centerX + baseRadius * 0.25f, centerY - baseRadius * 0.2f, 
                      baseRadius * 0.06f, BLACK);
            break;
            
        case ENEMY_BOSS:
            // Large menacing shape with multiple layers
            // Outer spikes
            for (int i = 0; i < 8; i++) {
                float angle = (45 * i) * DEG2RAD;
                Vector2 tip = {centerX + cosf(angle) * baseRadius, centerY + sinf(angle) * baseRadius};
                Vector2 base1 = {centerX + cosf(angle - 0.2f) * baseRadius * 0.6f, 
                                centerY + sinf(angle - 0.2f) * baseRadius * 0.6f};
                Vector2 base2 = {centerX + cosf(angle + 0.2f) * baseRadius * 0.6f, 
                                centerY + sinf(angle + 0.2f) * baseRadius * 0.6f};
                DrawTriangle(tip, base1, base2, MAROON);
            }
            // Main body
            DrawCircle(centerX, centerY, baseRadius * 0.7f, def->primaryColor);
            DrawCircle(centerX, centerY, baseRadius * 0.55f, def->secondaryColor);
            // Core with pulsing rings
            DrawRing((Vector2){centerX, centerY}, baseRadius * 0.4f, baseRadius * 0.45f, 0, 360, 32, RED);
            DrawRing((Vector2){centerX, centerY}, baseRadius * 0.3f, baseRadius * 0.35f, 0, 360, 32, ORANGE);
            // Evil eye
            DrawCircle(centerX, centerY, baseRadius * 0.25f, BLACK);
            DrawCircle(centerX, centerY, baseRadius * 0.15f, RED);
            DrawCircle(centerX, centerY, baseRadius * 0.08f, YELLOW);
            // Menacing details
            DrawLineEx(
                (Vector2){centerX - baseRadius * 0.4f, centerY - baseRadius * 0.3f},
                (Vector2){centerX - baseRadius * 0.15f, centerY - baseRadius * 0.1f},
                3, RED
            );
            DrawLineEx(
                (Vector2){centerX + baseRadius * 0.4f, centerY - baseRadius * 0.3f},
                (Vector2){centerX + baseRadius * 0.15f, centerY - baseRadius * 0.1f},
                3, RED
            );
            break;
            
        default:
            // Fallback circle
            DrawCircle(centerX, centerY, baseRadius, def->primaryColor);
            DrawCircle(centerX, centerY, baseRadius * 0.7f, def->secondaryColor);
            break;
    }
    
    EndTextureMode();
}

int main(void) {
    // Initialize
    InitWindow(SHEET_WIDTH, SHEET_HEIGHT, "Enemy Sprite Generator");
    SetTargetFPS(60);
    
    // Initialize enemy types
    InitEnemyTypes();
    
    // Create render texture for sprite sheet
    RenderTexture2D spriteSheet = LoadRenderTexture(SHEET_WIDTH, SHEET_HEIGHT);
    
    // Clear the sprite sheet with transparent background
    BeginTextureMode(spriteSheet);
    ClearBackground(BLANK);  // Transparent background
    EndTextureMode();
    
    // Generate sprites for each enemy type
    printf("Generating enemy sprites...\n");
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        int gridX = i % SPRITES_PER_ROW;
        int gridY = i / SPRITES_PER_ROW;
        
        DrawEnemySprite(&spriteSheet, (EnemyType)i, gridX, gridY);
        
        const char* name = GetEnemyTypeName((EnemyType)i);
        printf("  - Generated sprite for %s at position (%d, %d)\n", name, gridX, gridY);
    }
    
    // Also generate individual sprite files
    printf("\nGenerating individual sprite files...\n");
    
    // Create assets directory if it doesn't exist
    system("mkdir -p assets/sprites/enemies");
    
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        // Create individual render texture
        RenderTexture2D individualSprite = LoadRenderTexture(SPRITE_SIZE, SPRITE_SIZE);
        
        // Clear with transparent background
        BeginTextureMode(individualSprite);
        ClearBackground(BLANK);
        EndTextureMode();
        
        // Draw the enemy sprite
        DrawEnemySprite(&individualSprite, (EnemyType)i, 0, 0);
        
        // Export to file
        Image img = LoadImageFromTexture(individualSprite.texture);
        ImageFlipVertical(&img);  // Flip because render textures are upside down
        
        char filename[256];
        const char* name = GetEnemyTypeName((EnemyType)i);
        // Convert name to lowercase for filename
        char lowername[64];
        for (int j = 0; name[j]; j++) {
            lowername[j] = (name[j] >= 'A' && name[j] <= 'Z') ? name[j] + 32 : name[j];
            lowername[j+1] = '\0';
        }
        snprintf(filename, sizeof(filename), "assets/sprites/enemies/enemy_%s.png", lowername);
        
        ExportImage(img, filename);
        printf("  - Saved %s\n", filename);
        
        UnloadImage(img);
        UnloadRenderTexture(individualSprite);
    }
    
    // Export the complete sprite sheet
    Image spriteSheetImg = LoadImageFromTexture(spriteSheet.texture);
    ImageFlipVertical(&spriteSheetImg);  // Flip because render textures are upside down
    ExportImage(spriteSheetImg, "assets/sprites/enemy_spritesheet.png");
    printf("\nSaved complete sprite sheet to assets/sprites/enemy_spritesheet.png\n");
    printf("Sprite sheet dimensions: %dx%d\n", SHEET_WIDTH, SHEET_HEIGHT);
    printf("Individual sprite size: %dx%d\n", SPRITE_SIZE, SPRITE_SIZE);
    
    // Display the sprite sheet
    bool showGrid = true;
    while (!WindowShouldClose()) {
        // Toggle grid
        if (IsKeyPressed(KEY_G)) showGrid = !showGrid;
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        // Draw the sprite sheet
        DrawTextureRec(spriteSheet.texture, 
                      (Rectangle){0, 0, SHEET_WIDTH, -SHEET_HEIGHT},
                      (Vector2){0, 0}, WHITE);
        
        // Draw grid overlay
        if (showGrid) {
            for (int i = 0; i <= SPRITES_PER_ROW; i++) {
                DrawLine(i * SPRITE_SIZE, 0, i * SPRITE_SIZE, SHEET_HEIGHT, Fade(WHITE, 0.3f));
            }
            for (int i = 0; i <= 2; i++) {
                DrawLine(0, i * SPRITE_SIZE, SHEET_WIDTH, i * SPRITE_SIZE, Fade(WHITE, 0.3f));
            }
            
            // Label each sprite
            for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
                int gridX = i % SPRITES_PER_ROW;
                int gridY = i / SPRITES_PER_ROW;
                const char* name = GetEnemyTypeName((EnemyType)i);
                DrawText(name, gridX * SPRITE_SIZE + 2, gridY * SPRITE_SIZE + 2, 10, WHITE);
            }
        }
        
        DrawText("Press [G] to toggle grid | Press [ESC] to exit", 10, SHEET_HEIGHT - 20, 12, WHITE);
        
        EndDrawing();
    }
    
    // Cleanup
    UnloadImage(spriteSheetImg);
    UnloadRenderTexture(spriteSheet);
    CloseWindow();
    
    printf("\nSprite generation complete!\n");
    return 0;
}
