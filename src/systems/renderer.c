#include "renderer.h"
#include "constants.h"
#include "player_ship.h"
#include "game.h"
#include "projectile_types.h"
#include "wave_system.h"
#include "level_system.h"
#include "weapon.h"
#include "explosion.h"
#include "powerup.h"
#include <stdio.h>
#include <math.h>

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
    
    // Weapon heat bar removed - no overheating system
    
    // Wave phase information
    if (game->waveSystem) {
        const char* phaseName = GetCurrentPhaseName(game->waveSystem);
        float progress = GetWaveProgress(game->waveSystem);
        DrawText(TextFormat("Phase: %s", phaseName), 10, 115, 25, YELLOW);
        DrawText(TextFormat("Progress: %.1f%%", progress), 10, 145, 25, WHITE);
    }
    
    // Time (show level time)
    float levelTime = game->gameTime - game->levelStartTime;
    int minutes = (int)(levelTime / 60);
    int seconds = (int)levelTime % 60;
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
    DrawText("Controls: WASD/Arrows - Move | P - Pause", 
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
    // Darken full screen but center content in play zone
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    
    int centerY = PLAY_ZONE_TOP + PLAY_ZONE_HEIGHT/2;
    DrawText("GAME OVER", SCREEN_WIDTH/2 - 150, centerY - 150, 50, RED);
    
    // Show current phase number for easy debug restart
    if (game->waveSystem) {
        int phaseNum = GetCurrentPhaseNumber(game->waveSystem);
        float levelTime = game->gameTime - game->levelStartTime;
        int minutes = (int)(levelTime / 60);
        int seconds = (int)levelTime % 60;
        
        DrawRectangle(SCREEN_WIDTH/2 - 200, centerY - 90, 400, 60, Fade(DARKBLUE, 0.8f));
        DrawRectangleLines(SCREEN_WIDTH/2 - 200, centerY - 90, 400, 60, SKYBLUE);
        
        DrawText(TextFormat("Phase: %d  |  Time: %02d:%02d", phaseNum, minutes, seconds), 
                SCREEN_WIDTH/2 - 150, centerY - 75, 24, SKYBLUE);
        DrawText(TextFormat("To restart here: ./run_debug_game.sh -i -p %d", phaseNum), 
                SCREEN_WIDTH/2 - 190, centerY - 50, 18, LIGHTGRAY);
    }
    
    // Show death cause with better formatting
    int textWidth = MeasureText(game->deathCause, 20);
    DrawText("Cause of Death:", SCREEN_WIDTH/2 - 80, centerY + 10, 22, ORANGE);
    DrawText(game->deathCause, SCREEN_WIDTH/2 - textWidth/2, centerY + 40, 20, YELLOW);
    
    DrawText(TextFormat("Final Score: %d", game->score), SCREEN_WIDTH/2 - 120, centerY + 80, 30, WHITE);
    DrawText("Press R to Restart", SCREEN_WIDTH/2 - 110, centerY + 120, 25, WHITE);
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

void DrawLevelCompleteOverlay(const Game* game) {
    if (!game->showingLevelComplete) {
        return;
    }
    
    // Get current level information
    const LevelConfig* currentLevel = GetCurrentLevel(game->levelManager);
    if (!currentLevel) {
        return;
    }
    
    // Calculate time remaining until level ends (using level time, not total game time)
    float levelTime = game->gameTime - game->levelStartTime;
    float timeRemaining = currentLevel->duration - levelTime;
    
    // Fade in effect for first second
    float fadeAlpha = game->levelCompleteTimer < 1.0f ? game->levelCompleteTimer : 1.0f;
    
    // Semi-transparent background in upper section
    int panelY = PLAY_ZONE_TOP + 60;
    int panelHeight = 120;
    int panelWidth = 600;
    int panelX = (SCREEN_WIDTH - panelWidth) / 2;
    
    // Background with border
    DrawRectangle(panelX, panelY, panelWidth, panelHeight, Fade((Color){0, 20, 40, 200}, fadeAlpha * 0.85f));
    DrawRectangleLines(panelX, panelY, panelWidth, panelHeight, Fade((Color){100, 200, 255, 255}, fadeAlpha));
    
    // Add corner decorations
    int cornerSize = 20;
    DrawRectangle(panelX, panelY, cornerSize, 3, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX, panelY, 3, cornerSize, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX + panelWidth - cornerSize, panelY, cornerSize, 3, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX + panelWidth - 3, panelY, 3, cornerSize, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX, panelY + panelHeight - 3, cornerSize, 3, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX, panelY + panelHeight - cornerSize, 3, cornerSize, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX + panelWidth - cornerSize, panelY + panelHeight - 3, cornerSize, 3, Fade(GOLD, fadeAlpha));
    DrawRectangle(panelX + panelWidth - 3, panelY + panelHeight - cornerSize, 3, cornerSize, Fade(GOLD, fadeAlpha));
    
    // Level complete title
    const char* titleText = TextFormat("LEVEL %d: %s", currentLevel->levelNumber, currentLevel->name);
    int titleWidth = MeasureText(titleText, 30);
    DrawText(titleText, panelX + (panelWidth - titleWidth) / 2, panelY + 15, 30, Fade(GOLD, fadeAlpha));
    
    // Status text
    const char* statusText = "COMPLETING...";
    int statusWidth = MeasureText(statusText, 20);
    DrawText(statusText, panelX + (panelWidth - statusWidth) / 2, panelY + 50, 20, Fade(LIME, fadeAlpha));
    
    // Current score
    const char* scoreText = TextFormat("Current Score: %d", game->score);
    int scoreWidth = MeasureText(scoreText, 24);
    DrawText(scoreText, panelX + (panelWidth - scoreWidth) / 2, panelY + 78, 24, Fade(WHITE, fadeAlpha));
    
    // Time remaining indicator (small, at bottom of panel)
    if (timeRemaining > 0) {
        const char* timeText = TextFormat("Next level in: %.0f seconds", timeRemaining);
        int timeWidth = MeasureText(timeText, 14);
        DrawText(timeText, panelX + (panelWidth - timeWidth) / 2, panelY + panelHeight - 22, 14, Fade(SKYBLUE, fadeAlpha * 0.8f));
    }
}

void DrawGame(Game* game) {
    // Hitbox debug removed for cleaner gameplay
    
    // === TOP HUD (Phase, Progress, Time, Enemy Count) ===
    // Draw top HUD background bar
    DrawRectangle(0, 0, SCREEN_WIDTH, 30, Fade(BLACK, 0.7f));
    
    // Left: Phase name
    if (game->waveSystem) {
        const char* phaseName = GetCurrentPhaseName(game->waveSystem);
        DrawText(TextFormat("PHASE: %s", phaseName), 10, 8, 18, YELLOW);
    }
    
    // Center-Left: Wave progress bar
    if (game->waveSystem) {
        float progress = GetWaveProgress(game->waveSystem);
        int barX = 250;
        int barWidth = 300;
        DrawText("PROGRESS:", barX, 10, 14, GRAY);
        DrawRectangle(barX + 85, 10, barWidth, 12, Fade(DARKGRAY, 0.5f));
        DrawRectangle(barX + 85, 10, (int)(barWidth * progress / 100), 12, Fade(GREEN, 0.8f));
        DrawRectangleLines(barX + 85, 10, barWidth, 12, WHITE);
        DrawText(TextFormat("%.0f%%", progress), barX + 390, 10, 14, WHITE);
    }
    
    // Center-Right: Time (shows level time, not total game time)
    float levelTime = game->gameTime - game->levelStartTime;
    int minutes = (int)(levelTime / 60);
    int seconds = (int)levelTime % 60;
    DrawText(TextFormat("TIME: %02d:%02d", minutes, seconds), 680, 8, 18, WHITE);
    
    // Right: Enemy count
    if (game->waveSystem) {
        int enemyCount = CountActiveEnemies(game);
        Color enemyColor = enemyCount > 0 ? RED : GREEN;
        DrawText(TextFormat("ENEMIES: %d", enemyCount), 850, 8, 18, enemyColor);
    }
    
    // Debug indicators (top right corner)
    if (DEBUG_INVULNERABILITY) {
        DrawText("[INVULN]", 1050, 8, 16, LIME);
    }
    if (DEBUG_START_PHASE > 0) {
        DrawText(TextFormat("[P%d]", DEBUG_START_PHASE), 1150, 8, 16, ORANGE);
    }
    
    // Draw starfield background (only in play zone)
    DrawStarfield(game);
    
    // Draw game objects (all in play zone)
    DrawPlayerShip(game->playerShip);
    DrawBullets(game->bullets);
    DrawProjectiles(game, false);  // No hitbox display
    DrawEnemies(game, false);  // No hitbox display
    DrawPowerups(game->powerupSystem);
    DrawExplosions(game->explosionSystem);
    
    // Danger warning effect - warning before boss escape
    // Level 1: Warning at 60s after boss spawn (30s before 90s escape)
    // Level 2: Warning at 40s after boss spawn (30s before 70s escape)
    // IMPORTANT: Warning stops immediately if boss is killed!
    if (game->bossSpawnTime >= 0 && !game->bossEscapeTriggered && 
        game->bossEnemyIndex >= 0 && 
        game->enemies[game->bossEnemyIndex].active) {  // Only show if boss is still alive!
        
        const LevelConfig* currentLevel = GetCurrentLevel(game->levelManager);
        // Calculate boss battle time using level time
        float currentLevelTime = game->gameTime - game->levelStartTime;
        float bossBattleTime = currentLevelTime - game->bossSpawnTime;  // Both in level time
        float requiredBattleTime = (currentLevel && currentLevel->levelNumber == 2) ? 70.0f : 90.0f;
        float warningDuration = 30.0f;  // Both levels: 30s warning
        float warningStartTime = requiredBattleTime - warningDuration;
        
        if (bossBattleTime >= warningStartTime && bossBattleTime < requiredBattleTime) {
            // Calculate time remaining until escape countdown
            float timeRemaining = requiredBattleTime - bossBattleTime;
            float warningProgress = 1.0f - (timeRemaining / warningDuration);  // 0.0 to 1.0
            
            // Pulsing effect - faster as time runs out
            float pulseSpeed = 2.0f + (warningProgress * 6.0f);  // 2 to 8 Hz
            float pulse = (sinf(game->gameTime * pulseSpeed) + 1.0f) * 0.5f;  // 0.0 to 1.0
            
            // Alpha increases with time and pulse
            float baseAlpha = 0.1f + (warningProgress * 0.4f);  // 0.1 to 0.5
            float alpha = baseAlpha * pulse;
            
            // Draw red borders around play zone
            int borderThickness = 8 + (int)(warningProgress * 12.0f);  // 8 to 20 pixels
            Color dangerColor = Fade(RED, alpha);
            
            // Top border
            DrawRectangle(0, PLAY_ZONE_TOP, SCREEN_WIDTH, borderThickness, dangerColor);
            // Bottom border
            DrawRectangle(0, PLAY_ZONE_BOTTOM - borderThickness, SCREEN_WIDTH, borderThickness, dangerColor);
            // Left border
            DrawRectangle(0, PLAY_ZONE_TOP, borderThickness, PLAY_ZONE_HEIGHT, dangerColor);
            // Right border
            DrawRectangle(SCREEN_WIDTH - borderThickness, PLAY_ZONE_TOP, borderThickness, PLAY_ZONE_HEIGHT, dangerColor);
            
            // Add warning text in top center when close to escape (last 10 seconds)
            if (timeRemaining <= 10.0f) {
                int textSize = 30 + (int)((1.0f - timeRemaining / 10.0f) * 20.0f);  // 30 to 50
                const char* warningText = TextFormat("DANGER! BOSS ESCAPE IN %.0f!", timeRemaining);
                int textWidth = MeasureText(warningText, textSize);
                Color textColor = Fade(RED, 0.7f + pulse * 0.3f);
                DrawText(warningText, (SCREEN_WIDTH - textWidth) / 2, PLAY_ZONE_TOP + 40, textSize, textColor);
            }
        }
    }
    
    // Draw play zone separator line
    DrawLine(0, PLAY_ZONE_BOTTOM, SCREEN_WIDTH, PLAY_ZONE_BOTTOM, Fade(WHITE, 0.3f));
    
    // === BOTTOM HUD (from left to right) ===
    int hudY = PLAY_ZONE_BOTTOM + 5;  // Start HUD 5 pixels below play zone
    
    // Draw HUD background
    DrawRectangle(0, PLAY_ZONE_BOTTOM, SCREEN_WIDTH, BOTTOM_HUD_HEIGHT, Fade(BLACK, 0.8f));
    
    // Left section: Ship status bars (hull, shield, energy, mode)
    DrawShipHUD(game->playerShip);
    
    // Middle-Left section: Score
    int scoreX = 280;
    DrawText(TextFormat("SCORE: %d", game->score), scoreX, hudY + 15, 24, GOLD);
    
    // Middle section: Weapon info & damage (recovered from removal)
    int weaponX = 480;
    
    // Weapon Mode Display (recovered)
    DrawText("WEAPON:", weaponX, hudY + 8, 14, GRAY);
    const char* weaponModeNames[] = {
        "SINGLE", "DOUBLE", "SPREAD", "RAPID", "CHARGE", "DUAL"
    };
    Color weaponColor = (Color){100, 255, 100, 255};
    DrawText(weaponModeNames[game->playerShip->weaponMode], weaponX + 75, hudY + 8, 14, weaponColor);
    DrawText("(1-6/R)", weaponX + 145, hudY + 8, 11, Fade(WHITE, 0.5f));
    
    // Weapon Power Counter (recovered)
    DrawText("POWER:", weaponX, hudY + 28, 14, GRAY);
    int socketX = weaponX + 60;
    int socketSize = 12;
    for (int i = 0; i < 3; i++) {
        int x = socketX + i * 18;
        if (i < game->playerShip->weaponPowerupCount) {
            DrawRectangle(x, hudY + 29, socketSize, socketSize, ORANGE);
            DrawRectangleLines(x, hudY + 29, socketSize, socketSize, WHITE);
        } else {
            DrawRectangle(x, hudY + 29, socketSize, socketSize, Fade(DARKGRAY, 0.3f));
            DrawRectangleLines(x, hudY + 29, socketSize, socketSize, DARKGRAY);
        }
    }
    DrawText(TextFormat("%d/3", game->playerShip->weaponPowerupCount), socketX + 60, hudY + 28, 14, ORANGE);
    
    // Damage indicator with bonuses and multipliers
    DrawText("DAMAGE:", weaponX, hudY + 50, 14, GRAY);
    float damagePerShot = CalculateDamagePerShot(game->playerShip);
    Color damageColor = WHITE;
    if (damagePerShot >= 15.0f) {
        damageColor = (Color){255, 50, 50, 255};
    } else if (damagePerShot >= 10.0f) {
        damageColor = (Color){255, 100, 0, 255};
    } else if (damagePerShot >= 6.0f) {
        damageColor = ORANGE;
    } else if (damagePerShot >= 3.0f) {
        damageColor = YELLOW;
    }
    DrawText(TextFormat("%.1f", damagePerShot), weaponX + 75, hudY + 50, 14, damageColor);
    DrawText("per shot", weaponX + 120, hudY + 50, 11, Fade(WHITE, 0.5f));
    
    // Show active multipliers and bonuses
    int multX = weaponX + 180;
    bool hasMultiplier = false;
    
    // Weapon power multiplier
    if (game->playerShip->weaponPowerupCount > 0) {
        float powerMult = 1.0f;
        switch (game->playerShip->weaponPowerupCount) {
            case 1: powerMult = 1.5f; break;
            case 2: powerMult = 2.0f; break;
            case 3: powerMult = 2.5f; break;
            default: powerMult = 1.0f; break;
        }
        DrawText(TextFormat("x%.1f", powerMult), multX, hudY + 50, 12, ORANGE);
        DrawText("PWR", multX, hudY + 62, 9, Fade(ORANGE, 0.7f));
        multX += 42;
        hasMultiplier = true;
    }
    
    // Offensive mode bonus (2x damage when energy full)
    if (game->playerShip->energyMode == ENERGY_MODE_OFFENSIVE && game->playerShip->energyFull) {
        DrawText("x2.0", multX, hudY + 50, 12, RED);
        DrawText("MODE", multX - 2, hudY + 62, 9, Fade(RED, 0.7f));
        hasMultiplier = true;
    }
    
    // If no multipliers, show base damage note
    if (!hasMultiplier) {
        DrawText("(base)", multX, hudY + 52, 10, Fade(WHITE, 0.4f));
    }
    
    // Charge indicator for charge mode
    if (game->playerShip->weaponMode == WEAPON_MODE_CHARGE && game->playerShip->isCharging) {
        DrawText("CHARGING...", weaponX, hudY + 70, 12, YELLOW);
        DrawRectangle(weaponX + 80, hudY + 72, 100, 8, Fade(GRAY, 0.3f));
        DrawRectangle(weaponX + 80, hudY + 72, (game->playerShip->chargeLevel), 8, YELLOW);
        DrawRectangleLines(weaponX + 80, hudY + 72, 100, 8, WHITE);
    }
    
    // Right section: Controls
    int controlsX = 750;
    DrawText("CONTROLS:", controlsX, hudY + 5, 12, GRAY);
    DrawText("WASD/Arrows - Move", controlsX, hudY + 20, 11, WHITE);
    DrawText("SPACE - Fire", controlsX, hudY + 35, 11, WHITE);
    DrawText("Q - Mode", controlsX, hudY + 50, 11, WHITE);
    DrawText("E - Special", controlsX, hudY + 65, 11, WHITE);
    
    DrawText("P - Pause", controlsX + 120, hudY + 20, 11, WHITE);
    DrawText("ESC - Menu", controlsX + 120, hudY + 35, 11, WHITE);
    
    // Draw level complete overlay (semi-transparent, non-invasive)
    DrawLevelCompleteOverlay(game);
    
    // Draw pause overlay
    if (game->gamePaused) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));
        DrawText("PAUSED", SCREEN_WIDTH/2 - 100, PLAY_ZONE_TOP + PLAY_ZONE_HEIGHT/2 - 50, 60, WHITE);
        DrawText("Press P to Resume", SCREEN_WIDTH/2 - 100, PLAY_ZONE_TOP + PLAY_ZONE_HEIGHT/2 + 20, 25, WHITE);
    }
    
    // Draw game over screen if needed
    if (game->gameOver) {
        DrawGameOver(game);
    }
}
