#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include <stdbool.h>

// Menu states
typedef enum {
    MENU_MAIN,
    MENU_OPTIONS,
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

// Menu structure
typedef struct {
    MenuState currentState;
    int selectedOption;
    
    // Visual properties
    float titleBounce;
    float optionAlpha[MENU_TOTAL_OPTIONS];
    
    // Options state
    float soundVolume;
    float musicVolume;
    bool fullscreen;
    
    // Animation timers
    float animationTimer;
} Menu;

// Menu functions
void InitMenu(Menu* menu);
void UpdateMenu(Menu* menu, MenuState* gameState);
void DrawMenu(const Menu* menu);
void DrawMainMenu(const Menu* menu);
void DrawOptions(Menu* menu);
void DrawCredits(const Menu* menu);

#endif // MENU_H

