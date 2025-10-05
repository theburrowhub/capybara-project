# Debug Features

This document describes the debug features available for testing and debugging the game.

## Debug Constants

Debug constants are defined in `include/constants.h` and can be modified to change game behavior for testing purposes.

### 1. Invulnerability Mode

```c
#define DEBUG_INVULNERABILITY false  // Set to true to make player invulnerable
```

**Purpose**: Makes the player immune to all damage from enemies and projectiles.

**How to Enable**:
1. Edit `include/constants.h`
2. Change `DEBUG_INVULNERABILITY` from `false` to `true`
3. Recompile the game: `make clean && make`

**Visual Indicator**: When enabled, `[INVULNERABLE]` appears in green text next to the health display.

**Use Cases**:
- Testing enemy patterns without dying
- Debugging collision detection
- Exploring all game phases
- Testing weapon systems

### 2. Starting Phase Selection

```c
#define DEBUG_START_PHASE 0  // Phase to start at (0 = normal start, 1-17 for specific phases)
```

**Purpose**: Start the game at any specific wave phase for testing.

**How to Use**:
1. Edit `include/constants.h`
2. Set `DEBUG_START_PHASE` to the desired phase number (1-17)
3. Recompile the game: `make clean && make`

**Phase List**:
- **0**: Normal game start (default)
- **1**: Warm-Up (0-5 seconds) - No enemies
- **2**: First Wave (5-35 seconds) - Grunt enemies (no firing)
- **3**: Tank Squadron (35-55 seconds) - Tank enemies
- **4**: Swarm Attack (55-85 seconds) - Swarm formations
- **5**: Mixed Assault (85-120 seconds) - Speeders and Grunts
- **6**: Elite Squadron (120-150 seconds) - Elite enemies
- **7**: Zigzag Chaos (150-180 seconds) - Zigzag enemies
- **8**: Shield Wall (180-220 seconds) - Shield enemies
- **9**: Bomber Run (220-260 seconds) - Bomber enemies
- **10**: Ghost Ambush (260-300 seconds) - Ghost enemies
- **11**: Combined Arms (300-345 seconds) - Mixed enemy types
- **12**: Mini-Boss (345-390 seconds) - Boss encounter
- **13**: Recovery (390-420 seconds) - Light enemies
- **14**: Elite & Shield (420-460 seconds) - Elite and Shield combo
- **15**: Evasive Maneuvers (460-500 seconds) - Fast enemies
- **16**: Heavy Assault (500-540 seconds) - Heavy enemies
- **17**: Final Wave (540-590 seconds) - Final challenge

**Visual Indicator**: When starting at a non-zero phase, `[DEBUG: Started at Phase X]` appears in orange text at the bottom of the screen.

**Use Cases**:
- Testing specific enemy types
- Debugging late-game content
- Balancing difficulty curves
- Testing boss encounters

## Pause Functionality

**Controls**: Press `P` or `SPACE` during gameplay

**Features**:
- Freezes all game updates (enemies, projectiles, timers)
- Shows "PAUSED" overlay on screen
- Press P or SPACE again to resume

**Use Cases**:
- Analyzing enemy positions
- Taking screenshots
- Planning strategies
- Debugging specific moments

## How to Apply Debug Settings

1. **Edit the constants**:
   ```bash
   nano include/constants.h
   # or use your preferred text editor
   ```

2. **Modify the debug values**:
   ```c
   // For invulnerability
   #define DEBUG_INVULNERABILITY true
   
   // To start at phase 10 (Ghost Ambush)
   #define DEBUG_START_PHASE 9  // Note: phases are 0-indexed internally
   ```

3. **Recompile the game**:
   ```bash
   make clean && make
   ```

4. **Run the game**:
   ```bash
   ./bin/shootemup
   ```

## Example Testing Scenarios

### Test Boss Fight
```c
#define DEBUG_INVULNERABILITY true
#define DEBUG_START_PHASE 11  // Mini-Boss phase
```

### Test Late Game Difficulty
```c
#define DEBUG_INVULNERABILITY false
#define DEBUG_START_PHASE 15  // Heavy Assault phase
```

### Test Enemy Patterns
```c
#define DEBUG_INVULNERABILITY true
#define DEBUG_START_PHASE 0  // Play from start with invulnerability
```

## Debug Script

For advanced debugging with command-line options:
```bash
./run_debug_game.sh --help
```

This script allows you to launch the game with:
- Invulnerability mode
- Start at any specific phase
- Enable weapon overheating
- Debug logging
- And more options

## Important Notes

- Debug features are intended for development and testing only
- Remember to set debug flags back to their default values before release:
  - `DEBUG_INVULNERABILITY false`
  - `DEBUG_START_PHASE 0`
- The pause feature (P/SPACE) is always available and can be kept in the release version
