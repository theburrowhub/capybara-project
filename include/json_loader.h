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
    MetaLevel menu;          // Menu configuration
    MetaLevel* levels;       // Array of game levels
    int levelCount;
    MetaLevel credits;       // Credits configuration
} MetaOrchestration;

// Credit information structures
typedef struct MusicTrackCredit {
    char* level;      // e.g. "LEVEL 1", "MENU & LEVEL 5"
    char* title;      // Song title
    char* artist;     // Artist name
    char* column;     // "left" or "right"
} MusicTrackCredit;

typedef struct CreditInfo {
    char* title;                    // "CREDITS"
    char* projectName;              // "CAPIBARA PROJECT"
    char* engine;                   // "Developed with raylib"
    char* team;                     // Team members
    char* musicHeader;              // "MUSIC CREDITS"
    char* musicLicense;             // License text
    MusicTrackCredit* musicTracks;  // Array of music tracks
    int musicTrackCount;
    char* source;                   // Source attribution
    char* thanks;                   // Special thanks
    char* copyright;                // Copyright notice
    char* backText;                 // Back instruction text
} CreditInfo;

// Function declarations for JSON loading
MetaOrchestration* LoadMetaOrchestration(const char* filepath);
void FreeMetaOrchestration(MetaOrchestration* meta);

LevelConfig* LoadLevelConfig(const char* filepath, int* success);
void FreeLevelConfig(LevelConfig* config);

SpawnEvent* ParseSpawnEvents(const char* filepath, int* eventCount);
void FreeSpawnEvents(SpawnEvent* events);

CreditInfo* LoadCreditInfo(const char* filepath);
void FreeCreditInfo(CreditInfo* credits);

EnemyType ParseEnemyType(const char* typeString);
const char* EnemyTypeToString(EnemyType type);

#endif // JSON_LOADER_H

