#include "renderer.h"
#include "constants.h"
#include "player.h"
#include "enemy.h"
#include "obstacle.h"
#include "weapon.h"
#include <stdio.h>

void DrawBackground(float backgroundX) {
    for (int i = 0; i < 3; i++) {
        float x = backgroundX + (i * SCREEN_WIDTH);
        // Stars
        for (int j = 0; j < 20; j++) {
            int starX = (int)(x + (j * 60) + ((j * 17) % 40));
            int starY = (j * 47) % SCREEN_HEIGHT;
            if (starX >= 0 && starX <= SCREEN_WIDTH) {
                DrawCircle(starX, starY, 2, WHITE);
            }
        }
    }
}

void DrawUI(const Game* game) {
    // Score and health
    DrawText(TextFormat("Score: %d", game->score), 10, 10, 30, WHITE);
    DrawText(TextFormat("Health: %d", game->player.health), 10, 50, 30, WHITE);
    
    // Weapon heat bar
    DrawWeaponHeatBar(&game->player);
    
    // Speed level and time
    DrawText(TextFormat("Speed Level: %d", game->speedLevel), 10, 115, 25, YELLOW);
    int minutes = (int)(game->gameTime / 60);
    int seconds = (int)game->gameTime % 60;
    DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 10, 145, 25, WHITE);
    
    // Show speed increase warning
    int timeUntilNextSpeed = SPEED_UPDATE_INTERVAL - ((int)game->gameTime % SPEED_UPDATE_INTERVAL);
    if (timeUntilNextSpeed <= 5 && timeUntilNextSpeed > 0) {
        DrawText(TextFormat("SPEED INCREASE IN %d!", timeUntilNextSpeed), 
                SCREEN_WIDTH/2 - 150, 50, 30, RED);
    }
    
    // Draw controls
    DrawText("Controls: WASD/Arrows - Move | SPACE/Click - Shoot | H - Show Hitboxes", 
            10, SCREEN_HEIGHT - 30, 20, WHITE);
}

void DrawDebugInfo(const Game* game) {
    if (!game->gameOver) {
        DrawText(TextFormat("Enemies: %d | Obstacles: %d", 
                CountActiveEnemies(game), CountActiveObstacles(game)), 
                SCREEN_WIDTH - 250, 10, 20, GREEN);
    }
}

void DrawGameOver(const Game* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    DrawText("GAME OVER", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 50, RED);
    
    // Show death cause with better formatting
    int textWidth = MeasureText(game->deathCause, 20);
    DrawText("Cause of Death:", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 30, 22, ORANGE);
    DrawText(game->deathCause, SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2, 20, YELLOW);
    
    DrawText(TextFormat("Final Score: %d", game->score), SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 40, 30, WHITE);
    DrawText("Press R to Restart", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 80, 25, WHITE);
    DrawText("Check collision_log.txt for details", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 110, 18, GRAY);
}

void DrawGame(Game* game) {
    bool showHitbox = IsKeyDown(KEY_H);
    
    // Draw background
    DrawBackground(game->backgroundX);
    
    // Draw game objects
    DrawPlayer(&game->player, showHitbox);
    DrawBullets(game->bullets);
    DrawEnemies(game->enemies, showHitbox);
    DrawObstacles(game->obstacles, showHitbox);
    
    // Draw UI
    DrawUI(game);
    DrawDebugInfo(game);
    
    // Draw game over screen if needed
    if (game->gameOver) {
        DrawGameOver(game);
    }
}
