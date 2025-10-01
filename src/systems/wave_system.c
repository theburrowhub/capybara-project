#include "wave_system.h"
#include "enemy_types.h"
#include "constants.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// Helper function to create spawn events
static void AddSpawnEvent(SpawnEvent* events, int* count, float time, EnemyType type, 
                          float x, float y, int num, float interval, const char* pattern) {
    events[*count] = (SpawnEvent){
        .time = time,
        .type = type,
        .x = x,
        .y = y,
        .count = num,
        .interval = interval,
        .pattern = pattern
    };
    (*count)++;
}

void InitWaveSystem(WaveSystem* waveSystem) {
    // Initialize enemy types first
    InitEnemyTypes();
    
    // Setup phases based on Flight Plan
    waveSystem->phaseCount = 17;
    waveSystem->phases = (WavePhase*)malloc(sizeof(WavePhase) * waveSystem->phaseCount);
    
    // Phase definitions from Flight Plan
    waveSystem->phases[0] = (WavePhase){0, "Warm-Up", 0, 5, 5, false, false};
    waveSystem->phases[1] = (WavePhase){1, "First Wave", 5, 35, 30, false, false};
    waveSystem->phases[2] = (WavePhase){2, "Tank Squadron", 35, 55, 20, false, false};
    waveSystem->phases[3] = (WavePhase){3, "Swarm Attack", 55, 85, 30, false, false};
    waveSystem->phases[4] = (WavePhase){4, "Mixed Assault", 85, 120, 35, false, false};
    waveSystem->phases[5] = (WavePhase){5, "Elite Squadron", 120, 150, 30, false, false};
    waveSystem->phases[6] = (WavePhase){6, "Zigzag Chaos", 150, 180, 30, false, false};
    waveSystem->phases[7] = (WavePhase){7, "Shield Wall", 180, 220, 40, false, false};
    waveSystem->phases[8] = (WavePhase){8, "Bomber Run", 220, 260, 40, false, false};
    waveSystem->phases[9] = (WavePhase){9, "Ghost Ambush", 260, 300, 40, false, false};
    waveSystem->phases[10] = (WavePhase){10, "Combined Arms", 300, 345, 45, false, false};
    waveSystem->phases[11] = (WavePhase){11, "Mini-Boss", 345, 390, 45, false, false};
    waveSystem->phases[12] = (WavePhase){12, "Recovery", 390, 420, 30, false, false};
    waveSystem->phases[13] = (WavePhase){13, "Elite & Shield", 420, 460, 40, false, false};
    waveSystem->phases[14] = (WavePhase){14, "Evasive Maneuvers", 460, 500, 40, false, false};
    waveSystem->phases[15] = (WavePhase){15, "Heavy Assault", 500, 540, 40, false, false};
    waveSystem->phases[16] = (WavePhase){16, "Final Wave", 540, 590, 50, false, false};
    
    // Allocate spawn events (estimate ~500 events for all phases)
    waveSystem->spawnEvents = (SpawnEvent*)malloc(sizeof(SpawnEvent) * 500);
    waveSystem->eventCount = 0;
    
    // Phase 1: Warm-Up (0-5 seconds) - No enemies
    
    // Phase 2: First Wave (5-35 seconds) - 40 Grunts in 5 lines
    for (int line = 0; line < 5; line++) {
        for (int i = 0; i < 8; i++) {
            float spawnTime = 5.0f + line * 5.0f + i * 0.5f;
            float y = 100 + line * 120;
            AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                         spawnTime, ENEMY_GRUNT, SCREEN_WIDTH + 50, y, 1, 0, "straight");
        }
    }
    
    // Phase 3: Tank Squadron (35-55 seconds) - 5 Tanks
    for (int i = 0; i < 5; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     35.0f + i * 3.0f, ENEMY_TANK, SCREEN_WIDTH + 50, 
                     150 + i * 100, 1, 0, "slow_advance");
    }
    
    // Phase 4: Swarm Attack (55-85 seconds) - 30 Swarm in groups
    for (int group = 0; group < 5; group++) {
        float groupTime = 55.0f + group * 5.0f;
        float startY = (group % 2 == 0) ? 100 : SCREEN_HEIGHT - 100;
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     groupTime, ENEMY_SWARM, SCREEN_WIDTH + 50, startY, 
                     6, 0.2f, "v_formation");
    }
    
    // Phase 5: Mixed Assault (85-120 seconds) - Speeders and Grunts
    for (int i = 0; i < 10; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     85.0f + i * 3.0f, ENEMY_SPEEDER, SCREEN_WIDTH + 50,
                     GetRandomValue(100, SCREEN_HEIGHT - 100), 1, 0, "zigzag");
    }
    for (int i = 0; i < 15; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     90.0f + i * 2.0f, ENEMY_GRUNT, SCREEN_WIDTH + 50,
                     GetRandomValue(100, SCREEN_HEIGHT - 100), 1, 0, "straight");
    }
    
    // Phase 6: Elite Squadron (120-150 seconds) - 3 Elites
    AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                 120.0f, ENEMY_ELITE, SCREEN_WIDTH + 50, SCREEN_HEIGHT/2 - 100, 1, 0, "hover");
    AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                 120.0f, ENEMY_ELITE, SCREEN_WIDTH + 50, SCREEN_HEIGHT/2, 1, 0, "hover");
    AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                 120.0f, ENEMY_ELITE, SCREEN_WIDTH + 50, SCREEN_HEIGHT/2 + 100, 1, 0, "hover");
    
    // Phase 7: Zigzag Chaos (150-180 seconds) - 20 Zigzag enemies
    for (int i = 0; i < 20; i++) {
        float side = GetRandomValue(0, 3);
        float x = SCREEN_WIDTH + 50;
        float y = GetRandomValue(50, SCREEN_HEIGHT - 50);
        
        if (side == 1) { // Top
            x = GetRandomValue(100, SCREEN_WIDTH - 100);
            y = -50;
        } else if (side == 2) { // Bottom
            x = GetRandomValue(100, SCREEN_WIDTH - 100);
            y = SCREEN_HEIGHT + 50;
        }
        
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     150.0f + i * 1.5f, ENEMY_ZIGZAG, x, y, 1, 0, "erratic");
    }
    
    // Phase 8: Shield Wall (180-220 seconds) - 8 Shield enemies
    for (int i = 0; i < 8; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     180.0f + (i/4) * 10.0f, ENEMY_SHIELD, SCREEN_WIDTH + 50,
                     200 + (i % 4) * 80, 1, 0, "formation");
    }
    
    // Phase 9: Bomber Run (220-260 seconds) - 6 Bombers
    for (int i = 0; i < 6; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     220.0f + i * 6.5f, ENEMY_BOMBER, SCREEN_WIDTH + 50,
                     100 + (i % 3) * 200, 1, 0, "horizontal");
    }
    
    // Phase 10: Ghost Ambush (260-300 seconds) - 12 Ghosts
    for (int i = 0; i < 12; i++) {
        float x = (i % 2 == 0) ? SCREEN_WIDTH + 50 : GetRandomValue(100, SCREEN_WIDTH/2);
        float y = GetRandomValue(100, SCREEN_HEIGHT - 100);
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     260.0f + i * 3.0f, ENEMY_GHOST, x, y, 1, 0, "phasing");
    }
    
    // Phase 11: Combined Arms (300-345 seconds)
    // Tanks
    for (int i = 0; i < 5; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     300.0f + i * 4.0f, ENEMY_TANK, SCREEN_WIDTH + 50,
                     200 + i * 60, 1, 0, "front_line");
    }
    // Speeders
    for (int i = 0; i < 10; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     305.0f + i * 2.5f, ENEMY_SPEEDER, SCREEN_WIDTH + 50,
                     GetRandomValue(50, SCREEN_HEIGHT - 50), 1, 0, "flanking");
    }
    // Grunts
    for (int i = 0; i < 15; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     310.0f + i * 2.0f, ENEMY_GRUNT, SCREEN_WIDTH + 100,
                     GetRandomValue(100, SCREEN_HEIGHT - 100), 1, 0, "support");
    }
    
    // Phase 12: Mini-Boss (345-390 seconds)
    AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                 345.0f, ENEMY_BOSS, SCREEN_WIDTH + 100, SCREEN_HEIGHT/2, 1, 0, "boss");
    // Boss summons grunts every 10 seconds
    for (int i = 0; i < 4; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     355.0f + i * 10.0f, ENEMY_GRUNT, SCREEN_WIDTH + 50,
                     SCREEN_HEIGHT/2 + GetRandomValue(-100, 100), 3, 1.0f, "minion");
    }
    
    // Phase 13: Recovery (390-420 seconds) - 20 Swarm
    for (int i = 0; i < 20; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     390.0f + i * 1.5f, ENEMY_SWARM, SCREEN_WIDTH + 50,
                     GetRandomValue(100, SCREEN_HEIGHT - 100), 1, 0, "simple");
    }
    
    // Phase 14: Elite & Shield Combo (420-460 seconds)
    // Shields first
    for (int i = 0; i < 6; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     420.0f, ENEMY_SHIELD, SCREEN_WIDTH + 50,
                     150 + i * 80, 1, 0, "barrier");
    }
    // Then Elites
    for (int i = 0; i < 4; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     425.0f, ENEMY_ELITE, SCREEN_WIDTH + 150,
                     200 + i * 100, 1, 0, "protected");
    }
    
    // Phase 15: Evasive Maneuvers (460-500 seconds)
    // Zigzag enemies
    for (int i = 0; i < 10; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     460.0f + i * 3.0f, ENEMY_ZIGZAG, SCREEN_WIDTH + 50,
                     GetRandomValue(50, SCREEN_HEIGHT - 50), 1, 0, "erratic");
    }
    // Ghost enemies
    for (int i = 0; i < 8; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     465.0f + i * 4.0f, ENEMY_GHOST, SCREEN_WIDTH + 50,
                     GetRandomValue(100, SCREEN_HEIGHT - 100), 1, 0, "phasing");
    }
    
    // Phase 16: Heavy Assault (500-540 seconds)
    // Bombers
    for (int i = 0; i < 4; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     500.0f + i * 8.0f, ENEMY_BOMBER, SCREEN_WIDTH + 50,
                     150 + i * 120, 1, 0, "bombing_run");
    }
    // Tanks
    for (int i = 0; i < 6; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     505.0f + i * 5.0f, ENEMY_TANK, SCREEN_WIDTH + 50,
                     100 + i * 80, 1, 0, "suppressing");
    }
    
    // Phase 17: Final Wave (540-590 seconds)
    // 0:00-0:10: 15 Grunts
    for (int i = 0; i < 15; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     540.0f + i * 0.6f, ENEMY_GRUNT, SCREEN_WIDTH + 50,
                     GetRandomValue(50, SCREEN_HEIGHT - 50), 1, 0, "straight");
    }
    // 0:10-0:20: 10 Speeders + 5 Zigzags
    for (int i = 0; i < 10; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     550.0f + i * 0.8f, ENEMY_SPEEDER, SCREEN_WIDTH + 50,
                     GetRandomValue(50, SCREEN_HEIGHT - 50), 1, 0, "fast");
    }
    for (int i = 0; i < 5; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     552.0f + i * 1.5f, ENEMY_ZIGZAG, SCREEN_WIDTH + 50,
                     GetRandomValue(100, SCREEN_HEIGHT - 100), 1, 0, "erratic");
    }
    // 0:20-0:30: 4 Tanks + 4 Shields
    for (int i = 0; i < 4; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     560.0f + i * 2.0f, ENEMY_TANK, SCREEN_WIDTH + 50,
                     150 + i * 100, 1, 0, "heavy");
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     561.0f + i * 2.0f, ENEMY_SHIELD, SCREEN_WIDTH + 100,
                     200 + i * 100, 1, 0, "formation");
    }
    // 0:30-0:40: 3 Elites + 3 Bombers + 6 Ghosts
    for (int i = 0; i < 3; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     570.0f + i * 2.0f, ENEMY_ELITE, SCREEN_WIDTH + 50,
                     200 + i * 150, 1, 0, "elite");
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     571.0f + i * 2.0f, ENEMY_BOMBER, SCREEN_WIDTH + 100,
                     150 + i * 150, 1, 0, "bombing");
    }
    for (int i = 0; i < 6; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     572.0f + i * 1.5f, ENEMY_GHOST, SCREEN_WIDTH + 50,
                     GetRandomValue(50, SCREEN_HEIGHT - 50), 1, 0, "phasing");
    }
    // 0:40-0:50: 20 Swarm (final rush)
    for (int i = 0; i < 20; i++) {
        AddSpawnEvent(waveSystem->spawnEvents, &waveSystem->eventCount,
                     580.0f + i * 0.4f, ENEMY_SWARM, SCREEN_WIDTH + 50,
                     GetRandomValue(50, SCREEN_HEIGHT - 50), 1, 0, "rush");
    }
    
    // Initialize state
    waveSystem->currentPhase = 0;
    waveSystem->nextEventIndex = 0;
    waveSystem->waveTimer = 0;
    waveSystem->totalDuration = 600.0f; // 10 minutes
    waveSystem->isComplete = false;
    waveSystem->lastSpawnTime = 0;
    waveSystem->totalEnemiesSpawned = 0;
    waveSystem->totalEnemiesKilled = 0;
    
    // Debug: Start at specific phase if requested
    if (DEBUG_START_PHASE > 0 && DEBUG_START_PHASE <= waveSystem->phaseCount) {
        // DEBUG_START_PHASE uses 1-based indexing for user convenience
        // But internally phases are 0-indexed, so we need to subtract 1
        int targetPhaseIndex = DEBUG_START_PHASE - 1;
        
        // Validate the phase index
        if (targetPhaseIndex >= 0 && targetPhaseIndex < waveSystem->phaseCount) {
            // Mark all previous phases as completed
            for (int i = 0; i < targetPhaseIndex; i++) {
                waveSystem->phases[i].completed = true;
                waveSystem->phases[i].active = false;
            }
            
            // Set the wave timer to the start of the target phase
            waveSystem->waveTimer = waveSystem->phases[targetPhaseIndex].startTime;
            waveSystem->currentPhase = targetPhaseIndex;
            
            // Skip spawn events that occur before this phase
            while (waveSystem->nextEventIndex < waveSystem->eventCount && 
                   waveSystem->spawnEvents[waveSystem->nextEventIndex].time < waveSystem->waveTimer) {
                waveSystem->nextEventIndex++;
            }
            
            printf("[DEBUG] Starting at phase %d: %s (time: %.1f)\n", 
                   DEBUG_START_PHASE, waveSystem->phases[targetPhaseIndex].name, waveSystem->waveTimer);
        }
    }
}

void UpdateWaveSystem(WaveSystem* waveSystem, struct Game* game, float deltaTime) {
    if (waveSystem->isComplete) return;
    
    // Update timer
    waveSystem->waveTimer += deltaTime;
    
    // Check for phase transitions
    for (int i = 0; i < waveSystem->phaseCount; i++) {
        WavePhase* phase = &waveSystem->phases[i];
        
        if (waveSystem->waveTimer >= phase->startTime && !phase->active && !phase->completed) {
            // Activate phase
            phase->active = true;
            waveSystem->currentPhase = i;
            LogEvent(game, "[%.2f] Phase %d started: %s", 
                    waveSystem->waveTimer, i + 1, phase->name);
        }
        
        if (waveSystem->waveTimer >= phase->endTime && phase->active && !phase->completed) {
            // Complete phase
            phase->active = false;
            phase->completed = true;
            LogEvent(game, "[%.2f] Phase %d completed: %s", 
                    waveSystem->waveTimer, i + 1, phase->name);
        }
    }
    
    // Process spawn events
    static int eventSpawnCounts[500] = {0}; // Track how many enemies spawned per event
    
    while (waveSystem->nextEventIndex < waveSystem->eventCount) {
        SpawnEvent* event = &waveSystem->spawnEvents[waveSystem->nextEventIndex];
        int eventIdx = waveSystem->nextEventIndex;
        
        // Check if it's time to process this event
        if (waveSystem->waveTimer < event->time) {
            break; // Not time for this event yet
        }
        
        // Check how many enemies we've spawned for this event
        int spawnedCount = eventSpawnCounts[eventIdx];
        
        // If all enemies have been spawned, move to next event
        if (spawnedCount >= event->count) {
            waveSystem->nextEventIndex++;
            continue;
        }
        
        // Calculate when the next enemy should spawn
        float nextSpawnTime = event->time + spawnedCount * event->interval;
        
        // Check if it's time to spawn the next enemy
        if (waveSystem->waveTimer >= nextSpawnTime) {
            // Calculate Y position for this enemy
            float y = event->y;
            if (event->count > 1) {
                y = event->y + spawnedCount * 30;
            }
            
            // Spawn the enemy
            SpawnWaveEnemy(game, event->type, event->x, y, event->pattern);
            waveSystem->totalEnemiesSpawned++;
            eventSpawnCounts[eventIdx]++;
            
            // If this was the last enemy for this event, move to next event
            if (eventSpawnCounts[eventIdx] >= event->count) {
                waveSystem->nextEventIndex++;
            }
        }
        
        // Don't process next event until this one is done
        break;
    }
    
    // Check for wave completion
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
            
            // Check if this is the first wave (Phase 2) with Grunt enemies
            // According to flight plan: "No firing, passive movement only"
            // Use wave timer instead of game time to handle debug phase starts correctly
            WaveSystem* ws = game->waveSystem;
            if (ws && ws->waveTimer >= 5.0f && ws->waveTimer <= 35.0f && type == ENEMY_GRUNT) {
                game->enemies[i].can_fire = false;
            }
            
            LogEvent(game, "[%.2f] Enemy spawned - Type:%s ID:%d Pattern:%s Pos:(%.0f,%.0f) CanFire:%s", 
                    game->gameTime, GetEnemyTypeName(type), game->enemies[i].id, 
                    pattern ? pattern : "default", x, y, 
                    game->enemies[i].can_fire ? "Yes" : "No");
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
        enemy->speedY = (enemy->position.y < SCREEN_HEIGHT/2) ? 1.0f : -1.0f;
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
        enemy->speedY = (enemy->position.y < SCREEN_HEIGHT/2) ? 2.0f : -2.0f;
    } else if (strcmp(pattern, "boss") == 0) {
        enemy->speedX = enemy->speed * 0.4f;
        enemy->speedY = 0;
        enemy->targetY = SCREEN_HEIGHT/2;
    } else if (strcmp(pattern, "rush") == 0) {
        enemy->speedX = enemy->speed * 2.0f;
        enemy->speedY = GetRandomValue(-1, 1);
    }
}

void UpdateEnemyMovement(EnemyEx* enemy, float deltaTime) {
    if (!enemy->active) return;
    
    // Update position
    enemy->position.x -= enemy->speedX;
    enemy->position.y += enemy->speedY;
    
    // Update animation timer
    enemy->animationTimer += deltaTime;
    
    // Special behaviors based on type
    switch (enemy->type) {
        case ENEMY_ZIGZAG:
            // Zigzag pattern
            enemy->speedY = sinf(enemy->animationTimer * 3.0f) * 4.0f;
            break;
            
        case ENEMY_GHOST:
            // Phase in and out
            enemy->specialTimer += deltaTime;
            if (enemy->specialTimer >= 2.0f) {
                enemy->isVisible = !enemy->isVisible;
                enemy->specialTimer = 0;
            }
            break;
            
        case ENEMY_SHIELD:
            // Rotate shield
            enemy->shieldAngle += 60.0f * deltaTime;
            if (enemy->shieldAngle >= 360.0f) enemy->shieldAngle -= 360.0f;
            break;
            
        case ENEMY_SPEEDER:
            // Extra fast with slight wobble
            enemy->position.y += sinf(enemy->animationTimer * 10.0f) * 1.5f;
            break;
            
        case ENEMY_SWARM:
            // Slight circular motion
            enemy->position.y += sinf(enemy->animationTimer * 5.0f + enemy->id) * 2.0f;
            break;
            
        case ENEMY_BOSS:
            // Hover at target position with slight movement
            if (fabs(enemy->position.y - enemy->targetY) > 5.0f) {
                float diff = enemy->targetY - enemy->position.y;
                enemy->position.y += (diff > 0 ? 1 : -1) * 2.0f;
            } else {
                enemy->position.y += sinf(enemy->animationTimer * 2.0f) * 2.0f;
            }
            
            // Shield regeneration logic
            if (enemy->shieldAngle <= 0) {
                enemy->specialTimer += deltaTime;
                if (enemy->specialTimer >= BOSS_SHIELD_REGEN_TIME) {
                    enemy->shieldAngle = BOSS_SHIELD_HEALTH;
                    enemy->specialTimer = 0;
                    // Log shield regeneration (assuming LogEvent is available)
                    // LogEvent(game, "[%.2f] Boss shield REGENERATED!", game->gameTime);
                }
            }
            break;
            
        default:
            // Basic movement for others
            if (enemy->targetY != 0 && fabs(enemy->position.y - enemy->targetY) > 10.0f) {
                float diff = enemy->targetY - enemy->position.y;
                enemy->position.y += (diff > 0 ? 1 : -1) * 1.5f;
            }
            break;
    }
    
    // Keep enemies in bounds
    if (enemy->position.y < enemy->radius) {
        enemy->position.y = enemy->radius;
        enemy->speedY = fabs(enemy->speedY);
    }
    if (enemy->position.y > SCREEN_HEIGHT - enemy->radius) {
        enemy->position.y = SCREEN_HEIGHT - enemy->radius;
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
    if (waveSystem->spawnEvents) {
        free(waveSystem->spawnEvents);
        waveSystem->spawnEvents = NULL;
    }
}

const char* GetCurrentPhaseName(const WaveSystem* waveSystem) {
    if (waveSystem->currentPhase >= 0 && waveSystem->currentPhase < waveSystem->phaseCount) {
        return waveSystem->phases[waveSystem->currentPhase].name;
    }
    return "Unknown";
}

float GetWaveProgress(const WaveSystem* waveSystem) {
    return (waveSystem->waveTimer / waveSystem->totalDuration) * 100.0f;
}

bool IsWaveComplete(const WaveSystem* waveSystem) {
    return waveSystem->isComplete;
}
