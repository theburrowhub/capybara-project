#ifndef JSON_LOADER_H
#define JSON_LOADER_H

#include "types.h"
#include "wave_system.h"
#include "level_system.h"

// Meta orchestration structure
typedef struct MetaLevel {
    int id;
    char* filename;
} MetaLevel;

typedef struct MetaOrchestration {
    char* version;
    MetaLevel* levels;
    int levelCount;
} MetaOrchestration;

// Function declarations for JSON loading
MetaOrchestration* LoadMetaOrchestration(const char* filepath);
void FreeMetaOrchestration(MetaOrchestration* meta);

LevelConfig* LoadLevelConfig(const char* filepath, int* success);
void FreeLevelConfig(LevelConfig* config);

SpawnEvent* ParseSpawnEvents(const char* filepath, int* eventCount);
void FreeSpawnEvents(SpawnEvent* events);

EnemyType ParseEnemyType(const char* typeString);
const char* EnemyTypeToString(EnemyType type);

#endif // JSON_LOADER_H

