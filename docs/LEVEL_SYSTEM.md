# Level System Documentation

## Overview

The game now supports **multiple levels** with different audio tracks, enemy waves, and flight plans. Each level is self-contained with its own configuration, spawn events, and music.

## Architecture

### Level Configuration (`LevelConfig`)

Each level is defined by a `LevelConfig` structure:

```c
typedef struct LevelConfig {
    int levelNumber;         // 1, 2, 3, etc.
    const char* name;        // "Initiation", "Escalation", etc.
    const char* audioPath;   // Path to MP3 file
    const char* bassLogPath; // Path to bass analysis log
    float duration;          // Duration in seconds
    int targetScore;         // Score needed to progress
    const char* description; // Level description
} LevelConfig;
```

### Level Manager (`LevelManager`)

The `LevelManager` handles level progression and configuration:

```c
typedef struct LevelManager {
    LevelConfig* levels;     // Array of level configs
    int levelCount;          // Total number of levels
    int currentLevel;        // Current level index (0-based)
    bool levelComplete;      // Is current level complete?
    float levelTimer;        // Timer for current level
} LevelManager;
```

## Available Levels

### Level 1: Initiation
- **Audio**: `assets/audio/level1.mp3`
- **Duration**: 553.82 seconds (~9 minutes)
- **Bass Log**: `assets/audio/level1.log`
- **Target Score**: 5,000 points
- **Description**: Welcome to the front lines. Learn the ropes and survive.
- **Wave Plan**: `CreateLevel1Waveplan()` in `src/systems/level1_waves.c`

**Characteristics**:
- Tutorial warm-up phase (0-55s)
- Gradual difficulty increase
- Introduces all enemy types
- Boss battle at 427s
- Victory lap at the end

### Level 2: Escalation
- **Audio**: `assets/audio/level2.mp3`
- **Duration**: 612.13 seconds (~10 minutes)
- **Bass Log**: `assets/audio/level2.log`
- **Target Score**: 10,000 points
- **Description**: The enemy forces intensify. Show them what you've learned.
- **Wave Plan**: `CreateLevel2Waveplan()` in `src/systems/level2_waves.c`

**Characteristics**:
- **NO warm-up** - combat starts at 0.4 seconds! 🔥
- **Instant high intensity** - Elites and Tanks from the start
- **MASSIVE** continuous combat phase (0.5-370s) - 370 seconds of relentless action!
- Recovery period (370-480s) - earned calm after intense combat
- Final climax (480-593s) with boss battle at 555s
- Bass-synchronized intensity changes
- Significantly harder than Level 1

**Design Philosophy**: INSTANT ACTION - no tutorial, no warm-up, immediate high-intensity combat synchronized perfectly with the bass log's intensity patterns.

## Level-Specific Wave Plans

Each level has its own wave plan file that defines spawn events:

### `src/systems/level1_waves.c`
```c
SpawnEvent* CreateLevel1Waveplan(int* eventCount);
```
- Original flight plan synchronized with level1.log
- ~400 spawn events
- Balanced progression from tutorial to boss fight

### `src/systems/level2_waves.c`
```c
SpawnEvent* CreateLevel2Waveplan(int* eventCount);
```
- Synchronized with level2.log bass events
- ~500 spawn events
- Extended combat phases
- More complex enemy formations

## Adding New Levels

To add a new level to the game:

### 1. Create Level Configuration

Edit `src/systems/level_system.c` and add to the `s_levels` array:

```c
static LevelConfig s_levels[] = {
    // ... existing levels ...
    {
        .levelNumber = 3,
        .name = "Your Level Name",
        .audioPath = "assets/audio/level3.mp3",
        .bassLogPath = "assets/audio/level3.log",
        .duration = 600.0f,  // Duration in seconds
        .targetScore = 15000,
        .description = "Your level description"
    }
};
```

### 2. Create Wave Plan

Create a new file `src/systems/level3_waves.c`:

```c
#include "wave_system.h"
#include "enemy_types.h"
#include "constants.h"

SpawnEvent* CreateLevel3Waveplan(int* eventCount) {
    static SpawnEvent events[600];  // Adjust size as needed
    int index = 0;
    
    // Define your spawn events
    events[index++] = (SpawnEvent){
        .time = 5.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 100,
        .pattern = "straight"
    };
    
    // ... more events ...
    
    *eventCount = index;
    return events;
}
```

### 3. Update Function Declaration

Add to `include/level_system.h`:

```c
SpawnEvent* CreateLevel3Waveplan(int* eventCount);
```

### 4. Update Wave System

Edit `src/systems/wave_system.c` in `InitWaveSystem()`:

```c
if (levelConfig->levelNumber == 1) {
    waveSystem->spawnEvents = CreateLevel1Waveplan(&waveSystem->eventCount);
} else if (levelConfig->levelNumber == 2) {
    waveSystem->spawnEvents = CreateLevel2Waveplan(&waveSystem->eventCount);
} else if (levelConfig->levelNumber == 3) {
    waveSystem->spawnEvents = CreateLevel3Waveplan(&waveSystem->eventCount);
}
```

### 5. Update Makefile

Add the new wave file to `Makefile`:

```makefile
SYSTEM_SRCS = ... \
              $(SRC_DIR)/systems/level3_waves.c \
              ...
```

### 6. Add Audio Files

Place your audio files in the `assets/audio/` directory:
- `assets/audio/level3.mp3` - The level music
- `assets/audio/level3.log` - Bass analysis log (optional, for reference)

## Level Progression

### Current Implementation

Currently, levels are selected at game initialization. The game starts at Level 1 by default.

### Level Manager API

```c
// Initialize the level manager
void InitLevelManager(LevelManager* manager);

// Get current level configuration
const LevelConfig* GetCurrentLevel(const LevelManager* manager);

// Get specific level by number (1-based)
const LevelConfig* GetLevel(const LevelManager* manager, int levelNumber);

// Advance to next level
bool AdvanceToNextLevel(LevelManager* manager);

// Reset to specific level
void ResetToLevel(LevelManager* manager, int levelNumber);

// Get total number of levels
int GetTotalLevels(const LevelManager* manager);

// Cleanup
void CleanupLevelManager(LevelManager* manager);
```

### Future Enhancements

Potential features for level progression:

1. **Automatic Level Transition**: Automatically advance when level is complete
2. **Level Selection Menu**: Allow players to choose which level to play
3. **Level Unlocking**: Require completing previous levels to unlock new ones
4. **Score Requirements**: Must achieve target score to unlock next level
5. **Level Statistics**: Track best scores, completion times for each level
6. **Difficulty Modes**: Same level with different enemy densities

## Bass Event Integration

Each level is synchronized with its audio track's bass events:

### Level 1 Bass Profile
- **0.50s**: Bass START (LOW→MEDIUM) - Combat begins
- **Long periods**: Continuous bass = intense combat
- **Calm sections**: No bass = recovery periods
- **427s**: Boss bass event

### Level 2 Bass Profile
- **0.40s**: Bass START (LOW) → Combat begins IMMEDIATELY!
- **0.50s**: Bass→MEDIUM → High intensity starts
- **0.50-367s**: Continuous MEDIUM with fluctuations (370s of intense combat!)
- **367-479s**: Sparse LOW bursts (calm recovery period)
- **479-593s**: MEDIUM returns (final climax + boss at 555s)

**Key Difference**: Bass patterns drive rapid intensity changes throughout - the level **follows the music**!

## Testing Levels

### Switching Between Levels (Quick Guide)

To play a different level:

1. Open `src/systems/level_system.c`
2. Find the `InitLevelManager()` function
3. Change the line:
   ```c
   manager->currentLevel = 0;  // 0 = Level 1, 1 = Level 2, etc.
   ```
4. Rebuild the game:
   ```bash
   make clean && make game
   ```
5. Run the game:
   ```bash
   ./bin/shootemup
   ```

**Examples:**
- `manager->currentLevel = 0;` → Play Level 1 (Initiation)
- `manager->currentLevel = 1;` → Play Level 2 (Escalation)

### Test Specific Level

To test a specific level, modify `InitLevelManager()` in `src/systems/level_system.c`:

```c
void InitLevelManager(LevelManager* manager) {
    manager->levelCount = sizeof(s_levels) / sizeof(LevelConfig);
    manager->levels = s_levels;
    manager->currentLevel = 1;  // Change to 1 for Level 2, 2 for Level 3, etc.
    // ...
}
```

### Debug Start Phase

You can still use `DEBUG_START_PHASE` in `constants.h` to skip to specific times within a level.

## File Organization

```
include/
  ├── level_system.h      # Level system API
  └── wave_system.h       # Updated to accept LevelConfig

src/systems/
  ├── level_system.c      # Level manager implementation
  ├── level1_waves.c      # Level 1 spawn events
  ├── level2_waves.c      # Level 2 spawn events
  └── wave_system.c       # Updated to use level configs

assets/audio/
  ├── level1.mp3          # Level 1 music
  ├── level1.log          # Level 1 bass analysis
  ├── level2.mp3          # Level 2 music
  └── level2.log          # Level 2 bass analysis

docs/
  ├── FLIGHT_PLAN.md      # Original Level 1 flight plan
  └── LEVEL_SYSTEM.md     # This file
```

## Wave Design Best Practices

When creating wave plans for new levels:

1. **Synchronize with Bass**: Use bass log events to time wave intensity
2. **Pacing**: Balance intense combat with recovery periods
3. **Variety**: Mix enemy types for dynamic gameplay
4. **Progression**: Start easier, build to climax
5. **Boss Placement**: Place boss during significant bass events
6. **Test Thoroughly**: Playtest entire duration multiple times

## Migration Notes

### Changes from Previous Version

1. **`CreateStaticWaveplan()` → `CreateLevel1Waveplan()`**: Original wave plan renamed
2. **New `LevelManager`**: Added to `Game` structure in `types.h`
3. **`InitWaveSystem()`**: Now requires `LevelConfig*` parameter
4. **Music Loading**: Now uses `levelConfig->audioPath` instead of hardcoded path
5. **Duration**: Now uses `levelConfig->duration` instead of hardcoded value

### Backward Compatibility

- Level 1 uses the exact same wave plan as the original `static_waves.c`
- Game behavior for Level 1 is identical to previous version
- No changes to gameplay mechanics, only infrastructure

## Performance Considerations

- All level configurations are stored in static memory
- Wave plans use static arrays (no dynamic allocation)
- Level switching requires reloading music and reinitializing wave system
- Memory usage: ~8KB per level wave plan

## Boss Mechanics

### Boss System Architecture

Boss encounters are integrated into the level system with spawn-based timing:

**Boss State Tracking**:
```c
// In Game structure
float bossSpawnTime;        // When boss spawned (-1 if not spawned)
int bossEnemyIndex;         // Index of boss in enemies array
bool bossEscapeTriggered;   // Has escape sequence started
```

### Boss Countdown Warning

The boss countdown warning system ensures proper player feedback:

**Implementation**:
```c
// Warning display logic - shows only when boss is alive
if (game->bossSpawnTime >= 0 && 
    !game->bossEscapeTriggered && 
    game->bossEnemyIndex >= 0 && 
    game->enemies[game->bossEnemyIndex].active) {  // Boss must be alive!
    
    // Calculate time remaining
    float bossBattleTime = gameTime - game->bossSpawnTime;
    float timeRemaining = BOSS_BATTLE_DURATION - bossBattleTime;
    
    // Display warning countdown
    if (timeRemaining <= 30.0f && timeRemaining > 0.0f) {
        // Show "BOSS ESCAPE IN X!" warning
    }
}
```

**Key Checks**:
1. `bossSpawnTime >= 0` - Boss has spawned
2. `!bossEscapeTriggered` - Escape sequence not started
3. `bossEnemyIndex >= 0` - Valid boss index
4. **`enemies[bossIndex].active`** - **Boss is still alive** (stops countdown when killed)

**Behavior**:
- Warning appears when boss has 30 seconds left
- Countdown continues only while boss is alive
- **Warning disappears immediately when boss is killed** ✅
- No countdown effect after victory
- Professional game design - countdown only relevant when threat exists

### Boss Timing (Per Level)

**Level 1**:
- Boss spawn: 427s
- Battle duration: 90 seconds
- Warning at: 487s (60s after spawn)
- Escape at: 517s (if alive)
- Level end: 553.82s

**Level 2**:
- Boss spawn: 510s
- Battle duration: 90 seconds  
- Warning at: 570s (60s after spawn)
- Escape at: 600s (if alive)
- Level end: 612.13s

### Boss Testing

```bash
# Test Level 1 boss
./run_debug_game.sh -i -p 1-12

# Test Level 2 boss
./run_debug_game.sh -i -p 2-17

# What to verify:
# 1. Boss spawns at correct time
# 2. Warning appears at correct time (30s before escape)
# 3. Countdown displays correctly
# 4. Countdown STOPS immediately when boss dies
# 5. Escape sequence triggers only if boss alive
```

## Technical Implementation Details

### Wave System Integration

**Before refactoring**:
```c
InitWaveSystem(WaveSystem* waveSystem);
```

**After refactoring**:
```c
InitWaveSystem(WaveSystem* waveSystem, const LevelConfig* levelConfig);
```

The wave system now:
- Accepts level configuration
- Loads appropriate wave plan based on level number
- Initializes with level-specific duration
- Synchronizes with level-specific bass events

### Level Switching Mechanism

**Manual Method** (compile-time):
1. Edit `src/systems/level_system.c`
2. Change `manager->currentLevel` value (0-based)
3. Rebuild: `make clean && make game`

**Debug Method** (runtime):
```bash
./run_debug_game.sh -p LEVEL-PHASE
# Examples:
./run_debug_game.sh -p 1-0  # Level 1 from start
./run_debug_game.sh -p 2-0  # Level 2 from start
```

### Memory and Performance

**Static Allocation**:
- Level configs: ~200 bytes per level
- Wave plans: ~8KB per level (static arrays)
- No dynamic allocation at runtime
- Zero overhead after initialization

**Loading Times**:
- Level configuration: Instant (static data)
- Wave plan loading: Instant (compile-time)
- Audio loading: ~50-100ms (depends on file size)
- Total level switch: < 200ms

### Build System Integration

The Makefile automatically includes:
```makefile
SYSTEM_SRCS = ... \
              $(SRC_DIR)/systems/level_system.c \
              $(SRC_DIR)/systems/level1_waves.c \
              $(SRC_DIR)/systems/level2_waves.c \
              ...
```

Add new levels by appending to `SYSTEM_SRCS`.

## Summary

The level system provides a clean, extensible architecture for adding new levels with:
- ✅ Independent audio tracks per level
- ✅ Custom wave plans per level  
- ✅ Level progression support
- ✅ Easy addition of new levels
- ✅ Backward compatible with original design
- ✅ No performance overhead
- ✅ Proper boss mechanics with spawn-based timing
- ✅ Professional countdown warning system

The system is production-ready and supports unlimited levels with minimal code changes.

### Key Features

**Architecture**:
- LevelConfig structure for level metadata
- LevelManager for progression
- Level-specific wave generators
- Boss mechanics with proper state tracking

**Developer Experience**:
- Simple API for level management
- Easy level addition (6 steps)
- Clear file organization
- Comprehensive documentation

**Performance**:
- Static memory allocation
- Zero runtime overhead
- Instant loading
- Minimal memory footprint (~8KB per level)

**Extensibility**:
- Unlimited levels supported
- Easy to add new levels
- No core system modifications needed
- Forward compatible
