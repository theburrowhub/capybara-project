#ifndef LEVEL_SYSTEM_H
#define LEVEL_SYSTEM_H

#include "types.h"
#include "wave_system.h"

// Level configuration structure
typedef struct LevelConfig {
    int levelNumber;
    const char* name;
    const char* audioPath;
    const char* bassLogPath;
    float duration;              // Duration in seconds
    int targetScore;             // Score needed to unlock next level
    const char* description;
} LevelConfig;

// Level manager structure
typedef struct LevelManager {
    LevelConfig* levels;
    int levelCount;
    int currentLevel;
    bool levelComplete;
    float levelTimer;
} LevelManager;

// Function declarations
void InitLevelManager(LevelManager* manager);
void CleanupLevelManager(LevelManager* manager);
const LevelConfig* GetCurrentLevel(const LevelManager* manager);
const LevelConfig* GetLevel(const LevelManager* manager, int levelNumber);
bool AdvanceToNextLevel(LevelManager* manager);
void ResetToLevel(LevelManager* manager, int levelNumber);
int GetTotalLevels(const LevelManager* manager);

// Level-specific wave plan generators
SpawnEvent* CreateLevel1Waveplan(int* eventCount);
SpawnEvent* CreateLevel2Waveplan(int* eventCount);

#endif // LEVEL_SYSTEM_H
