#include "raylib.h"
#include "projectile_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SPRITE_SIZE 32
#define SPRITES_PER_ROW 4
#define SHEET_WIDTH (SPRITE_SIZE * SPRITES_PER_ROW)
#define SHEET_HEIGHT SPRITE_SIZE

// Helper function to draw projectile sprite at specific position
void DrawProjectileSpriteToTexture(RenderTexture2D* target, ProjectileType type, int gridX, int gridY) {
    BeginTextureMode(*target);
    
    const ProjectileDefinition* def = GetProjectileDefinition(type);
    
    // Calculate center position in the grid cell
    float centerX = gridX * SPRITE_SIZE + SPRITE_SIZE / 2.0f;
    float centerY = gridY * SPRITE_SIZE + SPRITE_SIZE / 2.0f;
    
    // Draw based on projectile type
    switch (type) {
        case PROJECTILE_LASER:
            // Laser bolt design
            {
                // Main beam
                float beamLength = SPRITE_SIZE * 0.8f;
                float beamWidth = SPRITE_SIZE * 0.15f;
                
                // Draw from left to right (standard orientation)
                Vector2 start = {centerX - beamLength/2, centerY};
                Vector2 end = {centerX + beamLength/2, centerY};
                
                // Outer glow
                DrawLineEx(start, end, beamWidth * 2.5f, Fade(def->glowColor, 0.3f));
                
                // Middle layer
                DrawLineEx(start, end, beamWidth * 1.5f, Fade(def->secondaryColor, 0.7f));
                
                // Core beam
                DrawLineEx(start, end, beamWidth, def->primaryColor);
                
                // Bright center line
                DrawLineEx(start, end, beamWidth * 0.5f, Fade(WHITE, 0.9f));
                
                // Front glow
                DrawCircle(end.x, end.y, beamWidth * 1.5f, Fade(def->glowColor, 0.5f));
                DrawCircle(end.x, end.y, beamWidth, def->primaryColor);
                DrawCircle(end.x, end.y, beamWidth * 0.5f, WHITE);
            }
            break;
            
        case PROJECTILE_PLASMA:
            // Plasma ball with electric effect
            {
                float radius = SPRITE_SIZE * 0.35f;
                
                // Outer glow
                DrawCircle(centerX, centerY, radius * 1.4f, Fade(def->glowColor, 0.2f));
                
                // Electric arcs
                for (int i = 0; i < 6; i++) {
                    float angle = (i * 60) * DEG2RAD;
                    float arcLength = radius * 1.3f;
                    Vector2 arcEnd = {
                        centerX + cosf(angle) * arcLength,
                        centerY + sinf(angle) * arcLength
                    };
                    
                    // Draw lightning bolt effect
                    Vector2 mid1 = {
                        centerX + cosf(angle + 0.2f) * radius * 0.7f,
                        centerY + sinf(angle + 0.2f) * radius * 0.7f
                    };
                    Vector2 mid2 = {
                        centerX + cosf(angle - 0.2f) * radius * 1.0f,
                        centerY + sinf(angle - 0.2f) * radius * 1.0f
                    };
                    
                    DrawLineEx((Vector2){centerX, centerY}, mid1, 1, Fade(def->glowColor, 0.6f));
                    DrawLineEx(mid1, mid2, 1, Fade(def->glowColor, 0.5f));
                    DrawLineEx(mid2, arcEnd, 1, Fade(def->glowColor, 0.4f));
                }
                
                // Main plasma ball
                DrawCircle(centerX, centerY, radius, def->primaryColor);
                
                // Inner bright core
                DrawCircle(centerX, centerY, radius * 0.7f, def->secondaryColor);
                DrawCircle(centerX, centerY, radius * 0.4f, Fade(WHITE, 0.9f));
                
                // Hot spot
                DrawCircle(centerX - radius * 0.2f, centerY - radius * 0.2f, 
                          radius * 0.2f, Fade(WHITE, 0.7f));
            }
            break;
            
        case PROJECTILE_MISSILE:
            // Missile with fins and exhaust
            {
                float length = SPRITE_SIZE * 0.7f;
                float width = SPRITE_SIZE * 0.2f;
                
                // Missile body (pointing right)
                DrawRectangle(centerX - length/2, centerY - width/2, 
                             length * 0.7f, width, def->primaryColor);
                
                // Warhead (cone shape)
                Vector2 tip = {centerX + length/2, centerY};
                Vector2 topBase = {centerX + length/2 - width, centerY - width/2};
                Vector2 bottomBase = {centerX + length/2 - width, centerY + width/2};
                DrawTriangle(tip, topBase, bottomBase, DARKGRAY);
                
                // Red tip
                DrawCircle(tip.x - 2, tip.y, width * 0.3f, RED);
                
                // Fins
                Vector2 finBack = {centerX - length/2, centerY};
                Vector2 finTop = {centerX - length/2 + width, centerY - width};
                Vector2 finBottom = {centerX - length/2 + width, centerY + width};
                Vector2 finMid = {centerX - length/2 + width * 0.5f, centerY};
                
                DrawTriangle(finBack, finTop, finMid, def->primaryColor);
                DrawTriangle(finBack, finBottom, finMid, def->primaryColor);
                
                // Exhaust flame
                float exhaustX = centerX - length/2 - width * 0.5f;
                
                // Outer flame
                DrawCircle(exhaustX, centerY, width * 0.8f, Fade(def->secondaryColor, 0.5f));
                
                // Middle flame
                DrawCircle(exhaustX, centerY, width * 0.6f, def->secondaryColor);
                
                // Inner hot flame
                DrawCircle(exhaustX, centerY, width * 0.4f, def->glowColor);
                DrawCircle(exhaustX, centerY, width * 0.2f, YELLOW);
                
                // Flame trail
                for (int i = 0; i < 3; i++) {
                    float trailX = exhaustX - i * width * 0.3f;
                    float trailRadius = width * 0.5f * (1.0f - i * 0.3f);
                    DrawCircle(trailX, centerY, trailRadius, 
                              Fade(def->secondaryColor, 0.5f - i * 0.15f));
                }
            }
            break;
            
        case PROJECTILE_ENERGY_ORB:
            // Energy orb with rotating rings
            {
                float radius = SPRITE_SIZE * 0.4f;
                
                // Outer aura
                DrawCircle(centerX, centerY, radius * 1.3f, Fade(def->glowColor, 0.15f));
                
                // Energy rings (static representation)
                // Horizontal ring
                DrawEllipse(centerX, centerY, radius * 1.2f, radius * 0.4f, 
                           Fade(def->glowColor, 0.4f));
                
                // Vertical ring
                DrawEllipse(centerX, centerY, radius * 0.4f, radius * 1.2f, 
                           Fade(def->glowColor, 0.4f));
                
                // Main orb
                DrawCircle(centerX, centerY, radius, def->primaryColor);
                
                // Inner layers
                DrawCircle(centerX, centerY, radius * 0.8f, def->secondaryColor);
                DrawCircle(centerX, centerY, radius * 0.6f, Fade(def->glowColor, 0.8f));
                
                // Energy core
                DrawCircle(centerX, centerY, radius * 0.3f, Fade(WHITE, 0.9f));
                
                // Particle effects around the orb
                for (int i = 0; i < 8; i++) {
                    float angle = (i * 45) * DEG2RAD;
                    float particleX = centerX + cosf(angle) * radius * 0.9f;
                    float particleY = centerY + sinf(angle) * radius * 0.9f;
                    DrawCircle(particleX, particleY, 2, Fade(def->glowColor, 0.6f));
                }
                
                // Highlight
                DrawCircle(centerX - radius * 0.3f, centerY - radius * 0.3f, 
                          radius * 0.25f, Fade(WHITE, 0.5f));
            }
            break;
            
        default:
            // Fallback circle
            DrawCircle(centerX, centerY, SPRITE_SIZE * 0.3f, def->primaryColor);
            break;
    }
    
    EndTextureMode();
}

int main(void) {
    // Initialize
    InitWindow(SHEET_WIDTH, SHEET_HEIGHT, "Projectile Sprite Generator");
    SetTargetFPS(60);
    
    // Initialize projectile types
    InitProjectileTypes();
    
    // Create render texture for sprite sheet
    RenderTexture2D spriteSheet = LoadRenderTexture(SHEET_WIDTH, SHEET_HEIGHT);
    
    // Clear the sprite sheet with transparent background
    BeginTextureMode(spriteSheet);
    ClearBackground(BLANK);  // Transparent background
    EndTextureMode();
    
    // Generate sprites for each projectile type
    printf("Generating projectile sprites...\n");
    for (int i = 0; i < PROJECTILE_TYPE_COUNT; i++) {
        int gridX = i % SPRITES_PER_ROW;
        int gridY = i / SPRITES_PER_ROW;
        
        DrawProjectileSpriteToTexture(&spriteSheet, (ProjectileType)i, gridX, gridY);
        
        const ProjectileDefinition* def = GetProjectileDefinition((ProjectileType)i);
        printf("  - Generated sprite for %s at position (%d, %d)\n", def->name, gridX, gridY);
    }
    
    // Also generate individual sprite files
    printf("\nGenerating individual projectile sprite files...\n");
    
    // Create assets directory if it doesn't exist
    system("mkdir -p assets/sprites/projectiles");
    
    for (int i = 0; i < PROJECTILE_TYPE_COUNT; i++) {
        // Create individual render texture
        RenderTexture2D individualSprite = LoadRenderTexture(SPRITE_SIZE, SPRITE_SIZE);
        
        // Clear with transparent background
        BeginTextureMode(individualSprite);
        ClearBackground(BLANK);
        EndTextureMode();
        
        // Draw the projectile sprite
        DrawProjectileSpriteToTexture(&individualSprite, (ProjectileType)i, 0, 0);
        
        // Export to file
        Image img = LoadImageFromTexture(individualSprite.texture);
        ImageFlipVertical(&img);  // Flip because render textures are upside down
        
        char filename[256];
        const ProjectileDefinition* def = GetProjectileDefinition((ProjectileType)i);
        // Convert name to lowercase for filename
        char lowername[64];
        for (int j = 0; def->name[j]; j++) {
            if (def->name[j] == ' ') {
                lowername[j] = '_';
            } else {
                lowername[j] = (def->name[j] >= 'A' && def->name[j] <= 'Z') ? 
                               def->name[j] + 32 : def->name[j];
            }
            lowername[j+1] = '\0';
        }
        snprintf(filename, sizeof(filename), "assets/sprites/projectiles/projectile_%s.png", lowername);
        
        ExportImage(img, filename);
        printf("  - Saved %s\n", filename);
        
        UnloadImage(img);
        UnloadRenderTexture(individualSprite);
    }
    
    // Export the complete sprite sheet
    Image spriteSheetImg = LoadImageFromTexture(spriteSheet.texture);
    ImageFlipVertical(&spriteSheetImg);  // Flip because render textures are upside down
    ExportImage(spriteSheetImg, "assets/sprites/projectile_spritesheet.png");
    printf("\nSaved complete projectile sprite sheet to assets/sprites/projectile_spritesheet.png\n");
    printf("Sprite sheet dimensions: %dx%d\n", SHEET_WIDTH, SHEET_HEIGHT);
    printf("Individual sprite size: %dx%d\n", SPRITE_SIZE, SPRITE_SIZE);
    
    // Display the sprite sheet with labels
    bool showGrid = true;
    bool showInfo = true;
    float scale = 4.0f;  // Scale up for better visibility
    
    SetWindowSize(SHEET_WIDTH * scale, SHEET_HEIGHT * scale + 100);
    
    while (!WindowShouldClose()) {
        // Toggle grid
        if (IsKeyPressed(KEY_G)) showGrid = !showGrid;
        if (IsKeyPressed(KEY_I)) showInfo = !showInfo;
        
        BeginDrawing();
        ClearBackground(DARKGRAY);
        
        // Draw the sprite sheet scaled up
        DrawTextureEx(spriteSheet.texture, (Vector2){0, 0}, 0, scale, WHITE);
        
        // Draw grid overlay
        if (showGrid) {
            for (int i = 0; i <= SPRITES_PER_ROW; i++) {
                DrawLine(i * SPRITE_SIZE * scale, 0, 
                        i * SPRITE_SIZE * scale, SHEET_HEIGHT * scale, 
                        Fade(WHITE, 0.3f));
            }
            for (int i = 0; i <= 1; i++) {
                DrawLine(0, i * SPRITE_SIZE * scale, 
                        SHEET_WIDTH * scale, i * SPRITE_SIZE * scale, 
                        Fade(WHITE, 0.3f));
            }
        }
        
        // Draw projectile info
        if (showInfo) {
            int y = SHEET_HEIGHT * scale + 10;
            DrawText("PROJECTILE TYPES:", 10, y, 16, WHITE);
            y += 20;
            
            for (int i = 0; i < PROJECTILE_TYPE_COUNT; i++) {
                const ProjectileDefinition* def = GetProjectileDefinition((ProjectileType)i);
                char info[256];
                snprintf(info, sizeof(info), "%d. %s - DMG:%d SPD:%.0f", 
                        i+1, def->name, def->damage, def->speed);
                
                Color textColor = def->primaryColor;
                DrawText(info, 10 + (i % 2) * 200, y + (i / 2) * 20, 14, textColor);
            }
        }
        
        DrawText("[G] Toggle Grid | [I] Toggle Info | [ESC] Exit", 
                10, GetScreenHeight() - 20, 12, LIGHTGRAY);
        
        EndDrawing();
    }
    
    // Cleanup
    UnloadImage(spriteSheetImg);
    UnloadRenderTexture(spriteSheet);
    CloseWindow();
    
    printf("\nProjectile sprite generation complete!\n");
    return 0;
}
