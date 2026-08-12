#include "game.h"
#include "constants.h"
#include "player_ship.h"
#include "enemy_types.h"
#include "projectile_types.h"
#include "level_system.h"
#include "wave_system.h"
#include "weapon.h"
#include "explosion.h"
#include "combat_system.h"
#include "projectile_manager.h"
#include "collision.h"
#include "powerup.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void InitGame(Game* game) {
    // Initialize logger
    InitLogger(game);
    
    // Initialize level manager
    game->levelManager = (LevelManager*)malloc(sizeof(LevelManager));
    InitLevelManager(game->levelManager);
    
    // Get current level configuration
    const LevelConfig* currentLevel = GetCurrentLevel(game->levelManager);
    
    // Try to load background music for current level
    // Note: Audio device is initialized once in main(), not here
    game->musicLoaded = false;
    game->musicVolume = 0.5f;  // Default 50% volume
    const char* musicPath = currentLevel->audioPath;
    if (FileExists(musicPath)) {
        game->backgroundMusic = LoadMusicStream(musicPath);
        // Check if music loaded successfully (ctxType will be non-zero if valid)
        if (game->backgroundMusic.ctxType > 0) {
            game->musicLoaded = true;
            PlayMusicStream(game->backgroundMusic);
            SetMusicVolume(game->backgroundMusic, game->musicVolume);
            printf("[GAME] Loaded music for level %d: %s\n", 
                   currentLevel->levelNumber, musicPath);
        }
    } else {
        printf("[GAME] WARNING: Music file not found: %s\n", musicPath);
    }
    
    // Allocate memory for arrays
    game->bullets = (Bullet*)malloc(MAX_BULLETS * sizeof(Bullet));
    game->projectiles = malloc(MAX_PROJECTILES * sizeof(Projectile));
    game->enemies = (EnemyEx*)malloc(MAX_ENEMIES * sizeof(EnemyEx));
    
    // Initialize player ship
    game->playerShip = (PlayerShip*)malloc(sizeof(PlayerShip));
    InitPlayerShip(game->playerShip);
    
    // Initialize starfield
    game->numStars = 150;
    game->stars = (Star*)malloc(game->numStars * sizeof(Star));
    for (int i = 0; i < game->numStars; i++) {
        game->stars[i].position.x = GetRandomValue(0, SCREEN_WIDTH);
        game->stars[i].position.y = GetRandomValue(0, SCREEN_HEIGHT);
        game->stars[i].speed = 1.0f + (i % 3) * 0.5f;
        game->stars[i].brightness = 100 + (i % 3) * 50;
    }
    
    // Initialize bullets
    InitBullets(game->bullets);
    
    // Initialize projectile types
    InitProjectileTypes();
    
    // Initialize projectiles array
    Projectile* projectiles = (Projectile*)game->projectiles;
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }
    
    // Initialize enemies array
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].active = false;
    }
    
    // Initialize wave system with current level configuration
    game->waveSystem = (WaveSystem*)malloc(sizeof(WaveSystem));
    InitWaveSystem(game->waveSystem, currentLevel, true);  // Apply debug phase on initial start
    
    // Initialize explosion system
    game->explosionSystem = (ExplosionSystem*)malloc(sizeof(ExplosionSystem));
    InitExplosionSystem(game->explosionSystem);
    
    // Initialize powerup system
    game->powerupSystem = (PowerupSystem*)malloc(sizeof(PowerupSystem));
    InitPowerupSystem(game->powerupSystem);
    
    // Initialize game state
    game->score = 0;
    game->backgroundX = 0;
    game->gameOver = false;
    game->gamePaused = false;
    game->justStarted = true;         // Prevent input on first frame
    game->bossEnemyIndex = -1;        // No boss initially
    game->bossSpawnTime = -1.0f;      // Boss not spawned yet
    game->bossEscapeTriggered = false;
    game->bossEscapeTimer = 0.0f;
    game->bossEscapePhase = 0;
    
    // Initialize interlevel transition system
    game->showingLevelComplete = false;
    game->levelCompleteTimer = 0.0f;
    game->transitioningToNextLevel = false;
    game->levelStartTime = 0.0f;  // Level starts at time 0
    game->maxScrollSpeed = 0.0f;  // No speed cap initially
    
    // Apply DEBUG_START_PHASE to set starting time
    float startTime = 0.0f;
    if (DEBUG_START_PHASE > 0 && DEBUG_START_PHASE <= 17) {
        // Phase timing mapping based on documentation
        const float phaseTimes[] = {
            0.0f,    // Phase 0: Normal start
            0.0f,    // Phase 1: Warm-Up (0-5s)
            5.0f,    // Phase 2: First Wave (5-35s)
            35.0f,   // Phase 3: Tank Squadron (35-55s)
            55.0f,   // Phase 4: Swarm Attack (55-85s)
            85.0f,   // Phase 5: Mixed Assault (85-120s)
            120.0f,  // Phase 6: Elite Squadron (120-150s)
            150.0f,  // Phase 7: Zigzag Chaos (150-180s)
            180.0f,  // Phase 8: Shield Wall (180-220s)
            220.0f,  // Phase 9: Bomber Run (220-260s)
            260.0f,  // Phase 10: Ghost Ambush (260-300s)
            300.0f,  // Phase 11: Combined Arms (300-345s)
            345.0f,  // Phase 12: Mini-Boss (345-390s)
            390.0f,  // Phase 13: Recovery (390-420s)
            420.0f,  // Phase 14: Elite & Shield (420-460s)
            460.0f,  // Phase 15: Evasive Maneuvers (460-500s)
            500.0f,  // Phase 16: Heavy Assault (500-540s)
            540.0f   // Phase 17: Final Wave (540-590s)
        };
        startTime = phaseTimes[DEBUG_START_PHASE];
        
        // Seek music to start time if music is loaded
        if (game->musicLoaded) {
            SeekMusicStream(game->backgroundMusic, startTime);
        }
    }
    
    game->gameTime = startTime;
    game->levelStartTime = 0.0f;  // Always start at 0 for the current level
    game->speedLevel = 1;
    
    // Calculate initial scroll speed based on start time
    // This ensures proper speed even when starting at different phases
    game->speedLevel = (int)(game->gameTime / SPEED_UPDATE_INTERVAL) + 1;
    game->scrollSpeed = BASE_SCROLL_SPEED + (SPEED_INCREMENT * (game->speedLevel - 1));
    
    // Note: maxScrollSpeed is set to 0.0 initially
    // UpdateGameSpeed() will cap speed based on current level number (not time)
    
    game->nextEnemyId = 1;
    game->nextProjectileId = 1;
    strcpy(game->deathCause, "Alive");
}

void UpdateGameSpeed(Game* game) {
    // Get the current level number from the level manager
    const LevelConfig* currentLevel = GetCurrentLevel(game->levelManager);
    int currentLevelNumber = currentLevel ? currentLevel->levelNumber : 1;
    
    // If we're in level 2 or higher, cap the speed at what it would be at end of level 1
    // This is simpler than tracking accumulated time across levels
    if (currentLevelNumber > 1) {
        // Calculate and set the cap if not already set
        if (game->maxScrollSpeed == 0.0f) {
            // Get level 1 duration to calculate final speed
            const LevelConfig* level1 = GetLevel(game->levelManager, 1);
            float level1Duration = level1 ? level1->duration : 553.82f;
            int level1FinalSpeedLevel = (int)(level1Duration / SPEED_UPDATE_INTERVAL) + 1;
            game->maxScrollSpeed = BASE_SCROLL_SPEED + (SPEED_INCREMENT * (level1FinalSpeedLevel - 1));
        }
        
        // Use the capped speed
        game->scrollSpeed = game->maxScrollSpeed;
        return;
    }
    
    // Normal speed progression during level 1
    // Calculate what the speed level should be based on game time
    int newSpeedLevel = (int)(game->gameTime / SPEED_UPDATE_INTERVAL) + 1;
    
    // Always update scroll speed based on current calculations
    // This ensures scrollSpeed is never left unset
    game->speedLevel = newSpeedLevel;
    game->scrollSpeed = BASE_SCROLL_SPEED + (SPEED_INCREMENT * (game->speedLevel - 1));
    
    // Ensure speed never goes below minimum
    if (game->scrollSpeed < BASE_SCROLL_SPEED) {
        game->scrollSpeed = BASE_SCROLL_SPEED;
    }
}

void FireEnemyProjectile(Game* game, EnemyEx* enemy) {
    // Use generic combat system
    CombatContext ctx = {
        .playerPosition = game->playerShip->position,
        .projectiles = (Projectile*)game->projectiles,
        .maxProjectiles = MAX_PROJECTILES,
        .screenWidth = SCREEN_WIDTH,
        .screenHeight = SCREEN_HEIGHT
    };
    
    Combat_FireEnemyProjectile(&ctx, enemy);
}

void UpdateEnemies(Game* game) {
    float deltaTime = GetFrameTime();
    
    // Create combat context for enemy firing
    CombatContext ctx = {
        .playerPosition = game->playerShip->position,
        .projectiles = (Projectile*)game->projectiles,
        .maxProjectiles = MAX_PROJECTILES,
        .screenWidth = SCREEN_WIDTH,
        .screenHeight = SCREEN_HEIGHT
    };
    
    // Update all active enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            EnemyEx* enemy = &game->enemies[i];
            
            // Update movement
            UpdateEnemyMovement(enemy, deltaTime);
            
            // Update firing using generic combat system
            Combat_UpdateEnemyFiring(enemy, &ctx, deltaTime, 
                                     0, SCREEN_WIDTH, 
                                     0, SCREEN_HEIGHT);
        }
    }
    
    // Check if boss has escaped or been destroyed
    if (game->bossEnemyIndex >= 0 && game->bossEnemyIndex < MAX_ENEMIES) {
        if (!game->enemies[game->bossEnemyIndex].active) {
            // Boss is no longer active - check if it escaped
            if (game->bossEscapeTriggered) {
                LogEvent(game, "[%.2f] Boss successfully escaped!", game->gameTime);
            }
            game->bossEnemyIndex = -1;  // Clear boss tracking
        }
    }
}

void DrawEnemies(const Game* game, bool showHitbox) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            DrawEnemyEx(&game->enemies[i]);
            
            // Debug: Show hitbox when requested
            if (showHitbox) {
                DrawRectangleLines(
                    game->enemies[i].bounds.x, game->enemies[i].bounds.y,
                    game->enemies[i].bounds.width, game->enemies[i].bounds.height,
                    PINK
                );
            }
        }
    }
}

int CountActiveEnemies(const Game* game) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) count++;
    }
    return count;
}

void UpdateProjectiles(Game* game) {
    float deltaTime = GetFrameTime();
    
    // Use projectile manager for cleaner code
    ProjectileManager mgr = {
        .projectiles = (Projectile*)game->projectiles,
        .maxProjectiles = MAX_PROJECTILES,
        .minX = -100, .maxX = SCREEN_WIDTH + 100,
        .minY = -100, .maxY = SCREEN_HEIGHT + 100
    };
    
    ProjectileManager_UpdateAll(&mgr, deltaTime);
}

void UpdateGame(Game* game) {
    // Skip input processing on first frame after starting
    if (game->justStarted) {
        game->justStarted = false;
        // Still update music
        if (game->musicLoaded) {
            UpdateMusicStream(game->backgroundMusic);
        }
        return;  // Skip all other updates on first frame
    }
    
    // Update music stream if loaded
    if (game->musicLoaded) {
        UpdateMusicStream(game->backgroundMusic);
    }
    
    if (!game->gameOver) {
        // Get level-specific timing (used throughout this function)
        const LevelConfig* currentLevel = GetCurrentLevel(game->levelManager);
        float levelDuration = currentLevel ? currentLevel->duration : 553.82f;
        
        // Only update if not paused
        if (!game->gamePaused) {
            // Update game time
            float deltaTime = GetFrameTime();
            game->gameTime += deltaTime;
            
            // Update speed levels
            UpdateGameSpeed(game);
            
            // Update game components
            // Update new player ship
            UpdatePlayerShip(game->playerShip, deltaTime, game->inputManager);
            
            // Check for weapon powerup revive event and log it
            static bool wasJustRevived = false;
            if (game->playerShip->justRevived && !wasJustRevived) {
                LogEvent(game, "[%.2f] SHIP REVIVED! Weapon powerup consumed. Hull restored: %d HP | Shield: 50%% | Weapon Power: Level %d",
                        game->gameTime, game->playerShip->health, game->playerShip->weaponPowerupCount);
                wasJustRevived = true;
            } else if (!game->playerShip->justRevived) {
                wasJustRevived = false;
            }
            
            // Player ship properties are used directly
            
            // Use original bullet system
            UpdateBullets(game);
            UpdateProjectiles(game);
            UpdateWaveSystem(game->waveSystem, game, deltaTime);
            UpdateEnemies(game);
            
            // Update explosion system
            UpdateExplosionSystem(game->explosionSystem, deltaTime);
            
            // Update powerup system
            UpdatePowerups(game->powerupSystem, game->playerShip, deltaTime);
            
            // Update starfield
            for (int i = 0; i < game->numStars; i++) {
                game->stars[i].position.x -= game->stars[i].speed * game->scrollSpeed;
                if (game->stars[i].position.x < 0) {
                    game->stars[i].position.x = SCREEN_WIDTH;
                    game->stars[i].position.y = GetRandomValue(0, SCREEN_HEIGHT);
                }
            }
            
            // Background scroll with dynamic speed
            game->backgroundX -= game->scrollSpeed;
            if (game->backgroundX <= -SCREEN_WIDTH) {
                game->backgroundX = 0;
            }
            
            // Boss escape sequence - starts AFTER boss has been alive for a certain time
            // Level 1: 90 seconds after boss spawn (427s → 517s)
            // Level 2: 70 seconds after boss spawn (530s → 600s)
            // DRAMATIC PHASED SEQUENCE
            if (!game->bossEscapeTriggered && game->bossSpawnTime >= 0) {
                // Calculate boss battle duration (using level time)
                float currentLevelTime = game->gameTime - game->levelStartTime;
                float bossBattleTime = currentLevelTime - game->bossSpawnTime;  // Both in level time
                float requiredBattleTime = (currentLevel && currentLevel->levelNumber == 2) ? 70.0f : 90.0f;
                
                // Check if boss has been alive long enough
                if (bossBattleTime >= requiredBattleTime) {
                    // Check if boss is still active
                    if (game->bossEnemyIndex >= 0 && 
                        game->bossEnemyIndex < MAX_ENEMIES && 
                        game->enemies[game->bossEnemyIndex].active &&
                        game->enemies[game->bossEnemyIndex].type == ENEMY_BOSS) {
                        
                        game->bossEscapeTriggered = true;
                        game->bossEscapePhase = 1;  // Start destruction phase
                        game->bossEscapeTimer = 0.0f;
                        game->enemies[game->bossEnemyIndex].isEscaping = true;
                        
                        LogEvent(game, "[%.2f] BOSS DOOMSDAY - Escape sequence initiated! (Boss alive for %.1fs)", 
                                game->gameTime, bossBattleTime);
                    }
                }
            }
            
            // Handle dramatic boss escape phases
            if (game->bossEscapeTriggered && game->bossEscapePhase > 0) {
                game->bossEscapeTimer += deltaTime;
                
                // PHASE 1: DESTRUCTION (0-2.5 seconds) - Staggered explosions
                if (game->bossEscapePhase == 1) {
                    // Destroy bullets immediately at start
                    if (game->bossEscapeTimer < 0.1f) {
                        for (int i = 0; i < MAX_BULLETS; i++) {
                            if (game->bullets[i].active) {
                                CreateExplosion(game->explosionSystem, game->bullets[i].position, EXPLOSION_SMALL);
                                game->bullets[i].active = false;
                            }
                        }
                    }
                    
                    // Destroy projectiles at 0.3s
                    if (game->bossEscapeTimer >= 0.3f && game->bossEscapeTimer < 0.4f) {
                        Projectile* projectiles = (Projectile*)game->projectiles;
                        for (int i = 0; i < MAX_PROJECTILES; i++) {
                            if (projectiles[i].active) {
                                CreateExplosion(game->explosionSystem, projectiles[i].position, EXPLOSION_SMALL);
                                projectiles[i].active = false;
                            }
                        }
                    }
                    
                    // Destroy enemies in waves (0.5s - 2.0s)
                    if (game->bossEscapeTimer >= 0.5f && game->bossEscapeTimer < 2.0f) {
                        // Destroy a few enemies each frame for dramatic effect
                        int destroyCount = 0;
                        for (int i = 0; i < MAX_ENEMIES && destroyCount < 2; i++) {
                            if (game->enemies[i].active && i != game->bossEnemyIndex) {
                                Color enemyColor = GetEnemyTypeColor(game->enemies[i].type);
                                CreateEnemyExplosion(game->explosionSystem, game->enemies[i].position, 
                                                   enemyColor, game->enemies[i].bounds.width);
                                game->enemies[i].active = false;
                                destroyCount++;
                            }
                        }
                    }
                    
                    // OBLITERATE THE PLAYER at 1.5s
                    if (game->bossEscapeTimer >= 1.5f && game->bossEscapeTimer < 1.6f) {
                        CreatePlayerExplosion(game->explosionSystem, game->playerShip->position);
                        game->playerShip->health = 0;
                        game->playerShip->maxHealth = 0;
                        game->playerShip->isVisible = false;  // Remove player ship from screen
                        LogEvent(game, "[%.2f] PLAYER OBLITERATED by boss doomsday attack!", game->gameTime);
                    }
                    
                    // Move to phase 2 after 2.5 seconds
                    if (game->bossEscapeTimer >= 2.5f) {
                        game->bossEscapePhase = 2;
                        game->bossEscapeTimer = 0.0f;
                        LogEvent(game, "[%.2f] Boss escaping through the chaos...", game->gameTime);
                    }
                }
                
                // PHASE 2: BOSS ESCAPE (2.5s - until boss off screen)
                else if (game->bossEscapePhase == 2) {
                    // Check if boss has escaped off screen
                    if (game->bossEnemyIndex >= 0 && game->bossEnemyIndex < MAX_ENEMIES) {
                        EnemyEx* boss = &game->enemies[game->bossEnemyIndex];
                        
                        if (!boss->active || boss->position.x > SCREEN_WIDTH + 150) {
                            // Boss has escaped!
                            game->bossEscapePhase = 3;
                            game->bossEscapeTimer = 0.0f;
                            LogEvent(game, "[%.2f] Boss successfully escaped! Preparing final message...", game->gameTime);
                        }
                    } else {
                        // Boss was destroyed somehow during escape, move to phase 3
                        game->bossEscapePhase = 3;
                        game->bossEscapeTimer = 0.0f;
                    }
                }
                
                // PHASE 3: SHOW GAME OVER (after 1 second delay)
                else if (game->bossEscapePhase == 3) {
                    if (game->bossEscapeTimer >= 1.0f) {
                        // NOW show game over
                        game->gameOver = true;
                        strcpy(game->deathCause, "DEFEAT! The Boss escaped and obliterated everything!");
                        LogEvent(game, "[%.2f] GAME OVER - Boss escape complete", game->gameTime);
                        game->bossEscapePhase = 4;  // Mark as complete
                    }
                }
            }
            
            // Interlevel transition system
            // Calculate time elapsed in current level (gameTime continues across levels)
            float levelTime = game->gameTime - game->levelStartTime;
            float timeRemaining = levelDuration - levelTime;
            
            // Show level complete overlay when 15 seconds remain
            if (timeRemaining <= 15.0f && timeRemaining > 0.0f && !game->showingLevelComplete) {
                game->showingLevelComplete = true;
                game->levelCompleteTimer = 0.0f;
                LogEvent(game, "[%.2f] Level %d completion overlay displayed - 15 seconds remaining", 
                        game->gameTime, currentLevel->levelNumber);
            }
            
            // Update level complete timer
            if (game->showingLevelComplete) {
                game->levelCompleteTimer += deltaTime;
            }
            
            // Check if level time limit reached
            if (levelTime >= levelDuration && !game->bossEscapeTriggered) {
                // Check if there's a next level
                if (game->levelManager->currentLevel + 1 < game->levelManager->levelCount) {
                    // Transition to next level seamlessly
                    if (!game->transitioningToNextLevel) {
                        game->transitioningToNextLevel = true;
                        
                        LogEvent(game, "[%.2f] Level %d complete! Score: %d - Transitioning to next level...", 
                                game->gameTime, currentLevel->levelNumber, game->score);
                        
                        // Note: Speed capping is now handled automatically in UpdateGameSpeed()
                        // based on game time vs level 1 duration
                        
                        // Advance to next level
                        AdvanceToNextLevel(game->levelManager);
                        const LevelConfig* nextLevel = GetCurrentLevel(game->levelManager);
                        
                        // Load new music
                        if (game->musicLoaded) {
                            StopMusicStream(game->backgroundMusic);
                            UnloadMusicStream(game->backgroundMusic);
                        }
                        
                        game->musicLoaded = false;
                        if (FileExists(nextLevel->audioPath)) {
                            game->backgroundMusic = LoadMusicStream(nextLevel->audioPath);
                            if (game->backgroundMusic.ctxType > 0) {
                                game->musicLoaded = true;
                                PlayMusicStream(game->backgroundMusic);
                                SetMusicVolume(game->backgroundMusic, game->musicVolume);
                                LogEvent(game, "[%.2f] Started music for level %d: %s", 
                                        game->gameTime, nextLevel->levelNumber, nextLevel->audioPath);
                            }
                        }
                        
                        // Clear remaining enemies from previous level
                        for (int i = 0; i < MAX_ENEMIES; i++) {
                            game->enemies[i].active = false;
                        }
                        
                        // Clear remaining projectiles from previous level
                        Projectile* projectiles = (Projectile*)game->projectiles;
                        for (int i = 0; i < MAX_PROJECTILES; i++) {
                            projectiles[i].active = false;
                        }
                        
                        // Reinitialize wave system for new level
                        CleanupWaveSystem(game->waveSystem);
                        InitWaveSystem(game->waveSystem, nextLevel, false);  // NO debug phase - start at 0
                        
                        // Mark the start of the new level (gameTime continues running)
                        // This allows speed to be preserved while level timer resets
                        game->levelStartTime = game->gameTime;
                        // Note: gameTime, speedLevel and scrollSpeed continue from previous level
                        
                        // Reset boss tracking
                        game->bossEnemyIndex = -1;
                        game->bossSpawnTime = -1.0f;
                        game->bossEscapeTriggered = false;
                        game->bossEscapeTimer = 0.0f;
                        game->bossEscapePhase = 0;
                        
                        // Reset interlevel transition state
                        game->showingLevelComplete = false;
                        game->levelCompleteTimer = 0.0f;
                        game->transitioningToNextLevel = false;
                        
                        LogEvent(game, "[%.2f] Now playing Level %d: %s (Level Time: 0.00)", 
                                game->gameTime, nextLevel->levelNumber, nextLevel->name);
                    }
                } else {
                    // Last level completed - end game with victory
                    game->gameOver = true;
                    strcpy(game->deathCause, "VICTORY! You survived all levels!");
                    LogEvent(game, "[%.2f] All levels completed! Final score: %d", 
                            game->gameTime, game->score);
                }
            }
            
            // Game over check (but not during boss escape sequence - that handles its own game over)
            if (game->playerShip->health <= 0 && !game->bossEscapeTriggered) {
                // Create dramatic player explosion
                CreatePlayerExplosion(game->explosionSystem, game->playerShip->position);
                game->gameOver = true;
            }
        }
    } else {
        // Restart game on R key press
        if (IsKeyPressed(KEY_R)) {
            CleanupGame(game);
            InitGame(game);
        }
    }
}

void CleanupGame(Game* game) {
    CloseLogger(game);
    
    // Clear input manager reference (it's owned by main.c, not freed here)
    game->inputManager = NULL;
    
    // Cleanup audio
    if (game->musicLoaded) {
        StopMusicStream(game->backgroundMusic);
        UnloadMusicStream(game->backgroundMusic);
        game->musicLoaded = false;
    }
    // Note: Don't call CloseAudioDevice() here - Raylib handles it on window close
    
    // Free level manager
    if (game->levelManager) {
        CleanupLevelManager(game->levelManager);
        free(game->levelManager);
        game->levelManager = NULL;
    }
    
    // Free wave system
    if (game->waveSystem) {
        CleanupWaveSystem(game->waveSystem);
        free(game->waveSystem);
        game->waveSystem = NULL;
    }
    
    // Free explosion system
    if (game->explosionSystem) {
        free(game->explosionSystem);
        game->explosionSystem = NULL;
    }
    
    // Free powerup system
    if (game->powerupSystem) {
        CleanupPowerupSystem(game->powerupSystem);
        free(game->powerupSystem);
        game->powerupSystem = NULL;
    }
    
    // Free player ship
    if (game->playerShip) {
        free(game->playerShip);
        game->playerShip = NULL;
    }
    
    // Free starfield
    if (game->stars) {
        free(game->stars);
        game->stars = NULL;
    }
    
    // Free allocated memory
    if (game->bullets) {
        free(game->bullets);
        game->bullets = NULL;
    }
    if (game->projectiles) {
        free(game->projectiles);
        game->projectiles = NULL;
    }
    if (game->enemies) {
        free(game->enemies);
        game->enemies = NULL;
    }
}

void SetGameMusicVolume(Game* game, float volume) {
    game->musicVolume = volume;
    if (game->musicLoaded) {
        SetMusicVolume(game->backgroundMusic, volume);
    }
}

