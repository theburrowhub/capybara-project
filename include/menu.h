#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <stdbool.h>

// Menu states
typedef enum {
    MENU_MAIN,
    MENU_OPTIONS,
    MENU_OPTIONS_SOUND,
    MENU_OPTIONS_VIDEO,
    MENU_OPTIONS_GAME,
    MENU_CREDITS,
    MENU_HIGH_SCORES,
    MENU_NAME_INPUT,
    MENU_GAME
} MenuState;

// Menu option enum
typedef enum {
    MENU_START_GAME = 0,
    MENU_SHOW_OPTIONS = 1,
    MENU_SHOW_HIGH_SCORES = 2,
    MENU_SHOW_CREDITS = 3,
    MENU_TOTAL_OPTIONS = 4
} MenuOption;

// Resolution presets
typedef struct {
    int width;
    int height;
    const char* name;
} Resolution;

// Fullscreen mode
typedef enum {
    FULLSCREEN_OFF = 0,
    FULLSCREEN_WINDOWED = 1,
    FULLSCREEN_EXCLUSIVE = 2
} FullscreenMode;

// Menu structure
typedef struct {
    MenuState currentState;
    int selectedOption;
    
    // Visual properties
    float titleBounce;
    float optionAlpha[MENU_TOTAL_OPTIONS];
    
    // Audio options
    float soundVolume;
    float musicVolume;
    
    // Video options
    int selectedResolution;
    FullscreenMode fullscreenMode;
    bool vsync;
    
    // High scores options
    int selectedDifficulty;  // 0=EASY, 1=NORMAL, 2=HARD, 3=INSANE
    
    // Name input for high scores
    char playerName[32];
    int nameLength;
    int pendingScore;
    int pendingDifficulty;
    bool nameInputActive;
    float nameInputBlink;
    
    // Animation timers
    float animationTimer;
} Menu;

// Menu functions
void InitMenu(Menu* menu);
void UpdateMenu(Menu* menu, MenuState* gameState);
void DrawMenu(const Menu* menu);
void DrawMainMenu(const Menu* menu);
void DrawOptions(Menu* menu);
void DrawOptionsSound(Menu* menu);
void DrawOptionsVideo(Menu* menu);
void DrawOptionsGame(Menu* menu);
void DrawHighScores(Menu* menu);
void DrawNameInput(Menu* menu);
void DrawCredits(const Menu* menu);

// Name input functions
void StartNameInput(Menu* menu, int score, int difficulty);
bool IsNameInputActive(const Menu* menu);
void FinishNameInput(Menu* menu);

// Resolution management
const Resolution* GetAvailableResolutions(int* count);
void ApplyResolution(Menu* menu);
void ApplyFullscreenMode(Menu* menu);

#endif // MENU_H

