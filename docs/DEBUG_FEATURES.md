# Debug Features

This document describes the debug features and testing tools available for development and debugging.

## Table of Contents
- [Compile-Time Debug Constants](#compile-time-debug-constants)
- [Showcase Programs](#showcase-programs)
- [Pause Functionality](#pause-functionality)
- [Debug Logging](#debug-logging)
- [Testing Workflows](#testing-workflows)

---

## Compile-Time Debug Constants

Debug constants are defined in `include/constants.h` and can be modified to change game behavior for testing purposes.

### 1. Invulnerability Mode

```c
#define DEBUG_INVULNERABILITY false  // Set to true to make player invulnerable
```

**Purpose**: Makes the player immune to all damage from enemies and projectiles.

**How to Enable**:
1. Edit `include/constants.h`
2. Change `DEBUG_INVULNERABILITY` from `false` to `true`
3. Recompile: `make clean && make`

**Visual Indicators**:
- `[INVULNERABLE]` appears in green text at the top of the screen
- `[INVULN]` badge appears in top-right corner

**Use Cases**:
- Testing enemy patterns without dying
- Debugging collision detection
- Exploring all game phases
- Testing weapon systems
- Recording gameplay footage

### 2. Starting Phase Selection

```c
#define DEBUG_START_PHASE 0  // Phase to start at (0 = normal start, 1-17 for specific phases)
```

**Purpose**: Start the game at any specific wave phase for testing.

**How to Use**:
1. Edit `include/constants.h`
2. Set `DEBUG_START_PHASE` to the desired phase number (0-17)
3. Recompile: `make clean && make`

**Phase List**:

| Phase | Time Range | Description | Enemy Focus |
|-------|------------|-------------|-------------|
| 0 | 0s | Normal game start (default) | - |
| 1 | 0-5s | Warm-Up | No enemies |
| 2 | 5-35s | First Wave | Grunt enemies (no firing) |
| 3 | 35-55s | Tank Squadron | Tank enemies |
| 4 | 55-85s | Swarm Attack | Swarm formations |
| 5 | 85-120s | Mixed Assault | Speeders and Grunts |
| 6 | 120-150s | Elite Squadron | Elite enemies |
| 7 | 150-180s | Zigzag Chaos | Zigzag enemies |
| 8 | 180-220s | Shield Wall | Shield enemies |
| 9 | 220-260s | Bomber Run | Bomber enemies |
| 10 | 260-300s | Ghost Ambush | Ghost enemies |
| 11 | 300-345s | Combined Arms | Mixed enemy types |
| 12 | 345-390s | Mini-Boss | Boss encounter |
| 13 | 390-420s | Recovery | Light enemies |
| 14 | 420-460s | Elite & Shield | Elite and Shield combo |
| 15 | 460-500s | Evasive Maneuvers | Fast enemies |
| 16 | 500-540s | Heavy Assault | Heavy enemies |
| 17 | 540-590s | Final Wave | Final challenge |

**Visual Indicators**:
- `[DEBUG: Started at Phase X]` appears in orange text at bottom of screen
- `[PX]` badge appears in top-right corner

**Use Cases**:
- Testing specific enemy types
- Debugging late-game content
- Balancing difficulty curves
- Testing boss encounters
- Quick iteration on specific phases

---

## Showcase Programs

Interactive testing environments for each major system. Build with `make all_showcases`.

### Enemy Showcase

Test all 10 enemy types with interactive controls.

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

### Player Ship Showcase

Test player ship controls, weapons, and energy modes.

```bash
make player_showcase
./bin/player_ship_showcase
```

**Controls:**
- **Arrow Keys / WASD**: Move
- **Space**: Shoot
- **1-6**: Select weapon mode
- **R**: Cycle energy mode (Offensive/Defensive)
- **+/-**: Adjust weapon power level
- **TAB**: Toggle stats display
- **H**: Toggle controls
- **ESC**: Exit

**Features:**
- Test all 6 weapon modes
- Toggle between Offensive/Defensive energy modes
- Adjust weapon power levels (0-3)
- Real-time damage calculations
- Shield and energy visualization

### Projectile Showcase

Test all 4 projectile types and enemy firing patterns.

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

### Powerup Showcase

Test powerup system, collection, and drop mechanics.

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
- Test all 4 powerup types
- Magnetic collection visualization
- Drop rate statistics
- Real-time ship status
- Collection effects

### Audio Analysis Tools

#### Audio Analysis GUI

Visual tool for analyzing audio and configuring bass detection.

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

#### Audio Analysis CLI

Command-line audio analysis tool.

```bash
make audio_cli
./bin/audio_analysis_cli assets/audio/your_music.mp3
```

**Features:**
- Terminal-based bass detection
- Real-time intensity display
- Configuration file generation
- Performance profiling

---

## Pause Functionality

**Controls**: Press `P` or `SPACE` during gameplay

**Features**:
- Freezes all game updates (enemies, projectiles, timers)
- Shows "PAUSED" overlay on screen
- Audio continues playing (for timing reference)
- Press P or SPACE again to resume

**Use Cases**:
- Analyzing enemy positions
- Taking screenshots
- Planning strategies
- Debugging specific moments
- Reviewing wave patterns

---

## Debug Logging

### Event Logging System

The game includes a comprehensive logging system in `src/utils/logger.c`.

**Log File**: Generated in project root (timestamped)

**Logged Events**:
- Enemy spawns with type and position
- Powerup drops with type and location
- Player deaths with cause
- Wave transitions
- Boss encounters
- Game state changes

**Example Log Output**:
```
[12.45] Enemy spawned: GRUNT at (1200, 250)
[15.32] Powerup dropped: ENERGY at (850, 300)
[23.18] Wave transition: Phase 2 → Phase 3
[45.67] Boss spawned at (1200, 300)
[67.89] Player death: Collision with TANK
```

**Enable Extended Logging**:
```c
// In src/utils/logger.c
#define VERBOSE_LOGGING true
```

### Console Output

Real-time console output during gameplay:
- Wave system initialization
- Audio analysis status
- Phase transitions
- Error messages

---

## Testing Workflows

### Test Specific Enemy Type

```c
// in include/constants.h
#define DEBUG_INVULNERABILITY true
#define DEBUG_START_PHASE 3  // Tank Squadron
```

Then run enemy showcase to spawn and test Tanks.

### Test Boss Fight

```c
#define DEBUG_INVULNERABILITY true
#define DEBUG_START_PHASE 12  // Mini-Boss phase
```

### Test Late Game Difficulty

```c
#define DEBUG_INVULNERABILITY false  // Real difficulty
#define DEBUG_START_PHASE 16  // Heavy Assault phase
```

### Test Weapon Progression

```bash
# Use player showcase
./bin/player_ship_showcase

# Adjust power levels with +/-
# Test all 6 weapon modes with 1-6
# Test both energy modes with R
```

### Test Powerup Drop Rates

```bash
# Use powerup showcase
./bin/powerup_showcase

# Enable auto-spawn with A
# Observe drop rate statistics
# Test magnetic collection range
```

### Full Playthrough with Invulnerability

```c
#define DEBUG_INVULNERABILITY true
#define DEBUG_START_PHASE 0  // Start from beginning
```

Run through entire game to test all phases without dying.

---

## Applying Debug Settings

### Step-by-Step Process

1. **Edit the constants**:
   ```bash
   nano include/constants.h
   # or use your preferred text editor: code, vim, emacs, etc.
   ```

2. **Modify the debug values**:
   ```c
   // For invulnerability
   #define DEBUG_INVULNERABILITY true
   
   // To start at Ghost Ambush phase
   #define DEBUG_START_PHASE 10
   ```

3. **Recompile the game**:
   ```bash
   make clean && make
   ```

4. **Run the game**:
   ```bash
   ./bin/shootemup
   ```

### Quick Test Script

Create a test script for rapid iteration:

```bash
#!/bin/bash
# test_phase.sh

PHASE=$1
INVULN=$2

# Update constants
sed -i '' "s/DEBUG_START_PHASE [0-9]*/DEBUG_START_PHASE $PHASE/" include/constants.h
sed -i '' "s/DEBUG_INVULNERABILITY [a-z]*/DEBUG_INVULNERABILITY $INVULN/" include/constants.h

# Rebuild and run
make clean && make && ./bin/shootemup
```

Usage: `./test_phase.sh 12 true`

---

## Debug Visualization

### HUD Debug Info

When debug flags are active, additional information appears:

**Top HUD**:
- `[INVULN]` - Invulnerability active (green)
- `[PX]` - Started at phase X (orange)

**Bottom HUD**:
- `[INVULNERABLE]` - Full invulnerability notice (green, large)
- `[DEBUG: Started at Phase X]` - Phase skip notice (orange)

**In-Game**:
- Hitbox visualization (in showcases)
- Collision detection highlights
- Enemy movement patterns
- Projectile trajectories

---

## Performance Profiling

### Frame Rate Monitoring

Monitor FPS to identify performance issues:
- Target: 60 FPS constant
- Check during heavy enemy spawns
- Monitor during multiple explosions
- Test with max powerups active (20)

### Memory Usage

Track memory allocation:
- Initial allocation: ~10-15 MB
- During gameplay: ~15-20 MB
- Should remain constant (no leaks)

### Profiling Tools

**macOS:**
```bash
# Use Instruments
instruments -t "Time Profiler" ./bin/shootemup
```

**Linux:**
```bash
# Use valgrind
valgrind --tool=memcheck --leak-check=full ./bin/shootemup
```

---

## Important Notes

### Before Release

Remember to reset debug flags to default values:

```c
#define DEBUG_INVULNERABILITY false
#define DEBUG_START_PHASE 0
```

### Showcase Programs

Showcase programs are for development only:
- Do not distribute in release builds
- Use for testing and demonstrations
- Can be removed from Makefile for release

### Debug Script

The `run_debug_game.sh` script provides additional options:
```bash
./run_debug_game.sh --help
```

### Log Files

Debug log files can grow large:
- Clear periodically: `rm *.log`
- Add to `.gitignore`
- Consider log rotation for long sessions

---

## Troubleshooting

### Debug Flags Not Working

1. Verify constants are changed: `cat include/constants.h | grep DEBUG`
2. Ensure clean rebuild: `make clean && make`
3. Check for compile errors: `make 2>&1 | grep error`

### Showcases Not Building

```bash
# Build individually
make enemy_showcase
make player_showcase
make projectile_showcase
make powerup_showcase

# Or all at once
make all_showcases
```

### Audio Analysis Issues

Ensure BASS library is installed:
```bash
# macOS
ls /usr/local/lib/libbass.dylib

# Linux  
ls /usr/local/lib/libbass.so
```

---

## See Also

- **[BUILD.md](BUILD.md)** - Compilation and build system
- **[GAMEPLAY.md](GAMEPLAY.md)** - Game controls and mechanics
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - System design
- **[TODO.md](TODO.md)** - Planned features and improvements