#include "raylib.h"
#include "types.h"
#include "constants.h"
#include "game.h"
#include "collision.h"
#include "renderer.h"
#include "menu.h"

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shoot'em Up - Prototype");
    SetTargetFPS(60);
    
    // Create and initialize menu
    Menu menu;
    InitMenu(&menu);
    MenuState gameState = MENU_MAIN;
    
    // Create game instance (but don't initialize until needed)
    Game game;
    bool gameInitialized = false;
    
    // Main loop
    while (!WindowShouldClose()) {
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
            }
            
            // Check if player wants to return to menu (ESC key when game is over)
            if (game.gameOver && IsKeyPressed(KEY_ESCAPE)) {
                CleanupGame(&game);
                gameInitialized = false;
                gameState = MENU_MAIN;
                menu.currentState = MENU_MAIN;
                menu.selectedOption = 0;
            }
            
            // Render game
            BeginDrawing();
            ClearBackground(BLACK);
            DrawGame(&game);
            
            // Draw "Press ESC to return to menu" if game is over
            if (game.gameOver) {
                const char* menuText = "Press ESC to return to menu";
                int textSize = 20;
                int textWidth = MeasureText(menuText, textSize);
                DrawText(menuText, (SCREEN_WIDTH - textWidth) / 2, 
                        SCREEN_HEIGHT - 100, textSize, WHITE);
            }
            
            EndDrawing();
        } else {
            // Update menu
            UpdateMenu(&menu, &gameState);
            
            // Render menu
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
    CloseWindow();
    
    return 0;
}
