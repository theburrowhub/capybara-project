# Interlevel Transition System

## Overview

The game now features a seamless interlevel transition system that allows players to progress from one level to the next without any loading screens, game interruptions, or abrupt transitions. The player's ship, score, and powerups are preserved across levels while level-specific variables are properly reset.

## Features

### 1. Level Completion Overlay (15 seconds warning)

When a level has **15 seconds remaining**, a semi-transparent overlay appears in the upper portion of the screen showing:

- **Level name and number**: "LEVEL X: [Level Name]"
- **Status**: "COMPLETING..."
- **Current score**: Shows the player's accumulated score
- **Countdown timer**: "Next level in: X seconds"

**Visual Design**:
- Semi-transparent dark blue background (85% opacity)
- Gold borders and corner decorations
- Positioned 60 pixels below the play zone top
- Non-invasive - player can still navigate and play comfortably

### 2. Seamless Level Transition

When a level timer reaches its duration:

**For all levels except the last**:
- Game continues without interruption
- New level music starts immediately
- New wave system (enemies) begins spawning
- Level-specific variables are reset
- Player state, score, and powerups are preserved

**For the last level**:
- Game ends with victory message: "VICTORY! You survived all levels!"
- Shows final score and game over screen

### 3. Variables Reset on Level Transition

The following variables are **reset** when transitioning to a new level:

#### Time Display (Level Time)
- `game->levelStartTime` → Set to current `gameTime` (marks start of new level)
- **Display Timer** → Shows 0:00 (calculated as `gameTime - levelStartTime`)
- **Note**: `game->gameTime` continues running (preserves speed calculations)

#### Boss Tracking
- `game->bossEnemyIndex` → -1
- `game->bossSpawnTime` → -1.0f
- `game->bossEscapeTriggered` → false
- `game->bossEscapeTimer` → 0.0f
- `game->bossEscapePhase` → 0

#### Transition State
- `game->showingLevelComplete` → false
- `game->levelCompleteTimer` → 0.0f
- `game->transitioningToNextLevel` → false

#### Entity Cleanup
- All active enemies cleared
- All active projectiles (enemy and player) cleared
- Wave system cleaned up and reinitialized

### 4. Variables Preserved Across Levels

The following variables are **preserved** when transitioning:

- `game->score` - Player's accumulated score
- **`game->gameTime`** - **CONTINUES RUNNING** (critical for speed preservation!)
- `game->speedLevel` - Difficulty speed level continues increasing
- `game->scrollSpeed` - Scroll speed maintained from previous level
- `game->playerShip` - All ship state including:
  - Health
  - Shield
  - Energy
  - Weapon powerups
  - Weapon mode
  - Position (player keeps flying)
- `game->powerupSystem` - Active powerups continue

#### How Speed Preservation Works

The key to preserving scroll speed is that **`gameTime` never resets**. Instead:

1. `gameTime` counts continuously from game start: 0s → 553s → 606s → ...
2. `levelStartTime` marks when each level begins
3. **Level Timer Display** = `gameTime - levelStartTime` (resets to 0:00 each level)
4. **Speed Calculation** = `gameTime / SPEED_UPDATE_INTERVAL` (keeps increasing!)

Example:
```
Level 1 ends:
  gameTime: 553.82s
  speedLevel: 10 (from gameTime / 60 = 553 / 60 ≈ 9.2 → level 10)
  scrollSpeed: 6.5
  
Level 2 starts:
  gameTime: 553.82s (CONTINUES, not reset!)
  levelStartTime: 553.82s (marked)
  Display Timer: 553.82 - 553.82 = 0.00 ✅
  speedLevel: 10 (still calculated from 553.82s) ✅
  scrollSpeed: 6.5 (stays the same) ✅
  
After 60 seconds in Level 2:
  gameTime: 613.82s
  Display Timer: 613.82 - 553.82 = 60.00
  speedLevel: 11 (from 613.82 / 60 ≈ 10.2 → level 11) ✅ INCREASES!
```

This ensures the scroll speed **continues naturally** without any reset!

## Implementation Details

### Code Structure

**Main files modified**:
1. `include/types.h` - Added transition state variables
2. `src/core/game.c` - Level transition logic
3. `include/renderer.h` - Level complete overlay function declaration
4. `src/systems/renderer.c` - Overlay rendering implementation

**Key Functions**:
- `UpdateGame()` - Detects 15-second warning and handles level transition
- `DrawLevelCompleteOverlay()` - Renders the completion overlay
- `AdvanceToNextLevel()` - Progresses to next level in level manager

### Transition Sequence

```
Level Duration - 15s: Show completion overlay
                     ↓
                  Continue gameplay
                     ↓
Level Duration: Check for next level
                     ↓
              ┌──────┴──────┐
              ↓              ↓
        Next Level?         No Next Level
              ↓              ↓
      Transition        Victory Screen
              ↓
      - Stop old music
      - Load new music
      - Clear enemies
      - Clear projectiles
      - Reset variables
      - Init new wave system
      - Continue gameplay
```

### Logging

The system provides detailed logging for debugging:

```
[XXX.XX] Level X completion overlay displayed - 15 seconds remaining
[XXX.XX] Level X complete! Score: XXXX - Transitioning to next level...
[XXX.XX] Started music for level X: assets/audio/levelX.mp3
[0.00] Now playing Level X: [Level Name]
```

## Player Experience

### Timeline Example (Level 1 → Level 2)

```
Level 1 Time: 538.82s (15s before end)
│ Overlay appears: "LEVEL 1: Initiation - COMPLETING..."
│ Player continues fighting
│
Level 1 Time: 553.82s (level end)
│ Music switches to Level 2
│ Screen clears of enemies
│ Overlay disappears
│ New enemies start spawning
│ Timer resets to 0:00
│
Level 2 Time: 0.00s
│ "Now playing Level 2: Escalation"
│ Fresh level begins seamlessly
```

### Visual Flow

```
Normal Gameplay → [15s warning] → Overlay Appears → [Countdown] → 
→ Seamless Transition → Fresh Level → Normal Gameplay
        ↑                                                 ↓
        └─────────── Player Never Stops Playing ─────────┘
```

## Configuration

To add new levels, simply add entries to the `s_levels` array in `src/systems/level_system.c`:

```c
{
    .levelNumber = 3,
    .name = "Your Level Name",
    .audioPath = "assets/audio/level3.mp3",
    .bassLogPath = "assets/audio/level3.log",
    .duration = XXX.XXf,  // Duration in seconds
    .targetScore = XXXXX,
    .description = "Level description"
}
```

The system automatically handles:
- Transition overlay at 15 seconds
- Variable reset
- Music switching
- Wave system initialization
- Victory detection (for the last level)

## Testing

To test the interlevel transition:

1. **Quick test**: Use debug mode to start near level end
   ```bash
   # Start Level 1 at phase 16 (500s in)
   ./run_debug_game.sh -i -p 16
   # Wait ~54 seconds to see transition
   ```

2. **Full test**: Play through an entire level naturally

3. **Verify**:
   - Overlay appears at 15 seconds remaining
   - Timer shows countdown
   - Score is preserved across levels
   - Time resets to 0:00 in new level
   - **Speed level and scroll speed are preserved** (difficulty continues)
   - New music starts
   - Player ship continues flying smoothly

## Example: Level 1 → Level 2 Transition

```
Level 1 at 553.82s (end of level)
Current State:
  ├─ Time: 9:13 (553 seconds)
  ├─ Score: 15,420 points
  ├─ Speed Level: 10
  ├─ Scroll Speed: 6.5
  ├─ Player Health: 85 HP
  ├─ Weapon Power: +2 (3 powerups)
  │
  └─ SEAMLESS TRANSITION
     │
     ├─ Music changes to Level 2
     ├─ All enemies cleared
     ├─ All projectiles cleared
     ├─ Timer resets to 0:00
     │
     └─ Level 2 at 0.00s (start)
        New State:
          ├─ Time: 0:00 (0 seconds) ✅ RESET
          ├─ Score: 15,420 points ✅ PRESERVED
          ├─ Speed Level: 10 ✅ PRESERVED (difficulty continues!)
          ├─ Scroll Speed: 6.5 ✅ PRESERVED (stays fast!)
          ├─ Player Health: 85 HP ✅ PRESERVED
          └─ Weapon Power: +2 (3 powerups) ✅ PRESERVED
```

**Key Point**: The game maintains its increasing difficulty. If you finished Level 1 at high speed (level 10), Level 2 starts at that same speed, making the progression feel continuous and challenging.

## Technical Solution: Dual Timing System

The system uses **two parallel timers** to achieve seamless transitions while preserving speed:

### 1. Game Time (Continuous)
- `game->gameTime` - Runs continuously from game start
- Used for: Speed calculations, scroll speed progression
- Never resets between levels

### 2. Level Time (Per-Level)
- Calculated as: `levelTime = gameTime - levelStartTime`
- Used for: Wave spawning, boss events, display timer
- Resets to 0:00 for each new level

### Implementation Details

**During Level Transition:**
```c
// Mark the start of new level
game->levelStartTime = game->gameTime;  // e.g., 553.82s

// Reinitialize wave system (waveTimer → 0)
// CRITICAL: Pass false to NOT apply DEBUG_START_PHASE
CleanupWaveSystem(game->waveSystem);
InitWaveSystem(game->waveSystem, nextLevel, false);  // waveTimer starts at 0
```

**During Initial Game Start:**
```c
// Initial game initialization
InitWaveSystem(game->waveSystem, currentLevel, true);  // Apply DEBUG_START_PHASE if set
```

**During Gameplay:**
```c
// Speed calculation (uses game time - continuous)
newSpeedLevel = (int)(game->gameTime / SPEED_UPDATE_INTERVAL) + 1;

// Boss spawn time (uses level time)
float levelTime = game->gameTime - game->levelStartTime;
game->bossSpawnTime = levelTime;  // Stored as level time

// Wave spawning (uses waveTimer - starts at 0 each level)
if (spawnEvents[nextEventIndex].time <= waveSystem->waveTimer)
```

### Systems Using Level Time

All level-specific systems use level time to ensure proper sequencing:

1. **Wave System**: `waveTimer` (0-based, reset each level)
2. **Boss Spawning**: `bossSpawnTime` (level time when boss spawns)
3. **Boss Escape**: `bossBattleTime = currentLevelTime - bossSpawnTime`
4. **Warning System**: Uses boss battle time (level time)
5. **Display Timers**: All UI timers show level time

### Result

- ✅ Display timer resets to 0:00 each level
- ✅ Speed calculations continue naturally (no reset)
- ✅ Wave events spawn at correct times each level
- ✅ Boss events trigger properly each level
- ✅ No abrupt speed changes
- ✅ Seamless gameplay experience

## Important: Level-Specific vs Global Timing

**Critical Fixes Applied**:

### Fix #1: Wave Timer Reset Bug

**Problem**: When transitioning to Level 2, the wave system was applying `DEBUG_START_PHASE` again, causing the `waveTimer` to start at the debug phase time (e.g., 500s) instead of 0!

**Solution**: Added `applyDebugPhase` parameter to `InitWaveSystem()`:
- Initial game start: `InitWaveSystem(waveSystem, level, true)` - applies DEBUG_START_PHASE
- Level transition: `InitWaveSystem(waveSystem, level, false)` - starts at 0

**Without this fix:**
```
Level 1 ends
Level 2 initializes with DEBUG_START_PHASE = 16
  waveTimer: 500s (WRONG!)
  Boss spawns immediately (event at 427s already passed)
  Level starts near the end! ❌
```

**With this fix:**
```
Level 1 ends
Level 2 initializes with applyDebugPhase = false
  waveTimer: 0s (CORRECT!)
  First enemies spawn at 0-10s
  Boss spawns at 427s
  Level starts from the beginning! ✅
```

### Fix #2: Boss Timing Uses Level Time

**Problem**: Boss spawn and escape timing was using `game->gameTime` instead of level time.

**Without this fix:**
```
Level 1 ends at game time 553 seconds
Level 2 starts with game time still at 553 seconds
Wave system checks: "spawn enemy at 10 seconds"
But game time is 553s → Level 2 skips to near the end! ❌
```

**With the fix:**
```
Level 1 ends at game time 553 seconds
Level 2 starts:
  - game time: 553s (continues for speed)
  - levelStartTime: 553s (marked)
  - waveTimer: 0s (reinitialized) ✅
  - Boss spawn: uses level time ✅
Wave system checks: "spawn enemy at 10 seconds"
waveTimer is at 10s → Enemy spawns correctly! ✅
```

## Notes

- The transition is completely seamless - no loading screens
- Player maintains full control throughout the transition
- The overlay is designed to be non-intrusive (upper screen area)
- All level-specific state is properly cleaned up
- Score and player progression are maintained across levels
- **Speed level and scroll speed continue increasing** - difficulty is preserved
- **`gameTime` runs continuously** - critical for speed preservation
- **Wave and boss systems use level time** - ensures proper event sequencing
- The last level in the list ends with a victory screen

