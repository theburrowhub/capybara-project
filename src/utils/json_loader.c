#define _POSIX_C_SOURCE 200809L

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
    
    // Parse menu
    cJSON* menu = cJSON_GetObjectItem(root, "menu");
    if (menu) {
        cJSON* menuId = cJSON_GetObjectItem(menu, "id");
        cJSON* menuFile = cJSON_GetObjectItem(menu, "file");
        meta->menu.id = menuId && cJSON_IsNumber(menuId) ? menuId->valueint : 0;
        meta->menu.filename = menuFile && cJSON_IsString(menuFile) ? strdup(menuFile->valuestring) : strdup("menu.json");
    } else {
        meta->menu.id = 0;
        meta->menu.filename = strdup("menu.json");
    }
    
    // Parse levels array
    cJSON* levels = cJSON_GetObjectItem(root, "levels");
    if (!levels || !cJSON_IsArray(levels)) {
        printf("[JSON LOADER] ERROR: 'levels' array not found in meta file\n");
        free(meta->version);
        if (meta->menu.filename) free(meta->menu.filename);
        free(meta);
        cJSON_Delete(root);
        return NULL;
    }
    
    meta->levelCount = cJSON_GetArraySize(levels);
    meta->levels = (MetaLevel*)malloc(sizeof(MetaLevel) * meta->levelCount);
    
    if (!meta->levels) {
        printf("[JSON LOADER] ERROR: Failed to allocate levels array\n");
        free(meta->version);
        if (meta->menu.filename) free(meta->menu.filename);
        free(meta);
        cJSON_Delete(root);
        return NULL;
    }
    
    for (int i = 0; i < meta->levelCount; i++) {
        cJSON* level = cJSON_GetArrayItem(levels, i);
        
        cJSON* id = cJSON_GetObjectItem(level, "id");
        cJSON* file = cJSON_GetObjectItem(level, "file");
        
        meta->levels[i].id = id && cJSON_IsNumber(id) ? id->valueint : (i + 1);
        meta->levels[i].filename = file && cJSON_IsString(file) ? strdup(file->valuestring) : strdup("level1.json");
        
        if (!meta->levels[i].filename) {
            printf("[JSON LOADER] ERROR: Failed to allocate filename for level %d\n", i);
        }
    }
    
    // Parse credits
    cJSON* credits = cJSON_GetObjectItem(root, "credits");
    if (credits) {
        cJSON* creditsId = cJSON_GetObjectItem(credits, "id");
        cJSON* creditsFile = cJSON_GetObjectItem(credits, "file");
        meta->credits.id = creditsId && cJSON_IsNumber(creditsId) ? creditsId->valueint : 99;
        meta->credits.filename = creditsFile && cJSON_IsString(creditsFile) ? strdup(creditsFile->valuestring) : strdup("credits.json");
    } else {
        meta->credits.id = 99;
        meta->credits.filename = strdup("credits.json");
    }
    
    cJSON_Delete(root);
    printf("[JSON LOADER] Loaded meta orchestration: menu=%s, %d levels, credits=%s\n", 
           meta->menu.filename, meta->levelCount, meta->credits.filename);
    return meta;
}

// Free meta orchestration
void FreeMetaOrchestration(MetaOrchestration* meta) {
    if (!meta) return;
    
    if (meta->version) free(meta->version);
    if (meta->menu.filename) free(meta->menu.filename);
    
    for (int i = 0; i < meta->levelCount; i++) {
        if (meta->levels[i].filename) free(meta->levels[i].filename);
    }
    
    if (meta->levels) free(meta->levels);
    if (meta->credits.filename) free(meta->credits.filename);
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

// Load credit information from JSON
CreditInfo* LoadCreditInfo(const char* filepath) {
    char* content = ReadFileContents(filepath);
    if (!content) return NULL;
    
    cJSON* root = cJSON_Parse(content);
    free(content);
    
    if (!root) {
        printf("[JSON LOADER] ERROR: Failed to parse credits JSON: %s\n", filepath);
        return NULL;
    }
    
    CreditInfo* credits = (CreditInfo*)malloc(sizeof(CreditInfo));
    if (!credits) {
        cJSON_Delete(root);
        return NULL;
    }
    
    // Initialize all pointers to NULL
    memset(credits, 0, sizeof(CreditInfo));
    
    // Get creditInfo object
    cJSON* creditInfo = cJSON_GetObjectItem(root, "creditInfo");
    if (!creditInfo) {
        printf("[JSON LOADER] WARNING: No creditInfo found in %s\n", filepath);
        cJSON_Delete(root);
        free(credits);
        return NULL;
    }
    
    // Parse title
    cJSON* title = cJSON_GetObjectItem(creditInfo, "title");
    credits->title = title && cJSON_IsString(title) ? strdup(title->valuestring) : strdup("CREDITS");
    
    // Parse development info
    cJSON* development = cJSON_GetObjectItem(creditInfo, "development");
    if (development) {
        cJSON* projectName = cJSON_GetObjectItem(development, "projectName");
        cJSON* engine = cJSON_GetObjectItem(development, "engine");
        cJSON* team = cJSON_GetObjectItem(development, "team");
        
        credits->projectName = projectName && cJSON_IsString(projectName) ? strdup(projectName->valuestring) : strdup("");
        credits->engine = engine && cJSON_IsString(engine) ? strdup(engine->valuestring) : strdup("");
        credits->team = team && cJSON_IsString(team) ? strdup(team->valuestring) : strdup("");
    } else {
        credits->projectName = strdup("");
        credits->engine = strdup("");
        credits->team = strdup("");
    }
    
    // Parse music header and license
    cJSON* musicHeader = cJSON_GetObjectItem(creditInfo, "musicHeader");
    cJSON* musicLicense = cJSON_GetObjectItem(creditInfo, "musicLicense");
    credits->musicHeader = musicHeader && cJSON_IsString(musicHeader) ? strdup(musicHeader->valuestring) : strdup("MUSIC CREDITS");
    credits->musicLicense = musicLicense && cJSON_IsString(musicLicense) ? strdup(musicLicense->valuestring) : strdup("");
    
    // Parse music tracks array
    cJSON* musicTracks = cJSON_GetObjectItem(creditInfo, "musicTracks");
    if (musicTracks && cJSON_IsArray(musicTracks)) {
        credits->musicTrackCount = cJSON_GetArraySize(musicTracks);
        credits->musicTracks = (MusicTrackCredit*)malloc(sizeof(MusicTrackCredit) * credits->musicTrackCount);
        
        for (int i = 0; i < credits->musicTrackCount; i++) {
            cJSON* track = cJSON_GetArrayItem(musicTracks, i);
            
            cJSON* level = cJSON_GetObjectItem(track, "level");
            cJSON* trackTitle = cJSON_GetObjectItem(track, "title");
            cJSON* artist = cJSON_GetObjectItem(track, "artist");
            cJSON* column = cJSON_GetObjectItem(track, "column");
            
            credits->musicTracks[i].level = level && cJSON_IsString(level) ? strdup(level->valuestring) : strdup("");
            credits->musicTracks[i].title = trackTitle && cJSON_IsString(trackTitle) ? strdup(trackTitle->valuestring) : strdup("");
            credits->musicTracks[i].artist = artist && cJSON_IsString(artist) ? strdup(artist->valuestring) : strdup("");
            credits->musicTracks[i].column = column && cJSON_IsString(column) ? strdup(column->valuestring) : strdup("left");
        }
    } else {
        credits->musicTrackCount = 0;
        credits->musicTracks = NULL;
    }
    
    // Parse footer info
    cJSON* footer = cJSON_GetObjectItem(creditInfo, "footer");
    if (footer) {
        cJSON* source = cJSON_GetObjectItem(footer, "source");
        cJSON* thanks = cJSON_GetObjectItem(footer, "thanks");
        cJSON* copyright = cJSON_GetObjectItem(footer, "copyright");
        
        credits->source = source && cJSON_IsString(source) ? strdup(source->valuestring) : strdup("");
        credits->thanks = thanks && cJSON_IsString(thanks) ? strdup(thanks->valuestring) : strdup("");
        credits->copyright = copyright && cJSON_IsString(copyright) ? strdup(copyright->valuestring) : strdup("");
    } else {
        credits->source = strdup("");
        credits->thanks = strdup("");
        credits->copyright = strdup("");
    }
    
    // Parse back text
    cJSON* backText = cJSON_GetObjectItem(creditInfo, "backText");
    credits->backText = backText && cJSON_IsString(backText) ? strdup(backText->valuestring) : strdup("Press ESC or ENTER to return");
    
    cJSON_Delete(root);
    printf("[JSON LOADER] Loaded credit information from %s\n", filepath);
    return credits;
}

// Free credit information
void FreeCreditInfo(CreditInfo* credits) {
    if (!credits) return;
    
    if (credits->title) free(credits->title);
    if (credits->projectName) free(credits->projectName);
    if (credits->engine) free(credits->engine);
    if (credits->team) free(credits->team);
    if (credits->musicHeader) free(credits->musicHeader);
    if (credits->musicLicense) free(credits->musicLicense);
    
    if (credits->musicTracks) {
        for (int i = 0; i < credits->musicTrackCount; i++) {
            if (credits->musicTracks[i].level) free(credits->musicTracks[i].level);
            if (credits->musicTracks[i].title) free(credits->musicTracks[i].title);
            if (credits->musicTracks[i].artist) free(credits->musicTracks[i].artist);
            if (credits->musicTracks[i].column) free(credits->musicTracks[i].column);
        }
        free(credits->musicTracks);
    }
    
    if (credits->source) free(credits->source);
    if (credits->thanks) free(credits->thanks);
    if (credits->copyright) free(credits->copyright);
    if (credits->backText) free(credits->backText);
    
    free(credits);
}

