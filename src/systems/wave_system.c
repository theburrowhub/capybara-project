#include "wave_system.h"
#include "bass_events.h"
#include "enemy_types.h"
#include "constants.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// Anticipation/lead time: enemies start spawning this many seconds BEFORE bass hits
#define BASS_ANTICIPATION_TIME 1.5f  // Adjust this value for better feel (1.0-2.0 seconds recommended)

// Enemy spawn configuration based on bass level
typedef struct {
    EnemyType type;
    int weight;         // Spawn probability weight
    float minInterval;  // Minimum spawn interval
    float maxInterval;  // Maximum spawn interval
} EnemySpawnConfig;

// Spawn configurations for different bass levels
static EnemySpawnConfig noBassSpawns[] = {
    {ENEMY_GRUNT, 70, 2.0f, 4.0f},
    {ENEMY_SWARM, 30, 1.5f, 3.0f},
};

static EnemySpawnConfig lowBassSpawns[] = {
    {ENEMY_GRUNT, 40, 1.5f, 2.5f},
    {ENEMY_SWARM, 30, 1.0f, 2.0f},
    {ENEMY_SPEEDER, 20, 2.0f, 4.0f},
    {ENEMY_ZIGZAG, 10, 3.0f, 5.0f},
};

static EnemySpawnConfig mediumBassSpawns[] = {
    {ENEMY_TANK, 20, 4.0f, 8.0f},
    {ENEMY_SPEEDER, 20, 1.5f, 3.0f},
    {ENEMY_ZIGZAG, 15, 2.0f, 4.0f},
    {ENEMY_SHIELD, 15, 5.0f, 10.0f},
    {ENEMY_BOMBER, 10, 6.0f, 12.0f},
    {ENEMY_GRUNT, 10, 1.0f, 2.0f},
    {ENEMY_SWARM, 10, 0.8f, 1.5f},
};

static EnemySpawnConfig highBassSpawns[] = {
    {ENEMY_ELITE, 30, 6.0f, 12.0f},
    {ENEMY_BOMBER, 25, 5.0f, 10.0f},
    {ENEMY_GHOST, 20, 4.0f, 8.0f},
    {ENEMY_TANK, 15, 3.0f, 6.0f},
    {ENEMY_SHIELD, 10, 4.0f, 8.0f},
};

// Helper function to select enemy type based on bass level
static EnemyType SelectEnemyForBassLevel(BassLevel level) {
    EnemySpawnConfig* configs = NULL;
    int configCount = 0;
    
    switch (level) {
        case BASS_NONE:
            configs = noBassSpawns;
            configCount = sizeof(noBassSpawns) / sizeof(EnemySpawnConfig);
            break;
        case BASS_LOW:
            configs = lowBassSpawns;
            configCount = sizeof(lowBassSpawns) / sizeof(EnemySpawnConfig);
            break;
        case BASS_MEDIUM:
            configs = mediumBassSpawns;
            configCount = sizeof(mediumBassSpawns) / sizeof(EnemySpawnConfig);
            break;
        case BASS_HIGH:
            configs = highBassSpawns;
            configCount = sizeof(highBassSpawns) / sizeof(EnemySpawnConfig);
            break;
    }
    
    if (configs == NULL || configCount == 0) {
        return ENEMY_GRUNT;
    }
    
    // Calculate total weight
    int totalWeight = 0;
    for (int i = 0; i < configCount; i++) {
        totalWeight += configs[i].weight;
    }
    
    // Select random enemy based on weights
    int randomValue = GetRandomValue(0, totalWeight - 1);
    int currentWeight = 0;
    for (int i = 0; i < configCount; i++) {
        currentWeight += configs[i].weight;
        if (randomValue < currentWeight) {
            return configs[i].type;
        }
    }
    
    return configs[0].type;
}

// Get spawn interval for bass level
static float GetSpawnIntervalForBassLevel(BassLevel level, EnemyType type) {
    EnemySpawnConfig* configs = NULL;
    int configCount = 0;
    
    switch (level) {
        case BASS_NONE:
            configs = noBassSpawns;
            configCount = sizeof(noBassSpawns) / sizeof(EnemySpawnConfig);
            break;
        case BASS_LOW:
            configs = lowBassSpawns;
            configCount = sizeof(lowBassSpawns) / sizeof(EnemySpawnConfig);
            break;
        case BASS_MEDIUM:
            configs = mediumBassSpawns;
            configCount = sizeof(mediumBassSpawns) / sizeof(EnemySpawnConfig);
            break;
        case BASS_HIGH:
            configs = highBassSpawns;
            configCount = sizeof(highBassSpawns) / sizeof(EnemySpawnConfig);
            break;
    }
    
    // Find the config for this type
    for (int i = 0; i < configCount; i++) {
        if (configs[i].type == type) {
            float min = configs[i].minInterval;
            float max = configs[i].maxInterval;
            return min + ((float)GetRandomValue(0, 100) / 100.0f) * (max - min);
        }
    }
    
    return 2.0f; // Default
}

void InitWaveSystem(WaveSystem* waveSystem) {
    // Initialize enemy types first
    InitEnemyTypes();
    
    // Initialize bass event system
    waveSystem->bassSystem = (BassEventSystem*)malloc(sizeof(BassEventSystem));
    InitBassEvents(waveSystem->bassSystem);
    
    // No predefined phases - everything is bass-driven
    waveSystem->phaseCount = 0;
    waveSystem->phases = NULL;
    waveSystem->currentPhase = 0;
    
    // No predefined spawn events - all dynamic
    waveSystem->spawnEvents = NULL;
    waveSystem->eventCount = 0;
    waveSystem->nextEventIndex = 0;
    
    // Initialize state
    waveSystem->waveTimer = 0;
    waveSystem->totalDuration = waveSystem->bassSystem->songDuration;
    waveSystem->isComplete = false;
    waveSystem->lastSpawnTime = 0;
    waveSystem->totalEnemiesSpawned = 0;
    waveSystem->totalEnemiesKilled = 0;
    
    // Bass-driven spawn state
    // Start spawning immediately for warm-up period
    waveSystem->nextBassSpawnTime = 5.0f; // First enemy spawns at 5 seconds
    waveSystem->bassSpawnInterval = 2.0f;
    waveSystem->bossSpawned = false;
    
    printf("[WAVE SYSTEM] Initialized bass-driven wave system (%.1f seconds)\n", 
           waveSystem->totalDuration);
    printf("[WAVE SYSTEM] Using %.1fs anticipation - enemies spawn before bass hits\n", 
           BASS_ANTICIPATION_TIME);
    printf("[WAVE SYSTEM] Warm-up period (0-55s): Light enemies, no firing\n");
}

void UpdateWaveSystem(WaveSystem* waveSystem, struct Game* game, float deltaTime) {
    if (waveSystem->isComplete) return;
    
    // Update timer
    waveSystem->waveTimer += deltaTime;
    
    // Get bass level with anticipation - enemies spawn BEFORE bass hits for perfect sync
    BassLevel currentBass = GetBassLevelWithAnticipation(waveSystem->bassSystem, 
                                                         waveSystem->waveTimer, 
                                                         BASS_ANTICIPATION_TIME);
    
    // Check if it's time to spawn the boss
    // Boss spawns earlier (anticipation) so it's on screen when bass hits at 427s
    float bossSpawnTime = 427.0f - BASS_ANTICIPATION_TIME;
    if (!waveSystem->bossSpawned && waveSystem->waveTimer >= bossSpawnTime && waveSystem->waveTimer <= 428.0f) {
        SpawnWaveEnemy(game, ENEMY_BOSS, SCREEN_WIDTH + 100, SCREEN_HEIGHT/2, "boss");
        waveSystem->bossSpawned = true;
        LogEvent(game, "[%.2f] BOSS SPAWNED (anticipating epic battle at 427s)!", waveSystem->waveTimer);
    }
    
    // Spawn enemies based on bass level
    if (waveSystem->waveTimer >= waveSystem->nextBassSpawnTime) {
        // Don't spawn regular enemies during boss fight (427-451s)
        if (waveSystem->waveTimer < 427.0f || waveSystem->waveTimer > 451.0f) {
            EnemyType enemyType;
            float interval;
            const char* pattern;
            
            // Special handling for warm-up period (before first bass)
            if (waveSystem->waveTimer < 55.85f) {
                // Warm-up: only easy enemies, slower spawns
                if (GetRandomValue(0, 100) < 70) {
                    enemyType = ENEMY_GRUNT;
                } else {
                    enemyType = ENEMY_SWARM;
                }
                pattern = "straight";
                interval = 3.0f; // Slower spawn rate during warm-up
            } else {
                // Normal bass-driven spawning
                enemyType = SelectEnemyForBassLevel(currentBass);
                
                // Determine movement pattern based on enemy type and bass intensity
                switch (enemyType) {
                    case ENEMY_SPEEDER:
                    case ENEMY_SWARM:
                        pattern = (currentBass >= BASS_MEDIUM) ? "zigzag" : "straight";
                        break;
                    case ENEMY_ZIGZAG:
                        pattern = "erratic";
                        break;
                    case ENEMY_TANK:
                        pattern = "slow_advance";
                        break;
                    case ENEMY_SHIELD:
                        pattern = "formation";
                        break;
                    case ENEMY_BOMBER:
                        pattern = "horizontal";
                        break;
                    case ENEMY_ELITE:
                        pattern = "hover";
                        break;
                    case ENEMY_GHOST:
                        pattern = "phasing";
                        break;
                    default:
                        pattern = "straight";
                        break;
                }
                
                // Calculate interval based on bass level
                interval = GetSpawnIntervalForBassLevel(currentBass, enemyType);
                
                // Adjust interval based on game progression (increase difficulty over time)
                float progressFactor = waveSystem->waveTimer / waveSystem->totalDuration;
                interval *= (1.0f - progressFactor * 0.3f); // Up to 30% faster spawns
            }
            
            // Calculate spawn position
            float x = SCREEN_WIDTH + 50;
            float y = GetRandomValue(100, SCREEN_HEIGHT - 100);
            
            // Spawn the enemy
            SpawnWaveEnemy(game, enemyType, x, y, pattern);
            waveSystem->totalEnemiesSpawned++;
            
            waveSystem->nextBassSpawnTime = waveSystem->waveTimer + interval;
        } else {
            // During boss fight, spawn support minions occasionally
            if ((int)waveSystem->waveTimer % 15 == 0 && 
                waveSystem->waveTimer - waveSystem->lastSpawnTime > 14.5f) {
                // Spawn 3 grunt minions
                for (int i = 0; i < 3; i++) {
                    float y = SCREEN_HEIGHT/2 + GetRandomValue(-150, 150);
                    SpawnWaveEnemy(game, ENEMY_GRUNT, SCREEN_WIDTH + 50, y, "minion");
                }
                waveSystem->lastSpawnTime = waveSystem->waveTimer;
            }
            waveSystem->nextBassSpawnTime = waveSystem->waveTimer + 2.0f;
        }
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
            
            // Enemies can fire once the first bass actually hits (not during anticipation)
            // This prevents enemies from firing too early
            game->enemies[i].can_fire = (game->waveSystem->waveTimer >= 55.85f);
            
            LogEvent(game, "[%.2f] Enemy spawned - Type:%s ID:%d Pattern:%s Pos:(%.0f,%.0f) BassLevel:%d", 
                    game->gameTime, GetEnemyTypeName(type), game->enemies[i].id, 
                    pattern ? pattern : "default", x, y,
                    GetBassLevelAtTime(game->waveSystem->bassSystem, game->waveSystem->waveTimer));
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
    } else if (strcmp(pattern, "minion") == 0) {
        enemy->speedX = enemy->speed * 1.2f;
        enemy->speedY = GetRandomValue(-2, 2);
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
    if (waveSystem->bassSystem) {
        CleanupBassEvents(waveSystem->bassSystem);
        free(waveSystem->bassSystem);
        waveSystem->bassSystem = NULL;
    }
    
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
    // Return bass-driven phase name based on ACTUAL bass (not anticipated)
    // This ensures the phase name matches what the player hears
    BassLevel level = GetBassLevelAtTime(waveSystem->bassSystem, waveSystem->waveTimer);
    
    if (waveSystem->waveTimer < 55.85f) {
        return "Warm-Up";
    } else if (waveSystem->waveTimer >= 427.0f && waveSystem->waveTimer <= 451.0f) {
        return "BOSS BATTLE";
    } else {
        switch (level) {
            case BASS_NONE: return "Calm";
            case BASS_LOW: return "Building";
            case BASS_MEDIUM: return "Intense";
            case BASS_HIGH: return "Maximum";
            default: return "Unknown";
        }
    }
}

float GetWaveProgress(const WaveSystem* waveSystem) {
    return (waveSystem->waveTimer / waveSystem->totalDuration) * 100.0f;
}

bool IsWaveComplete(const WaveSystem* waveSystem) {
    return waveSystem->isComplete;
}

