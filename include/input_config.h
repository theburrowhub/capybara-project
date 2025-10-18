#ifndef INPUT_CONFIG_H
#define INPUT_CONFIG_H

#include <stdbool.h>

// Default configuration file path
#define INPUT_CONFIG_FILE_DEFAULT ".input_config.conf"

// Input types
typedef enum {
    INPUT_TYPE_NONE = 0,
    INPUT_TYPE_KEY,           // Keyboard key
    INPUT_TYPE_MOUSE,         // Mouse button
    INPUT_TYPE_GAMEPAD_BUTTON, // Gamepad button
    INPUT_TYPE_GAMEPAD_AXIS   // Gamepad axis (for movement)
} InputType;

// Game actions
typedef enum {
    ACTION_MOVE_UP = 0,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_FIRE,
    ACTION_SPECIAL_ABILITY,
    ACTION_SWITCH_ENERGY_MODE,
    ACTION_SWITCH_WEAPON_MODE,
    ACTION_COUNT  // Total number of actions
} GameAction;

// Input binding structure
typedef struct {
    InputType type;
    int value;  // Key code, mouse button, or gamepad button/axis
    float axisThreshold;  // For gamepad axes (positive or negative)
} InputBinding;

// Maximum bindings per action (increased to support more keys)
#define MAX_BINDINGS_PER_ACTION 4

// Input configuration structure
typedef struct {
    InputBinding bindings[ACTION_COUNT][MAX_BINDINGS_PER_ACTION];  // 4 bindings per action
} InputConfig;

// Initialize configuration with defaults
void InputConfig_InitDefaults(InputConfig* config);

// Load configuration from file
bool InputConfig_Load(InputConfig* config, const char* configFilePath);

// Save configuration to file
bool InputConfig_Save(const InputConfig* config, const char* configFilePath);

// Set a binding for an action
void InputConfig_SetBinding(InputConfig* config, GameAction action, int slot, InputType type, int value);

// Get readable name for an action
const char* InputConfig_GetActionName(GameAction action);

// Get readable name for an input binding
void InputConfig_GetBindingName(const InputBinding* binding, char* buffer, int bufferSize);

// Get readable key name from key code
const char* InputConfig_GetKeyName(int keyCode);

#endif // INPUT_CONFIG_H

