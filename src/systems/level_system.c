#include "level_system.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Define all available levels
static LevelConfig s_levels[] = {
    {
        .levelNumber = 1,
        .name = "Initiation",
        .audioPath = "assets/audio/level1.mp3",
        .bassLogPath = "assets/audio/level1.log",
        .duration = 553.82f,
        .targetScore = 5000,
        .description = "Welcome to the front lines. Learn the ropes and survive."
    },
    {
        .levelNumber = 2,
        .name = "Escalation",
        .audioPath = "assets/audio/level2.mp3",
        .bassLogPath = "assets/audio/level2.log",
        .duration = 612.13f,
        .targetScore = 10000,
        .description = "The enemy forces intensify. Show them what you've learned."
    }
    // More levels can be added here
};

void InitLevelManager(LevelManager* manager) {
    manager->levelCount = sizeof(s_levels) / sizeof(LevelConfig);
    manager->levels = s_levels;
    
    // Use DEBUG_START_LEVEL if set, otherwise start at level 1
    int startLevel = DEBUG_START_LEVEL - 1;  // Convert 1-based to 0-based index
    if (startLevel < 0 || startLevel >= manager->levelCount) {
        printf("[LEVEL SYSTEM] WARNING: Invalid DEBUG_START_LEVEL %d, using Level 1\n", 
               DEBUG_START_LEVEL);
        startLevel = 0;
    }
    
    manager->currentLevel = startLevel;
    manager->levelComplete = false;
    manager->levelTimer = 0.0f;
    
    printf("[LEVEL SYSTEM] Initialized with %d levels\n", manager->levelCount);
    printf("[LEVEL SYSTEM] Starting level %d: %s\n", 
           startLevel + 1, s_levels[startLevel].name);
    
    if (DEBUG_START_LEVEL != 1) {
        printf("[LEVEL SYSTEM] DEBUG: Level override active (DEBUG_START_LEVEL = %d)\n", 
               DEBUG_START_LEVEL);
    }
}

void CleanupLevelManager(LevelManager* manager) {
    // Nothing to cleanup - using static level data
    manager->levels = NULL;
    manager->levelCount = 0;
}

const LevelConfig* GetCurrentLevel(const LevelManager* manager) {
    if (manager->currentLevel < 0 || manager->currentLevel >= manager->levelCount) {
        return NULL;
    }
    return &manager->levels[manager->currentLevel];
}

const LevelConfig* GetLevel(const LevelManager* manager, int levelNumber) {
    // levelNumber is 1-based, convert to 0-based index
    int index = levelNumber - 1;
    if (index < 0 || index >= manager->levelCount) {
        return NULL;
    }
    return &manager->levels[index];
}

bool AdvanceToNextLevel(LevelManager* manager) {
    if (manager->currentLevel + 1 < manager->levelCount) {
        manager->currentLevel++;
        manager->levelComplete = false;
        manager->levelTimer = 0.0f;
        printf("[LEVEL SYSTEM] Advanced to level %d: %s\n", 
               manager->currentLevel + 1, 
               manager->levels[manager->currentLevel].name);
        return true;
    }
    return false;  // No more levels
}

void ResetToLevel(LevelManager* manager, int levelNumber) {
    int index = levelNumber - 1;
    if (index >= 0 && index < manager->levelCount) {
        manager->currentLevel = index;
        manager->levelComplete = false;
        manager->levelTimer = 0.0f;
        printf("[LEVEL SYSTEM] Reset to level %d: %s\n", 
               levelNumber, 
               manager->levels[index].name);
    }
}

int GetTotalLevels(const LevelManager* manager) {
    return manager->levelCount;
}
