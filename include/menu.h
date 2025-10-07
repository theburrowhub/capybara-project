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
    MENU_GAME
} MenuState;

// Menu option enum
typedef enum {
    MENU_START_GAME = 0,
    MENU_SHOW_OPTIONS = 1,
    MENU_SHOW_CREDITS = 2,
    MENU_TOTAL_OPTIONS = 3
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
void DrawCredits(const Menu* menu);

// Resolution management
const Resolution* GetAvailableResolutions(int* count);
void ApplyResolution(Menu* menu);
void ApplyFullscreenMode(Menu* menu);

#endif // MENU_H

