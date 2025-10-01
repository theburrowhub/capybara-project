#include "raylib.h"
#include "types.h"
#include "constants.h"
#include "game.h"
#include "collision.h"
#include "renderer.h"

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shoot'em Up - Prototype");
    SetTargetFPS(60);
    
    // Create and initialize game
    Game game;
    InitGame(&game);
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update game logic
        UpdateGame(&game);
        
        // Check collisions
        if (!game.gameOver) {
            CheckCollisions(&game);
        }
        
        // Render everything
        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawGame(&game);
        EndDrawing();
    }
    
    // Cleanup
    CleanupGame(&game);
    CloseWindow();
    
    return 0;
}
