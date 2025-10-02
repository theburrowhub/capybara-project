#ifndef WAVE_SYSTEM_H
#define WAVE_SYSTEM_H

#include "types.h"
#include "enemy_types.h"

// Wave phase structure
typedef struct WavePhase {
    int phaseNumber;
    const char* name;
    float startTime;      // Time in seconds when phase starts
    float endTime;        // Time in seconds when phase ends
    float duration;       // Duration in seconds
    bool active;
    bool completed;
} WavePhase;

// Enemy spawn event
typedef struct SpawnEvent {
    float time;           // Time in seconds when enemy should spawn
    EnemyType type;
    float x;
    float y;
    int count;           // Number of enemies to spawn
    float interval;      // Interval between spawns (for multiple)
    const char* pattern; // Movement pattern identifier
} SpawnEvent;

// Wave system state
typedef struct WaveSystem {
    WavePhase* phases;
    int phaseCount;
    int currentPhase;
    
    SpawnEvent* spawnEvents;
    int eventCount;
    int nextEventIndex;
    
    float waveTimer;
    float totalDuration;
    bool isComplete;
    
    // Spawn tracking
    float lastSpawnTime;
    int totalEnemiesSpawned;
    int totalEnemiesKilled;
} WaveSystem;

// Function declarations
void InitWaveSystem(WaveSystem* waveSystem);
void UpdateWaveSystem(WaveSystem* waveSystem, struct Game* game, float deltaTime);
void SpawnWaveEnemy(struct Game* game, EnemyType type, float x, float y, const char* pattern);
void CleanupWaveSystem(WaveSystem* waveSystem);
const char* GetCurrentPhaseName(const WaveSystem* waveSystem);
float GetWaveProgress(const WaveSystem* waveSystem);
bool IsWaveComplete(const WaveSystem* waveSystem);

// Movement pattern functions
void ApplyMovementPattern(EnemyEx* enemy, const char* pattern);
void UpdateEnemyMovement(EnemyEx* enemy, float deltaTime);

#endif // WAVE_SYSTEM_H
