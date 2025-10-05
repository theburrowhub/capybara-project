#include "wave_system.h"
#include "enemy_types.h"
#include "constants.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

void InitWaveSystem(WaveSystem* waveSystem) {
    // Initialize enemy types first
    InitEnemyTypes();
    
    // No predefined phases - tracking is based on spawn events
    waveSystem->phaseCount = 0;
    waveSystem->phases = NULL;
    waveSystem->currentPhase = 0;
    
    // Load static spawn events
    waveSystem->spawnEvents = CreateStaticWaveplan(&waveSystem->eventCount);
    waveSystem->nextEventIndex = 0;
    
    printf("[WAVE SYSTEM] Loaded %d static spawn events\n", waveSystem->eventCount);
    
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
    }
    
    // Initialize state with debug start time
    waveSystem->waveTimer = startTime;
    waveSystem->totalDuration = 553.82f; // Level duration from audio analysis
    waveSystem->isComplete = false;
    waveSystem->lastSpawnTime = startTime;
    waveSystem->totalEnemiesSpawned = 0;
    waveSystem->totalEnemiesKilled = 0;
    
    // Skip spawn events that are before the debug start time
    if (startTime > 0) {
        while (waveSystem->nextEventIndex < waveSystem->eventCount &&
               waveSystem->spawnEvents[waveSystem->nextEventIndex].time < startTime) {
            waveSystem->nextEventIndex++;
        }
        printf("[WAVE SYSTEM] DEBUG: Starting at phase %d (%.1f seconds), skipped %d events\n", 
               DEBUG_START_PHASE, startTime, waveSystem->nextEventIndex);
    }
    
    printf("[WAVE SYSTEM] Initialized STATIC wave system (%.1f seconds)\n", 
            waveSystem->totalDuration);
    printf("[WAVE SYSTEM] Total spawn events: %d\n", waveSystem->eventCount);
}

void UpdateWaveSystem(WaveSystem* waveSystem, struct Game* game, float deltaTime) {
    if (waveSystem->isComplete) return;
    
    // Update timer
    waveSystem->waveTimer += deltaTime;
    
    // Process all spawn events that should trigger now
    while (waveSystem->nextEventIndex < waveSystem->eventCount &&
           waveSystem->spawnEvents[waveSystem->nextEventIndex].time <= waveSystem->waveTimer) {
        
        SpawnEvent* event = &waveSystem->spawnEvents[waveSystem->nextEventIndex];
        
        // Handle spawn event based on count
        if (event->count == 1) {
            // Single enemy spawn
            SpawnWaveEnemy(game, event->type, event->x, event->y, event->pattern);
            waveSystem->totalEnemiesSpawned++;
            
            LogEvent(game, "[%.2f] Spawn: %s at (%.0f, %.0f)", 
                    waveSystem->waveTimer, GetEnemyTypeName(event->type),
                    event->x, event->y);
                    
        } else if (event->count > 1) {
            // Multiple enemies - spawn with vertical spacing
            float spacing = event->interval;
            float totalHeight = (event->count - 1) * spacing;
            float startY = event->y - (totalHeight / 2.0f);  // Center around specified Y
            
            for (int i = 0; i < event->count; i++) {
                float spawnY = startY + (i * spacing);
                
                // Clamp to screen bounds
                if (spawnY < 80) spawnY = 80;
                if (spawnY > SCREEN_HEIGHT - 80) spawnY = SCREEN_HEIGHT - 80;
                
                SpawnWaveEnemy(game, event->type, event->x, spawnY, event->pattern);
                waveSystem->totalEnemiesSpawned++;
            }
            
            LogEvent(game, "[%.2f] Spawn: %d x %s in formation", 
                    waveSystem->waveTimer, event->count, GetEnemyTypeName(event->type));
        }
        
        // Move to next event
        waveSystem->nextEventIndex++;
    }
    
    // Wave completion check
    if (waveSystem->waveTimer >= waveSystem->totalDuration) {
        waveSystem->isComplete = true;
        LogEvent(game, "[%.2f] Wave completed! Total enemies spawned: %d", 
                waveSystem->waveTimer, waveSystem->totalEnemiesSpawned);
    }
}

void SpawnWaveEnemy(struct Game* game, EnemyType type, float x, float y, const char* pattern) {
    // Find an inactive enemy slot
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) {
            InitializeEnemyFromType(&game->enemies[i], type, x, y);
            game->enemies[i].id = game->nextEnemyId++;
            ApplyMovementPattern(&game->enemies[i], pattern);
            
            // Enemies can fire after the warm-up period (first 55 seconds)
            game->enemies[i].can_fire = (game->waveSystem->waveTimer >= 55.0f);
            
            // Track boss enemy
            if (type == ENEMY_BOSS) {
                game->bossEnemyIndex = i;
                LogEvent(game, "[%.2f] BOSS spawned - ID:%d at index %d", 
                        game->gameTime, game->enemies[i].id, i);
            }
            
            LogEvent(game, "[%.2f] Enemy spawned - Type:%s ID:%d Pattern:%s Pos:(%.0f,%.0f)", 
                    game->gameTime, GetEnemyTypeName(type), game->enemies[i].id, 
                    pattern ? pattern : "default", x, y);
            break;
        }
    }
}

void ApplyMovementPattern(EnemyEx* enemy, const char* pattern) {
    if (!pattern) return;
    
    if (strcmp(pattern, "straight") == 0) {
        enemy->speedX = enemy->speed;
        enemy->speedY = 0;
    } else if (strcmp(pattern, "zigzag") == 0) {
        enemy->speedX = enemy->speed;
        enemy->speedY = sinf(enemy->position.x * 0.05f) * 3.0f;
    } else if (strcmp(pattern, "erratic") == 0) {
        enemy->speedX = enemy->speed * 0.8f;
        enemy->speedY = GetRandomValue(-3, 3);
    } else if (strcmp(pattern, "slow_advance") == 0) {
        enemy->speedX = enemy->speed * 0.5f;
        enemy->speedY = 0;
    } else if (strcmp(pattern, "v_formation") == 0) {
        enemy->speedX = enemy->speed * 1.2f;
        enemy->speedY = (enemy->position.y < PLAY_ZONE_HEIGHT/2) ? 1.0f : -1.0f;
    } else if (strcmp(pattern, "hover") == 0) {
        enemy->speedX = enemy->speed * 0.3f;
        enemy->speedY = 0;
        enemy->targetY = enemy->position.y;
    } else if (strcmp(pattern, "horizontal") == 0) {
        enemy->speedX = enemy->speed * 0.7f;
        enemy->speedY = 0;
    } else if (strcmp(pattern, "phasing") == 0) {
        enemy->speedX = enemy->speed * 0.9f;
        enemy->speedY = GetRandomValue(-2, 2);
        enemy->specialTimer = 0;
    } else if (strcmp(pattern, "formation") == 0) {
        enemy->speedX = enemy->speed * 0.6f;
        enemy->speedY = 0;
    } else if (strcmp(pattern, "flanking") == 0) {
        enemy->speedX = enemy->speed * 1.5f;
        enemy->speedY = (enemy->position.y < PLAY_ZONE_HEIGHT/2) ? 2.0f : -2.0f;
    } else if (strcmp(pattern, "boss") == 0) {
        enemy->speedX = enemy->speed * 0.4f;
        enemy->speedY = 0;
        enemy->targetY = PLAY_ZONE_HEIGHT/2;
    } else if (strcmp(pattern, "minion") == 0) {
        enemy->speedX = enemy->speed * 1.2f;
        enemy->speedY = GetRandomValue(-2, 2);
    } else if (strcmp(pattern, "rush") == 0) {
        enemy->speedX = enemy->speed * 2.0f;
        enemy->speedY = GetRandomValue(-1, 1);
    } else if (strcmp(pattern, "tank_assault") == 0) {
        // Tank assault pattern: advance, pause, retreat
        // Each tank has different speed and pause timing
        float speedVariation = 0.5f + (GetRandomValue(0, 50) / 100.0f);  // 0.5x to 1.0x speed
        enemy->speedX = enemy->speed * speedVariation;  // Speed for advancing (positive value)
        enemy->speedY = 0;
        
        // Set random stop position (not reaching last left third of screen)
        // Screen is 1200 wide, last third is 400, so stop between 400 and 900
        float stopX = GetRandomValue(450, 900);
        enemy->targetY = stopX;  // Using targetY to store stop X position
        
        // Set random pause duration (5-10 seconds)
        enemy->specialTimer = 5.0f + (GetRandomValue(0, 50) / 10.0f);  // 5.0 to 10.0 seconds
        
        // Use moveTimer to track state: 0=advancing, 1=paused, 2=retreating
        enemy->moveTimer = 0;  // Start in advancing state
    } else if (strcmp(pattern, "elite_retreat") == 0) {
        // Elite retreat pattern: advance to center, pause briefly, retreat
        enemy->speedX = enemy->speed * 1.2f;  // Slightly faster than normal
        enemy->speedY = 0;
        
        // Set stop position at screen center
        float stopX = SCREEN_WIDTH / 2;  // Center of screen
        enemy->targetY = stopX;  // Using targetY to store stop X position
        
        // Set pause duration (3-5 seconds, shorter than tanks)
        enemy->specialTimer = 3.0f + (GetRandomValue(0, 20) / 10.0f);  // 3.0 to 5.0 seconds
        
        // Use moveTimer to track state: 0=advancing, 1=paused, 2=retreating
        enemy->moveTimer = 0;  // Start in advancing state
    }
}

void UpdateEnemyMovement(EnemyEx* enemy, float deltaTime) {
    if (!enemy->active) return;
    
    // Update animation timer
    enemy->animationTimer += deltaTime;
    
    // Special behaviors based on type
    switch (enemy->type) {
        case ENEMY_ZIGZAG:
            // Zigzag pattern
            enemy->speedY = sinf(enemy->animationTimer * 3.0f) * 4.0f;
            // Apply default movement
            enemy->position.x -= enemy->speedX;
            enemy->position.y += enemy->speedY;
            break;
            
        case ENEMY_GHOST:
            // Phase in and out
            enemy->specialTimer += deltaTime;
            if (enemy->specialTimer >= 2.0f) {
                enemy->isVisible = !enemy->isVisible;
                enemy->specialTimer = 0;
            }
            // Apply default movement
            enemy->position.x -= enemy->speedX;
            enemy->position.y += enemy->speedY;
            break;
            
        case ENEMY_SHIELD:
            // Rotate shield
            enemy->shieldAngle += 60.0f * deltaTime;
            if (enemy->shieldAngle >= 360.0f) enemy->shieldAngle -= 360.0f;
            // Apply default movement
            enemy->position.x -= enemy->speedX;
            enemy->position.y += enemy->speedY;
            break;
            
        case ENEMY_SPEEDER:
            // Extra fast with slight wobble
            enemy->position.x -= enemy->speedX;
            enemy->position.y += enemy->speedY;
            enemy->position.y += sinf(enemy->animationTimer * 10.0f) * 1.5f;
            break;
            
        case ENEMY_SWARM:
            // Slight circular motion
            enemy->position.x -= enemy->speedX;
            enemy->position.y += enemy->speedY;
            enemy->position.y += sinf(enemy->animationTimer * 5.0f + enemy->id) * 2.0f;
            break;
            
        case ENEMY_TANK:
            // Tank assault pattern: advance, pause, retreat
            // moveTimer: 0=advancing, 1=paused, 2=retreating, 3+=normal movement
            if (enemy->moveTimer < 3.0f) {  // Tank assault behavior active
                if (enemy->moveTimer == 0) {
                    // State 0: Advancing to stop position (moving LEFT toward player)
                    // targetY stores the stop X position
                    // Move the tank left (subtract from X)
                    enemy->position.x -= enemy->speedX;
                    
                    if (enemy->position.x <= enemy->targetY) {
                        // Reached stop position, transition to pause
                        enemy->position.x = enemy->targetY;  // Snap to exact position
                        enemy->speedX = 0;
                        enemy->speedY = 0;
                        enemy->moveTimer = 1;  // Transition to paused state
                        // specialTimer already set to pause duration (5-10 seconds)
                    }
                } else if (enemy->moveTimer == 1) {
                    // State 1: Paused - countdown pause timer
                    // DO NOT MOVE - stay at current position
                    enemy->specialTimer -= deltaTime;
                    if (enemy->specialTimer <= 0) {
                        // Pause finished, start retreating
                        enemy->speedX = enemy->speed;  // Set speed for retreating
                        enemy->speedY = 0;
                        enemy->moveTimer = 2;  // Transition to retreating state
                    }
                } else if (enemy->moveTimer == 2) {
                    // State 2: Retreating back to right edge (moving RIGHT away from player)
                    // Move the tank right (add to X)
                    enemy->position.x += enemy->speedX;
                    
                    if (enemy->position.x >= SCREEN_WIDTH + 100) {
                        // Reached right edge, deactivate
                        enemy->active = false;
                    }
                }
            } else {
                // Normal tank movement (for non-assault patterns)
                enemy->position.x -= enemy->speedX;
                enemy->position.y += enemy->speedY;
            }
            break;
        
        case ENEMY_ELITE:
            // Elite retreat pattern: advance to center, pause, retreat
            // moveTimer: 0=advancing, 1=paused, 2=retreating, 3+=normal movement
            if (enemy->moveTimer < 3.0f) {  // Elite retreat behavior active
                if (enemy->moveTimer == 0) {
                    // State 0: Advancing to screen center
                    enemy->position.x -= enemy->speedX;
                    
                    if (enemy->position.x <= enemy->targetY) {
                        // Reached center, transition to pause
                        enemy->position.x = enemy->targetY;
                        enemy->speedX = 0;
                        enemy->speedY = 0;
                        enemy->moveTimer = 1;
                        // specialTimer already set to pause duration (3-5 seconds)
                    }
                } else if (enemy->moveTimer == 1) {
                    // State 1: Paused at center
                    enemy->specialTimer -= deltaTime;
                    if (enemy->specialTimer <= 0) {
                        // Pause finished, start retreating
                        enemy->speedX = enemy->speed * 1.3f;  // Slightly faster retreat
                        enemy->speedY = 0;
                        enemy->moveTimer = 2;
                    }
                } else if (enemy->moveTimer == 2) {
                    // State 2: Retreating back to right edge
                    enemy->position.x += enemy->speedX;
                    
                    if (enemy->position.x >= SCREEN_WIDTH + 100) {
                        // Reached right edge, deactivate
                        enemy->active = false;
                    }
                }
            } else {
                // Normal elite movement
                enemy->position.x -= enemy->speedX;
                enemy->position.y += enemy->speedY;
            }
            break;
        
        case ENEMY_BOSS:
            {
                // Check for escape mode - boss is fleeing
                if (enemy->isEscaping) {
                    // Move rapidly to the right to escape off screen
                    enemy->position.x += 8.0f;  // Fast escape speed
                    
                    // Move towards center Y for dramatic exit
                    float centerY = PLAY_ZONE_HEIGHT / 2;
                    float diff = centerY - enemy->position.y;
                    if (fabs(diff) > 5.0f) {
                        enemy->position.y += (diff > 0 ? 1 : -1) * 3.0f;
                    }
                    
                    // Deactivate boss once off screen
                    if (enemy->position.x > SCREEN_WIDTH + 200) {
                        enemy->active = false;
                    }
                    break;  // Exit switch, skip normal boss behavior
                }
                
                // Define the last third boundary (right side of screen where boss enters)
                float lastThirdStart = (SCREEN_WIDTH * 2) / 3.0f;  // 800 pixels (start of last third)
                float minBossX = lastThirdStart + 20.0f;  // 820 pixels (stay in last third)
                float maxBossX = SCREEN_WIDTH - 80.0f;  // 1120 pixels (don't go off right edge)
                
                // State 0: Boss entering - move into last third zone
                if (enemy->moveTimer == 0) {
                    enemy->position.x -= enemy->speedX;
                    
                    // Once in the last third, switch to hovering mode
                    if (enemy->position.x <= maxBossX) {
                        enemy->moveTimer = 1;  // Switch to hovering state
                        enemy->specialTimer = 0;  // Reset for movement timer
                        // Set initial target in safe zone (play zone only)
                        enemy->targetY = GetRandomValue(PLAY_ZONE_TOP + 100, PLAY_ZONE_BOTTOM - 100);
                        enemy->speedX = GetRandomValue(minBossX + 30, maxBossX - 30);  // Initial target X
                    }
                } else {
                    // State 1: Boss hovering in last third with controlled random movement
                    
                    // Update movement timer for random direction changes
                    enemy->specialTimer += deltaTime;
                    
                    // Change direction every 2-3 seconds for smooth, non-chaotic movement
                    if (enemy->specialTimer >= 2.5f) {
                        enemy->specialTimer = 0;
                        
                        // Set new random target within play zone bounds
                        enemy->targetY = GetRandomValue(PLAY_ZONE_TOP + 100, PLAY_ZONE_BOTTOM - 100);
                        // speedX repurposed for horizontal target position in this mode
                        enemy->speedX = GetRandomValue(minBossX, maxBossX);
                    }
                    
                    // Smooth movement toward target Y (up/down)
                    float verticalSpeed = 1.5f;
                    if (fabs(enemy->position.y - enemy->targetY) > 5.0f) {
                        float diff = enemy->targetY - enemy->position.y;
                        enemy->position.y += (diff > 0 ? 1 : -1) * verticalSpeed;
                    }
                    
                    // Smooth movement toward target X (left/right within safe zone)
                    float horizontalSpeed = 1.2f;
                    float targetX = enemy->speedX;  // Using speedX as target storage
                    if (fabs(enemy->position.x - targetX) > 5.0f) {
                        float diff = targetX - enemy->position.x;
                        enemy->position.x += (diff > 0 ? 1 : -1) * horizontalSpeed;
                    }
                    
                    // Add subtle floating motion for visual interest
                    enemy->position.y += sinf(enemy->animationTimer * 1.5f) * 0.8f;
                    enemy->position.x += cosf(enemy->animationTimer * 1.2f) * 0.5f;
                    
                    // Enforce boundaries strictly - keep boss in safe zone
                    if (enemy->position.x < minBossX) {
                        enemy->position.x = minBossX;
                    }
                    if (enemy->position.x > maxBossX) {
                        enemy->position.x = maxBossX;
                    }
                }
            }
            break;
            
        default:
            // Basic movement for others
            enemy->position.x -= enemy->speedX;
            enemy->position.y += enemy->speedY;
            
            if (enemy->targetY != 0 && fabs(enemy->position.y - enemy->targetY) > 10.0f) {
                float diff = enemy->targetY - enemy->position.y;
                enemy->position.y += (diff > 0 ? 1 : -1) * 1.5f;
            }
            break;
    }
    
    // Keep enemies in bounds (within play zone, above HUD)
    if (enemy->position.y < PLAY_ZONE_TOP + enemy->radius) {
        enemy->position.y = PLAY_ZONE_TOP + enemy->radius;
        enemy->speedY = fabs(enemy->speedY);
    }
    if (enemy->position.y > PLAY_ZONE_BOTTOM - enemy->radius) {
        enemy->position.y = PLAY_ZONE_BOTTOM - enemy->radius;
        enemy->speedY = -fabs(enemy->speedY);
    }
    
    // Update bounds
    enemy->bounds.x = enemy->position.x - enemy->radius;
    enemy->bounds.y = enemy->position.y - enemy->radius;
    enemy->bounds.width = enemy->radius * 2;
    enemy->bounds.height = enemy->radius * 2;
    
    // Deactivate if off screen
    if (enemy->position.x < -100) {
        enemy->active = false;
    }
}

void CleanupWaveSystem(WaveSystem* waveSystem) {
    if (waveSystem->phases) {
        free(waveSystem->phases);
        waveSystem->phases = NULL;
    }
    // DON'T free spawnEvents - it's a static array from CreateStaticWaveplan
    // Static arrays are not heap-allocated and shouldn't be freed
    waveSystem->spawnEvents = NULL;
}

const char* GetCurrentPhaseName(const WaveSystem* waveSystem) {
    // Return phase name based on current time
    float time = waveSystem->waveTimer;
    
    if (time < 55.0f) {
        return "Warm-Up";
    } else if (time >= 427.0f && time <= 451.0f) {
        return "BOSS BATTLE";
    } else if (time < 90.0f) {
        return "Opening Assault";
    } else if (time < 150.0f) {
        return "Building Intensity";
    } else if (time < 260.0f) {
        return "Mixed Combat";
    } else if (time < 345.0f) {
        return "Heavy Assault";
    } else if (time < 420.0f) {
        return "Elite Encounters";
    } else if (time < 510.0f) {
        return "Final Push";
    } else {
        return "Victory Lap";
    }
}

float GetWaveProgress(const WaveSystem* waveSystem) {
    return (waveSystem->waveTimer / waveSystem->totalDuration) * 100.0f;
}

bool IsWaveComplete(const WaveSystem* waveSystem) {
    return waveSystem->isComplete;
}

int GetCurrentPhaseNumber(const WaveSystem* waveSystem) {
    // Return the phase number based on current time
    // This maps to DEBUG_START_PHASE values
    float time = waveSystem->waveTimer;
    
    if (time < 5.0f) return 1;
    else if (time < 55.0f) return 2;
    else if (time < 90.0f) return 3;
    else if (time < 110.0f) return 4;  // Tank assault
    else if (time < 125.0f) return 5;  // Recovery
    else if (time < 150.0f) return 6;  // Swarm attack
    else if (time < 175.0f) return 7;  // Mixed
    else if (time < 195.0f) return 8;  // Recovery
    else if (time < 225.0f) return 9;  // Speeders
    else if (time < 240.0f) return 10; // Elite
    else if (time < 260.0f) return 11; // Recovery
    else if (time < 295.0f) return 12; // Shield/Recovery
    else if (time < 320.0f) return 13; // Tank #2
    else if (time < 345.0f) return 14; // Recovery
    else if (time < 380.0f) return 15; // Bombers
    else if (time < 415.0f) return 16; // Elite #2/Recovery
    else if (time < 427.0f) return 17; // Pre-boss
    else if (time < 455.0f) return 18; // BOSS
    else if (time < 480.0f) return 19; // Recovery
    else if (time < 510.0f) return 20; // Final challenge
    else return 21; // Victory lap
}

