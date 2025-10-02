#include "renderer.h"
#include "constants.h"
#include "player_ship.h"
#include "game.h"
#include "projectile_types.h"
#include "wave_system.h"
#include "weapon.h"
#include "explosion.h"
#include <stdio.h>

void DrawStarfield(const Game* game) {
    // Draw parallax starfield
    for (int i = 0; i < game->numStars; i++) {
        Color starColor = (Color){
            game->stars[i].brightness,
            game->stars[i].brightness,
            game->stars[i].brightness,
            255
        };
        
        // Different star sizes based on depth
        int size = (game->stars[i].speed < 1.5f) ? 1 : 2;
        
        DrawCircle(
            (int)game->stars[i].position.x,
            (int)game->stars[i].position.y,
            size,
            starColor
        );
        
        // Add glow to brighter stars
        if (game->stars[i].brightness > 200) {
            DrawCircle(
                (int)game->stars[i].position.x,
                (int)game->stars[i].position.y,
                size + 1,
                Fade(starColor, 0.3f)
            );
        }
    }
}

void DrawBackground(float backgroundX) {
    // Legacy background - now just used for additional effects
    // The main starfield is drawn separately
}

void DrawUI(const Game* game) {
    // Score and health
    DrawText(TextFormat("Score: %d", game->score), 10, 10, 30, WHITE);
    DrawText(TextFormat("Health: %d", game->playerShip->health), 10, 50, 30, WHITE);
    
    // Show debug mode indicators
    if (DEBUG_INVULNERABILITY) {
        DrawText("[INVULNERABLE]", 200, 50, 30, LIME);
    }
    
    // Weapon heat bar
    DrawWeaponHeatBar(game->playerShip);
    
    // Wave phase information
    if (game->waveSystem) {
        const char* phaseName = GetCurrentPhaseName(game->waveSystem);
        float progress = GetWaveProgress(game->waveSystem);
        DrawText(TextFormat("Phase: %s", phaseName), 10, 115, 25, YELLOW);
        DrawText(TextFormat("Progress: %.1f%%", progress), 10, 145, 25, WHITE);
    }
    
    // Time
    int minutes = (int)(game->gameTime / 60);
    int seconds = (int)game->gameTime % 60;
    DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 10, 175, 25, WHITE);
    
    // Wave progress bar
    if (game->waveSystem) {
        float progress = GetWaveProgress(game->waveSystem);
        DrawRectangle(SCREEN_WIDTH/2 - 200, 20, 400, 20, Fade(BLACK, 0.5f));
        DrawRectangle(SCREEN_WIDTH/2 - 200, 20, (int)(400 * progress / 100), 20, Fade(GREEN, 0.7f));
        DrawRectangleLines(SCREEN_WIDTH/2 - 200, 20, 400, 20, WHITE);
        DrawText("WAVE PROGRESS", SCREEN_WIDTH/2 - 60, 22, 16, WHITE);
    }
    
    // Draw controls
    DrawText("Controls: WASD/Arrows - Move | P - Pause | H - Show Hitboxes", 
            10, SCREEN_HEIGHT - 30, 20, WHITE);
    
    // Show debug phase start if applicable
    if (DEBUG_START_PHASE > 0) {
        DrawText(TextFormat("[DEBUG: Started at Phase %d]", DEBUG_START_PHASE), 
                10, SCREEN_HEIGHT - 55, 20, ORANGE);
    }
}

void DrawDebugInfo(const Game* game) {
    if (!game->gameOver) {
        DrawText(TextFormat("Enemies: %d", CountActiveEnemies(game)), 
                SCREEN_WIDTH - 150, 10, 20, GREEN);
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

void DrawProjectiles(const Game* game, bool showHitbox) {
    Projectile* projectiles = (Projectile*)game->projectiles;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            DrawProjectile(&projectiles[i]);
            
            // Debug: Show hitbox when requested
            if (showHitbox) {
                const ProjectileDefinition* def = GetProjectileDefinition(projectiles[i].type);
                DrawCircleLines(
                    projectiles[i].position.x, 
                    projectiles[i].position.y,
                    def->hitboxRadius,
                    PINK
                );
            }
        }
    }
}

void DrawGame(Game* game) {
    bool showHitbox = IsKeyDown(KEY_H);
    
    // Draw starfield background
    DrawStarfield(game);
    
    // Draw game objects
    // Draw the new enhanced player ship
    DrawPlayerShip(game->playerShip);
    
    // Draw hitbox if requested
    if (showHitbox) {
        DrawRectangleLines(
            game->playerShip->bounds.x,
            game->playerShip->bounds.y,
            game->playerShip->bounds.width,
            game->playerShip->bounds.height,
            LIME
        );
    }
    
    DrawBullets(game->bullets);
    DrawProjectiles(game, showHitbox);
    DrawEnemies(game, showHitbox);
    
    // Draw explosions (after enemies but before HUD for proper layering)
    DrawExplosions(game->explosionSystem);
    
    // Draw player ship HUD
    DrawShipHUD(game->playerShip);
    
    // Draw additional UI
    DrawDebugInfo(game);
    
    // Draw wave information
    if (game->waveSystem) {
        const char* phaseName = GetCurrentPhaseName(game->waveSystem);
        float progress = GetWaveProgress(game->waveSystem);
        DrawText(TextFormat("Phase: %s", phaseName), SCREEN_WIDTH/2 - 100, 50, 25, YELLOW);
        
        // Wave progress bar
        DrawRectangle(SCREEN_WIDTH/2 - 200, 20, 400, 20, Fade(BLACK, 0.5f));
        DrawRectangle(SCREEN_WIDTH/2 - 200, 20, (int)(400 * progress / 100), 20, Fade(GREEN, 0.7f));
        DrawRectangleLines(SCREEN_WIDTH/2 - 200, 20, 400, 20, WHITE);
        DrawText("WAVE PROGRESS", SCREEN_WIDTH/2 - 60, 22, 16, WHITE);
    }
    
    // Draw controls hint
    DrawText("Controls: WASD - Move | SPACE - Fire | SHIFT - Boost | E/Q/R - Abilities | P - Pause | H - Hitboxes", 
            10, SCREEN_HEIGHT - 30, 16, Fade(WHITE, 0.7f));
    
    // Draw pause overlay
    if (game->gamePaused) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));
        DrawText("PAUSED", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 50, 60, WHITE);
        DrawText("Press P to Resume", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 20, 25, WHITE);
    }
    
    // Draw game over screen if needed
    if (game->gameOver) {
        DrawGameOver(game);
    }
}
