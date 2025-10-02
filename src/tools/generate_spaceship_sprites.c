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

// Helper function to draw spaceship-shaped enemy sprite at specific position
void DrawSpaceshipSprite(RenderTexture2D* target, EnemyType type, int gridX, int gridY) {
    BeginTextureMode(*target);
    
    const EnemyTypeDefinition* def = GetEnemyTypeDefinition(type);
    
    // Calculate center position in the grid cell
    float centerX = gridX * SPRITE_SIZE + SPRITE_SIZE / 2.0f;
    float centerY = gridY * SPRITE_SIZE + SPRITE_SIZE / 2.0f;
    
    // Base size scaled to fit in sprite
    float scale = def->size;
    
    // Draw based on enemy type - each one is a different spaceship design
    switch (type) {
        case ENEMY_GRUNT:
            // Small fighter ship - arrow design
            {
                // Main body
                Vector2 body[] = {
                    {centerX, centerY - 15 * scale},      // Nose
                    {centerX - 8 * scale, centerY + 5 * scale},   // Left wing base
                    {centerX - 4 * scale, centerY + 10 * scale},  // Left tail
                    {centerX, centerY + 8 * scale},       // Center tail
                    {centerX + 4 * scale, centerY + 10 * scale},  // Right tail
                    {centerX + 8 * scale, centerY + 5 * scale}    // Right wing base
                };
                DrawTriangle(body[0], body[1], body[5], def->primaryColor);
                DrawTriangle(body[1], body[2], body[3], def->primaryColor);
                DrawTriangle(body[3], body[4], body[5], def->primaryColor);
                DrawTriangle(body[1], body[3], body[5], def->primaryColor);
                
                // Cockpit
                DrawCircle(centerX, centerY - 5 * scale, 3 * scale, def->secondaryColor);
                DrawCircle(centerX, centerY - 5 * scale, 2 * scale, Fade(SKYBLUE, 0.8f));
                
                // Wing tips
                DrawTriangle(
                    (Vector2){centerX - 8 * scale, centerY + 5 * scale},
                    (Vector2){centerX - 12 * scale, centerY + 8 * scale},
                    (Vector2){centerX - 6 * scale, centerY + 10 * scale},
                    def->secondaryColor
                );
                DrawTriangle(
                    (Vector2){centerX + 8 * scale, centerY + 5 * scale},
                    (Vector2){centerX + 12 * scale, centerY + 8 * scale},
                    (Vector2){centerX + 6 * scale, centerY + 10 * scale},
                    def->secondaryColor
                );
            }
            break;
            
        case ENEMY_TANK:
            // Heavy cruiser - blocky design
            {
                // Main hull
                DrawRectangle(centerX - 12 * scale, centerY - 8 * scale, 
                             24 * scale, 20 * scale, def->primaryColor);
                DrawRectangle(centerX - 10 * scale, centerY - 6 * scale, 
                             20 * scale, 16 * scale, def->secondaryColor);
                
                // Side pods/engines
                DrawRectangle(centerX - 18 * scale, centerY - 4 * scale,
                             6 * scale, 12 * scale, def->primaryColor);
                DrawRectangle(centerX + 12 * scale, centerY - 4 * scale,
                             6 * scale, 12 * scale, def->primaryColor);
                
                // Front armor
                DrawTriangle(
                    (Vector2){centerX - 12 * scale, centerY - 8 * scale},
                    (Vector2){centerX, centerY - 15 * scale},
                    (Vector2){centerX + 12 * scale, centerY - 8 * scale},
                    DARKGRAY
                );
                
                // Bridge/cockpit
                DrawRectangle(centerX - 4 * scale, centerY - 10 * scale,
                             8 * scale, 4 * scale, Fade(SKYBLUE, 0.6f));
                
                // Engine exhausts
                DrawCircle(centerX - 15 * scale, centerY + 10 * scale, 3 * scale, ORANGE);
                DrawCircle(centerX, centerY + 12 * scale, 3 * scale, ORANGE);
                DrawCircle(centerX + 15 * scale, centerY + 10 * scale, 3 * scale, ORANGE);
            }
            break;
            
        case ENEMY_SPEEDER:
            // Fast interceptor - sleek design
            {
                // Main fuselage - thin and long
                Vector2 hull[] = {
                    {centerX, centerY - 20 * scale},      // Sharp nose
                    {centerX - 3 * scale, centerY - 10 * scale},
                    {centerX - 4 * scale, centerY + 15 * scale},  // Tail left
                    {centerX, centerY + 18 * scale},      // Tail center
                    {centerX + 4 * scale, centerY + 15 * scale},  // Tail right
                    {centerX + 3 * scale, centerY - 10 * scale}
                };
                
                // Draw hull
                DrawTriangle(hull[0], hull[1], hull[5], def->primaryColor);
                DrawTriangle(hull[1], hull[2], hull[3], def->primaryColor);
                DrawTriangle(hull[3], hull[4], hull[5], def->primaryColor);
                DrawTriangle(hull[1], hull[3], hull[5], def->primaryColor);
                
                // Swept wings
                DrawTriangle(
                    (Vector2){centerX - 3 * scale, centerY},
                    (Vector2){centerX - 15 * scale, centerY + 10 * scale},
                    (Vector2){centerX - 3 * scale, centerY + 8 * scale},
                    def->secondaryColor
                );
                DrawTriangle(
                    (Vector2){centerX + 3 * scale, centerY},
                    (Vector2){centerX + 15 * scale, centerY + 10 * scale},
                    (Vector2){centerX + 3 * scale, centerY + 8 * scale},
                    def->secondaryColor
                );
                
                // Speed stripes
                DrawRectangle(centerX - 1 * scale, centerY - 18 * scale,
                             2 * scale, 30 * scale, Fade(YELLOW, 0.6f));
                
                // Cockpit
                DrawTriangle(
                    (Vector2){centerX, centerY - 12 * scale},
                    (Vector2){centerX - 2 * scale, centerY - 8 * scale},
                    (Vector2){centerX + 2 * scale, centerY - 8 * scale},
                    Fade(SKYBLUE, 0.8f)
                );
                
                // Thruster
                DrawCircle(centerX, centerY + 15 * scale, 4 * scale, ORANGE);
                DrawCircle(centerX, centerY + 15 * scale, 3 * scale, YELLOW);
            }
            break;
            
        case ENEMY_ZIGZAG:
            // Agile fighter with angled wings
            {
                // Main body - diamond shaped
                Vector2 body[] = {
                    {centerX, centerY - 12 * scale},
                    {centerX - 6 * scale, centerY},
                    {centerX, centerY + 12 * scale},
                    {centerX + 6 * scale, centerY}
                };
                DrawTriangle(body[0], body[1], body[3], def->primaryColor);
                DrawTriangle(body[1], body[2], body[3], def->primaryColor);
                
                // Zigzag wings
                DrawTriangle(
                    (Vector2){centerX - 6 * scale, centerY},
                    (Vector2){centerX - 16 * scale, centerY - 5 * scale},
                    (Vector2){centerX - 14 * scale, centerY + 8 * scale},
                    def->secondaryColor
                );
                DrawTriangle(
                    (Vector2){centerX + 6 * scale, centerY},
                    (Vector2){centerX + 16 * scale, centerY - 5 * scale},
                    (Vector2){centerX + 14 * scale, centerY + 8 * scale},
                    def->secondaryColor
                );
                
                // Wing details
                DrawLine(centerX - 10 * scale, centerY + 2 * scale,
                        centerX - 14 * scale, centerY - 2 * scale, def->glowColor);
                DrawLine(centerX + 10 * scale, centerY + 2 * scale,
                        centerX + 14 * scale, centerY - 2 * scale, def->glowColor);
                
                // Cockpit
                DrawCircle(centerX, centerY - 4 * scale, 3 * scale, Fade(SKYBLUE, 0.7f));
                
                // Engines
                DrawCircle(centerX - 3 * scale, centerY + 10 * scale, 2 * scale, ORANGE);
                DrawCircle(centerX + 3 * scale, centerY + 10 * scale, 2 * scale, ORANGE);
            }
            break;
            
        case ENEMY_BOMBER:
            // Heavy bomber - wide body
            {
                // Main fuselage
                DrawEllipse(centerX, centerY, 15 * scale, 10 * scale, def->primaryColor);
                DrawEllipse(centerX, centerY - 2 * scale, 12 * scale, 8 * scale, def->secondaryColor);
                
                // Wing sections
                DrawRectangle(centerX - 20 * scale, centerY - 3 * scale,
                             8 * scale, 10 * scale, def->primaryColor);
                DrawRectangle(centerX + 12 * scale, centerY - 3 * scale,
                             8 * scale, 10 * scale, def->primaryColor);
                
                // Bomb bay
                DrawRectangle(centerX - 6 * scale, centerY + 2 * scale,
                             12 * scale, 6 * scale, BLACK);
                
                // Bombs visible
                for (int i = 0; i < 3; i++) {
                    DrawCircle(centerX - 4 * scale + i * 4 * scale, centerY + 5 * scale, 
                              1.5f * scale, DARKGRAY);
                }
                
                // Cockpit windows
                DrawRectangle(centerX - 6 * scale, centerY - 8 * scale,
                             3 * scale, 3 * scale, Fade(SKYBLUE, 0.6f));
                DrawRectangle(centerX - 1.5f * scale, centerY - 8 * scale,
                             3 * scale, 3 * scale, Fade(SKYBLUE, 0.6f));
                DrawRectangle(centerX + 3 * scale, centerY - 8 * scale,
                             3 * scale, 3 * scale, Fade(SKYBLUE, 0.6f));
                
                // Engine nacelles
                DrawCircle(centerX - 16 * scale, centerY + 8 * scale, 3 * scale, DARKGRAY);
                DrawCircle(centerX + 16 * scale, centerY + 8 * scale, 3 * scale, DARKGRAY);
                DrawCircle(centerX - 16 * scale, centerY + 8 * scale, 2 * scale, ORANGE);
                DrawCircle(centerX + 16 * scale, centerY + 8 * scale, 2 * scale, ORANGE);
                
                // Danger markings
                DrawRing((Vector2){centerX, centerY}, 18 * scale, 20 * scale, 0, 360, 32, 
                        Fade(RED, 0.4f));
            }
            break;
            
        case ENEMY_SHIELD:
            // Defensive craft with shield generators
            {
                // Hexagonal main body
                Vector2 hex[6];
                for (int i = 0; i < 6; i++) {
                    float angle = (60 * i) * DEG2RAD;
                    hex[i] = (Vector2){
                        centerX + cosf(angle) * 10 * scale,
                        centerY + sinf(angle) * 10 * scale
                    };
                }
                
                // Draw hexagon body
                for (int i = 0; i < 6; i++) {
                    DrawTriangle((Vector2){centerX, centerY}, hex[i], hex[(i+1)%6], def->primaryColor);
                }
                
                // Inner hexagon
                for (int i = 0; i < 6; i++) {
                    float angle = (60 * i) * DEG2RAD;
                    Vector2 inner = {
                        centerX + cosf(angle) * 7 * scale,
                        centerY + sinf(angle) * 7 * scale
                    };
                    DrawTriangle((Vector2){centerX, centerY}, inner, 
                               (Vector2){centerX + cosf((60*(i+1))*DEG2RAD) * 7 * scale,
                                        centerY + sinf((60*(i+1))*DEG2RAD) * 7 * scale}, 
                               def->secondaryColor);
                }
                
                // Shield generator nodes
                for (int i = 0; i < 6; i++) {
                    DrawCircle(hex[i].x, hex[i].y, 3 * scale, def->glowColor);
                    DrawCircle(hex[i].x, hex[i].y, 2 * scale, Fade(SKYBLUE, 0.8f));
                    DrawLine(centerX, centerY, hex[i].x, hex[i].y, Fade(def->glowColor, 0.5f));
                }
                
                // Central core
                DrawCircle(centerX, centerY, 4 * scale, DARKBLUE);
                DrawCircle(centerX, centerY, 3 * scale, Fade(SKYBLUE, 0.9f));
            }
            break;
            
        case ENEMY_SWARM:
            // Small drone ship
            {
                // Compact triangular design
                Vector2 drone[] = {
                    {centerX, centerY - 8 * scale},       // Front
                    {centerX - 6 * scale, centerY + 6 * scale},  // Left
                    {centerX + 6 * scale, centerY + 6 * scale}   // Right
                };
                DrawTriangle(drone[0], drone[1], drone[2], def->primaryColor);
                
                // Inner detail
                Vector2 inner[] = {
                    {centerX, centerY - 5 * scale},
                    {centerX - 4 * scale, centerY + 4 * scale},
                    {centerX + 4 * scale, centerY + 4 * scale}
                };
                DrawTriangle(inner[0], inner[1], inner[2], def->secondaryColor);
                
                // Small cockpit
                DrawCircle(centerX, centerY - 2 * scale, 2 * scale, Fade(RED, 0.8f));
                
                // Mini thrusters
                DrawCircle(centerX - 2 * scale, centerY + 5 * scale, 1.5f * scale, ORANGE);
                DrawCircle(centerX + 2 * scale, centerY + 5 * scale, 1.5f * scale, ORANGE);
                
                // Antenna
                DrawLine(centerX, centerY - 8 * scale, centerX, centerY - 12 * scale, GRAY);
                DrawCircle(centerX, centerY - 12 * scale, 1 * scale, RED);
            }
            break;
            
        case ENEMY_ELITE:
            // Advanced fighter with star configuration
            {
                // Star-shaped hull with 5 points
                Vector2 outer[5], inner[5];
                for (int i = 0; i < 5; i++) {
                    float angleOuter = (72 * i - 90) * DEG2RAD;
                    float angleInner = (72 * i - 90 + 36) * DEG2RAD;
                    outer[i] = (Vector2){
                        centerX + cosf(angleOuter) * 15 * scale,
                        centerY + sinf(angleOuter) * 15 * scale
                    };
                    inner[i] = (Vector2){
                        centerX + cosf(angleInner) * 8 * scale,
                        centerY + sinf(angleInner) * 8 * scale
                    };
                }
                
                // Draw star shape
                for (int i = 0; i < 5; i++) {
                    DrawTriangle((Vector2){centerX, centerY}, outer[i], inner[i], def->primaryColor);
                    DrawTriangle((Vector2){centerX, centerY}, inner[i], outer[(i+1)%5], def->primaryColor);
                }
                
                // Inner star
                for (int i = 0; i < 5; i++) {
                    float angle = (72 * i - 90) * DEG2RAD;
                    Vector2 p = {
                        centerX + cosf(angle) * 10 * scale,
                        centerY + sinf(angle) * 10 * scale
                    };
                    DrawTriangle((Vector2){centerX, centerY}, p,
                               (Vector2){centerX + cosf((72*(i+1)-90)*DEG2RAD) * 10 * scale,
                                        centerY + sinf((72*(i+1)-90)*DEG2RAD) * 10 * scale},
                               def->secondaryColor);
                }
                
                // Central cockpit
                DrawPoly((Vector2){centerX, centerY}, 6, 5 * scale, 0, DARKGRAY);
                DrawPoly((Vector2){centerX, centerY}, 6, 4 * scale, 0, Fade(GOLD, 0.8f));
                
                // Weapon pods on each point
                for (int i = 0; i < 5; i++) {
                    DrawCircle(outer[i].x, outer[i].y, 2 * scale, DARKGRAY);
                    DrawCircle(outer[i].x, outer[i].y, 1 * scale, RED);
                }
            }
            break;
            
        case ENEMY_GHOST:
            // Stealth ship - angular design
            {
                // Stealth fighter shape - angular
                Vector2 stealth[] = {
                    {centerX, centerY - 15 * scale},      // Nose
                    {centerX - 12 * scale, centerY + 5 * scale},  // Left wing
                    {centerX - 8 * scale, centerY + 12 * scale},  // Left tail outer
                    {centerX - 3 * scale, centerY + 10 * scale},  // Left tail inner
                    {centerX, centerY + 8 * scale},       // Center tail
                    {centerX + 3 * scale, centerY + 10 * scale},  // Right tail inner
                    {centerX + 8 * scale, centerY + 12 * scale},  // Right tail outer
                    {centerX + 12 * scale, centerY + 5 * scale}   // Right wing
                };
                
                // Semi-transparent body
                Color ghostColor = def->primaryColor;
                ghostColor.a = 180;
                
                // Draw main body
                DrawTriangle(stealth[0], stealth[1], stealth[7], ghostColor);
                DrawTriangle(stealth[1], stealth[2], stealth[3], ghostColor);
                DrawTriangle(stealth[3], stealth[4], stealth[5], ghostColor);
                DrawTriangle(stealth[5], stealth[6], stealth[7], ghostColor);
                DrawTriangle(stealth[1], stealth[3], stealth[7], ghostColor);
                DrawTriangle(stealth[3], stealth[5], stealth[7], ghostColor);
                
                // Phase effect - outline
                for (int i = 0; i < 7; i++) {
                    DrawLineEx(stealth[i], stealth[i+1], 1, Fade(def->glowColor, 0.5f));
                }
                DrawLineEx(stealth[7], stealth[0], 1, Fade(def->glowColor, 0.5f));
                
                // Cockpit (barely visible)
                DrawTriangle(
                    (Vector2){centerX, centerY - 10 * scale},
                    (Vector2){centerX - 3 * scale, centerY - 5 * scale},
                    (Vector2){centerX + 3 * scale, centerY - 5 * scale},
                    Fade(DARKBLUE, 0.4f)
                );
                
                // Cloaking field effect
                DrawCircleLines(centerX, centerY, 18 * scale, Fade(def->glowColor, 0.3f));
            }
            break;
            
        case ENEMY_BOSS:
            // Massive battleship
            {
                // Main hull - large and imposing
                Vector2 hull[] = {
                    {centerX, centerY - 20 * scale},      // Front point
                    {centerX - 15 * scale, centerY - 10 * scale},
                    {centerX - 18 * scale, centerY},
                    {centerX - 15 * scale, centerY + 10 * scale},
                    {centerX - 10 * scale, centerY + 15 * scale},
                    {centerX, centerY + 12 * scale},
                    {centerX + 10 * scale, centerY + 15 * scale},
                    {centerX + 15 * scale, centerY + 10 * scale},
                    {centerX + 18 * scale, centerY},
                    {centerX + 15 * scale, centerY - 10 * scale}
                };
                
                // Draw main hull
                for (int i = 0; i < 9; i++) {
                    DrawTriangle((Vector2){centerX, centerY}, hull[i], hull[i+1], def->primaryColor);
                }
                DrawTriangle((Vector2){centerX, centerY}, hull[9], hull[0], def->primaryColor);
                
                // Secondary hull layer
                for (int i = 0; i < 10; i++) {
                    Vector2 inner = {
                        centerX + (hull[i].x - centerX) * 0.7f,
                        centerY + (hull[i].y - centerY) * 0.7f
                    };
                    Vector2 innerNext = {
                        centerX + (hull[(i+1)%10].x - centerX) * 0.7f,
                        centerY + (hull[(i+1)%10].y - centerY) * 0.7f
                    };
                    DrawTriangle((Vector2){centerX, centerY}, inner, innerNext, def->secondaryColor);
                }
                
                // Weapon turrets
                DrawCircle(centerX - 10 * scale, centerY - 5 * scale, 3 * scale, DARKGRAY);
                DrawCircle(centerX + 10 * scale, centerY - 5 * scale, 3 * scale, DARKGRAY);
                DrawCircle(centerX - 10 * scale, centerY + 5 * scale, 3 * scale, DARKGRAY);
                DrawCircle(centerX + 10 * scale, centerY + 5 * scale, 3 * scale, DARKGRAY);
                
                // Turret barrels
                DrawRectangle(centerX - 12 * scale, centerY - 6 * scale, 4 * scale, 2 * scale, BLACK);
                DrawRectangle(centerX + 8 * scale, centerY - 6 * scale, 4 * scale, 2 * scale, BLACK);
                DrawRectangle(centerX - 12 * scale, centerY + 4 * scale, 4 * scale, 2 * scale, BLACK);
                DrawRectangle(centerX + 8 * scale, centerY + 4 * scale, 4 * scale, 2 * scale, BLACK);
                
                // Command bridge
                DrawRectangle(centerX - 4 * scale, centerY - 8 * scale,
                             8 * scale, 6 * scale, DARKGRAY);
                DrawRectangle(centerX - 3 * scale, centerY - 7 * scale,
                             6 * scale, 4 * scale, Fade(RED, 0.6f));
                
                // Engine array
                for (int i = -2; i <= 2; i++) {
                    DrawCircle(centerX + i * 4 * scale, centerY + 14 * scale, 2 * scale, DARKGRAY);
                    DrawCircle(centerX + i * 4 * scale, centerY + 14 * scale, 1.5f * scale, ORANGE);
                }
                
                // Spikes/antennas
                DrawLine(centerX - 18 * scale, centerY, centerX - 22 * scale, centerY, RED);
                DrawLine(centerX + 18 * scale, centerY, centerX + 22 * scale, centerY, RED);
                DrawCircle(centerX - 22 * scale, centerY, 1 * scale, RED);
                DrawCircle(centerX + 22 * scale, centerY, 1 * scale, RED);
            }
            break;
            
        default:
            // Fallback basic ship
            DrawTriangle(
                (Vector2){centerX, centerY - 10 * scale},
                (Vector2){centerX - 8 * scale, centerY + 10 * scale},
                (Vector2){centerX + 8 * scale, centerY + 10 * scale},
                def->primaryColor
            );
            break;
    }
    
    EndTextureMode();
}

int main(void) {
    // Initialize
    InitWindow(SHEET_WIDTH, SHEET_HEIGHT, "Spaceship Enemy Sprite Generator");
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
    printf("Generating spaceship enemy sprites...\n");
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        int gridX = i % SPRITES_PER_ROW;
        int gridY = i / SPRITES_PER_ROW;
        
        DrawSpaceshipSprite(&spriteSheet, (EnemyType)i, gridX, gridY);
        
        const char* name = GetEnemyTypeName((EnemyType)i);
        printf("  - Generated spaceship sprite for %s at position (%d, %d)\n", name, gridX, gridY);
    }
    
    // Also generate individual sprite files
    printf("\nGenerating individual spaceship sprite files...\n");
    
    // Create assets directory if it doesn't exist
    system("mkdir -p assets/sprites/spaceships");
    
    for (int i = 0; i < ENEMY_TYPE_COUNT; i++) {
        // Create individual render texture
        RenderTexture2D individualSprite = LoadRenderTexture(SPRITE_SIZE, SPRITE_SIZE);
        
        // Clear with transparent background
        BeginTextureMode(individualSprite);
        ClearBackground(BLANK);
        EndTextureMode();
        
        // Draw the spaceship sprite
        DrawSpaceshipSprite(&individualSprite, (EnemyType)i, 0, 0);
        
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
        snprintf(filename, sizeof(filename), "assets/sprites/spaceships/spaceship_%s.png", lowername);
        
        ExportImage(img, filename);
        printf("  - Saved %s\n", filename);
        
        UnloadImage(img);
        UnloadRenderTexture(individualSprite);
    }
    
    // Export the complete sprite sheet
    Image spriteSheetImg = LoadImageFromTexture(spriteSheet.texture);
    ImageFlipVertical(&spriteSheetImg);  // Flip because render textures are upside down
    ExportImage(spriteSheetImg, "assets/sprites/spaceship_spritesheet.png");
    printf("\nSaved complete spaceship sprite sheet to assets/sprites/spaceship_spritesheet.png\n");
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
        
        DrawText("SPACESHIP SPRITES | Press [G] to toggle grid | Press [ESC] to exit", 
                10, SHEET_HEIGHT - 20, 12, WHITE);
        
        EndDrawing();
    }
    
    // Cleanup
    UnloadImage(spriteSheetImg);
    UnloadRenderTexture(spriteSheet);
    CloseWindow();
    
    printf("\nSpaceship sprite generation complete!\n");
    return 0;
}
