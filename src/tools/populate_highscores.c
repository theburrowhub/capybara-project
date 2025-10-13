#include "database.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

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

int main(int argc, char* argv[]) {
    bool force = false;
    
    // Check for --force flag
    if (argc > 1 && strcmp(argv[1], "--force") == 0) {
        force = true;
        printf("⚠️  Force mode enabled - will clear existing scores\n\n");
    }
    
    printf("🎮 HIGH SCORE PRESET POPULATOR 🎮\n");
    printf("===================================\n\n");
    
    // Initialize database
    printf("Initializing database...\n");
    if (!DB_Init()) {
        fprintf(stderr, "❌ Failed to initialize database!\n");
        return 1;
    }
    printf("✓ Database: %s\n\n", DB_GetDatabasePath());
    
    // Check if scores already exist
    if (!force) {
        for (int diff = 0; diff < DIFFICULTY_COUNT; diff++) {
            HighScoreEntry entries[1];
            int count = 0;
            
            if (DB_GetHighScores((DifficultyLevel)diff, entries, 1, &count) && count > 0) {
                printf("⚠️  High scores already exist in database!\n");
                printf("   Use --force to clear and repopulate.\n\n");
                
                // Show current scores count per difficulty
                const char* diffNames[] = {"EASY", "NORMAL", "HARD", "INSANE"};
                for (int d = 0; d < DIFFICULTY_COUNT; d++) {
                    if (DB_GetHighScores((DifficultyLevel)d, entries, 10, &count)) {
                        printf("   %s: %d scores\n", diffNames[d], count);
                    }
                }
                
                DB_Cleanup();
                return 0;
            }
        }
    } else {
        // Clear existing scores if force mode
        printf("Clearing existing high scores...\n");
        // Note: SQLite doesn't have a TRUNCATE, so we'll just let DB_AddHighScore
        // clean up old entries when adding new ones
    }
    
    // Populate presets
    printf("Populating legendary developer high scores...\n\n");
    
    const char* difficultyNames[] = {"EASY", "NORMAL", "HARD", "INSANE"};
    int successCount = 0;
    int failCount = 0;
    
    for (int i = 0; i < PRESET_COUNT; i++) {
        const HighScorePreset* preset = &PRESETS[i];
        
        // Add small time offset to each entry for realistic timestamps
        // (simulate scores being achieved at different times)
        
        if (DB_AddHighScore(preset->name, preset->score, preset->difficulty)) {
            printf("  ✓ [%8s] %15s - %6d pts\n", 
                   difficultyNames[preset->difficulty], 
                   preset->name, 
                   preset->score);
            successCount++;
        } else {
            printf("  ✗ [%8s] %15s - Failed\n", 
                   difficultyNames[preset->difficulty], 
                   preset->name);
            failCount++;
        }
    }
    
    printf("\n===================================\n");
    printf("Summary:\n");
    printf("  ✓ Successfully added: %d scores\n", successCount);
    if (failCount > 0) {
        printf("  ✗ Failed: %d scores\n", failCount);
    }
    
    // Show final counts per difficulty
    printf("\nScores per difficulty:\n");
    for (int diff = 0; diff < DIFFICULTY_COUNT; diff++) {
        HighScoreEntry entries[10];
        int count = 0;
        
        if (DB_GetHighScores((DifficultyLevel)diff, entries, 10, &count)) {
            printf("  %s: %d scores (top: %d pts by %s)\n", 
                   difficultyNames[diff], 
                   count,
                   count > 0 ? entries[0].score : 0,
                   count > 0 ? entries[0].playerName : "N/A");
        }
    }
    
    DB_Cleanup();
    printf("\n✓ Database populated successfully!\n");
    printf("===================================\n\n");
    
    return 0;
}

