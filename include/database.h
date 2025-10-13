#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

// Difficulty levels (for future use)
typedef enum {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_NORMAL = 1,
    DIFFICULTY_HARD = 2,
    DIFFICULTY_INSANE = 3,
    DIFFICULTY_COUNT = 4
} DifficultyLevel;

// User settings structure
typedef struct {
    // Audio settings
    float soundVolume;
    float musicVolume;
    
    // Video settings
    int resolutionWidth;
    int resolutionHeight;
    int fullscreenMode;  // 0=Off, 1=Windowed, 2=Exclusive
    bool vsync;
    
    // Legacy field (kept for backwards compatibility)
    bool fullscreen;  // Maps to fullscreenMode != 0
} UserSettings;

// High score entry structure
typedef struct {
    int id;
    char playerName[64];
    int score;
    DifficultyLevel difficulty;
    long timestamp;
} HighScoreEntry;

// Database initialization and cleanup
bool DB_Init(void);
void DB_Cleanup(void);

// Settings functions
bool DB_LoadSettings(UserSettings* settings);
bool DB_SaveSettings(const UserSettings* settings);

// High score functions
bool DB_AddHighScore(const char* playerName, int score, DifficultyLevel difficulty);
bool DB_GetHighScores(DifficultyLevel difficulty, HighScoreEntry* entries, int maxEntries, int* outCount);
bool DB_IsHighScore(int score, DifficultyLevel difficulty);

// Utility function to get the database file path
const char* DB_GetDatabasePath(void);

#endif // DATABASE_H

