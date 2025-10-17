#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "raylib.h"
#include "input_config.h"
#include <stdbool.h>

// Input manager state
typedef struct InputManager {
    InputConfig* config;
    int gamepadId;  // Which gamepad to use (0 by default)
    bool gamepadAvailable;
} InputManager;

// Initialize input manager with a configuration
void InputManager_Init(InputManager* manager, InputConfig* config);

// Update input manager state (call each frame)
void InputManager_Update(InputManager* manager);

// Check if an action was just pressed this frame
bool InputManager_IsActionPressed(const InputManager* manager, GameAction action);

// Check if an action is currently held down
bool InputManager_IsActionDown(const InputManager* manager, GameAction action);

// Check if an action was just released this frame
bool InputManager_IsActionReleased(const InputManager* manager, GameAction action);

// Detect any input and return the binding (for rebinding controls)
// Returns true if an input was detected, false otherwise
bool InputManager_DetectInput(InputBinding* outBinding);

#endif // INPUT_MANAGER_H

