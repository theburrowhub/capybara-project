#include "menu.h"
#include "constants.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

void InitMenu(Menu* menu) {
    menu->currentState = MENU_MAIN;
    menu->selectedOption = 0;
    menu->titleBounce = 0.0f;
    menu->animationTimer = 0.0f;
    
    // Initialize option alphas
    for (int i = 0; i < MENU_TOTAL_OPTIONS; i++) {
        menu->optionAlpha[i] = 0.8f;
    }
    
    // Default options
    menu->soundVolume = 1.0f;
    menu->musicVolume = 0.5f;
    menu->fullscreen = false;
}

void UpdateMenu(Menu* menu, MenuState* gameState) {
    float deltaTime = GetFrameTime();
    menu->animationTimer += deltaTime;
    
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
            // Navigate options with arrow keys
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
            
            // Select option with Enter or Space
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                switch (menu->selectedOption) {
                    case MENU_START_GAME:
                        *gameState = MENU_GAME;
                        break;
                    case MENU_SHOW_OPTIONS:
                        menu->currentState = MENU_OPTIONS;
                        break;
                    case MENU_SHOW_CREDITS:
                        menu->currentState = MENU_CREDITS;
                        break;
                }
            }
            break;
            
        case MENU_OPTIONS:
            // Update options values with arrow keys
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                menu->selectedOption--;
                if (menu->selectedOption < 0) menu->selectedOption = 2;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                menu->selectedOption++;
                if (menu->selectedOption > 2) menu->selectedOption = 0;
            }
            
            // Adjust values with left/right
            if (menu->selectedOption == 0) { // Sound volume
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                    menu->soundVolume = fmaxf(0.0f, menu->soundVolume - 0.1f);
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                    menu->soundVolume = fminf(1.0f, menu->soundVolume + 0.1f);
                }
            } else if (menu->selectedOption == 1) { // Music volume
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                    menu->musicVolume = fmaxf(0.0f, menu->musicVolume - 0.1f);
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                    menu->musicVolume = fminf(1.0f, menu->musicVolume + 0.1f);
                }
            } else if (menu->selectedOption == 2) { // Fullscreen
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || 
                    IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D) ||
                    IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    menu->fullscreen = !menu->fullscreen;
                    if (menu->fullscreen) {
                        ToggleFullscreen();
                    } else {
                        ToggleFullscreen();
                    }
                }
            }
            
            // Go back with ESC or Backspace
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_OPTIONS;
            }
            break;
            
        case MENU_CREDITS:
            // Go back with any key
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || 
                IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                menu->currentState = MENU_MAIN;
                menu->selectedOption = MENU_SHOW_CREDITS;
            }
            break;
            
        case MENU_GAME:
            // Game is running
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
        case MENU_CREDITS:
            DrawCredits(menu);
            break;
        case MENU_GAME:
            // Game is rendering
            break;
    }
}

void DrawMainMenu(const Menu* menu) {
    // Draw background with gradient
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Draw animated stars in background
    for (int i = 0; i < 100; i++) {
        int x = (i * 73 + (int)(menu->animationTimer * 20)) % SCREEN_WIDTH;
        int y = (i * 37) % SCREEN_HEIGHT;
        int brightness = 100 + (i % 3) * 50;
        DrawPixel(x, y, (Color){brightness, brightness, brightness, 255});
    }
    
    // Draw title with bounce effect
    const char* title = "CAPIBARA PROJECT";
    int titleSize = 60;
    int titleWidth = MeasureText(title, titleSize);
    float titleY = 100 + menu->titleBounce;
    
    // Draw title shadow
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2 + 3, titleY + 3, 
             titleSize, (Color){0, 0, 0, 128});
    
    // Draw title with gradient effect
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, titleY, 
             titleSize, (Color){255, 100, 100, 255});
    
    // Menu options
    const char* options[MENU_TOTAL_OPTIONS] = {
        "START",
        "SETTINGS",
        "CREDITS"
    };
    
    int optionSize = 30;
    int optionSpacing = 60;
    int startY = 280;
    
    for (int i = 0; i < MENU_TOTAL_OPTIONS; i++) {
        int optionWidth = MeasureText(options[i], optionSize);
        int x = (SCREEN_WIDTH - optionWidth) / 2;
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
    DrawText(instructions, (SCREEN_WIDTH - instructionWidth) / 2, 
             SCREEN_HEIGHT - 50, instructionSize, (Color){150, 150, 150, 200});
}

void DrawOptions(Menu* menu) {
    // Draw background
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                           (Color){10, 10, 30, 255}, (Color){30, 10, 60, 255});
    
    // Draw title
    const char* title = "OPTIONS";
    int titleSize = 50;
    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, (SCREEN_WIDTH - titleWidth) / 2, 80, titleSize, WHITE);
    
    // Option settings
    int optionSize = 24;
    int startY = 200;
    int spacing = 80;
    
    // Sound Volume
    const char* soundText = "Sound Volume:";
    int soundY = startY;
    Color soundColor = (menu->selectedOption == 0) ? YELLOW : WHITE;
    DrawText(soundText, 200, soundY, optionSize, soundColor);
    
    // Draw sound volume bar
    int barWidth = 200;
    int barHeight = 20;
    int barX = 450;
    DrawRectangle(barX, soundY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, soundY, (int)(barWidth * menu->soundVolume), barHeight, GREEN);
    DrawRectangleLines(barX, soundY, barWidth, barHeight, WHITE);
    
    // Draw percentage
    char volumeText[16];
    snprintf(volumeText, sizeof(volumeText), "%d%%", (int)(menu->soundVolume * 100));
    DrawText(volumeText, barX + barWidth + 20, soundY, optionSize, WHITE);
    
    // Music Volume
    const char* musicText = "Music Volume:";
    int musicY = startY + spacing;
    Color musicColor = (menu->selectedOption == 1) ? YELLOW : WHITE;
    DrawText(musicText, 200, musicY, optionSize, musicColor);
    
    // Draw music volume bar
    DrawRectangle(barX, musicY, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barX, musicY, (int)(barWidth * menu->musicVolume), barHeight, GREEN);
    DrawRectangleLines(barX, musicY, barWidth, barHeight, WHITE);
    
    // Draw percentage
    snprintf(volumeText, sizeof(volumeText), "%d%%", (int)(menu->musicVolume * 100));
    DrawText(volumeText, barX + barWidth + 20, musicY, optionSize, WHITE);
    
    // Fullscreen
    const char* fullscreenText = "Fullscreen:";
    int fullscreenY = startY + spacing * 2;
    Color fullscreenColor = (menu->selectedOption == 2) ? YELLOW : WHITE;
    DrawText(fullscreenText, 200, fullscreenY, optionSize, fullscreenColor);
    
    // Draw checkbox
    int checkboxSize = 20;
    DrawRectangleLines(barX, fullscreenY, checkboxSize, checkboxSize, WHITE);
    if (menu->fullscreen) {
        DrawRectangle(barX + 3, fullscreenY + 3, checkboxSize - 6, checkboxSize - 6, GREEN);
    }
    const char* fullscreenStatus = menu->fullscreen ? "ON" : "OFF";
    DrawText(fullscreenStatus, barX + checkboxSize + 10, fullscreenY, optionSize, WHITE);
    
    // Draw instructions
    const char* instructions = "Use ARROW KEYS to navigate, LEFT/RIGHT to adjust, ESC to go back";
    int instructionSize = 16;
    int instructionWidth = MeasureText(instructions, instructionSize);
    DrawText(instructions, (SCREEN_WIDTH - instructionWidth) / 2, 
             SCREEN_HEIGHT - 50, instructionSize, (Color){150, 150, 150, 200});
}

void DrawCredits(const Menu* menu) {
    // Draw background
    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
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
            int x = (SCREEN_WIDTH - textWidth) / 2;
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
    DrawText(backText, (SCREEN_WIDTH - backWidth) / 2, 
             SCREEN_HEIGHT - 50, backSize, (Color){150, 150, 150, 200});
}
