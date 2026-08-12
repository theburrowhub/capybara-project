#include "raylib.h"
#include "types.h"
#include "constants.h"
#include "game.h"
#include "collision.h"
#include "powerup.h"
#include "renderer.h"
#include "menu.h"
#include "database.h"
#include "input_config.h"
#include "input_manager.h"
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
    
    // Initialize input configuration and manager
    InputConfig inputConfig;
    InputConfig_Load(&inputConfig, INPUT_CONFIG_FILE_DEFAULT);
    
    InputManager inputManager;
    InputManager_Init(&inputManager, &inputConfig);
    
    // Create and initialize menu
    Menu menu;
    InitMenu(&menu);
    menu.inputManager = &inputManager;  // Link input manager to menu
    MenuState gameState = MENU_MAIN;
    
    // Create game instance (but don't initialize until needed)
    Game game;
    bool gameInitialized = false;
    bool shouldQuit = false;
    bool awaitingNameInput = false;
    
    // Main loop
    while (!WindowShouldClose() && !shouldQuit) {
        // Update input manager each frame
        InputManager_Update(&inputManager);
        
        if ((gameState == MENU_GAME || gameState == MENU_PAUSE_CONFIRM) && !awaitingNameInput) {
            // Initialize game if not already done
            if (!gameInitialized) {
                InitGame(&game);
                game.inputManager = &inputManager;  // Link input manager to game
                SetGameMusicVolume(&game, menu.musicVolume);
                gameInitialized = true;
                game.justStarted = true;  // Ensure flag is set (also set in InitGame)
            }
            
            // Check if player wants to pause/exit (ESC key or START button) - check BEFORE updating
            bool pauseExitPressed = IsKeyPressed(KEY_ESCAPE) || 
                                   (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT));
            
            if (pauseExitPressed) {
                if (game.gameOver) {
                    // Game over - check if score qualifies as high score
                    if (DB_IsHighScore(game.score, DIFFICULTY_NORMAL)) {
                        // Show name input dialog
                        StartNameInput(&menu, game.score, DIFFICULTY_NORMAL);
                        awaitingNameInput = true;
                        gameState = MENU_NAME_INPUT;
                    } else {
                        // No high score, go directly to menu
                        CleanupGame(&game);
                        gameInitialized = false;
                        gameState = MENU_MAIN;
                        menu.currentState = MENU_MAIN;
                        menu.selectedOption = 0;
                        menu.justReturnedToMainMenu = true;  // Prevent immediate exit from same ESC
                    }
                } else {
                    // During gameplay - show pause confirmation menu
                    menu.currentState = MENU_PAUSE_CONFIRM;
                    menu.pauseMenuOption = 0;  // Default to Resume
                    menu.pauseMenuCooldown = 0.2f;  // 200ms cooldown to prevent immediate close
                    gameState = MENU_PAUSE_CONFIRM;
                    // Pause the game
                    game.gamePaused = true;
                    if (game.musicLoaded) {
                        PauseMusicStream(game.backgroundMusic);
                    }
                }
            }
            
            // Check if returning from pause menu
            if (menu.currentState == MENU_GAME && gameState == MENU_GAME && game.gamePaused) {
                // User selected "Resume" from pause dialog
                game.gamePaused = false;
                game.justStarted = true;  // Prevent immediate input processing after resume
                menu.pauseMenuCooldown = 0.2f;  // Also set cooldown in case pause is triggered again
                if (game.musicLoaded) {
                    ResumeMusicStream(game.backgroundMusic);
                }
            }
            
            // Update and render game
            if (gameInitialized) {
                // Only update game if not showing pause menu
                if (menu.currentState != MENU_PAUSE_CONFIRM) {
                    // Update game logic
                    UpdateGame(&game);
                    
                    // Check collisions
                    if (!game.gameOver) {
                        CheckCollisions(&game);
                        // Check powerup collisions
                        CheckPowerupCollisions(game.powerupSystem, game.playerShip, &game.score);
                    }
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
                
                // Draw pause menu overlay if active
                if (menu.currentState == MENU_PAUSE_CONFIRM) {
                    // Update menu input for pause dialog
                    UpdateMenu(&menu, &gameState);
                    DrawPauseConfirm(&menu);
                }
                
                // Draw name input overlay if active
                if (awaitingNameInput) {
                    DrawNameInput(&menu);
                }
                
                EndDrawing();
            }
            
            // Handle name input completion
            if (awaitingNameInput && !IsNameInputActive(&menu)) {
                // Name input finished, cleanup and return to menu
                CleanupGame(&game);
                gameInitialized = false;
                awaitingNameInput = false;
                gameState = MENU_MAIN;
                menu.currentState = MENU_MAIN;
                menu.justReturnedToMainMenu = true;  // Prevent immediate exit
            }
        } else {
            // Cleanup game if we just returned to menu from gameplay
            if (gameInitialized && menu.currentState == MENU_MAIN) {
                CleanupGame(&game);
                gameInitialized = false;
            }
            
            // Clear the "just returned" flag after one full frame in main menu
            // This prevents ESC from exiting immediately when returning from game
            static bool wasInMainMenu = false;
            if (menu.currentState == MENU_MAIN) {
                if (!wasInMainMenu) {
                    // First frame in main menu - keep the flag
                    wasInMainMenu = true;
                } else {
                    // Subsequent frames - safe to clear flag
                    menu.justReturnedToMainMenu = false;
                }
            } else {
                wasInMainMenu = false;
            }
            
            // Update menu
            UpdateMenu(&menu, &gameState);
            
            // Check if user selected EXIT from main menu
            if (gameState == (MenuState)-1) {
                shouldQuit = true;
            }
            
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
