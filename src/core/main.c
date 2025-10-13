#include "raylib.h"
#include "types.h"
#include "constants.h"
#include "game.h"
#include "collision.h"
#include "powerup.h"
#include "renderer.h"
#include "menu.h"
#include "database.h"
#include <math.h>
#include <stdio.h>

// Base resolution for game rendering
#define BASE_WIDTH 1200
#define BASE_HEIGHT 600

// Calculate scaled rendering parameters
typedef struct {
    float scale;
    Vector2 offset;
    Rectangle sourceRec;
    Rectangle destRec;
} RenderScale;

RenderScale CalculateRenderScale(int windowWidth, int windowHeight) {
    RenderScale rs;
    
    // Calculate scale to fit window while maintaining aspect ratio
    float scaleX = (float)windowWidth / BASE_WIDTH;
    float scaleY = (float)windowHeight / BASE_HEIGHT;
    rs.scale = fminf(scaleX, scaleY);
    
    // Calculate offset to center the scaled image
    float scaledWidth = BASE_WIDTH * rs.scale;
    float scaledHeight = BASE_HEIGHT * rs.scale;
    rs.offset.x = (windowWidth - scaledWidth) / 2.0f;
    rs.offset.y = (windowHeight - scaledHeight) / 2.0f;
    
    // Source rectangle (entire render texture)
    rs.sourceRec = (Rectangle){ 0, 0, BASE_WIDTH, -BASE_HEIGHT };
    
    // Destination rectangle (scaled and centered)
    rs.destRec = (Rectangle){ rs.offset.x, rs.offset.y, scaledWidth, scaledHeight };
    
    return rs;
}

int main(void) {
    // Initialize database
    if (!DB_Init()) {
        fprintf(stderr, "Warning: Failed to initialize database. Settings and high scores will not be saved.\n");
    }
    
    // Initialize window with default resolution
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shoot'em Up - Prototype");
    SetTargetFPS(60);
    
    // Disable ESC key from closing window by default
    SetExitKey(KEY_NULL);
    
    // Initialize audio device (only once at program start)
    InitAudioDevice();
    
    // Create render texture for game rendering at base resolution
    RenderTexture2D gameRenderTarget = LoadRenderTexture(BASE_WIDTH, BASE_HEIGHT);
    
    // Create and initialize menu
    Menu menu;
    InitMenu(&menu);
    MenuState gameState = MENU_MAIN;
    
    // Create game instance (but don't initialize until needed)
    Game game;
    bool gameInitialized = false;
    bool shouldQuit = false;
    
    // Main loop
    while (!WindowShouldClose() && !shouldQuit) {
        if (gameState == MENU_GAME) {
            // Initialize game if not already done
            if (!gameInitialized) {
                InitGame(&game);
                SetGameMusicVolume(&game, menu.musicVolume);
                gameInitialized = true;
            }
            
            // Update game logic
            UpdateGame(&game);
            
            // Check collisions
            if (!game.gameOver) {
                CheckCollisions(&game);
                // Check powerup collisions
                CheckPowerupCollisions(game.powerupSystem, game.playerShip, &game.score);
            }
            
            // Check if player wants to return to menu (ESC key when game is over)
            if (game.gameOver && IsKeyPressed(KEY_ESCAPE)) {
                // Save high score if it qualifies (using NORMAL difficulty for now)
                if (DB_IsHighScore(game.score, DIFFICULTY_NORMAL)) {
                    // For now, use a default player name
                    // TODO: In the future, implement a name input dialog
                    DB_AddHighScore("Player", game.score, DIFFICULTY_NORMAL);
                }
                
                CleanupGame(&game);
                gameInitialized = false;
                gameState = MENU_MAIN;
                menu.currentState = MENU_MAIN;
                menu.selectedOption = 0;
            }
            
            // Render game to texture at base resolution
            BeginTextureMode(gameRenderTarget);
            ClearBackground(BLACK);
            DrawGame(&game);
            
            // Draw "Press ESC to return to menu" if game is over
            if (game.gameOver) {
                const char* menuText = "Press ESC to return to menu";
                int textSize = 20;
                int textWidth = MeasureText(menuText, textSize);
                DrawText(menuText, (BASE_WIDTH - textWidth) / 2, 
                        BASE_HEIGHT - 100, textSize, WHITE);
            }
            EndTextureMode();
            
            // Draw scaled texture to actual window
            int windowWidth = GetScreenWidth();
            int windowHeight = GetScreenHeight();
            RenderScale scale = CalculateRenderScale(windowWidth, windowHeight);
            
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(gameRenderTarget.texture, scale.sourceRec, scale.destRec, 
                          (Vector2){0, 0}, 0.0f, WHITE);
            EndDrawing();
        } else {
            // Check if we should quit (ESC in main menu)
            if (menu.currentState == MENU_MAIN && IsKeyPressed(KEY_ESCAPE)) {
                shouldQuit = true;
            }
            
            // Update menu
            UpdateMenu(&menu, &gameState);
            
            // Render menu (menus adapt to window size dynamically)
            BeginDrawing();
            ClearBackground(BLACK);
            DrawMenu(&menu);
            EndDrawing();
        }
    }
    
    // Cleanup
    if (gameInitialized) {
        CleanupGame(&game);
    }
    UnloadRenderTexture(gameRenderTarget);
    DB_Cleanup();
    CloseWindow();
    
    return 0;
}
