#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include <errno.h>

// Platform-specific headers
#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <unistd.h>
    #include <pwd.h>
#endif

// Global database handle
static sqlite3* db = NULL;
static char dbPath[512] = {0};

// High score presets based on legendary game developers
typedef struct {
    const char* name;
    int score;
    DifficultyLevel difficulty;
} HighScorePreset;

static const HighScorePreset PRESETS[] = {
    // INSANE Difficulty (TOP 1-10)
    {"shigmoto", 100000, DIFFICULTY_INSANE},      // Shigeru Miyamoto
    {"kojimaster", 96500, DIFFICULTY_INSANE},     // Hideo Kojima
    {"carmatron", 94000, DIFFICULTY_INSANE},      // John Carmack
    {"romeroid", 92500, DIFFICULTY_INSANE},       // John Romero
    {"simeierX", 91000, DIFFICULTY_INSANE},       // Sid Meier
    {"gabenator", 90000, DIFFICULTY_INSANE},      // Gabe Newell
    {"yusonic", 87800, DIFFICULTY_INSANE},        // Yu Suzuki
    {"simwright", 85300, DIFFICULTY_INSANE},      // Will Wright
    {"howardcore", 82900, DIFFICULTY_INSANE},     // Todd Howard
    {"hirofinal", 80400, DIFFICULTY_INSANE},      // Hironobu Sakaguchi
    
    // HARD Difficulty (TOP 11-20)
    {"sakurush", 78700, DIFFICULTY_HARD},         // Masahiro Sakurai
    {"levinecore", 77900, DIFFICULTY_HARD},       // Ken Levine
    {"sonaknaka", 76800, DIFFICULTY_HARD},        // Yuji Naka
    {"granYamauchi", 75600, DIFFICULTY_HARD},     // Kazunori Yamauchi
    {"notchcode", 74800, DIFFICULTY_HARD},        // Markus Persson
    {"robertadream", 73900, DIFFICULTY_HARD},     // Roberta Williams
    {"jadevision", 72600, DIFFICULTY_HARD},       // Jade Raymond
    {"hennigstory", 71800, DIFFICULTY_HARD},      // Amy Hennig
    {"iwapac", 70900, DIFFICULTY_HARD},           // Toru Iwatani
    {"iwataheart", 70000, DIFFICULTY_HARD},       // Satoru Iwata
    
    // NORMAL Difficulty (TOP 21-30)
    {"boonfatal", 69500, DIFFICULTY_NORMAL},      // Ed Boon
    {"barloggamer", 68700, DIFFICULTY_NORMAL},    // Cory Barlog
    {"cliffyB", 68000, DIFFICULTY_NORMAL},        // Cliff Bleszinski
    {"molygod", 67300, DIFFICULTY_NORMAL},        // Peter Molyneux
    {"kondobeat", 66800, DIFFICULTY_NORMAL},      // Koji Kondo
    {"mikahorror", 66000, DIFFICULTY_NORMAL},     // Shinji Mikami
    {"russoAI", 65400, DIFFICULTY_NORMAL},        // Carmine Russo
    {"aonulink", 65000, DIFFICULTY_NORMAL},       // Eiji Aonuma
    {"kamiRage", 64700, DIFFICULTY_NORMAL},       // Hideki Kamiya
    {"uedadreamer", 63800, DIFFICULTY_NORMAL},    // Fumito Ueda
    
    // EASY Difficulty (TOP 31-40)
    {"masktaro", 63000, DIFFICULTY_EASY},         // Yoko Taro
    {"nomustyle", 62500, DIFFICULTY_EASY},        // Tetsuya Nomura
    {"kapover", 61900, DIFFICULTY_EASY},          // Jeff Kaplan
    {"miyaborn", 61500, DIFFICULTY_EASY},         // Hidetaka Miyazaki
    {"ledesmactrl", 60800, DIFFICULTY_EASY},      // Cory Ledesma
    {"baertech", 60000, DIFFICULTY_EASY},         // Ralph Baer
    {"yokoitech", 59500, DIFFICULTY_EASY},        // Gunpei Yokoi
    {"spencore", 58700, DIFFICULTY_EASY},         // Phil Spencer
    {"regginator", 58000, DIFFICULTY_EASY},       // Reggie Fils-Aimé
    {"hulstcore", 57500, DIFFICULTY_EASY}         // Herman Hulst
};

static const int PRESET_COUNT = sizeof(PRESETS) / sizeof(PRESETS[0]);

// Get the user's home directory
static const char* GetHomeDirectory(void) {
#ifdef _WIN32
    // Windows: use USERPROFILE or APPDATA
    const char* home = getenv("USERPROFILE");
    if (home) {
        return home;
    }
    home = getenv("APPDATA");
    if (home) {
        return home;
    }
    return "C:\\";
#else
    // Unix/Linux/macOS
    const char* home = getenv("HOME");
    if (home) {
        return home;
    }
    
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        return pw->pw_dir;
    }
    
    return NULL;
#endif
}

// Create directory if it doesn't exist
static bool CreateDirectoryIfNeeded(const char* path) {
    struct stat st = {0};
    
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) != 0) {
            fprintf(stderr, "Failed to create directory: %s (error: %s)\n", path, strerror(errno));
            return false;
        }
    }
    
    return true;
}

// Get the database file path
const char* DB_GetDatabasePath(void) {
    if (dbPath[0] == '\0') {
        const char* home = GetHomeDirectory();
        if (!home) {
            fprintf(stderr, "Failed to get home directory\n");
            return NULL;
        }
        
        // Create config directory path
        char configDir[512];
        
#ifdef __APPLE__
        // macOS: ~/Library/Application Support/CapybaraProject
        snprintf(configDir, sizeof(configDir), "%s/Library/Application Support/CapybaraProject", home);
#else
        // Linux/Unix: ~/.config/capybara-project
        snprintf(configDir, sizeof(configDir), "%s/.config/capybara-project", home);
#endif
        
        // Create directory if needed
        if (!CreateDirectoryIfNeeded(configDir)) {
            return NULL;
        }
        
        // Set database path
        snprintf(dbPath, sizeof(dbPath), "%s/game.db", configDir);
    }
    
    return dbPath;
}

// Populate high scores with legendary developer presets
static void PopulateHighScorePresets(void) {
    if (!db) {
        return;
    }
    
    // Check if high_scores table is empty
    const char* countQuery = "SELECT COUNT(*) FROM high_scores;";
    sqlite3_stmt* stmt = NULL;
    
    int rc = sqlite3_prepare_v2(db, countQuery, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to check high scores: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        
        if (count > 0) {
            // High scores already exist, don't populate
            return;
        }
    } else {
        sqlite3_finalize(stmt);
        return;
    }
    
    // Table is empty, populate with presets
    printf("Populating high scores with legendary game developers...\n");
    
    int successCount = 0;
    for (int i = 0; i < PRESET_COUNT; i++) {
        const HighScorePreset* preset = &PRESETS[i];
        
        if (DB_AddHighScore(preset->name, preset->score, preset->difficulty)) {
            successCount++;
        }
    }
    
    printf("✓ Added %d legendary developer high scores\n", successCount);
}

// Migrate database schema to add new columns
static void MigrateSchema(void) {
    // Check if new columns exist, if not add them
    const char* alterQueries[] = {
        "ALTER TABLE settings ADD COLUMN resolution_width INTEGER DEFAULT 1200;",
        "ALTER TABLE settings ADD COLUMN resolution_height INTEGER DEFAULT 600;",
        "ALTER TABLE settings ADD COLUMN fullscreen_mode INTEGER DEFAULT 0;",
        "ALTER TABLE settings ADD COLUMN vsync INTEGER DEFAULT 1;"
    };
    
    for (int i = 0; i < 4; i++) {
        char* errMsg = NULL;
        int rc = sqlite3_exec(db, alterQueries[i], NULL, NULL, &errMsg);
        if (rc != SQLITE_OK) {
            // Ignore errors (column might already exist)
            sqlite3_free(errMsg);
        }
    }
}

// Initialize the database
bool DB_Init(void) {
    const char* path = DB_GetDatabasePath();
    if (!path) {
        return false;
    }
    
    // Open database
    int rc = sqlite3_open(path, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        db = NULL;
        return false;
    }
    
    // Create settings table with all columns
    const char* createSettingsTable = 
        "CREATE TABLE IF NOT EXISTS settings ("
        "    id INTEGER PRIMARY KEY CHECK (id = 1),"
        "    sound_volume REAL DEFAULT 1.0,"
        "    music_volume REAL DEFAULT 0.5,"
        "    fullscreen INTEGER DEFAULT 0,"
        "    resolution_width INTEGER DEFAULT 1200,"
        "    resolution_height INTEGER DEFAULT 600,"
        "    fullscreen_mode INTEGER DEFAULT 0,"
        "    vsync INTEGER DEFAULT 1"
        ");";
    
    char* errMsg = NULL;
    rc = sqlite3_exec(db, createSettingsTable, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create settings table: %s\n", errMsg);
        sqlite3_free(errMsg);
        DB_Cleanup();
        return false;
    }
    
    // Create high_scores table
    const char* createHighScoresTable = 
        "CREATE TABLE IF NOT EXISTS high_scores ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    player_name TEXT NOT NULL,"
        "    score INTEGER NOT NULL,"
        "    difficulty INTEGER NOT NULL,"
        "    timestamp INTEGER NOT NULL"
        ");";
    
    rc = sqlite3_exec(db, createHighScoresTable, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create high_scores table: %s\n", errMsg);
        sqlite3_free(errMsg);
        DB_Cleanup();
        return false;
    }
    
    // Create index on difficulty for faster queries
    const char* createIndex = 
        "CREATE INDEX IF NOT EXISTS idx_difficulty_score "
        "ON high_scores(difficulty, score DESC);";
    
    rc = sqlite3_exec(db, createIndex, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create index: %s\n", errMsg);
        sqlite3_free(errMsg);
        // Continue anyway, index is just for performance
    }
    
    // Migrate schema for existing databases
    MigrateSchema();
    
    // Populate high scores with presets if table is empty
    PopulateHighScorePresets();
    
    printf("Database initialized at: %s\n", path);
    return true;
}

// Cleanup the database
void DB_Cleanup(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

// Load user settings from database
bool DB_LoadSettings(UserSettings* settings) {
    if (!db || !settings) {
        return false;
    }
    
    const char* query = 
        "SELECT sound_volume, music_volume, fullscreen, "
        "resolution_width, resolution_height, fullscreen_mode, vsync "
        "FROM settings WHERE id = 1;";
    sqlite3_stmt* stmt = NULL;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        settings->soundVolume = (float)sqlite3_column_double(stmt, 0);
        settings->musicVolume = (float)sqlite3_column_double(stmt, 1);
        settings->fullscreen = sqlite3_column_int(stmt, 2) != 0;
        settings->resolutionWidth = sqlite3_column_int(stmt, 3);
        settings->resolutionHeight = sqlite3_column_int(stmt, 4);
        settings->fullscreenMode = sqlite3_column_int(stmt, 5);
        settings->vsync = sqlite3_column_int(stmt, 6) != 0;
        
        sqlite3_finalize(stmt);
        return true;
    } else if (rc == SQLITE_DONE) {
        // No settings found, use defaults
        settings->soundVolume = 1.0f;
        settings->musicVolume = 0.5f;
        settings->fullscreen = false;
        settings->resolutionWidth = 1200;
        settings->resolutionHeight = 600;
        settings->fullscreenMode = 0;  // FULLSCREEN_OFF
        settings->vsync = true;
        
        sqlite3_finalize(stmt);
        
        // Save default settings
        DB_SaveSettings(settings);
        return true;
    }
    
    sqlite3_finalize(stmt);
    return false;
}

// Save user settings to database
bool DB_SaveSettings(const UserSettings* settings) {
    if (!db || !settings) {
        return false;
    }
    
    const char* query = 
        "INSERT OR REPLACE INTO settings "
        "(id, sound_volume, music_volume, fullscreen, "
        "resolution_width, resolution_height, fullscreen_mode, vsync) "
        "VALUES (1, ?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt = NULL;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    sqlite3_bind_double(stmt, 1, settings->soundVolume);
    sqlite3_bind_double(stmt, 2, settings->musicVolume);
    sqlite3_bind_int(stmt, 3, settings->fullscreen ? 1 : 0);
    sqlite3_bind_int(stmt, 4, settings->resolutionWidth);
    sqlite3_bind_int(stmt, 5, settings->resolutionHeight);
    sqlite3_bind_int(stmt, 6, settings->fullscreenMode);
    sqlite3_bind_int(stmt, 7, settings->vsync ? 1 : 0);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to save settings: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    return true;
}

// Add a high score to the database
bool DB_AddHighScore(const char* playerName, int score, DifficultyLevel difficulty) {
    if (!db || !playerName) {
        return false;
    }
    
    const char* query = 
        "INSERT INTO high_scores (player_name, score, difficulty, timestamp) "
        "VALUES (?, ?, ?, ?);";
    
    sqlite3_stmt* stmt = NULL;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, playerName, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, score);
    sqlite3_bind_int(stmt, 3, (int)difficulty);
    sqlite3_bind_int64(stmt, 4, (long)time(NULL));
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to add high score: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    // Keep only top 10 scores per difficulty
    const char* cleanupQuery = 
        "DELETE FROM high_scores "
        "WHERE difficulty = ? "
        "AND id NOT IN ("
        "    SELECT id FROM high_scores "
        "    WHERE difficulty = ? "
        "    ORDER BY score DESC "
        "    LIMIT 10"
        ");";
    
    rc = sqlite3_prepare_v2(db, cleanupQuery, -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, (int)difficulty);
        sqlite3_bind_int(stmt, 2, (int)difficulty);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    
    return true;
}

// Get high scores for a specific difficulty
bool DB_GetHighScores(DifficultyLevel difficulty, HighScoreEntry* entries, int maxEntries, int* outCount) {
    if (!db || !entries || !outCount) {
        return false;
    }
    
    *outCount = 0;
    
    const char* query = 
        "SELECT id, player_name, score, difficulty, timestamp "
        "FROM high_scores "
        "WHERE difficulty = ? "
        "ORDER BY score DESC "
        "LIMIT ?;";
    
    sqlite3_stmt* stmt = NULL;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, (int)difficulty);
    sqlite3_bind_int(stmt, 2, maxEntries);
    
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW && count < maxEntries) {
        entries[count].id = sqlite3_column_int(stmt, 0);
        
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        strncpy(entries[count].playerName, name ? name : "Unknown", sizeof(entries[count].playerName) - 1);
        entries[count].playerName[sizeof(entries[count].playerName) - 1] = '\0';
        
        entries[count].score = sqlite3_column_int(stmt, 2);
        entries[count].difficulty = (DifficultyLevel)sqlite3_column_int(stmt, 3);
        entries[count].timestamp = sqlite3_column_int64(stmt, 4);
        
        count++;
    }
    
    sqlite3_finalize(stmt);
    
    *outCount = count;
    return true;
}

// Check if a score qualifies as a high score
bool DB_IsHighScore(int score, DifficultyLevel difficulty) {
    if (!db) {
        return false;
    }
    
    // Count how many scores are higher
    const char* query = 
        "SELECT COUNT(*) FROM high_scores "
        "WHERE difficulty = ? AND score >= ?;";
    
    sqlite3_stmt* stmt = NULL;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, (int)difficulty);
    sqlite3_bind_int(stmt, 2, score);
    
    bool isHighScore = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        isHighScore = (count < 10); // Top 10
    }
    
    sqlite3_finalize(stmt);
    return isHighScore;
}

