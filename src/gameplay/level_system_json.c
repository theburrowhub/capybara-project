#include "level_system.h"
#include "json_loader.h"
#include "constants.h"
#include <stdio.h>

// Load wave plan from JSON file
SpawnEvent* LoadWaveplanFromJSON(const char* jsonFilePath, int* eventCount) {
    *eventCount = 0;
    
    if (!jsonFilePath) {
        printf("[LEVEL SYSTEM] ERROR: NULL JSON file path\n");
        return NULL;
    }
    
    SpawnEvent* events = ParseSpawnEvents(jsonFilePath, eventCount);
    
    if (!events || *eventCount == 0) {
        printf("[LEVEL SYSTEM] ERROR: Failed to load wave plan from %s\n", jsonFilePath);
        return NULL;
    }
    
    printf("[LEVEL SYSTEM] Loaded %d spawn events from %s\n", *eventCount, jsonFilePath);
    return events;
}

