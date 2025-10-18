#include "json_loader.h"
#include "cJSON.h"
#include "enemy_types.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to read file contents
static char* ReadFileContents(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("[JSON LOADER] ERROR: Could not open file: %s\n", filepath);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(fileSize + 1);
    if (!content) {
        printf("[JSON LOADER] ERROR: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';
    fclose(file);
    
    return content;
}

// Parse enemy type from string
EnemyType ParseEnemyType(const char* typeString) {
    if (!typeString) return ENEMY_GRUNT;
    
    if (strcmp(typeString, "ENEMY_GRUNT") == 0) return ENEMY_GRUNT;
    if (strcmp(typeString, "ENEMY_SWARM") == 0) return ENEMY_SWARM;
    if (strcmp(typeString, "ENEMY_SPEEDER") == 0) return ENEMY_SPEEDER;
    if (strcmp(typeString, "ENEMY_ZIGZAG") == 0) return ENEMY_ZIGZAG;
    if (strcmp(typeString, "ENEMY_TANK") == 0) return ENEMY_TANK;
    if (strcmp(typeString, "ENEMY_ELITE") == 0) return ENEMY_ELITE;
    if (strcmp(typeString, "ENEMY_BOMBER") == 0) return ENEMY_BOMBER;
    if (strcmp(typeString, "ENEMY_GHOST") == 0) return ENEMY_GHOST;
    if (strcmp(typeString, "ENEMY_SHIELD") == 0) return ENEMY_SHIELD;
    if (strcmp(typeString, "ENEMY_BOSS") == 0) return ENEMY_BOSS;
    
    printf("[JSON LOADER] WARNING: Unknown enemy type '%s', defaulting to ENEMY_GRUNT\n", typeString);
    return ENEMY_GRUNT;
}

// Convert enemy type to string
const char* EnemyTypeToString(EnemyType type) {
    switch (type) {
        case ENEMY_GRUNT: return "ENEMY_GRUNT";
        case ENEMY_SWARM: return "ENEMY_SWARM";
        case ENEMY_SPEEDER: return "ENEMY_SPEEDER";
        case ENEMY_ZIGZAG: return "ENEMY_ZIGZAG";
        case ENEMY_TANK: return "ENEMY_TANK";
        case ENEMY_ELITE: return "ENEMY_ELITE";
        case ENEMY_BOMBER: return "ENEMY_BOMBER";
        case ENEMY_GHOST: return "ENEMY_GHOST";
        case ENEMY_SHIELD: return "ENEMY_SHIELD";
        case ENEMY_BOSS: return "ENEMY_BOSS";
        default: return "ENEMY_GRUNT";
    }
}

// Load meta orchestration file
MetaOrchestration* LoadMetaOrchestration(const char* filepath) {
    char* content = ReadFileContents(filepath);
    if (!content) return NULL;
    
    cJSON* root = cJSON_Parse(content);
    free(content);
    
    if (!root) {
        printf("[JSON LOADER] ERROR: Failed to parse JSON: %s\n", filepath);
        const char* error = cJSON_GetErrorPtr();
        if (error) printf("[JSON LOADER] Parse error: %s\n", error);
        return NULL;
    }
    
    MetaOrchestration* meta = (MetaOrchestration*)malloc(sizeof(MetaOrchestration));
    if (!meta) {
        cJSON_Delete(root);
        return NULL;
    }
    
    // Parse version
    cJSON* version = cJSON_GetObjectItem(root, "version");
    if (version && cJSON_IsString(version)) {
        meta->version = strdup(version->valuestring);
    } else {
        meta->version = strdup("1.0");
    }
    
    // Parse levels array
    cJSON* levels = cJSON_GetObjectItem(root, "levels");
    if (!levels || !cJSON_IsArray(levels)) {
        printf("[JSON LOADER] ERROR: 'levels' array not found in meta file\n");
        free(meta->version);
        free(meta);
        cJSON_Delete(root);
        return NULL;
    }
    
    meta->levelCount = cJSON_GetArraySize(levels);
    meta->levels = (MetaLevel*)malloc(sizeof(MetaLevel) * meta->levelCount);
    
    for (int i = 0; i < meta->levelCount; i++) {
        cJSON* level = cJSON_GetArrayItem(levels, i);
        
        cJSON* id = cJSON_GetObjectItem(level, "id");
        cJSON* file = cJSON_GetObjectItem(level, "file");
        
        meta->levels[i].id = id && cJSON_IsNumber(id) ? id->valueint : (i + 1);
        meta->levels[i].filename = file && cJSON_IsString(file) ? strdup(file->valuestring) : NULL;
    }
    
    cJSON_Delete(root);
    printf("[JSON LOADER] Loaded meta orchestration: %d levels\n", meta->levelCount);
    return meta;
}

// Free meta orchestration
void FreeMetaOrchestration(MetaOrchestration* meta) {
    if (!meta) return;
    
    if (meta->version) free(meta->version);
    
    for (int i = 0; i < meta->levelCount; i++) {
        if (meta->levels[i].filename) free(meta->levels[i].filename);
    }
    
    if (meta->levels) free(meta->levels);
    free(meta);
}

// Load level configuration from JSON
LevelConfig* LoadLevelConfig(const char* filepath, int* success) {
    *success = 0;
    
    char* content = ReadFileContents(filepath);
    if (!content) return NULL;
    
    cJSON* root = cJSON_Parse(content);
    free(content);
    
    if (!root) {
        printf("[JSON LOADER] ERROR: Failed to parse level JSON: %s\n", filepath);
        const char* error = cJSON_GetErrorPtr();
        if (error) printf("[JSON LOADER] Parse error: %s\n", error);
        return NULL;
    }
    
    LevelConfig* config = (LevelConfig*)malloc(sizeof(LevelConfig));
    if (!config) {
        cJSON_Delete(root);
        return NULL;
    }
    
    // Parse level fields
    cJSON* levelNumber = cJSON_GetObjectItem(root, "levelNumber");
    cJSON* name = cJSON_GetObjectItem(root, "name");
    cJSON* description = cJSON_GetObjectItem(root, "description");
    cJSON* audioPath = cJSON_GetObjectItem(root, "audioPath");
    cJSON* duration = cJSON_GetObjectItem(root, "duration");
    cJSON* targetScore = cJSON_GetObjectItem(root, "targetScore");
    
    config->levelNumber = levelNumber && cJSON_IsNumber(levelNumber) ? levelNumber->valueint : 1;
    config->name = name && cJSON_IsString(name) ? strdup(name->valuestring) : strdup("Unknown");
    config->description = description && cJSON_IsString(description) ? strdup(description->valuestring) : strdup("");
    config->audioPath = audioPath && cJSON_IsString(audioPath) ? strdup(audioPath->valuestring) : strdup("");
    config->bassLogPath = NULL;  // Not used from JSON
    config->duration = duration && cJSON_IsNumber(duration) ? (float)duration->valuedouble : 0.0f;
    config->targetScore = targetScore && cJSON_IsNumber(targetScore) ? targetScore->valueint : 0;
    
    cJSON_Delete(root);
    
    *success = 1;
    printf("[JSON LOADER] Loaded level config: Level %d - %s (%.2fs)\n", 
           config->levelNumber, config->name, config->duration);
    return config;
}

// Free level configuration
void FreeLevelConfig(LevelConfig* config) {
    if (!config) return;
    
    if (config->name) free((void*)config->name);
    if (config->description) free((void*)config->description);
    if (config->audioPath) free((void*)config->audioPath);
    if (config->bassLogPath) free((void*)config->bassLogPath);
    
    free(config);
}

// Parse spawn events from JSON
SpawnEvent* ParseSpawnEvents(const char* filepath, int* eventCount) {
    *eventCount = 0;
    
    char* content = ReadFileContents(filepath);
    if (!content) return NULL;
    
    cJSON* root = cJSON_Parse(content);
    free(content);
    
    if (!root) {
        printf("[JSON LOADER] ERROR: Failed to parse spawn events JSON: %s\n", filepath);
        return NULL;
    }
    
    // Get waves array
    cJSON* waves = cJSON_GetObjectItem(root, "waves");
    if (!waves || !cJSON_IsArray(waves)) {
        printf("[JSON LOADER] ERROR: 'waves' array not found in level file\n");
        cJSON_Delete(root);
        return NULL;
    }
    
    int waveCount = cJSON_GetArraySize(waves);
    if (waveCount == 0) {
        printf("[JSON LOADER] WARNING: No waves found in level file\n");
        cJSON_Delete(root);
        return NULL;
    }
    
    // Allocate spawn events array
    SpawnEvent* events = (SpawnEvent*)malloc(sizeof(SpawnEvent) * waveCount);
    if (!events) {
        printf("[JSON LOADER] ERROR: Failed to allocate spawn events\n");
        cJSON_Delete(root);
        return NULL;
    }
    
    // Parse each wave
    for (int i = 0; i < waveCount; i++) {
        cJSON* wave = cJSON_GetArrayItem(waves, i);
        
        cJSON* time = cJSON_GetObjectItem(wave, "time");
        cJSON* type = cJSON_GetObjectItem(wave, "type");
        cJSON* x = cJSON_GetObjectItem(wave, "x");
        cJSON* y = cJSON_GetObjectItem(wave, "y");
        cJSON* count = cJSON_GetObjectItem(wave, "count");
        cJSON* interval = cJSON_GetObjectItem(wave, "interval");
        cJSON* pattern = cJSON_GetObjectItem(wave, "pattern");
        
        events[i].time = time && cJSON_IsNumber(time) ? (float)time->valuedouble : 0.0f;
        events[i].type = type && cJSON_IsString(type) ? ParseEnemyType(type->valuestring) : ENEMY_GRUNT;
        events[i].x = x && cJSON_IsNumber(x) ? (float)x->valuedouble : (float)SCREEN_WIDTH + 50.0f;
        events[i].y = y && cJSON_IsNumber(y) ? (float)y->valuedouble : 300.0f;
        events[i].count = count && cJSON_IsNumber(count) ? count->valueint : 1;
        events[i].interval = interval && cJSON_IsNumber(interval) ? (float)interval->valuedouble : 0.0f;
        events[i].pattern = pattern && cJSON_IsString(pattern) ? strdup(pattern->valuestring) : strdup("straight");
    }
    
    *eventCount = waveCount;
    cJSON_Delete(root);
    
    printf("[JSON LOADER] Loaded %d spawn events from %s\n", waveCount, filepath);
    return events;
}

// Free spawn events
void FreeSpawnEvents(SpawnEvent* events) {
    if (!events) return;
    // Note: Individual pattern strings are handled separately if allocated
    free(events);
}

