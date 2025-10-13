#include "menu.h"
#include "constants.h"
#include "database.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

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
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) {
                    menu->selectedOption = MENU_TOTAL_OPTIONS - 1;
                }
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                menu->selectedOption++;
                if (menu->selectedOption >= MENU_TOTAL_OPTIONS) {
                    menu->selectedOption = 0;
                }
            }
            
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
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
                }
            }
            break;
            
        case MENU_OPTIONS:
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) menu->selectedOption = 2;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                menu->selectedOption++;
                if (menu->selectedOption > 2) menu->selectedOption = 0;
            }
            
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
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
                }
            }
            
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_OPTIONS;
            }
            break;
            
        case MENU_OPTIONS_SOUND: {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) menu->selectedOption = 1;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                menu->selectedOption++;
                if (menu->selectedOption > 1) menu->selectedOption = 0;
            }
            
            bool settingsChanged = false;
            if (menu->selectedOption == 0) {
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                    menu->soundVolume = fmaxf(0.0f, menu->soundVolume - 0.1f);
                    settingsChanged = true;
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                    menu->soundVolume = fminf(1.0f, menu->soundVolume + 0.1f);
                    settingsChanged = true;
                }
            } else if (menu->selectedOption == 1) {
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                    menu->musicVolume = fmaxf(0.0f, menu->musicVolume - 0.1f);
                    settingsChanged = true;
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
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
            
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                menu->currentState = MENU_OPTIONS;
                menu->selectedOption = 0;
            }
            break;
        }
            
         case MENU_OPTIONS_VIDEO: {
             int count = 0;
             GetAvailableResolutions(&count);
             
             if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                 menu->selectedOption--;
                 if (menu->selectedOption < 0) menu->selectedOption = 2;
             }
             if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                 menu->selectedOption++;
                 if (menu->selectedOption > 2) menu->selectedOption = 0;
             }
             
             bool settingsChanged = false;
             if (menu->selectedOption == 0) {
                 if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                     menu->selectedResolution--;
                     if (menu->selectedResolution < 0) menu->selectedResolution = count - 1;
                     ApplyResolution(menu);
                     settingsChanged = true;
                 }
                 if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                     menu->selectedResolution++;
                     if (menu->selectedResolution >= count) menu->selectedResolution = 0;
                     ApplyResolution(menu);
                     settingsChanged = true;
                 }
             } else if (menu->selectedOption == 1) {
                 if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                     int mode = (int)menu->fullscreenMode - 1;
                     if (mode < 0) mode = 2;
                     menu->fullscreenMode = (FullscreenMode)mode;
                     ApplyFullscreenMode(menu);
                     settingsChanged = true;
                 }
                 if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                     int mode = (int)menu->fullscreenMode + 1;
                     if (mode > 2) mode = 0;
                     menu->fullscreenMode = (FullscreenMode)mode;
                     ApplyFullscreenMode(menu);
                     settingsChanged = true;
                 }
             } else if (menu->selectedOption == 2) {
                 if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || 
                     IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) ||
                     IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
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
            
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                menu->currentState = MENU_OPTIONS;
                menu->selectedOption = 1;
            }
            break;
        }
            
        case MENU_OPTIONS_GAME:
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                menu->currentState = MENU_OPTIONS;
                menu->selectedOption = 2;
            }
            break;
            
        case MENU_HIGH_SCORES:
            // Navigate between difficulties with left/right arrows
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                menu->selectedDifficulty--;
                if (menu->selectedDifficulty < 0) {
                    menu->selectedDifficulty = 3;  // Wrap to INSANE
                }
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                menu->selectedDifficulty++;
                if (menu->selectedDifficulty > 3) {
                    menu->selectedDifficulty = 0;  // Wrap to EASY
                }
            }
            
            // Go back with ESC, Backspace, Enter or Space
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || 
                IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_HIGH_SCORES;
            }
            break;
            
        case MENU_CREDITS:
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || 
                IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_CREDITS;
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
        case MENU_HIGH_SCORES:
            DrawHighScores((Menu*)menu);
            break;
        case MENU_CREDITS:
            DrawCredits(menu);
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
        "CREDITS"
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
    
    // Draw instructions
    const char* instructions = "Use ARROW KEYS to navigate, ENTER to select";
    int instructionSize = 16;
    int instructionWidth = MeasureText(instructions, instructionSize);
    DrawText(instructions, (screenWidth - instructionWidth) / 2, 
             screenHeight - 50, instructionSize, (Color){150, 150, 150, 200});
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
        "GAME OPTIONS"
    };
    
    int optionSize = 30;
    int optionSpacing = 60;
    int startY = 200;
    
    for (int i = 0; i < 3; i++) {
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
    
    const char* instructions = "Use ARROW KEYS to navigate, ENTER to select, ESC to go back";
    int instructionSize = 16;
    int instructionWidth = MeasureText(instructions, instructionSize);
    DrawText(instructions, (screenWidth - instructionWidth) / 2, 
             screenHeight - 50, instructionSize, (Color){150, 150, 150, 200});
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
    
    const char* instructions = "Use ARROW KEYS to navigate, LEFT/RIGHT to adjust, ESC to go back";
    int instructionSize = 16;
    int instructionWidth = MeasureText(instructions, instructionSize);
    DrawText(instructions, (screenWidth - instructionWidth) / 2, 
             screenHeight - 50, instructionSize, (Color){150, 150, 150, 200});
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
    
    // Info text
    const char* infoText = "Press F11 or Alt+Enter for quick fullscreen toggle";
    int infoSize = 14;
    int infoWidth = MeasureText(infoText, infoSize);
    DrawText(infoText, (screenWidth - infoWidth) / 2, 
             screenHeight - 90, infoSize, (Color){100, 150, 255, 200});
    
    const char* instructions = "Use ARROW KEYS to navigate, LEFT/RIGHT to change, ESC to go back";
    int instructionSize = 16;
    int instructionWidth = MeasureText(instructions, instructionSize);
    DrawText(instructions, (screenWidth - instructionWidth) / 2, 
             screenHeight - 50, instructionSize, (Color){150, 150, 150, 200});
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
    
    const char* instructions = "Press ESC to go back";
    int instructionSize = 16;
    int instructionWidth = MeasureText(instructions, instructionSize);
    DrawText(instructions, (screenWidth - instructionWidth) / 2, 
             screenHeight - 50, instructionSize, (Color){150, 150, 150, 200});
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
    
    // Draw navigation instructions
    const char* navText = "Use LEFT/RIGHT arrows to change difficulty";
    int navSize = 14;
    int navWidth = MeasureText(navText, navSize);
    DrawText(navText, (screenWidth - navWidth) / 2, 
             screenHeight - 80, navSize, (Color){100, 150, 255, 200});
    
    // Draw back instruction
    const char* backText = "Press ESC or ENTER to go back";
    int backSize = 16;
    int backWidth = MeasureText(backText, backSize);
    DrawText(backText, (screenWidth - backWidth) / 2, 
             screenHeight - 50, backSize, (Color){150, 150, 150, 200});
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
    
    // Draw back instruction
    const char* backText = "Press ESC or ENTER to go back";
    int backSize = 18;
    int backWidth = MeasureText(backText, backSize);
    DrawText(backText, (screenWidth - backWidth) / 2, 
             screenHeight - 50, backSize, (Color){150, 150, 150, 200});
}
