#include "game.h"
#include "constants.h"
#include "player_ship.h"
#include "enemy_types.h"
#include "projectile_types.h"
#include "wave_system.h"
#include "weapon.h"
#include "explosion.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void InitGame(Game* game) {
    // Initialize logger
    InitLogger(game);
    
    // Initialize audio device
    InitAudioDevice();
    
    // Try to load background music
    game->musicLoaded = false;
    game->musicVolume = 0.5f;  // Default 50% volume
    const char* musicPath = "assets/audio/level1.mp3";
    if (FileExists(musicPath)) {
        game->backgroundMusic = LoadMusicStream(musicPath);
        // Check if music loaded successfully (ctxType will be non-zero if valid)
        if (game->backgroundMusic.ctxType > 0) {
            game->musicLoaded = true;
            PlayMusicStream(game->backgroundMusic);
            SetMusicVolume(game->backgroundMusic, game->musicVolume);
        }
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
    
    // Initialize wave system
    game->waveSystem = (WaveSystem*)malloc(sizeof(WaveSystem));
    InitWaveSystem(game->waveSystem);
    
    // Initialize explosion system
    game->explosionSystem = (ExplosionSystem*)malloc(sizeof(ExplosionSystem));
    InitExplosionSystem(game->explosionSystem);
    
    // Initialize game state
    game->score = 0;
    game->backgroundX = 0;
    game->gameOver = false;
    game->gamePaused = false;
    game->bossEnemyIndex = -1;        // No boss initially
    game->bossEscapeTriggered = false;
    game->bossEscapeTimer = 0.0f;
    game->bossEscapePhase = 0;
    
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
    game->scrollSpeed = BASE_SCROLL_SPEED;
    game->speedLevel = 1;
    
    // Update scroll speed based on start time
    UpdateGameSpeed(game);
    
    game->nextEnemyId = 1;
    game->nextProjectileId = 1;
    strcpy(game->deathCause, "Alive");
}

void UpdateGameSpeed(Game* game) {
    int newSpeedLevel = (int)(game->gameTime / SPEED_UPDATE_INTERVAL) + 1;
    if (newSpeedLevel != game->speedLevel) {
        game->speedLevel = newSpeedLevel;
        game->scrollSpeed = BASE_SCROLL_SPEED + (SPEED_INCREMENT * (game->speedLevel - 1));
    }
}

void FireEnemyProjectile(Game* game, EnemyEx* enemy) {
    const EnemyWeaponConfig* config = GetEnemyWeaponConfig(enemy->type);
    Vector2 playerPos = game->playerShip->position;
    
    // Fire burst pattern
    for (int burst = 0; burst < config->burstCount; burst++) {
        float angle = 0;
        
        if (config->spreadAngle > 0 && config->burstCount > 1) {
            if (config->spreadAngle >= 360) {
                // Full circle spread
                angle = (360.0f / config->burstCount) * burst;
            } else {
                // Arc spread
                float startAngle = -config->spreadAngle / 2.0f;
                float angleStep = config->spreadAngle / (config->burstCount - 1);
                angle = startAngle + angleStep * burst;
            }
        }
        
        // Calculate target direction
        Vector2 direction = {
            playerPos.x - enemy->position.x,
            playerPos.y - enemy->position.y
        };
        float dist = sqrtf(direction.x * direction.x + direction.y * direction.y);
        if (dist > 0) {
            direction.x /= dist;
            direction.y /= dist;
        }
        
        // Apply angle rotation
        float radAngle = angle * DEG2RAD;
        float cosA = cosf(radAngle);
        float sinA = sinf(radAngle);
        Vector2 rotatedDir = {
            direction.x * cosA - direction.y * sinA,
            direction.x * sinA + direction.y * cosA
        };
        
        Vector2 target = {
            enemy->position.x + rotatedDir.x * 1000,
            enemy->position.y + rotatedDir.y * 1000
        };
        
        // Find free projectile slot
        Projectile* projectiles = (Projectile*)game->projectiles;
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (!projectiles[i].active) {
                // Alternate between primary and secondary projectiles
                ProjectileType projType = (burst % 2 == 0) ? 
                    config->primaryProjectile : config->secondaryProjectile;
                
                InitializeProjectile(&projectiles[i], projType, 
                                   enemy->position, target, false);
                projectiles[i].enemyId = enemy->id;
                break;
            }
        }
    }
}

void UpdateEnemies(Game* game) {
    float deltaTime = GetFrameTime();
    
    // Update all active enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            EnemyEx* enemy = &game->enemies[i];
            
            // Update movement
            UpdateEnemyMovement(enemy, deltaTime);
            
            // Update firing timer
            enemy->fireTimer += deltaTime;
            
            // Check if enemy should fire
            const EnemyWeaponConfig* config = GetEnemyWeaponConfig(enemy->type);
            float fireInterval = 1.0f / config->fireRate;
            
            if (enemy->fireTimer >= fireInterval && enemy->can_fire) {
                // Only fire if enemy is on screen and facing player
                if (enemy->position.x > 0 && enemy->position.x < SCREEN_WIDTH &&
                    enemy->position.y > 0 && enemy->position.y < SCREEN_HEIGHT) {
                    
                    // Ghost enemies only fire when visible
                    if (enemy->type != ENEMY_GHOST || enemy->isVisible) {
                        FireEnemyProjectile(game, enemy);
                        enemy->fireTimer = 0;
                    }
                }
            }
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
    Projectile* projectiles = (Projectile*)game->projectiles;
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            UpdateProjectile(&projectiles[i], deltaTime);
            
            // Remove projectiles that go off screen
            Projectile* proj = &projectiles[i];
            if (proj->position.x < -100 || proj->position.x > SCREEN_WIDTH + 100 ||
                proj->position.y < -100 || proj->position.y > SCREEN_HEIGHT + 100) {
                proj->active = false;
            }
        }
    }
}

void UpdateGame(Game* game) {
    // Update music stream if loaded
    if (game->musicLoaded) {
        UpdateMusicStream(game->backgroundMusic);
    }
    
    // Handle pause toggle (P key only)
    if (IsKeyPressed(KEY_P)) {
        game->gamePaused = !game->gamePaused;
        // Pause/resume music based on game state
        if (game->musicLoaded) {
            if (game->gamePaused) {
                PauseMusicStream(game->backgroundMusic);
            } else {
                ResumeMusicStream(game->backgroundMusic);
            }
        }
    }
    
    if (!game->gameOver) {
        // Only update if not paused
        if (!game->gamePaused) {
            // Update game time
            float deltaTime = GetFrameTime();
            game->gameTime += deltaTime;
            
            // Update speed levels
            UpdateGameSpeed(game);
            
            // Update game components
            // Update new player ship
            UpdatePlayerShip(game->playerShip, deltaTime);
            
            // Player ship properties are used directly
            
            // Use original bullet system
            UpdateBullets(game);
            UpdateProjectiles(game);
            UpdateWaveSystem(game->waveSystem, game, deltaTime);
            UpdateEnemies(game);
            
            // Update explosion system
            UpdateExplosionSystem(game->explosionSystem, deltaTime);
            
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
            
            // Update score from player ship
            game->score = game->playerShip->score;
            
            // Boss escape sequence - 30 seconds before level end (523.82s)
            // DRAMATIC PHASED SEQUENCE
            if (!game->bossEscapeTriggered && game->gameTime >= 523.82f) {
                // Check if boss is still active
                if (game->bossEnemyIndex >= 0 && 
                    game->bossEnemyIndex < MAX_ENEMIES && 
                    game->enemies[game->bossEnemyIndex].active &&
                    game->enemies[game->bossEnemyIndex].type == ENEMY_BOSS) {
                    
                    game->bossEscapeTriggered = true;
                    game->bossEscapePhase = 1;  // Start destruction phase
                    game->bossEscapeTimer = 0.0f;
                    game->enemies[game->bossEnemyIndex].isEscaping = true;
                    
                    LogEvent(game, "[%.2f] BOSS DOOMSDAY - Escape sequence initiated!", 
                            game->gameTime, game->enemies[game->bossEnemyIndex].id);
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
            
            // Check if level time limit reached (9 minutes 13 seconds)
            if (game->gameTime >= 553.0f && !game->bossEscapeTriggered) {
                game->gameOver = true;
                strcpy(game->deathCause, "Victory! You survived the entire level!");
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
    
    // Cleanup audio
    if (game->musicLoaded) {
        StopMusicStream(game->backgroundMusic);
        UnloadMusicStream(game->backgroundMusic);
        game->musicLoaded = false;
    }
    CloseAudioDevice();
    
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

