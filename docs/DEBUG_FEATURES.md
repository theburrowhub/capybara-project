# Debug Features - Complete Guide

This document describes all debug features and testing tools available for development, testing, and debugging.

## Table of Contents
- [Quick Reference](#quick-reference)
- [Debug Script (run_debug_game.sh)](#debug-script-run_debug_gamesh)
- [Compile-Time Debug Constants](#compile-time-debug-constants)
- [Showcase Programs](#showcase-programs)
- [Pause Functionality](#pause-functionality)
- [Debug Logging](#debug-logging)
- [Testing Workflows](#testing-workflows)
- [Troubleshooting](#troubleshooting)

---

## Quick Reference

### Common Debug Commands

```bash
# Test Level 1 from beginning
./run_debug_game.sh -p 1-0

# Test Level 2 from beginning
./run_debug_game.sh -p 2-0

# Test Level 1, Tank Squadron (with invulnerability)
./run_debug_game.sh -i -p 1-3

# Test Level 2, Extreme Phase (with invulnerability)
./run_debug_game.sh -i -p 2-16

# Test boss battle with invulnerability and logging
./run_debug_game.sh -i -d -p 1-12

# List all available phases
./run_debug_game.sh --list

# Show help
./run_debug_game.sh --help
```

### Quick Phase Reference

| Phase | Name | Best For |
|-------|------|----------|
| 0 | Normal Start | Full playthrough |
| 3 | Tank Squadron | Tank testing 🎯 |
| 5 | Mixed Assault | Mixed combat |
| 11 | Combined Arms | Intense action 🔥 |
| 12 | Mini-Boss | Boss testing 👹 |
| 16 | Heavy Assault | Extreme challenge 🔥🔥 |

### Available Levels

| Level | Name | Duration | Music | Difficulty |
|-------|------|----------|-------|------------|
| 1 | Initiation | 553.82s (~9 min) | level1.mp3 | Tutorial 🎓 |
| 2 | Escalation | 612.13s (~10 min) | level2.mp3 | Intense 🔥 |

---

## Debug Script (run_debug_game.sh)

The `run_debug_game.sh` script provides convenient access to all debug features without manually editing constants.

### Basic Usage

```bash
./run_debug_game.sh [OPTIONS]
```

### Command Line Options

```
Options:
  -i, --invulnerable         Enable invulnerability mode
  -d, --debug-log            Enable debug logging to game.log
  -p, --phase LEVEL-PHASE    Start at specific level and phase
  -l, --list                 List all available phases
  -h, --help                 Show help message
```

### Level-Phase Format

The script uses the format: **`LEVEL-PHASE`**

- **LEVEL**: The level number (1, 2, etc.)
- **PHASE**: The phase number (0-17)
- **Separator**: Dash (`-`)

**Examples:**

| Command | Level | Phase | Description |
|---------|-------|-------|-------------|
| `1-0` | 1 | 0 | Level 1, from beginning |
| `1-3` | 1 | 3 | Level 1, Tank Squadron |
| `1-12` | 1 | 12 | Level 1, Boss Battle |
| `2-0` | 2 | 0 | Level 2, from beginning |
| `2-13` | 2 | 13 | Level 2, Calm Zone |
| `2-16` | 2 | 16 | Level 2, Extreme Intensity |
| `12` | 1 | 12 | Legacy format (assumes Level 1) |

### Usage Examples

#### Testing Different Levels

```bash
# Test Level 1 from beginning
./run_debug_game.sh -p 1-0

# Test Level 2 from beginning  
./run_debug_game.sh -p 2-0

# Test Level 1, Phase 3 (Tank Squadron)
./run_debug_game.sh -i -p 1-3

# Test Level 2, Phase 13 (Calm zone with increasing cadence)
./run_debug_game.sh -p 2-13

# Test Level 2, Phase 16 (Extreme intensity)
./run_debug_game.sh -i -p 2-16
```

#### Comparing Features Across Levels

**Tank Testing:**
```bash
# Level 1 tanks
./run_debug_game.sh -i -p 1-3   # Tank Squadron
./run_debug_game.sh -i -p 1-11  # Combined Arms

# Level 2 tanks (compare!)
./run_debug_game.sh -i -p 2-3   # Tank Squadron
./run_debug_game.sh -i -p 2-11  # Combined Arms
```

**Boss Testing:**
```bash
# Level 1 boss (427s spawn, 90s battle)
./run_debug_game.sh -i -p 1-12

# Level 2 boss (510s spawn, 90s battle)
./run_debug_game.sh -i -p 2-17
```

#### Advanced Combinations

```bash
# Level 2, Phase 5, invulnerable, with logging
./run_debug_game.sh -i -d -p 2-5

# Level 1, Phase 16 (Heavy Assault), invulnerable
./run_debug_game.sh -i -p 1-16

# Level 2 from start with logging
./run_debug_game.sh -d -p 2-0
```

### How the Script Works

The script automates the debug process:

1. **Backs up** your current `constants.h`
2. **Modifies** debug constants (`DEBUG_START_LEVEL`, `DEBUG_START_PHASE`, `DEBUG_INVULNERABILITY`)
3. **Recompiles** the game with new settings
4. **Runs** the game
5. **Restores** original settings on exit (even if interrupted)

### Script Features

✅ **Automatic backup and restoration**  
✅ **Input validation** (level and phase numbers)  
✅ **Color-coded output**  
✅ **Shows configuration** before starting  
✅ **Comprehensive help text**  
✅ **Legacy format support**  
✅ **Safe interrupt handling**  

### Validation

The script validates all inputs:

- **Level**: Must be 1 or 2 (expandable for future levels)
- **Phase**: Must be 0-17
- **Format**: Must be `LEVEL-PHASE` or just `NUMBER` (legacy)

Invalid inputs show helpful error messages.

---

## Compile-Time Debug Constants

Debug constants are defined in `include/constants.h` and can be modified for testing.

### 1. Invulnerability Mode

```c
#define DEBUG_INVULNERABILITY false  // Set to true to make player invulnerable
```

**Purpose**: Makes the player immune to all damage from enemies and projectiles.

**How to Enable Manually**:
1. Edit `include/constants.h`
2. Change `DEBUG_INVULNERABILITY` from `false` to `true`
3. Recompile: `make clean && make`

**Or use the debug script**: `./run_debug_game.sh -i -p 1-0`

**Visual Indicators**:
- `[INVULNERABLE]` appears in green text at the top of the screen
- `[INVULN]` badge appears in top-right corner

**Use Cases**:
- Testing enemy patterns without dying
- Debugging collision detection
- Exploring all game phases
- Testing weapon systems
- Recording gameplay footage
- Learning level layout

### 2. Level Selection

```c
#define DEBUG_START_LEVEL 1  // Level to start at (1 = Level 1, 2 = Level 2, etc.)
```

**Purpose**: Choose which level to play for testing different level content.

**How to Use Manually**:
1. Edit `include/constants.h`
2. Set `DEBUG_START_LEVEL` to 1 or 2
3. Recompile: `make clean && make`

**Or use the debug script**: `./run_debug_game.sh -p 2-0`

**Available Levels**:

| Level | Name | Duration | Description |
|-------|------|----------|-------------|
| **1** | Initiation | 553.82s | Tutorial-focused, gradual progression |
| **2** | Escalation | 612.13s | Intense action, experienced players |

### 3. Starting Phase Selection

```c
#define DEBUG_START_PHASE 0  // Phase to start at (0 = normal start, 1-17 for specific phases)
```

**Purpose**: Start the game at any specific wave phase within the selected level for testing.

**How to Use Manually**:
1. Edit `include/constants.h`
2. Set `DEBUG_START_PHASE` to desired phase (0-17)
3. Recompile: `make clean && make`

**Or use the debug script**: `./run_debug_game.sh -p 1-3`

**Visual Indicators**:
- `[DEBUG: Level X, Phase Y]` appears in orange text at bottom of screen
- `[LX-PY]` badge appears in top-right corner

### Phase List

Both levels use the same phase structure (timing and enemies differ):

| Phase | Name | Description | Enemy Focus | Intensity |
|-------|------|-------------|-------------|-----------|
| 0 | Normal Start | Start from beginning | - | Start |
| 1 | Warm-Up | No enemies (tutorial) | None | None |
| 2 | First Wave | Initial enemies | Grunts | Low |
| 3 | Tank Squadron | Tank assault | **Tanks** 🎯 | High |
| 4 | Swarm Attack | Swarm formations | Swarms | Medium |
| 5 | Mixed Assault | Tanks + mixed | **Tanks + Mix** 🎯 | High |
| 6 | Elite Squadron | Elite enemies | **Elites** ⭐ | High |
| 7 | Zigzag Chaos | Zigzag enemies | Zigzags | Medium |
| 8 | Shield Wall | Shield enemies | Shields | Medium |
| 9 | Bomber Run | Bomber enemies | Bombers | High |
| 10 | Ghost Ambush | Ghost enemies | Ghosts | Medium |
| 11 | Combined Arms | Intense mixed | **Tanks + All** 🎯🔥 | Very High |
| 12 | Mini-Boss | Boss encounter | **Boss** 👹 | Boss |
| 13 | Recovery | Light enemies | Grunts, Swarms | Low |
| 14 | Elite & Shield | Elite/Shield combo | Elites + Shields | High |
| 15 | Evasive Maneuvers | Fast enemies | Speeders, Zigzags | Medium |
| 16 | Heavy Assault | Very intense | **All Heavy** 🎯🔥🔥 | Maximum |
| 17 | Final Wave | Final challenge | Mixed | High |

**Level-Specific Notes**:

**Level 1** phases follow tutorial → gradual escalation pattern  
**Level 2** phases are more intense with different timing:
- Phase 13 in Level 2 = Calm zone with **increasing cadence** (12s→3s gaps)
- Phase 16 in Level 2 = **Extreme intensity** (waves every 2.5s)
- Phase 17 in Level 2 = Boss battle (510s spawn)

---

## Showcase Programs

Interactive testing environments for each major system. Build with `make all_showcases`.

### Enemy Showcase

Test all 10 enemy types with interactive controls.

**Build and Run:**
```bash
make enemy_showcase
./bin/enemy_showcase
```

**Controls:**
- **Arrow Keys / WASD**: Move player
- **Space**: Shoot
- **Left/Right Arrows**: Select enemy type
- **S**: Spawn selected enemy
- **C**: Clear all enemies
- **A**: Toggle auto-spawn
- **H**: Toggle hitboxes
- **I**: Toggle invulnerability
- **P**: Pause
- **ESC**: Exit

**Features:**
- Test all 10 enemy types
- Visualize hitboxes
- Toggle invulnerability
- Spawn enemies on demand
- Auto-spawn mode for stress testing
- See enemy health and behavior

**Use Cases:**
- Test enemy AI patterns
- Verify collision detection
- Balance enemy health values
- Test enemy projectiles
- Stress test with many enemies

### Player Ship Showcase

Test player ship controls, weapons, and energy modes.

**Build and Run:**
```bash
make player_showcase
./bin/player_ship_showcase
```

**Controls:**
- **Arrow Keys / WASD**: Move
- **Space**: Shoot
- **1-6**: Select weapon mode (Basic, Dual, Triple, Spread, Pierce, Burst)
- **R**: Cycle energy mode (Offensive/Defensive)
- **+/-**: Adjust weapon power level (0-3)
- **TAB**: Toggle stats display
- **H**: Toggle controls help
- **ESC**: Exit

**Features:**
- Test all 6 weapon modes
- Toggle between Offensive/Defensive energy modes
- Adjust weapon power levels (0-3)
- Real-time damage calculations
- Shield and energy visualization
- See weapon stats and power levels

**Use Cases:**
- Test weapon balance
- Verify energy mode switching
- Test power level progression
- Verify movement controls
- Test rapid fire mechanics

### Projectile Showcase

Test all 4 projectile types and enemy firing patterns.

**Build and Run:**
```bash
make projectile_showcase
./bin/projectile_showcase
```

**Controls:**
- **Arrow Keys**: Move
- **Left Mouse**: Fire single projectile
- **Right Mouse**: Fire enemy pattern
- **1-4**: Select projectile type (Laser/Plasma/Missile/Energy Orb)
- **Left/Right**: Select enemy type
- **A**: Toggle auto-fire
- **T**: Toggle trails
- **C**: Clear all projectiles
- **I**: Toggle info panel
- **ESC**: Exit

**Features:**
- Test Laser, Plasma, Missile, and Energy Orb projectiles
- Visualize enemy firing patterns
- Toggle trail effects
- Auto-fire mode
- Real-time projectile count
- See projectile speeds and damage

**Use Cases:**
- Test projectile collision
- Verify firing patterns
- Test projectile speeds
- Verify damage values
- Stress test with many projectiles

### Powerup Showcase

Test powerup system, collection, and drop mechanics.

**Build and Run:**
```bash
make powerup_showcase
./bin/powerup_showcase
```

**Controls:**
- **Arrow Keys**: Move ship
- **Space**: Spawn random powerup
- **1**: Spawn Energy powerup
- **2**: Spawn Shield powerup
- **3**: Spawn Hull powerup
- **4**: Spawn Weapon powerup
- **A**: Toggle auto-spawn
- **R**: Reset player stats
- **I**: Toggle instructions
- **ESC**: Exit

**Features:**
- Test all 4 powerup types (Energy, Shield, Hull, Weapon)
- Magnetic collection visualization
- Drop rate statistics
- Real-time ship status
- Collection effects
- Test magnetic attraction range

**Use Cases:**
- Test powerup drop rates
- Verify magnetic collection
- Test powerup effects
- Balance powerup values
- Test spawn mechanics

### Audio Analysis Tools

#### Audio Analysis GUI

Visual tool for analyzing audio and configuring bass detection.

**Build and Run:**
```bash
make audio_gui
./bin/audio_analysis_gui assets/audio/your_music.mp3
```

**Features:**
- Real-time bass waveform
- FFT spectrum analyzer
- Bass threshold configuration
- Visual intensity indicators
- Configurable smoothing and sensitivity
- Generate bass event logs

**Use Cases:**
- Configure bass detection for new music
- Analyze music intensity patterns
- Generate bass event logs for levels
- Visualize frequency spectrum
- Tune sensitivity settings

#### Audio Analysis CLI

Command-line audio analysis tool.

**Build and Run:**
```bash
make audio_cli
./bin/audio_analysis_cli assets/audio/your_music.mp3
```

**Features:**
- Terminal-based bass detection
- Real-time intensity display
- Configuration file generation
- Performance profiling
- Bass event logging

**Use Cases:**
- Quick bass analysis
- Generate logs for scripting
- Performance testing
- Automated analysis

---

## Pause & Exit Functionality

### Pause/Exit Dialog

**Controls**: Press `ESC` or `START button` during gameplay

**Features**:
- Automatically pauses the game when the dialog appears
- Freezes all game updates (enemies, projectiles, timers)
- Music pauses
- Shows dialog with options to Resume or Exit to Menu
- Press ESC or START again to resume
- Select "Exit to Menu" to return to main menu (game will restart from beginning when you start again)

**Use Cases**:
- Taking a break during gameplay
- Taking screenshots of paused game
- Planning strategies
- Reviewing current situation
- Exiting to main menu safely

**Visual Indicator**:
- Dialog overlay with "GAME PAUSED" title
- Two options: "RESUME GAME" and "EXIT TO MENU"
- Screen dimmed with semi-transparent overlay
- All entities frozen in place

---

## Debug Logging

### Event Logging System

The game includes a comprehensive logging system in `src/utils/logger.c`.

**Enable with debug script**: `./run_debug_game.sh -d -p 1-0`

**Log File**: Generated in project root with timestamp  
Example: `game_2025-10-07_143022.log`

**Logged Events**:
- Enemy spawns with type and position
- Powerup drops with type and location
- Player deaths with cause
- Wave transitions
- Boss encounters
- Game state changes
- Phase changes
- Level transitions

**Example Log Output**:
```
[12.45] Enemy spawned: GRUNT at (1200, 250)
[15.32] Powerup dropped: ENERGY at (850, 300)
[23.18] Wave transition: Phase 2 → Phase 3
[45.67] Boss spawned at (1200, 300)
[67.89] Player death: Collision with TANK
[87.00] Level 1 complete
```

**Enable Extended Logging**:
```c
// In src/utils/logger.c
#define VERBOSE_LOGGING true
```

### Console Output

Real-time console output during gameplay:
- Wave system initialization
- Level loading
- Audio analysis status
- Phase transitions
- Debug settings active
- Error messages
- Performance warnings

**Example Console Output**:
```
[INFO] Loading Level 1: Initiation
[DEBUG] Invulnerability: ENABLED
[DEBUG] Starting at Phase 3
[INFO] Wave system initialized: 127 events
[INFO] Audio loaded: level1.mp3 (553.82s)
[WARN] Phase skipped to 3
```

---

## Testing Workflows

### Test Specific Enemy Type

**Goal**: Test Tank enemies in isolation

**Method 1 - Using Debug Script**:
```bash
./run_debug_game.sh -i -p 1-3  # Level 1, Tank Squadron
# or
./run_debug_game.sh -i -p 2-3  # Level 2, Tank Squadron
```

**Method 2 - Using Enemy Showcase**:
```bash
make enemy_showcase
./bin/enemy_showcase
# Press Left/Right to select Tank
# Press S to spawn
# Press A for auto-spawn
```

### Test Boss Fight

**Goal**: Test boss mechanics and balance

**Level 1 Boss** (427s spawn, 90s battle):
```bash
./run_debug_game.sh -i -p 1-12  # With invulnerability
# or
./run_debug_game.sh -p 1-12     # Without invulnerability (real challenge)
```

**Level 2 Boss** (510s spawn, 90s battle):
```bash
./run_debug_game.sh -i -p 2-17  # With invulnerability
```

### Test Late Game Difficulty

**Goal**: Test endgame balance without playing through entire level

**Level 1 Heavy Assault**:
```bash
./run_debug_game.sh -p 1-16  # Without invulnerability (real challenge)
```

**Level 2 Extreme Intensity**:
```bash
./run_debug_game.sh -p 2-16  # Without invulnerability
```

### Test Weapon Progression

**Goal**: Test all weapons at different power levels

```bash
# Use player showcase
./bin/player_ship_showcase

# In showcase:
# - Press 1-6 to test different weapon modes
# - Press +/- to adjust power levels (0-3)
# - Press R to toggle energy modes
# - Verify damage and firing rate
```

### Test Powerup Drop Rates

**Goal**: Verify powerup spawn rates and collection

```bash
# Use powerup showcase
./bin/powerup_showcase

# In showcase:
# - Press A to enable auto-spawn
# - Observe drop rate statistics
# - Test magnetic collection range
# - Verify powerup effects
```

### Test Level Pacing

**Goal**: Test complete level pacing and rhythm

**Level 1 Full Playthrough**:
```bash
./run_debug_game.sh -i -p 1-0  # With invulnerability
```

**Level 2 Full Playthrough**:
```bash
./run_debug_game.sh -i -p 2-0  # With invulnerability
```

### Compare Levels

**Goal**: Compare difficulty and design between levels

**Test same phase in both levels**:
```bash
# Tank Squadron
./run_debug_game.sh -i -p 1-3  # Level 1
./run_debug_game.sh -i -p 2-3  # Level 2 (compare)

# Boss Battle
./run_debug_game.sh -i -p 1-12  # Level 1
./run_debug_game.sh -i -p 2-17  # Level 2 (compare)
```

### Stress Testing

**Goal**: Test performance under heavy load

**Method**:
```bash
# Use enemy showcase with auto-spawn
make enemy_showcase
./bin/enemy_showcase
# Press A to enable auto-spawn
# Monitor FPS and performance
```

### Collision Testing

**Goal**: Test collision detection accuracy

**Method**:
```bash
# Use enemy showcase with hitboxes
./bin/enemy_showcase
# Press H to show hitboxes
# Press I to enable invulnerability
# Verify hitbox accuracy visually
```

---

## Troubleshooting

### Debug Flags Not Working

**Problem**: Changes to `constants.h` not taking effect

**Solutions**:
1. Verify constants are changed:
   ```bash
   cat include/constants.h | grep DEBUG
   ```

2. Ensure clean rebuild:
   ```bash
   make clean && make
   ```

3. Check for compile errors:
   ```bash
   make 2>&1 | grep error
   ```

4. Use debug script instead (handles all this automatically):
   ```bash
   ./run_debug_game.sh -i -p 1-0
   ```

### Debug Script Not Running

**Problem**: Script fails to execute

**Solutions**:
1. Make script executable:
   ```bash
   chmod +x run_debug_game.sh
   ```

2. Check script exists:
   ```bash
   ls -la run_debug_game.sh
   ```

3. Run with explicit shell:
   ```bash
   bash run_debug_game.sh -i -p 1-0
   ```

### Constants Not Restoring

**Problem**: Debug settings remain after script exit

**Solution**:
```bash
# Manually restore constants
cp include/constants.h.backup include/constants.h

# Or reset manually
nano include/constants.h
# Set:
#   DEBUG_INVULNERABILITY false
#   DEBUG_START_LEVEL 1
#   DEBUG_START_PHASE 0

make clean && make
```

### Showcases Not Building

**Problem**: Showcase programs fail to compile

**Solutions**:
```bash
# Build individually to see specific errors
make enemy_showcase
make player_showcase
make projectile_showcase
make powerup_showcase

# Or all at once
make all_showcases

# Check for library issues
pkg-config --libs --cflags sdl2
```

### Audio Analysis Issues

**Problem**: Audio tools not working

**Solutions**:
1. Ensure BASS library is installed:
   ```bash
   # macOS
   ls /usr/local/lib/libbass.dylib
   
   # Linux  
   ls /usr/local/lib/libbass.so
   ```

2. Check audio file exists:
   ```bash
   ls assets/audio/level1.mp3
   ls assets/audio/level2.mp3
   ```

3. Verify file permissions:
   ```bash
   chmod 644 assets/audio/*.mp3
   ```

### Log Files Growing Too Large

**Problem**: Debug log files consuming disk space

**Solutions**:
```bash
# Clear old logs
rm *.log

# Or keep only recent logs
find . -name "*.log" -mtime +7 -delete

# Add to .gitignore
echo "*.log" >> .gitignore
```

### Phase Number Invalid

**Problem**: `./run_debug_game.sh -p 1-25` fails

**Solution**:
- Phase numbers must be 0-17
- Use `./run_debug_game.sh --list` to see valid phases
- Correct example: `./run_debug_game.sh -p 1-12`

### Level Number Invalid

**Problem**: `./run_debug_game.sh -p 3-0` fails

**Solution**:
- Only levels 1 and 2 exist currently
- Use level 1 or 2
- Correct example: `./run_debug_game.sh -p 2-0`

---

## Debug Visualization

### HUD Debug Info

When debug flags are active, additional information appears on screen:

**Top HUD** (compact badges):
- `[INVULN]` - Invulnerability active (green)
- `[L1-P3]` - Level 1, Phase 3 (orange)

**Bottom HUD** (detailed info):
- `[INVULNERABLE]` - Full invulnerability notice (green, large text)
- `[DEBUG: Level 1, Phase 3]` - Full phase info (orange)

**In-Game Indicators**:
- Hitbox visualization (in showcases)
- Collision detection highlights
- Enemy movement patterns
- Projectile trajectories
- Energy mode indicators

### Visual Debug Elements

**Hitboxes** (in showcases):
- Green rectangle: Player hitbox
- Red rectangles: Enemy hitboxes
- Blue rectangles: Projectile hitboxes
- Yellow rectangles: Powerup hitboxes

**Pause Overlay**:
- Semi-transparent dark overlay
- "PAUSED" text in center
- All entities frozen in place

---

## Performance Profiling

### Frame Rate Monitoring

**Target**: 60 FPS constant

**Monitor During**:
- Heavy enemy spawns (Phase 11, 16)
- Multiple explosions
- Many projectiles on screen
- Max powerups active (20)

**Check FPS**:
- Most systems: Built-in FPS counter in game
- macOS: Use Instruments
- Linux: Use perf or gprof

### Memory Usage

**Track Memory**:
- Initial allocation: ~10-15 MB
- During gameplay: ~15-20 MB
- Should remain constant (no leaks)

**Check Memory**:
- macOS: Activity Monitor
- Linux: top or htop

### Profiling Tools

**macOS**:
```bash
# Use Instruments
instruments -t "Time Profiler" ./bin/shootemup

# Or Activity Monitor
open -a "Activity Monitor"
```

**Linux**:
```bash
# Use valgrind for memory leaks
valgrind --tool=memcheck --leak-check=full ./bin/shootemup

# Use perf for CPU profiling
perf record ./bin/shootemup
perf report
```

---

## Important Notes

### Before Release

Remember to reset debug flags to default values:

```c
#define DEBUG_INVULNERABILITY false
#define DEBUG_START_LEVEL 1
#define DEBUG_START_PHASE 0
```

**Or verify with**:
```bash
cat include/constants.h | grep DEBUG
```

### Showcase Programs

Showcase programs are for development only:
- Do not distribute in release builds
- Use for testing and demonstrations
- Can be removed from Makefile for release
- Not included in main game binary

### Log Files

Debug log files:
- Should be added to `.gitignore`
- Can grow large during long sessions
- Clear periodically: `rm *.log`
- Consider log rotation for extended testing

### Debug Script Safety

The debug script:
- ✅ Automatically backs up `constants.h`
- ✅ Restores settings on normal exit
- ✅ Restores settings on interrupt (Ctrl+C)
- ✅ Uses trap handlers for safety
- ✅ Validates all inputs

---

## Additional Resources

### Documentation

- **[LEVEL1_PHASES.md](LEVEL1_PHASES.md)** - Level 1 complete phase guide
- **[LEVEL2_PHASES.md](LEVEL2_PHASES.md)** - Level 2 complete phase guide
- **[LEVEL_SYSTEM.md](LEVEL_SYSTEM.md)** - Multi-level system architecture
- **[BUILD.md](BUILD.md)** - Compilation and build system
- **[GAMEPLAY.md](GAMEPLAY.md)** - Game controls and mechanics
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design
- **[TODO.md](TODO.md)** - Planned features and improvements

### Quick Links

- **Enemy Types**: See [ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md)
- **Weapons**: See [WEAPONS_AND_DAMAGE.md](WEAPONS_AND_DAMAGE.md)
- **Powerups**: See [POWERUPS.md](POWERUPS.md)
- **Audio**: See [AUDIO_ANALYZER.md](AUDIO_ANALYZER.md)

---

## Summary

This debug system provides comprehensive tools for:

✅ **Quick Testing** - Jump to any level/phase instantly  
✅ **Safe Testing** - Invulnerability mode for exploration  
✅ **Detailed Analysis** - Logging and visualization  
✅ **Isolated Testing** - Showcase programs for each system  
✅ **Easy Access** - Convenient debug script  
✅ **Professional Tools** - Audio analysis and profiling  

**The debug script (`run_debug_game.sh`) is the recommended way to access all debug features!**

```bash
# Remember: Use the debug script for easiest testing!
./run_debug_game.sh -i -p LEVEL-PHASE
```

---

*For the most up-to-date information, check the individual documentation files linked throughout this guide.*