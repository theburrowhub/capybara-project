#include "input_config.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Initialize configuration with default bindings
void InputConfig_InitDefaults(InputConfig* config) {
    // Clear all bindings
    memset(config, 0, sizeof(InputConfig));
    
    // Move Up: UP Arrow key (keyboard), D-Pad Up (gamepad)
    config->bindings[ACTION_MOVE_UP][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_MOVE_UP][0].value = KEY_UP;
    config->bindings[ACTION_MOVE_UP][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_MOVE_UP][1].value = GAMEPAD_BUTTON_LEFT_FACE_UP;
    
    // Move Down: DOWN Arrow key (keyboard), D-Pad Down (gamepad)
    config->bindings[ACTION_MOVE_DOWN][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_MOVE_DOWN][0].value = KEY_DOWN;
    config->bindings[ACTION_MOVE_DOWN][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_MOVE_DOWN][1].value = GAMEPAD_BUTTON_LEFT_FACE_DOWN;
    
    // Move Left: LEFT Arrow key (keyboard), D-Pad Left (gamepad)
    config->bindings[ACTION_MOVE_LEFT][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_MOVE_LEFT][0].value = KEY_LEFT;
    config->bindings[ACTION_MOVE_LEFT][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_MOVE_LEFT][1].value = GAMEPAD_BUTTON_LEFT_FACE_LEFT;
    
    // Move Right: RIGHT Arrow key (keyboard), D-Pad Right (gamepad)
    config->bindings[ACTION_MOVE_RIGHT][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_MOVE_RIGHT][0].value = KEY_RIGHT;
    config->bindings[ACTION_MOVE_RIGHT][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_MOVE_RIGHT][1].value = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;
    
    // Fire: Space key (keyboard), R2 button (gamepad)
    config->bindings[ACTION_FIRE][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_FIRE][0].value = KEY_SPACE;
    config->bindings[ACTION_FIRE][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_FIRE][1].value = GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
    
    // Special Ability: E key (keyboard), X button (gamepad - button 8)
    config->bindings[ACTION_SPECIAL_ABILITY][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_SPECIAL_ABILITY][0].value = KEY_E;
    config->bindings[ACTION_SPECIAL_ABILITY][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_SPECIAL_ABILITY][1].value = 8;  // X button on this gamepad
    
    // Switch Energy Mode: Q key (keyboard), L2 button (gamepad)
    config->bindings[ACTION_SWITCH_ENERGY_MODE][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_SWITCH_ENERGY_MODE][0].value = KEY_Q;
    config->bindings[ACTION_SWITCH_ENERGY_MODE][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_SWITCH_ENERGY_MODE][1].value = GAMEPAD_BUTTON_LEFT_TRIGGER_2;
    
    // Switch Weapon Mode: R key (keyboard), L1 button (gamepad)
    config->bindings[ACTION_SWITCH_WEAPON_MODE][0].type = INPUT_TYPE_KEY;
    config->bindings[ACTION_SWITCH_WEAPON_MODE][0].value = KEY_R;
    config->bindings[ACTION_SWITCH_WEAPON_MODE][1].type = INPUT_TYPE_GAMEPAD_BUTTON;
    config->bindings[ACTION_SWITCH_WEAPON_MODE][1].value = GAMEPAD_BUTTON_LEFT_TRIGGER_1;
}

// Load configuration from file
bool InputConfig_Load(InputConfig* config, const char* configFilePath) {
    // Set defaults first
    InputConfig_InitDefaults(config);
    
    // Use provided path or default
    const char* configPath = (configFilePath != NULL) ? configFilePath : INPUT_CONFIG_FILE_DEFAULT;
    
    FILE* file = fopen(configPath, "r");
    if (file == NULL) {
        printf("[INPUT CONFIG] No config file found at '%s', using defaults\n", configPath);
        return false;  // No config file, use defaults
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        // Parse key=value pairs
        char key[64];
        char typeStr[32];
        int value;
        
        // Format: action_slot=TYPE:value
        if (sscanf(line, "%63[^=]=%31[^:]:%d", key, typeStr, &value) == 3) {
            // Parse action and slot from key
            GameAction action = ACTION_COUNT;
            int slot = -1;
            
            // Parse action and slot from key (supports up to 4 slots: 0-3)
            const char* actionNames[] = {
                "move_up", "move_down", "move_left", "move_right",
                "fire", "special", "switch_energy", "switch_weapon"
            };
            GameAction actions[] = {
                ACTION_MOVE_UP, ACTION_MOVE_DOWN, ACTION_MOVE_LEFT, ACTION_MOVE_RIGHT,
                ACTION_FIRE, ACTION_SPECIAL_ABILITY, 
                ACTION_SWITCH_ENERGY_MODE, ACTION_SWITCH_WEAPON_MODE
            };
            
            // Try to match action name with slot (0 or 1)
            for (int i = 0; i < 8; i++) {
                for (int s = 0; s < 2; s++) {  // Only slots 0 and 1
                    char keyPattern[64];
                    snprintf(keyPattern, sizeof(keyPattern), "%s_%d", actionNames[i], s);
                    if (strstr(key, keyPattern)) {
                        action = actions[i];
                        slot = s;
                        break;
                    }
                }
                if (action != ACTION_COUNT) break;
            }
            
            // Parse input type
            InputType type = INPUT_TYPE_NONE;
            if (strcmp(typeStr, "KEY") == 0) type = INPUT_TYPE_KEY;
            else if (strcmp(typeStr, "GAMEPAD_BUTTON") == 0) type = INPUT_TYPE_GAMEPAD_BUTTON;
            else if (strcmp(typeStr, "GAMEPAD_AXIS") == 0) type = INPUT_TYPE_GAMEPAD_AXIS;
            
            // Set the binding (slot 0 for keyboard, slot 1 for gamepad)
            if (action < ACTION_COUNT && slot >= 0 && slot < 2 && type != INPUT_TYPE_NONE) {
                config->bindings[action][slot].type = type;
                config->bindings[action][slot].value = value;
            }
        }
    }
    
    fclose(file);
    printf("[INPUT CONFIG] Configuration loaded from: %s\n", configPath);
    return true;
}

// Helper to get type string
static const char* typeToString(InputType type) {
    switch (type) {
        case INPUT_TYPE_KEY: return "KEY";
        case INPUT_TYPE_MOUSE: return "MOUSE";
        case INPUT_TYPE_GAMEPAD_BUTTON: return "GAMEPAD_BUTTON";
        case INPUT_TYPE_GAMEPAD_AXIS: return "GAMEPAD_AXIS";
        default: return "NONE";
    }
}

// Helper to get action key name
static const char* getActionKey(GameAction action, int slot) {
    static char buffer[64];
    const char* actionName = "";
    
    switch (action) {
        case ACTION_MOVE_UP: actionName = "move_up"; break;
        case ACTION_MOVE_DOWN: actionName = "move_down"; break;
        case ACTION_MOVE_LEFT: actionName = "move_left"; break;
        case ACTION_MOVE_RIGHT: actionName = "move_right"; break;
        case ACTION_FIRE: actionName = "fire"; break;
        case ACTION_SPECIAL_ABILITY: actionName = "special"; break;
        case ACTION_SWITCH_ENERGY_MODE: actionName = "switch_energy"; break;
        case ACTION_SWITCH_WEAPON_MODE: actionName = "switch_weapon"; break;
        default: actionName = "unknown"; break;
    }
    
    snprintf(buffer, sizeof(buffer), "%s_%d", actionName, slot);
    return buffer;
}

// Save configuration to file
bool InputConfig_Save(const InputConfig* config, const char* configFilePath) {
    // Use provided path or default
    const char* configPath = (configFilePath != NULL) ? configFilePath : INPUT_CONFIG_FILE_DEFAULT;
    
    FILE* file = fopen(configPath, "w");
    if (file == NULL) {
        printf("[INPUT CONFIG] Warning: Could not save configuration file: %s\n", configPath);
        return false;
    }
    
    fprintf(file, "# Input Configuration\n");
    fprintf(file, "# Format: action_slot=type:value\n");
    fprintf(file, "# Slot 0 = Keyboard, Slot 1 = Gamepad\n");
    fprintf(file, "# Types: KEY (keyboard), GAMEPAD_BUTTON\n\n");
    
    // Write both keyboard (slot 0) and gamepad (slot 1) bindings
    for (int action = 0; action < ACTION_COUNT; action++) {
        // Keyboard binding (slot 0)
        const InputBinding* keyboardBinding = &config->bindings[action][0];
        if (keyboardBinding->type != INPUT_TYPE_NONE) {
            fprintf(file, "%s=%s:%d\n", 
                   getActionKey(action, 0),
                   typeToString(keyboardBinding->type),
                   keyboardBinding->value);
        }
        
        // Gamepad binding (slot 1)
        const InputBinding* gamepadBinding = &config->bindings[action][1];
        if (gamepadBinding->type != INPUT_TYPE_NONE) {
            fprintf(file, "%s=%s:%d\n", 
                   getActionKey(action, 1),
                   typeToString(gamepadBinding->type),
                   gamepadBinding->value);
        }
    }
    
    fclose(file);
    printf("[INPUT CONFIG] Configuration saved to: %s\n", configPath);
    return true;
}

// Set a binding for an action
void InputConfig_SetBinding(InputConfig* config, GameAction action, int slot, InputType type, int value) {
    if (action >= ACTION_COUNT || slot < 0 || slot >= 2) return;  // Only slots 0-1 used
    
    config->bindings[action][slot].type = type;
    config->bindings[action][slot].value = value;
    config->bindings[action][slot].axisThreshold = 0.5f;  // Default threshold for axes
}

// Get readable name for an action
const char* InputConfig_GetActionName(GameAction action) {
    switch (action) {
        case ACTION_MOVE_UP: return "Move Up";
        case ACTION_MOVE_DOWN: return "Move Down";
        case ACTION_MOVE_LEFT: return "Move Left";
        case ACTION_MOVE_RIGHT: return "Move Right";
        case ACTION_FIRE: return "Fire";
        case ACTION_SPECIAL_ABILITY: return "Special Ability";
        case ACTION_SWITCH_ENERGY_MODE: return "Switch Energy Mode";
        case ACTION_SWITCH_WEAPON_MODE: return "Switch Weapon Mode";
        default: return "Unknown";
    }
}

// Get readable key name from key code
const char* InputConfig_GetKeyName(int keyCode) {
    // Common keys with readable names
    switch (keyCode) {
        case KEY_SPACE: return "Space";
        case KEY_ENTER: return "Enter";
        case KEY_TAB: return "Tab";
        case KEY_BACKSPACE: return "Backspace";
        case KEY_ESCAPE: return "Escape";
        case KEY_UP: return "Up Arrow";
        case KEY_DOWN: return "Down Arrow";
        case KEY_LEFT: return "Left Arrow";
        case KEY_RIGHT: return "Right Arrow";
        case KEY_LEFT_SHIFT: return "Left Shift";
        case KEY_RIGHT_SHIFT: return "Right Shift";
        case KEY_LEFT_CONTROL: return "Left Ctrl";
        case KEY_RIGHT_CONTROL: return "Right Ctrl";
        case KEY_LEFT_ALT: return "Left Alt";
        case KEY_RIGHT_ALT: return "Right Alt";
        // Letters
        case KEY_A: return "A";
        case KEY_B: return "B";
        case KEY_C: return "C";
        case KEY_D: return "D";
        case KEY_E: return "E";
        case KEY_F: return "F";
        case KEY_G: return "G";
        case KEY_H: return "H";
        case KEY_I: return "I";
        case KEY_J: return "J";
        case KEY_K: return "K";
        case KEY_L: return "L";
        case KEY_M: return "M";
        case KEY_N: return "N";
        case KEY_O: return "O";
        case KEY_P: return "P";
        case KEY_Q: return "Q";
        case KEY_R: return "R";
        case KEY_S: return "S";
        case KEY_T: return "T";
        case KEY_U: return "U";
        case KEY_V: return "V";
        case KEY_W: return "W";
        case KEY_X: return "X";
        case KEY_Y: return "Y";
        case KEY_Z: return "Z";
        // Numbers
        case KEY_ZERO: return "0";
        case KEY_ONE: return "1";
        case KEY_TWO: return "2";
        case KEY_THREE: return "3";
        case KEY_FOUR: return "4";
        case KEY_FIVE: return "5";
        case KEY_SIX: return "6";
        case KEY_SEVEN: return "7";
        case KEY_EIGHT: return "8";
        case KEY_NINE: return "9";
        default: {
            static char buffer[16];
            snprintf(buffer, sizeof(buffer), "Key %d", keyCode);
            return buffer;
        }
    }
}

// Get readable name for an input binding
void InputConfig_GetBindingName(const InputBinding* binding, char* buffer, int bufferSize) {
    if (binding->type == INPUT_TYPE_NONE) {
        snprintf(buffer, bufferSize, "Not Bound");
        return;
    }
    
    switch (binding->type) {
        case INPUT_TYPE_KEY:
            snprintf(buffer, bufferSize, "%s", InputConfig_GetKeyName(binding->value));
            break;
            
        case INPUT_TYPE_GAMEPAD_BUTTON:
            switch (binding->value) {
                case GAMEPAD_BUTTON_LEFT_FACE_UP: snprintf(buffer, bufferSize, "D-Pad Up"); break;
                case GAMEPAD_BUTTON_LEFT_FACE_DOWN: snprintf(buffer, bufferSize, "D-Pad Down"); break;
                case GAMEPAD_BUTTON_LEFT_FACE_LEFT: snprintf(buffer, bufferSize, "D-Pad Left"); break;
                case GAMEPAD_BUTTON_LEFT_FACE_RIGHT: snprintf(buffer, bufferSize, "D-Pad Right"); break;
                case GAMEPAD_BUTTON_RIGHT_FACE_DOWN: snprintf(buffer, bufferSize, "A Button"); break;
                case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT: snprintf(buffer, bufferSize, "B Button"); break;
                case GAMEPAD_BUTTON_RIGHT_FACE_LEFT: snprintf(buffer, bufferSize, "X Button"); break;
                case GAMEPAD_BUTTON_RIGHT_FACE_UP: snprintf(buffer, bufferSize, "Y Button"); break;
                case GAMEPAD_BUTTON_LEFT_TRIGGER_1: snprintf(buffer, bufferSize, "L1"); break;
                case GAMEPAD_BUTTON_LEFT_TRIGGER_2: snprintf(buffer, bufferSize, "L2"); break;
                case GAMEPAD_BUTTON_RIGHT_TRIGGER_1: snprintf(buffer, bufferSize, "R1"); break;
                case GAMEPAD_BUTTON_RIGHT_TRIGGER_2: snprintf(buffer, bufferSize, "R2"); break;
                case GAMEPAD_BUTTON_MIDDLE_LEFT: snprintf(buffer, bufferSize, "Select"); break;
                case GAMEPAD_BUTTON_MIDDLE_RIGHT: snprintf(buffer, bufferSize, "Start"); break;
                default: snprintf(buffer, bufferSize, "Button %d", binding->value); break;
            }
            break;
            
        case INPUT_TYPE_GAMEPAD_AXIS:
            snprintf(buffer, bufferSize, "Axis %d", binding->value);
            break;
            
        default:
            snprintf(buffer, bufferSize, "Unknown");
            break;
    }
}

