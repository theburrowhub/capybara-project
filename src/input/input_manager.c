#include "input_manager.h"
#include "raylib.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>

// Initialize input manager with a configuration
void InputManager_Init(InputManager* manager, InputConfig* config) {
    manager->config = config;
    manager->gamepadId = 0;  // Use first gamepad
    manager->gamepadAvailable = false;
    manager->activeInputMethod = INPUT_METHOD_KEYBOARD;  // Default to keyboard
}

// Update input manager state (call each frame)
void InputManager_Update(InputManager* manager) {
    // Check if gamepad is available
    manager->gamepadAvailable = IsGamepadAvailable(manager->gamepadId);
}

// Helper function to check if a single binding is pressed
static bool IsBindingPressed(const InputBinding* binding, int gamepadId, bool gamepadAvailable) {
    switch (binding->type) {
        case INPUT_TYPE_KEY:
            return IsKeyPressed(binding->value);
            
        case INPUT_TYPE_GAMEPAD_BUTTON:
            if (gamepadAvailable) {
                return IsGamepadButtonPressed(gamepadId, binding->value);
            }
            return false;
            
        case INPUT_TYPE_GAMEPAD_AXIS:
            if (gamepadAvailable) {
                float axisValue = GetGamepadAxisMovement(gamepadId, binding->value);
                // Check if axis crossed threshold this frame
                // This is a simplified version - might need improvement for better feel
                return fabsf(axisValue) > binding->axisThreshold;
            }
            return false;
            
        default:
            return false;
    }
}

// Helper function to check if a single binding is down
static bool IsBindingDown(const InputBinding* binding, int gamepadId, bool gamepadAvailable) {
    switch (binding->type) {
        case INPUT_TYPE_KEY:
            return IsKeyDown(binding->value);
            
        case INPUT_TYPE_GAMEPAD_BUTTON:
            if (gamepadAvailable) {
                return IsGamepadButtonDown(gamepadId, binding->value);
            }
            return false;
            
        case INPUT_TYPE_GAMEPAD_AXIS:
            if (gamepadAvailable) {
                float axisValue = GetGamepadAxisMovement(gamepadId, binding->value);
                return fabsf(axisValue) > binding->axisThreshold;
            }
            return false;
            
        default:
            return false;
    }
}

// Helper function to check if a single binding is released
static bool IsBindingReleased(const InputBinding* binding, int gamepadId, bool gamepadAvailable) {
    switch (binding->type) {
        case INPUT_TYPE_KEY:
            return IsKeyReleased(binding->value);
            
        case INPUT_TYPE_GAMEPAD_BUTTON:
            if (gamepadAvailable) {
                return IsGamepadButtonReleased(gamepadId, binding->value);
            }
            return false;
            
        case INPUT_TYPE_GAMEPAD_AXIS:
            // For axes, released means it went below threshold
            // This is simplified - might need improvement
            if (gamepadAvailable) {
                float axisValue = GetGamepadAxisMovement(gamepadId, binding->value);
                return fabsf(axisValue) < binding->axisThreshold;
            }
            return false;
            
        default:
            return false;
    }
}

// Check if an action was just pressed this frame
bool InputManager_IsActionPressed(const InputManager* manager, GameAction action) {
    if (action >= ACTION_COUNT) return false;
    
    // Check all bindings (up to 4 per action)
    for (int i = 0; i < MAX_BINDINGS_PER_ACTION; i++) {
        const InputBinding* binding = &manager->config->bindings[action][i];
        if (binding->type != INPUT_TYPE_NONE) {
            if (IsBindingPressed(binding, manager->gamepadId, manager->gamepadAvailable)) {
                // Track which input method was used
                if (binding->type == INPUT_TYPE_KEY) {
                    ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_KEYBOARD;
                } else if (binding->type == INPUT_TYPE_GAMEPAD_BUTTON || binding->type == INPUT_TYPE_GAMEPAD_AXIS) {
                    ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                }
                return true;
            }
        }
    }
    
    // Hardcoded gamepad support for critical actions (works alongside configured bindings)
    if (manager->gamepadAvailable) {
        if (action == ACTION_FIRE && IsGamepadButtonPressed(manager->gamepadId, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // R2 (Right Lower Button)
        }
        if (action == ACTION_SPECIAL_ABILITY && IsGamepadButtonPressed(manager->gamepadId, 8)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // X button (button 8)
        }
        if (action == ACTION_SWITCH_ENERGY_MODE && IsGamepadButtonPressed(manager->gamepadId, GAMEPAD_BUTTON_LEFT_TRIGGER_2)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // L2 (Left Lower Button)
        }
        if (action == ACTION_SWITCH_WEAPON_MODE && IsGamepadButtonPressed(manager->gamepadId, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // L1 (Left Upper Button)
        }
    }
    
    return false;
}

// Check if an action is currently held down
bool InputManager_IsActionDown(const InputManager* manager, GameAction action) {
    if (action >= ACTION_COUNT) return false;
    
    // Check all bindings (up to 4 per action)
    for (int i = 0; i < MAX_BINDINGS_PER_ACTION; i++) {
        const InputBinding* binding = &manager->config->bindings[action][i];
        if (binding->type != INPUT_TYPE_NONE) {
            if (IsBindingDown(binding, manager->gamepadId, manager->gamepadAvailable)) {
                // Track which input method was used
                if (binding->type == INPUT_TYPE_KEY) {
                    ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_KEYBOARD;
                } else if (binding->type == INPUT_TYPE_GAMEPAD_BUTTON || binding->type == INPUT_TYPE_GAMEPAD_AXIS) {
                    ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                }
                return true;
            }
        }
    }
    
    // Hardcoded gamepad support (works alongside configured bindings)
    if (manager->gamepadAvailable) {
        // Movement: Always check left analog stick
        float threshold = 0.3f;  // Lower threshold for better responsiveness
        
        if (action == ACTION_MOVE_UP) {
            float axisY = GetGamepadAxisMovement(manager->gamepadId, GAMEPAD_AXIS_LEFT_Y);
            if (axisY < -threshold) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
            // Also check D-Pad up
            if (IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
        } else if (action == ACTION_MOVE_DOWN) {
            float axisY = GetGamepadAxisMovement(manager->gamepadId, GAMEPAD_AXIS_LEFT_Y);
            if (axisY > threshold) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
            // Also check D-Pad down
            if (IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
        } else if (action == ACTION_MOVE_LEFT) {
            float axisX = GetGamepadAxisMovement(manager->gamepadId, GAMEPAD_AXIS_LEFT_X);
            if (axisX < -threshold) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
            // Also check D-Pad left
            if (IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
        } else if (action == ACTION_MOVE_RIGHT) {
            float axisX = GetGamepadAxisMovement(manager->gamepadId, GAMEPAD_AXIS_LEFT_X);
            if (axisX > threshold) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
            // Also check D-Pad right
            if (IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
                ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
                return true;
            }
        }
        
        // Fire: Always check R2 (Right Lower Button)
        if (action == ACTION_FIRE && IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // R2
        }
        
        // Special Ability: Always check X button
        if (action == ACTION_SPECIAL_ABILITY && IsGamepadButtonDown(manager->gamepadId, 8)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // X button (button 8)
        }
        
        // Switch Energy Mode: Always check L2 (Left Lower Button)
        if (action == ACTION_SWITCH_ENERGY_MODE && IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_LEFT_TRIGGER_2)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // L2
        }
        
        // Switch Weapon Mode: Always check L1 (Left Upper Button)
        if (action == ACTION_SWITCH_WEAPON_MODE && IsGamepadButtonDown(manager->gamepadId, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
            ((InputManager*)manager)->activeInputMethod = INPUT_METHOD_GAMEPAD;
            return true;  // L1
        }
    }
    
    return false;
}

// Check if an action was just released this frame
bool InputManager_IsActionReleased(const InputManager* manager, GameAction action) {
    if (action >= ACTION_COUNT) return false;
    
    // Check all bindings (up to 4 per action)
    for (int i = 0; i < MAX_BINDINGS_PER_ACTION; i++) {
        const InputBinding* binding = &manager->config->bindings[action][i];
        if (binding->type != INPUT_TYPE_NONE) {
            if (IsBindingReleased(binding, manager->gamepadId, manager->gamepadAvailable)) {
                return true;
            }
        }
    }
    
    // Hardcoded gamepad support for critical actions
    if (manager->gamepadAvailable) {
        if (action == ACTION_FIRE && IsGamepadButtonReleased(manager->gamepadId, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) {
            return true;  // R2 (Right Lower Button)
        }
        if (action == ACTION_SPECIAL_ABILITY && IsGamepadButtonReleased(manager->gamepadId, 8)) {
            return true;  // X button (button 8)
        }
    }
    
    return false;
}

// Detect any input and return the binding (for rebinding controls)
// Note: Mouse support has been removed - only keyboard and gamepad
// ESC and B button ARE bindable (cancel is handled before this function in menu.c)
bool InputManager_DetectInput(InputBinding* outBinding) {
    // Check keyboard keys (common keys only)
    for (int key = 32; key <= 96; key++) {  // Space to `
        if (IsKeyPressed(key)) {
            outBinding->type = INPUT_TYPE_KEY;
            outBinding->value = key;
            outBinding->axisThreshold = 0.5f;
            return true;
        }
    }
    
    // Check arrow keys and special keys (including ESC - it's bindable for game actions)
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
    
    // Check gamepad buttons (if gamepad is available)
    if (IsGamepadAvailable(0)) {
        // Check all standard gamepad buttons (including B button - it's bindable)
        for (int button = 0; button < 32; button++) {
            if (IsGamepadButtonPressed(0, button)) {
                outBinding->type = INPUT_TYPE_GAMEPAD_BUTTON;
                outBinding->value = button;
                outBinding->axisThreshold = 0.5f;
                return true;
            }
        }
        
        // Note: Gamepad axes (sticks) are hardcoded for movement, so we don't detect them
        // for configuration to avoid accidental triggers during rebinding
    }
    
    return false;
}

// Get the active input method (keyboard or gamepad)
ActiveInputMethod InputManager_GetActiveInputMethod(const InputManager* manager) {
    return manager->activeInputMethod;
}

// Helper function to get a short gamepad button name
static const char* GetGamepadButtonShortName(int button) {
    switch (button) {
        // Face buttons (right side)
        case GAMEPAD_BUTTON_RIGHT_FACE_DOWN: return "A";      // Xbox A, PS Cross
        case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT: return "B";     // Xbox B, PS Circle
        case GAMEPAD_BUTTON_RIGHT_FACE_LEFT: return "X";      // Xbox X, PS Square
        case GAMEPAD_BUTTON_RIGHT_FACE_UP: return "Y";        // Xbox Y, PS Triangle
        
        // Shoulder buttons
        case GAMEPAD_BUTTON_LEFT_TRIGGER_1: return "LB";      // Left Bumper
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_1: return "RB";     // Right Bumper
        case GAMEPAD_BUTTON_LEFT_TRIGGER_2: return "LT";      // Left Trigger
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_2: return "RT";     // Right Trigger
        
        // D-Pad
        case GAMEPAD_BUTTON_LEFT_FACE_UP: return "D-Up";
        case GAMEPAD_BUTTON_LEFT_FACE_DOWN: return "D-Down";
        case GAMEPAD_BUTTON_LEFT_FACE_LEFT: return "D-Left";
        case GAMEPAD_BUTTON_LEFT_FACE_RIGHT: return "D-Right";
        
        // Special buttons
        case GAMEPAD_BUTTON_MIDDLE_LEFT: return "Back";       // Back/Select
        case GAMEPAD_BUTTON_MIDDLE: return "Guide";           // Xbox/PS button
        case GAMEPAD_BUTTON_MIDDLE_RIGHT: return "Start";     // Start
        
        // Stick clicks
        case GAMEPAD_BUTTON_LEFT_THUMB: return "L-Stick";
        case GAMEPAD_BUTTON_RIGHT_THUMB: return "R-Stick";
        
        default: 
            return "Button";
    }
}

// Get a readable string for an action based on current bindings and active input method
void InputManager_GetActionString(const InputManager* manager, GameAction action, char* buffer, int bufferSize) {
    if (action >= ACTION_COUNT || !manager || !manager->config) {
        snprintf(buffer, bufferSize, "???");
        return;
    }
    
    // Determine which slot to check based on active input method
    int slot = (manager->activeInputMethod == INPUT_METHOD_KEYBOARD) ? 0 : 1;
    const InputBinding* binding = &manager->config->bindings[action][slot];
    
    // If using gamepad method and we have hardcoded fallbacks for some actions
    if (manager->activeInputMethod == INPUT_METHOD_GAMEPAD && manager->gamepadAvailable) {
        // Check for hardcoded gamepad controls first
        if (action == ACTION_MOVE_UP || action == ACTION_MOVE_DOWN || 
            action == ACTION_MOVE_LEFT || action == ACTION_MOVE_RIGHT) {
            snprintf(buffer, bufferSize, "L-Stick");
            return;
        } else if (action == ACTION_FIRE) {
            snprintf(buffer, bufferSize, "RT");
            return;
        } else if (action == ACTION_SPECIAL_ABILITY) {
            snprintf(buffer, bufferSize, "X");
            return;
        } else if (action == ACTION_SWITCH_ENERGY_MODE) {
            snprintf(buffer, bufferSize, "LT");
            return;
        } else if (action == ACTION_SWITCH_WEAPON_MODE) {
            snprintf(buffer, bufferSize, "LB");
            return;
        }
    }
    
    // Try to get the binding name from configured bindings
    if (binding->type != INPUT_TYPE_NONE) {
        InputConfig_GetBindingName(binding, buffer, bufferSize);
    } else {
        // No binding configured for this slot, try to show something useful
        snprintf(buffer, bufferSize, "---");
    }
}

