#include "menu.h"
#include "constants.h"
#include "database.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

// Helper functions for menu navigation with gamepad support
static bool MenuInput_Up(void) {
    bool pressed = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    
    if (IsGamepadAvailable(0)) {
        // D-Pad up
        pressed = pressed || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP);
        // Left stick up (with debouncing via button press simulation)
        static float lastUpAxis = 0.0f;
        float axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (axisY < -0.5f && lastUpAxis >= -0.5f) {
            pressed = true;
        }
        lastUpAxis = axisY;
    }
    
    return pressed;
}

static bool MenuInput_Down(void) {
    bool pressed = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
    
    if (IsGamepadAvailable(0)) {
        // D-Pad down
        pressed = pressed || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
        // Left stick down (with debouncing)
        static float lastDownAxis = 0.0f;
        float axisY = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (axisY > 0.5f && lastDownAxis <= 0.5f) {
            pressed = true;
        }
        lastDownAxis = axisY;
    }
    
    return pressed;
}

static bool MenuInput_Left(void) {
    bool pressed = IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A);
    
    if (IsGamepadAvailable(0)) {
        // D-Pad left
        pressed = pressed || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
        // Left stick left (with debouncing)
        static float lastLeftAxis = 0.0f;
        float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        if (axisX < -0.5f && lastLeftAxis >= -0.5f) {
            pressed = true;
        }
        lastLeftAxis = axisX;
    }
    
    return pressed;
}

static bool MenuInput_Right(void) {
    bool pressed = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);
    
    if (IsGamepadAvailable(0)) {
        // D-Pad right
        pressed = pressed || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
        // Left stick right (with debouncing)
        static float lastRightAxis = 0.0f;
        float axisX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        if (axisX > 0.5f && lastRightAxis <= 0.5f) {
            pressed = true;
        }
        lastRightAxis = axisX;
    }
    
    return pressed;
}

static bool MenuInput_Select(void) {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || 
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));  // A button
}

static bool MenuInput_Back(void) {
    return IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || 
           (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));  // B button
}

// Detect keyboard input only (no mouse)
static bool DetectKeyboardInput(InputBinding* outBinding) {
    // Check keyboard keys (common keys only)
    for (int key = 32; key <= 96; key++) {  // Space to `
        if (IsKeyPressed(key)) {
            outBinding->type = INPUT_TYPE_KEY;
            outBinding->value = key;
            outBinding->axisThreshold = 0.5f;
            return true;
        }
    }
    
    // Check arrow keys and special keys (including ESC - it's bindable)
    int specialKeys[] = {
        KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
        KEY_ENTER, KEY_TAB, KEY_BACKSPACE, KEY_ESCAPE,
        KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT,
        KEY_LEFT_CONTROL, KEY_RIGHT_CONTROL,
        KEY_LEFT_ALT, KEY_RIGHT_ALT
    };
    for (size_t i = 0; i < sizeof(specialKeys) / sizeof(specialKeys[0]); i++) {
        if (IsKeyPressed(specialKeys[i])) {
            outBinding->type = INPUT_TYPE_KEY;
            outBinding->value = specialKeys[i];
            outBinding->axisThreshold = 0.5f;
            return true;
        }
    }
    
    return false;
}

// Detect gamepad input only
static bool DetectGamepadInput(InputBinding* outBinding) {
    if (!IsGamepadAvailable(0)) return false;
    
    // Check all standard gamepad buttons (including B button - it's bindable)
    for (int button = 0; button < 32; button++) {
        if (IsGamepadButtonPressed(0, button)) {
            outBinding->type = INPUT_TYPE_GAMEPAD_BUTTON;
            outBinding->value = button;
            outBinding->axisThreshold = 0.5f;
            return true;
        }
    }
    
    return false;
}

// Available resolutions
static const Resolution RESOLUTIONS[] = {
    {800, 600, "800x600"},
    {1024, 768, "1024x768"},
    {1200, 600, "1200x600"},
    {1280, 720, "1280x720"},
    {1366, 768, "1366x768"},
    {1600, 900, "1600x900"},
    {1920, 1080, "1920x1080"},
    {2560, 1440, "2560x1440"}
};

const Resolution* GetAvailableResolutions(int* count) {
    *count = sizeof(RESOLUTIONS) / sizeof(RESOLUTIONS[0]);
    return RESOLUTIONS;
}

void ApplyResolution(Menu* menu) {
    int count = 0;
    const Resolution* resolutions = GetAvailableResolutions(&count);
    
    if (menu->selectedResolution >= 0 && menu->selectedResolution < count) {
        const Resolution* res = &resolutions[menu->selectedResolution];
        SetWindowSize(res->width, res->height);
    }
}

void ApplyFullscreenMode(Menu* menu) {
    bool isCurrentlyFullscreen = IsWindowFullscreen();
    
    switch (menu->fullscreenMode) {
        case FULLSCREEN_OFF:
            if (isCurrentlyFullscreen) {
                ToggleFullscreen();
            }
            ClearWindowState(FLAG_WINDOW_UNDECORATED);
            break;
            
        case FULLSCREEN_WINDOWED:
            if (isCurrentlyFullscreen) {
                ToggleFullscreen();
            }
            SetWindowState(FLAG_WINDOW_UNDECORATED);
            SetWindowState(FLAG_WINDOW_TOPMOST);
            {
                int monitor = GetCurrentMonitor();
                int monitorWidth = GetMonitorWidth(monitor);
                int monitorHeight = GetMonitorHeight(monitor);
                SetWindowSize(monitorWidth, monitorHeight);
                SetWindowPosition(0, 0);
            }
            ClearWindowState(FLAG_WINDOW_TOPMOST);
            break;
            
        case FULLSCREEN_EXCLUSIVE:
            if (!isCurrentlyFullscreen) {
                ToggleFullscreen();
            }
            break;
    }
}

void InitMenu(Menu* menu) {
    menu->currentState = MENU_MAIN;
    menu->selectedOption = 0;
    menu->titleBounce = 0.0f;
    menu->animationTimer = 0.0f;
    
    // Initialize option alphas
    for (int i = 0; i < MENU_TOTAL_OPTIONS; i++) {
        menu->optionAlpha[i] = 0.8f;
    }
    
    // Initialize high scores options
    menu->selectedDifficulty = 1;  // Start with NORMAL difficulty
    
    // Initialize name input
    menu->playerName[0] = '\0';
    menu->nameLength = 0;
    menu->pendingScore = 0;
    menu->pendingDifficulty = 0;
    menu->nameInputActive = false;
    menu->nameInputBlink = 0.0f;
    
    // Initialize controls configuration
    menu->inputManager = NULL;  // Will be set by main.c
    menu->selectedAction = 0;
    menu->selectedBinding = 0;
    menu->controlsTab = 0;  // Start with Keyboard tab
    menu->waitingForInput = false;
    menu->controlsModified = false;
    
    // Initialize pause menu
    menu->pauseMenuOption = 0;  // Default to Resume
    menu->pauseMenuCooldown = 0.0f;
    
    // Initialize main menu
    menu->justReturnedToMainMenu = false;
    
    // Load settings from database
    UserSettings settings;
    if (DB_LoadSettings(&settings)) {
        menu->soundVolume = settings.soundVolume;
        menu->musicVolume = settings.musicVolume;
        menu->fullscreenMode = (FullscreenMode)settings.fullscreenMode;
        menu->vsync = settings.vsync;
        
        // Find matching resolution index
        int count = 0;
        const Resolution* resolutions = GetAvailableResolutions(&count);
        menu->selectedResolution = 2; // Default to 1200x600
        for (int i = 0; i < count; i++) {
            if (resolutions[i].width == settings.resolutionWidth && 
                resolutions[i].height == settings.resolutionHeight) {
                menu->selectedResolution = i;
                break;
            }
        }
        
        // Apply loaded resolution
        ApplyResolution(menu);
        ApplyFullscreenMode(menu);
        
        // Apply VSync setting
        if (menu->vsync) {
            SetTargetFPS(60);
        } else {
            SetTargetFPS(0);
        }
    } else {
        // Default options if loading fails
        menu->soundVolume = 1.0f;
        menu->musicVolume = 0.5f;
        menu->fullscreenMode = FULLSCREEN_OFF;
        menu->selectedResolution = 2; // 1200x600 by default
        menu->vsync = true;
    }
}

void UpdateMenu(Menu* menu, MenuState* gameState) {
    float deltaTime = GetFrameTime();
    menu->animationTimer += deltaTime;
    menu->nameInputBlink += deltaTime * 2.0f;  // Blink cursor
    
    // Global hotkeys for fullscreen (F11 or Alt+Enter)
    if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))) {
        if (menu->fullscreenMode == FULLSCREEN_EXCLUSIVE) {
            menu->fullscreenMode = FULLSCREEN_OFF;
        } else {
            menu->fullscreenMode = FULLSCREEN_EXCLUSIVE;
        }
        ApplyFullscreenMode(menu);
        
        // Save fullscreen mode change
        int count = 0;
        const Resolution* resolutions = GetAvailableResolutions(&count);
        const Resolution* currentRes = &resolutions[menu->selectedResolution];
        
        UserSettings settings = {
            .soundVolume = menu->soundVolume,
            .musicVolume = menu->musicVolume,
            .resolutionWidth = currentRes->width,
            .resolutionHeight = currentRes->height,
            .fullscreenMode = (int)menu->fullscreenMode,
            .vsync = menu->vsync,
            .fullscreen = (menu->fullscreenMode != FULLSCREEN_OFF)
        };
        DB_SaveSettings(&settings);
    }
    
    // Update title bounce animation
    menu->titleBounce = sinf(menu->animationTimer * 2.0f) * 10.0f;
    
    // Update option alphas with hover effect
    for (int i = 0; i < MENU_TOTAL_OPTIONS; i++) {
        if (i == menu->selectedOption) {
            menu->optionAlpha[i] = fminf(menu->optionAlpha[i] + deltaTime * 3.0f, 1.0f);
        } else {
            menu->optionAlpha[i] = fmaxf(menu->optionAlpha[i] - deltaTime * 3.0f, 0.5f);
        }
    }
    
    switch (menu->currentState) {
        case MENU_MAIN:
            if (MenuInput_Up()) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) {
                    menu->selectedOption = MENU_TOTAL_OPTIONS - 1;
                }
            }
            if (MenuInput_Down()) {
                menu->selectedOption++;
                if (menu->selectedOption >= MENU_TOTAL_OPTIONS) {
                    menu->selectedOption = 0;
                }
            }
            
            if (MenuInput_Select()) {
                switch (menu->selectedOption) {
                    case MENU_START_GAME:
                        *gameState = MENU_GAME;
                        break;
                    case MENU_SHOW_OPTIONS:
                        menu->currentState = MENU_OPTIONS;
                        menu->selectedOption = 0;
                        break;
                    case MENU_SHOW_HIGH_SCORES:
                        menu->currentState = MENU_HIGH_SCORES;
                        break;
                    case MENU_SHOW_CREDITS:
                        menu->currentState = MENU_CREDITS;
                        break;
                    case MENU_EXIT_GAME:
                        // Signal to exit the game
                        *gameState = (MenuState)-1;  // Special value to indicate exit
                        break;
                }
            }
            break;
            
        case MENU_OPTIONS:
            if (MenuInput_Up()) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) menu->selectedOption = 3;
            }
            if (MenuInput_Down()) {
                menu->selectedOption++;
                if (menu->selectedOption > 3) menu->selectedOption = 0;
            }
            
            if (MenuInput_Select()) {
                switch (menu->selectedOption) {
                    case 0:
                        menu->currentState = MENU_OPTIONS_SOUND;
                        menu->selectedOption = 0;
                        break;
                    case 1:
                        menu->currentState = MENU_OPTIONS_VIDEO;
                        menu->selectedOption = 0;
                        break;
                    case 2:
                        menu->currentState = MENU_OPTIONS_GAME;
                        menu->selectedOption = 0;
                        break;
                    case 3:
                        menu->currentState = MENU_OPTIONS_CONTROLS;
                        menu->selectedOption = 0;
                        menu->selectedAction = 0;
                        menu->waitingForInput = false;
                        menu->controlsModified = false;
                        break;
                }
            }
            
            if (MenuInput_Back()) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_OPTIONS;
            }
            break;
            
        case MENU_OPTIONS_SOUND: {
            if (MenuInput_Up()) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) menu->selectedOption = 1;
            }
            if (MenuInput_Down()) {
                menu->selectedOption++;
                if (menu->selectedOption > 1) menu->selectedOption = 0;
            }
            
            bool settingsChanged = false;
            if (menu->selectedOption == 0) {
                if (MenuInput_Left()) {
                    menu->soundVolume = fmaxf(0.0f, menu->soundVolume - 0.1f);
                    settingsChanged = true;
                }
                if (MenuInput_Right()) {
                    menu->soundVolume = fminf(1.0f, menu->soundVolume + 0.1f);
                    settingsChanged = true;
                }
            } else if (menu->selectedOption == 1) {
                if (MenuInput_Left()) {
                    menu->musicVolume = fmaxf(0.0f, menu->musicVolume - 0.1f);
                    settingsChanged = true;
                }
                if (MenuInput_Right()) {
                    menu->musicVolume = fminf(1.0f, menu->musicVolume + 0.1f);
                    settingsChanged = true;
                }
            }
            
             // Save audio settings to database when changed
             if (settingsChanged) {
                 int count = 0;
                 const Resolution* resolutions = GetAvailableResolutions(&count);
                 const Resolution* currentRes = &resolutions[menu->selectedResolution];
                 
                 UserSettings settings = {
                     .soundVolume = menu->soundVolume,
                     .musicVolume = menu->musicVolume,
                     .resolutionWidth = currentRes->width,
                     .resolutionHeight = currentRes->height,
                     .fullscreenMode = (int)menu->fullscreenMode,
                     .vsync = menu->vsync,
                     .fullscreen = (menu->fullscreenMode != FULLSCREEN_OFF)
                 };
                 DB_SaveSettings(&settings);
             }
            
            if (MenuInput_Back()) {
                menu->currentState = MENU_OPTIONS;
                menu->selectedOption = 0;
            }
            break;
        }
            
         case MENU_OPTIONS_VIDEO: {
             int count = 0;
             GetAvailableResolutions(&count);
             
             if (MenuInput_Up()) {
                 menu->selectedOption--;
                 if (menu->selectedOption < 0) menu->selectedOption = 2;
             }
             if (MenuInput_Down()) {
                 menu->selectedOption++;
                 if (menu->selectedOption > 2) menu->selectedOption = 0;
             }
             
             bool settingsChanged = false;
             if (menu->selectedOption == 0) {
                 if (MenuInput_Left()) {
                     menu->selectedResolution--;
                     if (menu->selectedResolution < 0) menu->selectedResolution = count - 1;
                     ApplyResolution(menu);
                     settingsChanged = true;
                 }
                 if (MenuInput_Right()) {
                     menu->selectedResolution++;
                     if (menu->selectedResolution >= count) menu->selectedResolution = 0;
                     ApplyResolution(menu);
                     settingsChanged = true;
                 }
             } else if (menu->selectedOption == 1) {
                 if (MenuInput_Left()) {
                     int mode = (int)menu->fullscreenMode - 1;
                     if (mode < 0) mode = 2;
                     menu->fullscreenMode = (FullscreenMode)mode;
                     ApplyFullscreenMode(menu);
                     settingsChanged = true;
                 }
                 if (MenuInput_Right()) {
                     int mode = (int)menu->fullscreenMode + 1;
                     if (mode > 2) mode = 0;
                     menu->fullscreenMode = (FullscreenMode)mode;
                     ApplyFullscreenMode(menu);
                     settingsChanged = true;
                 }
             } else if (menu->selectedOption == 2) {
                 if (MenuInput_Left() || MenuInput_Right() || MenuInput_Select()) {
                     menu->vsync = !menu->vsync;
                     if (menu->vsync) {
                         SetTargetFPS(60);
                     } else {
                         SetTargetFPS(0);
                     }
                     settingsChanged = true;
                 }
             }
             
             // Save video settings to database when changed
             if (settingsChanged) {
                 const Resolution* resolutions = GetAvailableResolutions(&count);
                 const Resolution* currentRes = &resolutions[menu->selectedResolution];
                 
                 UserSettings settings = {
                     .soundVolume = menu->soundVolume,
                     .musicVolume = menu->musicVolume,
                     .resolutionWidth = currentRes->width,
                     .resolutionHeight = currentRes->height,
                     .fullscreenMode = (int)menu->fullscreenMode,
                     .vsync = menu->vsync,
                     .fullscreen = (menu->fullscreenMode != FULLSCREEN_OFF)
                 };
                 DB_SaveSettings(&settings);
             }
            
            if (MenuInput_Back()) {
                menu->currentState = MENU_OPTIONS;
                menu->selectedOption = 1;
            }
            break;
        }
            
        case MENU_OPTIONS_GAME:
            if (MenuInput_Back()) {
                menu->currentState = MENU_OPTIONS;
                menu->selectedOption = 2;
            }
            break;
            
        case MENU_OPTIONS_CONTROLS: {
            if (!menu->waitingForInput) {
                // Switch tabs with Q/E, LB/RB, or Left/Right arrows
                if (IsKeyPressed(KEY_Q) || MenuInput_Left() || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1))) {
                    menu->controlsTab = 0;  // Keyboard tab
                }
                if (IsKeyPressed(KEY_E) || MenuInput_Right() || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))) {
                    // Only allow gamepad tab if gamepad is connected
                    if (IsGamepadAvailable(0)) {
                        menu->controlsTab = 1;  // Gamepad tab
                    }
                }
                
                // Navigation through actions
                if (MenuInput_Up()) {
                    menu->selectedAction--;
                    if (menu->selectedAction < 0) menu->selectedAction = ACTION_COUNT + 1;  // +2 for Save and Reset
                }
                if (MenuInput_Down()) {
                    menu->selectedAction++;
                    if (menu->selectedAction > ACTION_COUNT + 1) menu->selectedAction = 0;
                }
                
                // Start rebinding with Enter - only for actions, not for buttons
                if (MenuInput_Select()) {
                    if (menu->selectedAction < ACTION_COUNT) {
                        // Use slot 0 for keyboard, slot 1 for gamepad
                        if (menu->inputManager && menu->inputManager->config) {
                            menu->selectedBinding = (menu->controlsTab == 0) ? 0 : 1;
                            menu->waitingForInput = true;
                        }
                    } else if (menu->selectedAction == ACTION_COUNT) {
                        // Reset to defaults (now first button)
                        if (menu->inputManager && menu->inputManager->config) {
                            InputConfig_InitDefaults(menu->inputManager->config);
                            menu->controlsModified = true;
                        }
                    } else if (menu->selectedAction == ACTION_COUNT + 1) {
                        // Save configuration (now second button)
                        if (menu->inputManager && menu->inputManager->config) {
                            InputConfig_Save(menu->inputManager->config, INPUT_CONFIG_FILE_DEFAULT);
                            menu->controlsModified = false;
                        }
                    }
                }
                
                // Go back
                if (MenuInput_Back()) {
                    menu->currentState = MENU_OPTIONS;
                    menu->selectedOption = 3;
                }
            } else {
                // Waiting for input to rebind
                // Check for cancel FIRST (before detecting input for binding)
                // This allows ESC/B to cancel even though they're bindable for game actions
                bool wantCancel = IsKeyPressed(KEY_ESCAPE) || 
                                 (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT));
                
                if (wantCancel) {
                    menu->waitingForInput = false;
                    break;
                }
                
                // Now detect input for binding (ESC and B button ARE bindable for game actions)
                InputBinding newBinding;
                bool detected = false;
                
                // Detect based on current tab
                if (menu->controlsTab == 0) {
                    detected = DetectKeyboardInput(&newBinding);
                } else {
                    detected = DetectGamepadInput(&newBinding);
                }
                
                if (detected) {
                    // Remove this binding from any other action that has it (in the same slot/tab)
                    if (menu->inputManager && menu->inputManager->config) {
                        int slot = menu->selectedBinding;  // 0 for keyboard, 1 for gamepad
                        
                        for (int action = 0; action < ACTION_COUNT; action++) {
                            InputBinding* existingBinding = &menu->inputManager->config->bindings[action][slot];
                            if (existingBinding->type == newBinding.type && 
                                existingBinding->value == newBinding.value) {
                                // Clear this binding
                                existingBinding->type = INPUT_TYPE_NONE;
                                existingBinding->value = 0;
                            }
                        }
                        
                        // Set the new binding for current action
                        InputConfig_SetBinding(menu->inputManager->config, 
                                             (GameAction)menu->selectedAction,
                                             slot,
                                             newBinding.type,
                                             newBinding.value);
                        menu->controlsModified = true;
                    }
                    menu->waitingForInput = false;
                }
            }
            break;
        }
            
        case MENU_HIGH_SCORES:
            // Navigate between difficulties with left/right arrows
            if (MenuInput_Left()) {
                menu->selectedDifficulty--;
                if (menu->selectedDifficulty < 0) {
                    menu->selectedDifficulty = 3;  // Wrap to INSANE
                }
            }
            if (MenuInput_Right()) {
                menu->selectedDifficulty++;
                if (menu->selectedDifficulty > 3) {
                    menu->selectedDifficulty = 0;  // Wrap to EASY
                }
            }
            
            // Go back with ESC, Backspace, Enter or Space
            if (MenuInput_Back() || MenuInput_Select()) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_HIGH_SCORES;
            }
            break;
            
        case MENU_NAME_INPUT: {
            // Handle text input
            int key = GetCharPressed();
            while (key > 0) {
                // Only allow alphanumeric and some special characters
                if ((key >= 32 && key <= 125) && menu->nameLength < 31) {
                    menu->playerName[menu->nameLength] = (char)key;
                    menu->nameLength++;
                    menu->playerName[menu->nameLength] = '\0';
                }
                key = GetCharPressed();
            }
            
            // Handle backspace
            if (IsKeyPressed(KEY_BACKSPACE) && menu->nameLength > 0) {
                menu->nameLength--;
                menu->playerName[menu->nameLength] = '\0';
            }
            
            // Submit name with Enter
            if (IsKeyPressed(KEY_ENTER) && menu->nameLength > 0) {
                FinishNameInput(menu);
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_HIGH_SCORES;
                *gameState = MENU_MAIN;
            }
            
            // Cancel with ESC (use default name)
            if (IsKeyPressed(KEY_ESCAPE)) {
                strcpy(menu->playerName, "Player");
                menu->nameLength = 6;
                FinishNameInput(menu);
                menu->currentState = MENU_MAIN;
                menu->selectedOption = 0;
                *gameState = MENU_MAIN;
            }
            break;
        }
            
        case MENU_CREDITS:
            if (MenuInput_Back() || MenuInput_Select()) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_CREDITS;
            }
            break;
            
        case MENU_PAUSE_CONFIRM:
            // Decrease cooldown timer
            if (menu->pauseMenuCooldown > 0.0f) {
                menu->pauseMenuCooldown -= deltaTime;
                if (menu->pauseMenuCooldown < 0.0f) {
                    menu->pauseMenuCooldown = 0.0f;
                }
                break;  // Skip all input while cooldown is active
            }
            
            // Navigate between Resume and Exit
            if (MenuInput_Up() || MenuInput_Down() || MenuInput_Left() || MenuInput_Right()) {
                menu->pauseMenuOption = 1 - menu->pauseMenuOption;  // Toggle between 0 and 1
            }
            
            // Select option
            if (MenuInput_Select()) {
                if (menu->pauseMenuOption == 0) {
                    // Resume game
                    menu->currentState = MENU_GAME;
                    *gameState = MENU_GAME;
                } else {
                    // Exit to menu - this will be handled in main.c
                    menu->currentState = MENU_MAIN;
                    *gameState = MENU_MAIN;
                }
            }
            
            // ESC or B button to resume (cancel)
            if (MenuInput_Back()) {
                menu->pauseMenuOption = 0;  // Reset to Resume
                menu->currentState = MENU_GAME;
                *gameState = MENU_GAME;
            }
            break;
            
        case MENU_GAME:
            break;
    }
}

void DrawMenu(const Menu* menu) {
    switch (menu->currentState) {
        case MENU_MAIN:
            DrawMainMenu(menu);
            break;
        case MENU_OPTIONS:
            DrawOptions((Menu*)menu);
            break;
        case MENU_OPTIONS_SOUND:
            DrawOptionsSound((Menu*)menu);
            break;
        case MENU_OPTIONS_VIDEO:
            DrawOptionsVideo((Menu*)menu);
            break;
        case MENU_OPTIONS_GAME:
            DrawOptionsGame((Menu*)menu);
            break;
        case MENU_OPTIONS_CONTROLS:
            DrawOptionsControls((Menu*)menu);
            break;
        case MENU_HIGH_SCORES:
            DrawHighScores((Menu*)menu);
            break;
        case MENU_NAME_INPUT:
            DrawNameInput((Menu*)menu);
            break;
        case MENU_CREDITS:
            DrawCredits(menu);
            break;
        case MENU_PAUSE_CONFIRM:
            DrawPauseConfirm(menu);
            break;
        case MENU_GAME:
            break;
    }
}

void DrawMainMenu(const Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw background with gradient
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Draw animated stars in background
    for (int i = 0; i < 100; i++) {
        int x = (i * 73 + (int)(menu->animationTimer * 20)) % screenWidth;
        int y = (i * 37) % screenHeight;
        int brightness = 100 + (i % 3) * 50;
        DrawPixel(x, y, (Color){brightness, brightness, brightness, 255});
    }
    
    // Draw title with bounce effect
    const char* title = "CAPIBARA PROJECT";
    int titleSize = 60;
    int titleWidth = MeasureText(title, titleSize);
    float titleY = 100 + menu->titleBounce;
    
    // Draw title shadow
    DrawText(title, (screenWidth - titleWidth) / 2 + 3, titleY + 3, 
             titleSize, (Color){0, 0, 0, 128});
    
    // Draw title with gradient effect
    DrawText(title, (screenWidth - titleWidth) / 2, titleY, 
             titleSize, (Color){255, 100, 100, 255});
    
    // Menu options
    const char* options[MENU_TOTAL_OPTIONS] = {
        "START",
        "SETTINGS",
        "HIGH SCORES",
        "CREDITS",
        "EXIT"
    };
    
    int optionSize = 30;
    int optionSpacing = 60;
    int startY = 280;
    
    for (int i = 0; i < MENU_TOTAL_OPTIONS; i++) {
        int optionWidth = MeasureText(options[i], optionSize);
        int x = (screenWidth - optionWidth) / 2;
        int y = startY + i * optionSpacing;
        
        // Draw selection indicator
        if (i == menu->selectedOption) {
            // Draw selection arrows with animation
            float arrowOffset = sinf(menu->animationTimer * 4.0f) * 5.0f;
            DrawText(">", x - 40 - arrowOffset, y, optionSize, WHITE);
            DrawText("<", x + optionWidth + 20 + arrowOffset, y, optionSize, WHITE);
            
            // Draw selection glow
            DrawRectangle(x - 10, y - 5, optionWidth + 20, optionSize + 10, 
                         (Color){255, 255, 255, 20});
        }
        
        // Calculate color with alpha
        int alpha = (int)(menu->optionAlpha[i] * 255);
        Color optionColor = (i == menu->selectedOption) ? 
                           (Color){255, 255, 255, alpha} : 
                           (Color){200, 200, 200, alpha};
        
        // Draw option shadow
        DrawText(options[i], x + 2, y + 2, optionSize, (Color){0, 0, 0, alpha / 2});
        // Draw option
        DrawText(options[i], x, y, optionSize, optionColor);
    }
}

void DrawOptions(Menu* menu) {
    // Get screen dimensions
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw background
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Draw title
    const char* title = "OPTIONS";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 80, titleSize, WHITE);
    
    // Menu options
    const char* options[] = {
        "SOUND OPTIONS",
        "VIDEO OPTIONS",
        "GAME OPTIONS",
        "CONTROLS"
    };
    
    int optionSize = 30;
    int optionSpacing = 60;
    int startY = 200;
    
    for (int i = 0; i < 4; i++) {
        int optionWidth = MeasureText(options[i], optionSize);
        int x = (screenWidth - optionWidth) / 2;
        int y = startY + i * optionSpacing;
        
        Color optionColor = (i == menu->selectedOption) ? YELLOW : WHITE;
        
        if (i == menu->selectedOption) {
            float arrowOffset = sinf(menu->animationTimer * 4.0f) * 5.0f;
            DrawText(">", x - 40 - arrowOffset, y, optionSize, WHITE);
            DrawText("<", x + optionWidth + 20 + arrowOffset, y, optionSize, WHITE);
            
            DrawRectangle(x - 10, y - 5, optionWidth + 20, optionSize + 10, 
                         (Color){255, 255, 255, 20});
        }
        
        DrawText(options[i], x + 2, y + 2, optionSize, (Color){0, 0, 0, 128});
        DrawText(options[i], x, y, optionSize, optionColor);
    }
}

void DrawOptionsSound(Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    const char* title = "SOUND OPTIONS";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 80, titleSize, WHITE);
    
    int optionSize = 24;
    int startY = 200;
    int spacing = 80;
    int barWidth = 200;
    int barHeight = 20;
    int barX = 450;
    
    // Sound Volume
    const char* soundText = "Sound Volume:";
    int soundY = startY;
    Color soundColor = (menu->selectedOption == 0) ? YELLOW : WHITE;
    DrawText(soundText, 200, soundY, optionSize, soundColor);
    
    DrawRectangle(barX, soundY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, soundY, (int)(barWidth * menu->soundVolume), barHeight, GREEN);
    DrawRectangleLines(barX, soundY, barWidth, barHeight, WHITE);
    
    char volumeText[16];
    snprintf(volumeText, sizeof(volumeText), "%d%%", (int)(menu->soundVolume * 100));
    DrawText(volumeText, barX + barWidth + 20, soundY, optionSize, WHITE);
    
    // Music Volume
    const char* musicText = "Music Volume:";
    int musicY = startY + spacing;
    Color musicColor = (menu->selectedOption == 1) ? YELLOW : WHITE;
    DrawText(musicText, 200, musicY, optionSize, musicColor);
    
    DrawRectangle(barX, musicY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, musicY, (int)(barWidth * menu->musicVolume), barHeight, GREEN);
    DrawRectangleLines(barX, musicY, barWidth, barHeight, WHITE);
    
    snprintf(volumeText, sizeof(volumeText), "%d%%", (int)(menu->musicVolume * 100));
    DrawText(volumeText, barX + barWidth + 20, musicY, optionSize, WHITE);
}

void DrawOptionsVideo(Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    const char* title = "VIDEO OPTIONS";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 80, titleSize, WHITE);
    
    int optionSize = 24;
    int startY = 180;
    int spacing = 70;
    int labelX = 150;
    int valueX = 500;
    
    // Resolution
    const char* resText = "Resolution:";
    int resY = startY;
    Color resColor = (menu->selectedOption == 0) ? YELLOW : WHITE;
    DrawText(resText, labelX, resY, optionSize, resColor);
    
    int count = 0;
    const Resolution* resolutions = GetAvailableResolutions(&count);
    const Resolution* currentRes = &resolutions[menu->selectedResolution];
    DrawText(currentRes->name, valueX, resY, optionSize, WHITE);
    DrawText("< >", valueX - 40, resY, optionSize, (Color){150, 150, 150, 200});
    
    // Fullscreen Mode
    const char* fullscreenText = "Fullscreen Mode:";
    int fullscreenY = startY + spacing;
    Color fullscreenColor = (menu->selectedOption == 1) ? YELLOW : WHITE;
    DrawText(fullscreenText, labelX, fullscreenY, optionSize, fullscreenColor);
    
    const char* modeNames[] = {"Windowed", "Borderless", "Fullscreen"};
    DrawText(modeNames[menu->fullscreenMode], valueX, fullscreenY, optionSize, WHITE);
    DrawText("< >", valueX - 40, fullscreenY, optionSize, (Color){150, 150, 150, 200});
    
    // VSync
    const char* vsyncText = "VSync:";
    int vsyncY = startY + spacing * 2;
    Color vsyncColor = (menu->selectedOption == 2) ? YELLOW : WHITE;
    DrawText(vsyncText, labelX, vsyncY, optionSize, vsyncColor);
    
    int checkboxSize = 20;
    DrawRectangleLines(valueX, vsyncY, checkboxSize, checkboxSize, WHITE);
    if (menu->vsync) {
        DrawRectangle(valueX + 3, vsyncY + 3, checkboxSize - 6, checkboxSize - 6, GREEN);
    }
    const char* vsyncStatus = menu->vsync ? "ON" : "OFF";
    DrawText(vsyncStatus, valueX + checkboxSize + 10, vsyncY, optionSize, WHITE);
    
}

void DrawOptionsGame(Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    const char* title = "GAME OPTIONS";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 80, titleSize, WHITE);
    
    // Empty for now
    const char* emptyText = "No game options available yet";
    int emptySize = 20;
    int emptyWidth = MeasureText(emptyText, emptySize);
    DrawText(emptyText, (screenWidth - emptyWidth) / 2, 
             screenHeight / 2, emptySize, (Color){150, 150, 150, 200});
}

void DrawHighScores(Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw background
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Draw title
    const char* title = "HIGH SCORES";
    int titleSize = 45;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 60, titleSize, WHITE);
    
    // Difficulty names and colors
    const char* difficultyNames[] = {"EASY", "NORMAL", "HARD", "INSANE"};
    Color difficultyColors[] = {
        (Color){100, 255, 100, 255},  // Green for EASY
        (Color){100, 200, 255, 255},  // Blue for NORMAL
        (Color){255, 165, 0, 255},    // Orange for HARD
        (Color){255, 50, 50, 255}     // Red for INSANE
    };
    
    // Draw difficulty selector with arrows
    const char* difficultyLabel = "Difficulty:";
    int labelSize = 18;
    int labelWidth = MeasureText(difficultyLabel, labelSize);
    int difficultySize = 24;
    const char* currentDifficulty = difficultyNames[menu->selectedDifficulty];
    int difficultyWidth = MeasureText(currentDifficulty, difficultySize);
    
    int labelX = (screenWidth - labelWidth - 10 - difficultyWidth) / 2;
    int difficultyX = labelX + labelWidth + 10;
    int difficultyY = 115;
    
    // Draw label
    DrawText(difficultyLabel, labelX, difficultyY + 3, labelSize, (Color){180, 180, 180, 255});
    
    // Draw difficulty with color and arrows
    float arrowOffset = sinf(menu->animationTimer * 4.0f) * 3.0f;
    DrawText("<", difficultyX - 30 - arrowOffset, difficultyY, difficultySize, WHITE);
    DrawText(currentDifficulty, difficultyX, difficultyY, difficultySize, difficultyColors[menu->selectedDifficulty]);
    DrawText(">", difficultyX + difficultyWidth + 10 + arrowOffset, difficultyY, difficultySize, WHITE);
    
    // Load high scores from database for selected difficulty
    HighScoreEntry entries[10];
    int count = 0;
    
    if (DB_GetHighScores((DifficultyLevel)menu->selectedDifficulty, entries, 10, &count)) {
        if (count > 0) {
            // Draw table header
            int headerY = 170;
            int headerSize = 20;
            DrawText("RANK", 150, headerY, headerSize, YELLOW);
            DrawText("PLAYER", 250, headerY, headerSize, YELLOW);
            DrawText("SCORE", 600, headerY, headerSize, YELLOW);
            
            // Draw high scores
            int entrySize = 18;
            int entrySpacing = 35;
            int startY = 210;
            
            for (int i = 0; i < count; i++) {
                int y = startY + i * entrySpacing;
                
                // Rank
                char rankText[8];
                snprintf(rankText, sizeof(rankText), "%d.", i + 1);
                DrawText(rankText, 150, y, entrySize, WHITE);
                
                // Player name
                DrawText(entries[i].playerName, 250, y, entrySize, WHITE);
                
                // Score
                char scoreText[32];
                snprintf(scoreText, sizeof(scoreText), "%d", entries[i].score);
                DrawText(scoreText, 600, y, entrySize, WHITE);
                
                // Animated glow for top 3
                if (i < 3) {
                    float alpha = sinf(menu->animationTimer * 2.0f + i) * 0.2f + 0.3f;
                    Color glowColor;
                    if (i == 0) glowColor = (Color){255, 215, 0, (int)(alpha * 255)};      // Gold
                    else if (i == 1) glowColor = (Color){192, 192, 192, (int)(alpha * 255)}; // Silver
                    else glowColor = (Color){205, 127, 50, (int)(alpha * 255)};            // Bronze
                    
                    DrawRectangle(140, y - 3, 600, entrySize + 6, glowColor);
                    
                    // Redraw text on top of glow
                    DrawText(rankText, 150, y, entrySize, WHITE);
                    DrawText(entries[i].playerName, 250, y, entrySize, WHITE);
                    DrawText(scoreText, 600, y, entrySize, WHITE);
                }
            }
        } else {
            // No high scores yet
            const char* emptyText = "No high scores yet. Start playing!";
            int emptySize = 22;
            int emptyWidth = MeasureText(emptyText, emptySize);
            DrawText(emptyText, (screenWidth - emptyWidth) / 2, 
                    screenHeight / 2, emptySize, (Color){150, 150, 150, 255});
        }
    } else {
        // Error loading scores
        const char* errorText = "Error loading high scores";
        int errorSize = 20;
        int errorWidth = MeasureText(errorText, errorSize);
        DrawText(errorText, (screenWidth - errorWidth) / 2, 
                screenHeight / 2, errorSize, RED);
    }
}

void DrawCredits(const Menu* menu) {
    // Get screen dimensions
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw background
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Draw title
    const char* title = "CREDITS";
    int titleSize = 45;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 80, titleSize, WHITE);
    
    // Credits content
    int creditSize = 20;
    int spacing = 30;
    int startY = 180;
    
    const char* credits[] = {
        "CAPIBARA PROJECT",
        "Developed with raylib",
        "",
        "Programming & Design:",
        "Sergio Tejón",
        "José A. Muriano",
        "",
        "Special Thanks:",
        "raylib community",
        "© 2025"
    };
    
    int numCredits = sizeof(credits) / sizeof(credits[0]);
    
    for (int i = 0; i < numCredits; i++) {
        if (strlen(credits[i]) > 0) {
            int textWidth = MeasureText(credits[i], creditSize);
            int x = (screenWidth - textWidth) / 2;
            int y = startY + i * spacing;
            
            // Animated fade effect based on position
            float alpha = sinf(menu->animationTimer + i * 0.3f) * 0.3f + 0.7f;
            Color textColor = (Color){255, 255, 255, (int)(alpha * 255)};
            
            DrawText(credits[i], x, y, creditSize, textColor);
        }
    }
}

void DrawNameInput(Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 180});
    
    // Draw dialog box
    int boxWidth = 500;
    int boxHeight = 300;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = (screenHeight - boxHeight) / 2;
    
    // Box shadow
    DrawRectangle(boxX + 5, boxY + 5, boxWidth, boxHeight, (Color){0, 0, 0, 150});
    
    // Box background with border
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){20, 20, 40, 255});
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, (Color){100, 150, 255, 255});
    DrawRectangleLines(boxX + 2, boxY + 2, boxWidth - 4, boxHeight - 4, (Color){60, 90, 180, 255});
    
    // Title
    const char* title = "NEW HIGH SCORE!";
    int titleSize = 36;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, boxX + (boxWidth - titleWidth) / 2, boxY + 30, titleSize, YELLOW);
    
    // Score display
    char scoreText[64];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", menu->pendingScore);
    int scoreSize = 24;
    int scoreWidth = MeasureText(scoreText, scoreSize);
    DrawText(scoreText, boxX + (boxWidth - scoreWidth) / 2, boxY + 80, scoreSize, WHITE);
    
    // Difficulty display
    const char* diffNames[] = {"EASY", "NORMAL", "HARD", "INSANE"};
    Color diffColors[] = {
        (Color){100, 255, 100, 255},
        (Color){100, 200, 255, 255},
        (Color){255, 165, 0, 255},
        (Color){255, 50, 50, 255}
    };
    
    char diffText[64];
    snprintf(diffText, sizeof(diffText), "Difficulty: %s", diffNames[menu->pendingDifficulty]);
    int diffSize = 20;
    int diffWidth = MeasureText(diffText, diffSize);
    DrawText(diffText, boxX + (boxWidth - diffWidth) / 2, boxY + 110, 
             diffSize, diffColors[menu->pendingDifficulty]);
    
    // Prompt
    const char* prompt = "Enter your name:";
    int promptSize = 20;
    int promptWidth = MeasureText(prompt, promptSize);
    DrawText(prompt, boxX + (boxWidth - promptWidth) / 2, boxY + 150, promptSize, 
             (Color){200, 200, 200, 255});
    
    // Input box
    int inputBoxWidth = 400;
    int inputBoxHeight = 40;
    int inputBoxX = boxX + (boxWidth - inputBoxWidth) / 2;
    int inputBoxY = boxY + 180;
    
    DrawRectangle(inputBoxX, inputBoxY, inputBoxWidth, inputBoxHeight, (Color){10, 10, 20, 255});
    DrawRectangleLines(inputBoxX, inputBoxY, inputBoxWidth, inputBoxHeight, WHITE);
    
    // Name text
    int nameSize = 24;
    int nameY = inputBoxY + (inputBoxHeight - nameSize) / 2;
    DrawText(menu->playerName, inputBoxX + 10, nameY, nameSize, WHITE);
    
    // Blinking cursor
    if (menu->nameLength < 31) {
        float blinkAlpha = (sinf(menu->nameInputBlink) + 1.0f) / 2.0f;
        int cursorX = inputBoxX + 10 + MeasureText(menu->playerName, nameSize);
        DrawText("_", cursorX, nameY, nameSize, (Color){255, 255, 255, (int)(blinkAlpha * 255)});
    }
    
    // Character count
    char countText[16];
    snprintf(countText, sizeof(countText), "%d/31", menu->nameLength);
    int countSize = 14;
    int countWidth = MeasureText(countText, countSize);
    DrawText(countText, inputBoxX + inputBoxWidth - countWidth - 10, inputBoxY + inputBoxHeight + 5, 
             countSize, (Color){150, 150, 150, 255});
}

// Name input management functions
void StartNameInput(Menu* menu, int score, int difficulty) {
    menu->currentState = MENU_NAME_INPUT;
    menu->nameInputActive = true;
    menu->pendingScore = score;
    menu->pendingDifficulty = difficulty;
    menu->playerName[0] = '\0';
    menu->nameLength = 0;
    menu->nameInputBlink = 0.0f;
}

bool IsNameInputActive(const Menu* menu) {
    return menu->nameInputActive;
}

void FinishNameInput(Menu* menu) {
    if (menu->nameInputActive && menu->nameLength > 0) {
        // Save high score with entered name
        DB_AddHighScore(menu->playerName, menu->pendingScore, (DifficultyLevel)menu->pendingDifficulty);
        printf("High score saved: %s - %d pts (Difficulty: %d)\n", 
               menu->playerName, menu->pendingScore, menu->pendingDifficulty);
    }
    
    menu->nameInputActive = false;
    menu->pendingScore = 0;
    menu->pendingDifficulty = 0;
}

void DrawOptionsControls(Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Draw background
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight,
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Show gamepad status (top left)
    bool gamepadConnected = IsGamepadAvailable(0);
    const char* gamepadStatus = gamepadConnected ? "Gamepad: CONNECTED" : "Gamepad: NOT CONNECTED";
    Color gamepadColor = gamepadConnected ? (Color){100, 255, 100, 255} : (Color){150, 150, 150, 255};
    int statusSize = 14;
    DrawText(gamepadStatus, 20, 20, statusSize, gamepadColor);
    
    // Draw title
    const char* title = "CONTROLS CONFIGURATION";
    int titleSize = 40;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 50, titleSize, WHITE);
    
    // Show modified indicator if controls were changed
    if (menu->controlsModified) {
        const char* modified = "* Configuration Modified - Remember to Save! *";
        int modSize = 16;
        int modWidth = MeasureText(modified, modSize);
        DrawText(modified, (screenWidth - modWidth) / 2, 100, modSize, YELLOW);
    }
    
    // Draw tabs below the title (upper right area)
    int tabY = 130;
    int tabHeight = 32;
    int tabWidth = 160;
    int tabSpacing = 8;
    int tabStartX = screenWidth - (tabWidth * 2 + tabSpacing + 20);
    
    // Keyboard tab
    Color keyboardTabColor = (menu->controlsTab == 0) ? (Color){50, 50, 100, 255} : (Color){20, 20, 40, 255};
    Color keyboardTextColor = (menu->controlsTab == 0) ? YELLOW : (Color){150, 150, 150, 255};
    DrawRectangle(tabStartX, tabY, tabWidth, tabHeight, keyboardTabColor);
    DrawRectangleLines(tabStartX, tabY, tabWidth, tabHeight, (menu->controlsTab == 0) ? YELLOW : GRAY);
    const char* keyboardText = "KEYBOARD";
    int keyboardTextWidth = MeasureText(keyboardText, 18);
    DrawText(keyboardText, tabStartX + (tabWidth - keyboardTextWidth) / 2, tabY + 7, 18, keyboardTextColor);
    
    // Tab indicator arrows
    if (menu->controlsTab == 0) {
        DrawText("<", tabStartX - 20, tabY + 6, 18, YELLOW);
    }
    
    // Gamepad tab
    Color gamepadTabColor = gamepadConnected ? 
        ((menu->controlsTab == 1) ? (Color){50, 50, 100, 255} : (Color){20, 20, 40, 255}) :
        (Color){10, 10, 20, 255};
    Color gamepadTextColor = gamepadConnected ?
        ((menu->controlsTab == 1) ? YELLOW : (Color){150, 150, 150, 255}) :
        (Color){80, 80, 80, 255};
    DrawRectangle(tabStartX + tabWidth + tabSpacing, tabY, tabWidth, tabHeight, gamepadTabColor);
    DrawRectangleLines(tabStartX + tabWidth + tabSpacing, tabY, tabWidth, tabHeight, 
                      gamepadConnected ? ((menu->controlsTab == 1) ? YELLOW : GRAY) : DARKGRAY);
    const char* gamepadText = gamepadConnected ? "GAMEPAD" : "GAMEPAD (N/A)";
    int gamepadTextWidth = MeasureText(gamepadText, 18);
    DrawText(gamepadText, tabStartX + tabWidth + tabSpacing + (tabWidth - gamepadTextWidth) / 2, 
             tabY + 7, 18, gamepadTextColor);
    
    // Tab indicator arrows
    if (menu->controlsTab == 1 && gamepadConnected) {
        DrawText(">", tabStartX + tabWidth * 2 + tabSpacing + 5, tabY + 6, 18, YELLOW);
    }
    
    // Show waiting for input overlay
    if (menu->waitingForInput) {
        // Darken background
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 180});
        
        // Draw prompt box
        int boxWidth = 500;
        int boxHeight = 150;
        int boxX = (screenWidth - boxWidth) / 2;
        int boxY = (screenHeight - boxHeight) / 2;
        
        DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){20, 20, 40, 255});
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, YELLOW);
        
        const char* prompt = (menu->controlsTab == 0) ? 
            "Press any key..." :
            "Press any gamepad button...";
        int promptSize = 28;
        
        int promptWidth = MeasureText(prompt, promptSize);
        DrawText(prompt, (screenWidth - promptWidth) / 2, boxY + 60, promptSize, WHITE);
        
        return;  // Don't draw the rest of the menu while waiting for input
    }
    
    // Draw actions list
    int startY = 180;
    int lineHeight = 40;
    int leftMargin = 80;
    
    if (!menu->inputManager || !menu->inputManager->config) {
        DrawText("Error: Input Manager not initialized", leftMargin, startY, 24, RED);
        return;
    }
    
    // Draw column headers
    DrawText("ACTION", leftMargin, startY - 25, 18, (Color){150, 150, 150, 255});
    DrawText("BINDINGS", leftMargin + 300, startY - 25, 18, (Color){150, 150, 150, 255});
    
    // Draw each action with binding (slot 0 for keyboard, slot 1 for gamepad)
    for (int i = 0; i < ACTION_COUNT; i++) {
        int y = startY + i * lineHeight;
        bool isSelected = (menu->selectedAction == i);
        
        // Action name
        const char* actionName = InputConfig_GetActionName((GameAction)i);
        Color actionColor = isSelected ? YELLOW : WHITE;
        DrawText(actionName, leftMargin, y, 20, actionColor);
        
        // Check binding for current tab (slot 0 = keyboard, slot 1 = gamepad)
        int slot = (menu->controlsTab == 0) ? 0 : 1;
        const InputBinding* binding = &menu->inputManager->config->bindings[i][slot];
        InputType filterType = (menu->controlsTab == 0) ? INPUT_TYPE_KEY : INPUT_TYPE_GAMEPAD_BUTTON;
        
        char bindingsText[64] = "";
        bool hasBinding = false;
        
        if (binding->type == filterType) {
            InputConfig_GetBindingName(binding, bindingsText, sizeof(bindingsText));
            hasBinding = true;
        } else if (binding->type == INPUT_TYPE_NONE) {
            strcpy(bindingsText, "< Not Bound >");
        } else {
            // Has binding but wrong type (shouldn't happen with separate slots)
            strcpy(bindingsText, "< Not Bound >");
        }
        
        // Color: Green if selected, Orange/Red if no binding, Gray if has binding but not selected
        Color bindingsColor;
        if (isSelected) {
            bindingsColor = hasBinding ? GREEN : ORANGE;
        } else {
            bindingsColor = hasBinding ? (Color){200, 200, 200, 255} : (Color){255, 150, 100, 255};
        }
        
        DrawText(bindingsText, leftMargin + 300, y, 18, bindingsColor);
        
        // Selection indicator
        if (isSelected) {
            float arrowOffset = sinf(menu->animationTimer * 4.0f) * 5.0f;
            DrawText(">", leftMargin - 25 - arrowOffset, y, 20, YELLOW);
        }
    }
    
    // Draw Reset and Save buttons (horizontally centered, same line)
    // Reset is ACTION_COUNT (selected first), Save is ACTION_COUNT + 1
    int buttonsY = startY + ACTION_COUNT * lineHeight + 20;
    int buttonSpacing = 30;
    
    // Reset button (left)
    bool resetSelected = (menu->selectedAction == ACTION_COUNT);
    const char* resetText = "[ RESET DEFAULTS ]";
    Color resetColor = resetSelected ? YELLOW : (Color){200, 100, 100, 255};
    int resetWidth = MeasureText(resetText, 20);
    
    // Save button (right)
    bool saveSelected = (menu->selectedAction == ACTION_COUNT + 1);
    const char* saveText = "[ SAVE CONFIGURATION ]";
    Color saveColor = saveSelected ? YELLOW : (Color){100, 200, 100, 255};
    int saveWidth = MeasureText(saveText, 20);
    
    // Calculate total width and center position
    int totalButtonWidth = resetWidth + buttonSpacing + saveWidth;
    int startButtonX = (screenWidth - totalButtonWidth) / 2;
    
    // Draw Reset button (left)
    int resetX = startButtonX;
    if (resetSelected) {
        DrawRectangle(resetX - 10, buttonsY - 5, resetWidth + 20, 30,
                     (Color){255, 255, 0, 30});
        float arrowOffset = sinf(menu->animationTimer * 4.0f) * 5.0f;
        DrawText(">", resetX - 30 - arrowOffset, buttonsY, 20, YELLOW);
    }
    DrawText(resetText, resetX, buttonsY, 20, resetColor);
    
    // Draw Save button (right)
    int saveX = resetX + resetWidth + buttonSpacing;
    if (saveSelected) {
        DrawRectangle(saveX - 10, buttonsY - 5, saveWidth + 20, 30,
                     (Color){255, 255, 0, 30});
        float arrowOffset = sinf(menu->animationTimer * 4.0f) * 5.0f;
        DrawText(">", saveX - 30 - arrowOffset, buttonsY, 20, YELLOW);
    }
    DrawText(saveText, saveX, buttonsY, 20, saveColor);
}

void DrawPauseConfirm(const Menu* menu) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Semi-transparent dark overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0, 0, 0, 200});
    
    // Draw dialog box
    int boxWidth = 600;
    int boxHeight = 350;
    int boxX = (screenWidth - boxWidth) / 2;
    int boxY = (screenHeight - boxHeight) / 2;
    
    // Box background and border
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){20, 20, 40, 255});
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, (Color){100, 100, 200, 255});
    DrawRectangleLines(boxX + 2, boxY + 2, boxWidth - 4, boxHeight - 4, (Color){70, 70, 150, 255});
    
    // Title
    const char* title = "GAME PAUSED";
    int titleSize = 48;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, boxX + (boxWidth - titleWidth) / 2, boxY + 40, titleSize, YELLOW);
    
    // Warning message
    const char* message = "Do you want to exit to the main menu?";
    int msgSize = 24;
    int msgWidth = MeasureText(message, msgSize);
    DrawText(message, boxX + (boxWidth - msgWidth) / 2, boxY + 120, msgSize, WHITE);
    
    // Options
    const char* options[] = {"RESUME GAME", "EXIT TO MENU"};
    int optionSize = 32;
    int optionSpacing = 80;
    int optionsStartY = boxY + 200;
    
    for (int i = 0; i < 2; i++) {
        int optionWidth = MeasureText(options[i], optionSize);
        int x = boxX + (boxWidth - optionWidth) / 2;
        int y = optionsStartY + i * optionSpacing;
        
        bool isSelected = (menu->pauseMenuOption == i);
        Color optionColor = isSelected ? YELLOW : (Color){200, 200, 200, 255};
        
        if (isSelected) {
            // Highlight background
            DrawRectangle(x - 15, y - 5, optionWidth + 30, optionSize + 10, 
                         (Color){255, 255, 0, 30});
            
            // Animated arrows
            float arrowOffset = sinf(menu->animationTimer * 4.0f) * 8.0f;
            DrawText(">", x - 50 - arrowOffset, y, optionSize, YELLOW);
            DrawText("<", x + optionWidth + 20 + arrowOffset, y, optionSize, YELLOW);
        }
        
        DrawText(options[i], x, y, optionSize, optionColor);
    }
}

