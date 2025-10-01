#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define SPRITE_SIZE 64
#define SHEET_COLS 4
#define SHEET_ROWS 2

// Generate multiple frames for animation
void DrawPlayerShipFrame(RenderTexture2D* target, int frame, int gridX, int gridY) {
    BeginTextureMode(*target);
    
    // Calculate position in sprite sheet
    int x = gridX * SPRITE_SIZE + SPRITE_SIZE / 2;
    int y = gridY * SPRITE_SIZE + SPRITE_SIZE / 2;
    
    // Animation parameters
    float enginePulse = (frame % 4) * 0.25f;
    float shieldPulse = sinf(frame * 0.5f) * 0.5f + 0.5f;
    
    // Colors
    Color primaryColor = (Color){0, 150, 255, 255};      // Cyan blue
    Color secondaryColor = (Color){255, 200, 0, 255};    // Gold
    Color tertiaryColor = (Color){180, 180, 220, 255};   // Light blue-gray
    Color engineColor = (Color){100, 200, 255, 255};     // Light blue
    Color engineCore = (Color){255, 255, 255, 255};      // White core
    
    // Draw engine exhaust/trail first (behind ship)
    if (frame < 4) {
        // Normal engine effect
        for (int i = 0; i < 3; i++) {
            float size = (8 - i * 2) * (1.0f + enginePulse * 0.3f);
            Color exhaustColor = Fade(engineColor, 0.6f - i * 0.15f);
            DrawCircle(x - 18 - i * 4, y - 6, size, exhaustColor);
            DrawCircle(x - 18 - i * 4, y + 6, size, exhaustColor);
        }
    } else {
        // Boost effect
        for (int i = 0; i < 5; i++) {
            float size = (10 - i * 1.5f) * (1.0f + enginePulse * 0.5f);
            Color boostColor = Fade((Color){255, 150, 0, 255}, 0.7f - i * 0.12f);
            DrawCircle(x - 20 - i * 5, y - 6, size, boostColor);
            DrawCircle(x - 20 - i * 5, y + 6, size, boostColor);
            DrawCircle(x - 20 - i * 5, y, size * 0.8f, Fade(boostColor, 0.5f));
        }
    }
    
    // Main hull - sleek fighter design
    // Central body
    DrawTriangle(
        (Vector2){x + 20, y},           // Nose
        (Vector2){x - 15, y - 8},       // Upper rear
        (Vector2){x - 15, y + 8},       // Lower rear
        primaryColor
    );
    
    // Upper wing
    DrawTriangle(
        (Vector2){x + 15, y - 5},
        (Vector2){x - 5, y - 12},
        (Vector2){x - 20, y - 18},
        primaryColor
    );
    DrawTriangle(
        (Vector2){x - 5, y - 12},
        (Vector2){x - 20, y - 18},
        (Vector2){x - 22, y - 12},
        Fade(primaryColor, 0.9f)
    );
    
    // Lower wing (mirrored)
    DrawTriangle(
        (Vector2){x + 15, y + 5},
        (Vector2){x - 5, y + 12},
        (Vector2){x - 20, y + 18},
        primaryColor
    );
    DrawTriangle(
        (Vector2){x - 5, y + 12},
        (Vector2){x - 20, y + 18},
        (Vector2){x - 22, y + 12},
        Fade(primaryColor, 0.9f)
    );
    
    // Wing tips with weapons
    DrawCircle(x - 18, y - 16, 3, secondaryColor);
    DrawCircle(x - 18, y + 16, 3, secondaryColor);
    DrawCircle(x - 18, y - 16, 2, Fade(WHITE, 0.8f));
    DrawCircle(x - 18, y + 16, 2, Fade(WHITE, 0.8f));
    
    // Engine nacelles
    DrawRectangleRounded((Rectangle){x - 22, y - 10, 12, 6}, 0.5f, 4, tertiaryColor);
    DrawRectangleRounded((Rectangle){x - 22, y + 4, 12, 6}, 0.5f, 4, tertiaryColor);
    
    // Engine cores
    float engineGlow = 0.7f + enginePulse * 0.3f;
    DrawCircle(x - 18, y - 7, 3 * engineGlow, engineColor);
    DrawCircle(x - 18, y + 7, 3 * engineGlow, engineColor);
    DrawCircle(x - 18, y - 7, 2, engineCore);
    DrawCircle(x - 18, y + 7, 2, engineCore);
    
    // Cockpit canopy
    DrawEllipse(x + 8, y, 8, 6, secondaryColor);
    DrawEllipse(x + 8, y, 6, 4, Fade(WHITE, 0.9f));
    DrawEllipse(x + 8, y, 4, 3, Fade((Color){200, 220, 255, 255}, 0.8f));
    
    // Hull details
    // Nose cone
    DrawTriangle(
        (Vector2){x + 20, y},
        (Vector2){x + 15, y - 3},
        (Vector2){x + 15, y + 3},
        Fade(tertiaryColor, 0.7f)
    );
    
    // Panel lines
    DrawLineEx((Vector2){x + 10, y - 6}, (Vector2){x - 5, y - 10}, 1, Fade(BLACK, 0.3f));
    DrawLineEx((Vector2){x + 10, y + 6}, (Vector2){x - 5, y + 10}, 1, Fade(BLACK, 0.3f));
    DrawLineEx((Vector2){x, y}, (Vector2){x - 10, y}, 1, Fade(BLACK, 0.2f));
    
    // Weapon ports on nose
    DrawCircle(x + 16, y - 4, 2, secondaryColor);
    DrawCircle(x + 16, y + 4, 2, secondaryColor);
    DrawCircle(x + 16, y - 4, 1, WHITE);
    DrawCircle(x + 16, y + 4, 1, WHITE);
    
    // Shield generator indicator
    if (frame >= 4) {
        // Draw shield effect for some frames
        DrawCircleLines(x, y, 28 + shieldPulse * 3, Fade((Color){0, 255, 200, 255}, 0.3f + shieldPulse * 0.2f));
        
        // Hexagonal shield pattern
        for (int i = 0; i < 6; i++) {
            float angle = (i * PI * 2.0f / 6.0f) + frame * 0.1f;
            float nextAngle = ((i + 1) * PI * 2.0f / 6.0f) + frame * 0.1f;
            Vector2 p1 = {x + cosf(angle) * 26, y + sinf(angle) * 26};
            Vector2 p2 = {x + cosf(nextAngle) * 26, y + sinf(nextAngle) * 26};
            DrawLineEx(p1, p2, 1, Fade((Color){0, 255, 200, 255}, 0.2f + shieldPulse * 0.1f));
        }
    }
    
    // Add some glow effects
    if (frame < 4) {
        // Normal glow
        DrawCircle(x + 8, y, 12, Fade(primaryColor, 0.1f));
    } else {
        // Boosted glow
        DrawCircle(x + 8, y, 15, Fade((Color){255, 200, 100, 255}, 0.15f));
    }
    
    EndTextureMode();
}

// Generate individual player ship sprite
void GenerateIndividualPlayerSprite(const char* filename) {
    InitWindow(100, 100, "Sprite Generator");
    SetTargetFPS(60);
    
    RenderTexture2D sprite = LoadRenderTexture(SPRITE_SIZE, SPRITE_SIZE);
    
    // Clear sprite
    BeginTextureMode(sprite);
    ClearBackground(BLANK);
    EndTextureMode();
    
    // Draw the player ship (frame 0)
    DrawPlayerShipFrame(&sprite, 0, 0, 0);
    
    // Save the sprite
    Image img = LoadImageFromTexture(sprite.texture);
    ImageFlipVertical(&img);
    ExportImage(img, filename);
    UnloadImage(img);
    
    UnloadRenderTexture(sprite);
    CloseWindow();
    
    printf("Generated individual player sprite: %s\n", filename);
}

int main(void) {
    InitWindow(100, 100, "Player Sprite Generator");
    SetTargetFPS(60);
    
    int sheetWidth = SPRITE_SIZE * SHEET_COLS;
    int sheetHeight = SPRITE_SIZE * SHEET_ROWS;
    
    RenderTexture2D spriteSheet = LoadRenderTexture(sheetWidth, sheetHeight);
    
    // Clear sprite sheet
    BeginTextureMode(spriteSheet);
    ClearBackground(BLANK);
    EndTextureMode();
    
    // Generate 8 frames of animation
    // Frames 0-3: Normal flight with engine pulse
    // Frames 4-7: Boosted/shielded variants
    for (int frame = 0; frame < 8; frame++) {
        int col = frame % SHEET_COLS;
        int row = frame / SHEET_COLS;
        DrawPlayerShipFrame(&spriteSheet, frame, col, row);
    }
    
    // Save the sprite sheet
    Image img = LoadImageFromTexture(spriteSheet.texture);
    ImageFlipVertical(&img);
    ExportImage(img, "assets/sprites/player_ship_spritesheet.png");
    UnloadImage(img);
    
    printf("Generated player ship sprite sheet: assets/sprites/player_ship_spritesheet.png\n");
    
    // Also generate individual sprite
    UnloadRenderTexture(spriteSheet);
    CloseWindow();
    
    // Generate individual sprite file
    GenerateIndividualPlayerSprite("assets/sprites/player_ship.png");
    
    return 0;
}
