#include "level_system.h"
#include "json_loader.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Meta orchestration path
#define META_JSON_PATH "assets/levels/meta.json"
#define LEVELS_DIR "assets/levels/"

void InitLevelManager(LevelManager* manager) {
    // Load meta orchestration
    MetaOrchestration* meta = LoadMetaOrchestration(META_JSON_PATH);
    if (!meta) {
        printf("[LEVEL SYSTEM] ERROR: Failed to load meta orchestration file\n");
        manager->levelCount = 0;
        manager->levels = NULL;
        return;
    }
    
    // Allocate level configs array
    manager->levelCount = meta->levelCount;
    manager->levels = (LevelConfig*)malloc(sizeof(LevelConfig) * manager->levelCount);
    if (!manager->levels) {
        printf("[LEVEL SYSTEM] ERROR: Failed to allocate level configs\n");
        FreeMetaOrchestration(meta);
        manager->levelCount = 0;
        return;
    }
    
    // Load each level configuration from JSON
    for (int i = 0; i < meta->levelCount; i++) {
        char filepath[256];
        snprintf(filepath, sizeof(filepath), "%s%s", LEVELS_DIR, meta->levels[i].filename);
        
        int success = 0;
        LevelConfig* config = LoadLevelConfig(filepath, &success);
        
        if (config && success) {
            // Copy the loaded config
            manager->levels[i] = *config;
            // Store the JSON file path
            manager->levels[i].jsonFilePath = strdup(filepath);
            
            // Free the temporary config structure (strings were already copied)
            free(config);
        } else {
            printf("[LEVEL SYSTEM] ERROR: Failed to load level %d from %s\n", 
                   meta->levels[i].id, filepath);
            // Use default values
            manager->levels[i].levelNumber = i + 1;
            manager->levels[i].name = strdup("Unknown Level");
            manager->levels[i].audioPath = strdup("");
            manager->levels[i].bassLogPath = NULL;
            manager->levels[i].duration = 0.0f;
            manager->levels[i].targetScore = 0;
            manager->levels[i].description = strdup("");
            manager->levels[i].jsonFilePath = strdup(filepath);
        }
    }
    
    FreeMetaOrchestration(meta);
    
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
    
    printf("[LEVEL SYSTEM] Initialized with %d levels from JSON\n", manager->levelCount);
    printf("[LEVEL SYSTEM] Starting level %d: %s\n", 
           startLevel + 1, manager->levels[startLevel].name);
    
    if (DEBUG_START_LEVEL != 1) {
        printf("[LEVEL SYSTEM] DEBUG: Level override active (DEBUG_START_LEVEL = %d)\n", 
               DEBUG_START_LEVEL);
    }
}

void CleanupLevelManager(LevelManager* manager) {
    if (manager->levels) {
        for (int i = 0; i < manager->levelCount; i++) {
            // Free dynamically allocated strings
            if (manager->levels[i].name) free((void*)manager->levels[i].name);
            if (manager->levels[i].description) free((void*)manager->levels[i].description);
            if (manager->levels[i].audioPath) free((void*)manager->levels[i].audioPath);
            if (manager->levels[i].bassLogPath) free((void*)manager->levels[i].bassLogPath);
            if (manager->levels[i].jsonFilePath) free((void*)manager->levels[i].jsonFilePath);
        }
        free(manager->levels);
    }
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
