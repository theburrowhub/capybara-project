# Audio Analyzer - Level Design Tool

## Purpose

The Audio Analyzer is a **game development tool** used to analyze music tracks and design static wave patterns for game levels. It helps you understand the intensity and rhythm of your soundtrack so you can create enemy spawn patterns that feel synchronized with the music.

### Important: This is NOT a Runtime System

⚠️ **Key Concept:** The game uses a **static wave system** with predetermined spawn times (see `src/systems/static_waves.c`). This analyzer is a **design-time tool** that helps you create those static waves by analyzing music intensity patterns.

```
DESIGN TIME (This Tool)          →    GAME RUNTIME (Static System)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━         ━━━━━━━━━━━━━━━━━━━━━━━━━━━━
1. Analyze music track                  1. Load predefined spawn times
   (Audio Analyzer)                        from static_waves.c
   ↓                                       ↓
2. Get log with timestamps              2. Spawn enemies at exact times
   and bass intensity                      (no audio analysis)
   ↓                                       ↓
3. Design wave spawns in                3. Consistent experience
   static_waves.c based on                 every playthrough
   intensity patterns
```

### Workflow: From Music to Game Level

```
Step 1: ANALYZE          Step 2: DESIGN           Step 3: PLAY
━━━━━━━━━━━━━━━━━       ━━━━━━━━━━━━━━━━━        ━━━━━━━━━━━━
[Audio Analyzer]    →   [static_waves.c]    →    [Game]
                                                    
level1.mp3              addSpawn(10.5f,          Enemies spawn
    ↓                      ENEMY_ELITE, ...)     at 10.5s
level1.log                                       (predetermined)
Bass at 10.5s HIGH
Bass at 15.2s MEDIUM
Bass at 20.1s LOW
```

### Why Static Waves?

- ✅ **Consistent Experience** - Same spawns every playthrough
- ✅ **Designed Difficulty** - Carefully balanced encounters  
- ✅ **Performance** - No real-time audio analysis overhead
- ✅ **Precise Timing** - Exact spawn times for tight choreography
- ✅ **Music-Informed Design** - Based on actual bass analysis data
- ✅ **Testable** - Predictable behavior for debugging

See **[FLIGHT_PLAN.md](FLIGHT_PLAN.md)** for the current static wave system design.

---

## Game Design Workflow

### Creating a New Level

#### Step 1: Analyze Your Music Track

```bash
# Start the analyzer GUI
make run_audio_gui

# Or use CLI for batch processing
make run_audio_cli
```

**Adjust thresholds to match your track's character:**
- **EDM/Electronic:** `--low 0.10 --medium 0.25 --high 0.50`
- **Rock/Metal:** `--low 0.15 --medium 0.35 --high 0.60` (default)
- **Hip-Hop:** `--low 0.12 --medium 0.30 --high 0.55`
- **Classical:** `--low 0.08 --medium 0.20 --high 0.40`

**Save your configuration:**
```bash
./bin/audio_analysis_cli -f level2.mp3 --low 0.12 --medium 0.30 --high 0.55 --save-config
```

#### Step 2: Review the Generated Log

```bash
# Check the log file (created in assets/audio/)
cat assets/audio/level2.log
```

**Example log output:**
```
=== Bass Detection Log ===
Audio file: assets/audio/level2.mp3
Duration: 180.50 seconds
Thresholds: LOW=0.120, MEDIUM=0.300, HIGH=0.550

[10.50] Bass START - Level: HIGH (Energy: 0.650)
[12.20] Bass CHANGE - Level: MEDIUM (Energy: 0.380)
[14.15] Bass END - Duration: 3.65s

[20.30] Bass START - Level: LOW (Energy: 0.150)
[22.10] Bass END - Duration: 1.80s

[30.00] Bass START - Level: HIGH (Energy: 0.720)
[32.50] Bass CHANGE - Level: MEDIUM (Energy: 0.420)
[35.00] Bass END - Duration: 5.00s

=== Session Summary ===
Total bass events detected: 42
```

**What to look for:**
- 🎵 **Timestamps** - When do bass hits occur?
- ⚡ **Intensity levels** - HIGH/MEDIUM/LOW energy?
- 🌊 **Patterns** - Are there waves of intensity?
- 😌 **Calm periods** - When is there no bass? (recovery time)
- 🔥 **Intense sections** - Where are the peak moments?

#### Step 3: Design Static Waves

Open `src/systems/static_waves.c` and add spawn events based on your analysis.

**Design Principle: Match Intensity to Enemy Types**

```c
// HIGH bass section at 10.5s → spawn challenging enemies
addSpawn(&system->spawns, 10.5f, ENEMY_ELITE, 400, 50);
addSpawn(&system->spawns, 11.0f, ENEMY_TANK, 500, 50);
addSpawn(&system->spawns, 11.5f, ENEMY_ELITE, 300, 50);

// MEDIUM bass at 12.2s → moderate difficulty
addSpawn(&system->spawns, 12.2f, ENEMY_SPEEDER, 350, 50);
addSpawn(&system->spawns, 13.0f, ENEMY_ZIGZAG, 450, 50);

// Calm period (no bass) → sparse spawns for recovery
addSpawn(&system->spawns, 16.0f, ENEMY_GRUNT, 350, 50);
addSpawn(&system->spawns, 18.0f, ENEMY_GRUNT, 400, 50);

// LOW bass at 20.3s → light challenge
addSpawn(&system->spawns, 20.3f, ENEMY_SWARM, 300, 50);
addSpawn(&system->spawns, 20.3f, ENEMY_SWARM, 400, 50);
addSpawn(&system->spawns, 20.3f, ENEMY_SWARM, 500, 50);

// HIGH bass climax at 30.0s → intense wave
addSpawn(&system->spawns, 30.0f, ENEMY_BOSS, 400, 50);
```

**Intensity Mapping:**
- **HIGH Bass** (>0.55) → Elite, Tank, Boss, Bomber
- **MEDIUM Bass** (0.30-0.55) → Speeder, Zigzag, Shield
- **LOW Bass** (0.12-0.30) → Grunt, Swarm, Ghost
- **NONE (Calm)** → Sparse Grunts, recovery time

**Anticipation Timing:**
- Spawn enemies **1-2 seconds BEFORE** bass hits
- This gives them time to enter the screen
- Creates synchronized feel when bass actually hits
- Example: Bass at 10.5s → spawn at 8.5s or 9.0s

#### Step 4: Test and Iterate

```bash
# Build and run the game
make
./bin/shootemup

# Play through your level:
# - Does the action intensity match the music?
# - Are calm periods too empty or too busy?
# - Do peaks feel climactic?
# - Is difficulty progression smooth?

# If not satisfied:
# - Re-analyze with different thresholds
# - Adjust spawn timings in static_waves.c
# - Add/remove spawns to match intensity better
```

---

## Tool Overview

### Features

- **File Selection UI** - Choose from available MP3 files in the audio directory
- **Manual Start Control** - Song starts paused, giving you time to configure
- **Configurable Thresholds** - Tune sensitivity to match your music genre
- **Real-time Visualization** - See spectrum analyzer and spectrogram
- **Three-level Bass Detection** - LOW, MEDIUM, HIGH intensity levels
- **Optional Peak Detection** - Identifies sudden energy spikes/climactic moments (disabled by default)
- **Automatic Logging** - Timestamped logs saved with MP3 name
- **Automatic Backups** - Previous logs backed up (no data loss)
- **Restart Hotkey** - Reset and re-analyze with different settings
- **Command-line Support** - Scriptable for batch processing

---

## Quick Start

### GUI Version (Recommended)

```bash
make run_audio_gui
```

### CLI Version

```bash
# Interactive file selection
make run_audio_cli

# Analyze specific file
./bin/audio_analysis_cli -f level1.mp3

# With custom thresholds
./bin/audio_analysis_cli -f level1.mp3 --low 0.10 --medium 0.25 --high 0.50 --save-config

# Quick test (first 30 seconds)
./bin/audio_analysis_cli -f level1.mp3 -t 30
```

---

## GUI Controls & Workflow

### File Selection
- Use `UP/DOWN` arrows or mouse to select file
- `ENTER` or `SPACE` to confirm selection
- `ESC` to exit

### Playback Controls
- `SPACE` - Start playback (first time) / Pause/Resume
- `R` - Restart song and reset all log data
- `ESC` - Exit

### Threshold Adjustment
- **Hold `1`** + `UP/DOWN` - Adjust LOW threshold (tap repeatedly)
- **Hold `2`** + `UP/DOWN` - Adjust MEDIUM threshold
- **Hold `3`** + `UP/DOWN` - Adjust HIGH threshold
- **Hold `4`** + `UP/DOWN` - Adjust PEAK threshold (only when peak detection enabled)
- `SHIFT` + above - Fine adjustment (0.001 step instead of 0.01)
- `P` - Toggle peak detection on/off
- `S` - Save current settings to config file

### Visual Indicators
- **[Ready]** - Yellow text, song loaded but not started
- **[Playing]** - Green text, analyzing
- **[Paused]** - Orange text, paused
- **Progress bar** - Green when playing, gray when paused
- **Threshold markers** - L/M/H lines on bass energy bar
- **Peak status** - Shows "ENABLED" (green) or "DISABLED" (gray)
- **PEAK! indicator** - Yellow flash when peak detected (only if enabled)
- **Peak count** - Shows total peaks detected (only if enabled)
- **Yellow warning** - Config changed but not saved

### Recommended Workflow
1. Select audio file from list
2. **BEFORE starting**, adjust thresholds (hold 1/2/3 + UP/DOWN)
3. (Optional) Press `P` to enable peak detection
4. Press `SPACE` to start analysis
5. Watch real-time bass detection (and peaks if enabled)
6. Press `R` to restart with different settings if needed
7. Press `S` to save configuration
8. Check generated log file in `assets/audio/`

---

## Configuration System

### Configuration Files

Both programs use persistent configuration files:

- **Default location**: `.audio_analysis.conf` (hidden file in repository root)
- **Custom location**: Can be specified via `--config` option
- **Preserved**: Not deleted by `make clean`
- **Git-ignored**: Config files won't be committed to repository

**File Format:**
```ini
# Bass Analyzer Configuration
# Bass detection thresholds (0.0 - 1.0)

threshold_low=0.150
threshold_medium=0.350
threshold_high=0.600

# Peak detection configuration
# peak_enabled: 0=disabled, 1=enabled (default: 0)
# peak_threshold: energy increase percentage (e.g., 0.20 = 20% jump)
peak_enabled=0
peak_threshold=0.200
```

### Default Values

| Setting | GUI Default | CLI Default |
|---------|-------------|-------------|
| LOW       | 0.05        | 0.15        |
| MEDIUM    | 0.15        | 0.35        |
| HIGH      | 0.30        | 0.60        |
| PEAK Enabled | **Disabled** | **Disabled** |
| PEAK Threshold | 0.20 | 0.20 |

### Command-Line Options

#### CLI Version (`audio_analysis_cli`)

```bash
./bin/audio_analysis_cli [OPTIONS]

Options:
  -f, --file <filename>    Specify MP3 file (relative to assets/audio/)
  -c, --config <path>      Specify configuration file path (default: .audio_analysis.conf)
  -t, --time <seconds>     Limit analysis to first N seconds (default: full song)
  -v, --verbose            Show all energy readings every 0.5s (for debugging)
  --low <value>            Set LOW threshold (default: 0.15)
  --medium <value>         Set MEDIUM threshold (default: 0.35)
  --high <value>           Set HIGH threshold (default: 0.60)
  --enable-peak            Enable peak detection (disabled by default)
  --peak <value>           Set PEAK threshold (default: 0.20 = 20% increase)
  --save-config            Save current settings to config file
  -h, --help               Show help message
```

**CLI Examples:**

```bash
# Use default config (.audio_analysis.conf in repo root)
./bin/audio_analysis_cli -f level1.mp3

# Use custom config file
./bin/audio_analysis_cli -f level1.mp3 --config my_settings.conf

# Save to custom config
./bin/audio_analysis_cli -f level1.mp3 --config my_settings.conf --low 0.10 --save-config

# Config path can be anywhere
./bin/audio_analysis_cli -f level1.mp3 --config ~/configs/audio.conf

# Custom thresholds for EDM track (with peak detection enabled)
./bin/audio_analysis_cli -f edm_level.mp3 --low 0.10 --medium 0.25 --high 0.50 --enable-peak --peak 0.25 --save-config

# Quick 30-second test
./bin/audio_analysis_cli -f new_track.mp3 -t 30

# Verbose mode - see all energy readings (debugging)
./bin/audio_analysis_cli -f level2.mp3 -t 30 -v
```

#### GUI Version (`audio_analysis_gui`)

```bash
./bin/audio_analysis_gui [OPTIONS]

Options:
  -c, --config <path>   Specify configuration file path (default: .audio_analysis.conf)
  -h, --help            Show this help message
```

**GUI Examples:**

```bash
# Use default config
./bin/audio_analysis_gui

# Use custom config file
./bin/audio_analysis_gui --config my_settings.conf

# Show help
./bin/audio_analysis_gui --help
```

### Configuration Behavior

**Default Behavior (No --config specified):**
- Looks for `.audio_analysis.conf` in the current directory (repo root)
- If not found, uses built-in defaults
- No error if config file doesn't exist

**With --config Argument:**
- Loads config from specified path (can be relative or absolute)
- Saves to the same path when using `--save-config` (CLI) or `S` key (GUI)
- If file doesn't exist, uses defaults (no error)

### Multiple Configuration Profiles

You can maintain different configurations for different music genres:

```bash
# Create configs for different genres
./bin/audio_analysis_cli --config rock.conf --low 0.15 --medium 0.35 --high 0.60 --save-config
./bin/audio_analysis_cli --config edm.conf --low 0.10 --medium 0.25 --high 0.50 --save-config
./bin/audio_analysis_cli --config classical.conf --low 0.08 --medium 0.20 --high 0.40 --save-config

# Use them
./bin/audio_analysis_cli -f rock_song.mp3 --config rock.conf
./bin/audio_analysis_cli -f edm_song.mp3 --config edm.conf
./bin/audio_analysis_gui --config classical.conf
```

### Managing Configuration

```bash
# View current default config
cat .audio_analysis.conf

# View specific config
cat my_config.conf

# Manual editing (any text editor)
nano .audio_analysis.conf

# Create default config with your settings
./bin/audio_analysis_cli -f song.mp3 --low 0.10 --medium 0.25 --save-config

# Note: Config files are preserved by 'make clean'
make clean  # Configs NOT deleted
```

---

## Bass Detection System

### How It Works

Bass energy is calculated from the 0-250Hz frequency range using FFT analysis:

```c
// Weight lower frequencies more heavily
for (int i = 2; i < bassEndBin; i++) {
    float weight = 1.0 + (bassEndBin - i) / (float)bassEndBin;
    bassSum += magnitude[i] * magnitude[i] * weight;  // Power
}
bassEnergy = sqrt(bassSum / totalWeight);
```

**Detection Logic:**
- **START:** Previous level = NONE, Current level ≠ NONE
- **CHANGE:** Both levels ≠ NONE, different levels, >100ms apart
- **END:** Previous level ≠ NONE, Current level = NONE

### Threshold Levels

| Level  | Energy Range | Use In Game |
|--------|--------------|-------------|
| NONE   | < threshold_low | Calm periods - sparse enemies, recovery time |
| LOW    | threshold_low to threshold_medium | Light challenge - Grunt, Swarm |
| MEDIUM | threshold_medium to threshold_high | Moderate intensity - Speeder, Zigzag |
| HIGH   | ≥ threshold_high | Peak moments - Elite, Tank, Boss |

### Tuning Tips

**Common Issues:**
1. **Too many detections?** → Increase thresholds
2. **Missing bass hits?** → Decrease thresholds
3. **False positives?** → Increase LOW threshold
4. **Missing subtle bass?** → Decrease LOW threshold

**Recommended Settings by Genre:**

| Genre | LOW | MEDIUM | HIGH | Notes |
|-------|-----|--------|------|-------|
| Electronic/EDM | 0.10 | 0.25 | 0.50 | Heavy, consistent bass |
| Rock/Metal | 0.15 | 0.35 | 0.60 | Default settings |
| Classical/Acoustic | 0.08 | 0.20 | 0.40 | Subtle, nuanced bass |
| Hip-Hop/Rap | 0.12 | 0.30 | 0.55 | Prominent bass lines |

---

## Peak Detection System

### Enabling Peak Detection

**Peak detection is disabled by default.** You must explicitly enable it:

**GUI Version:**
- Press `P` to toggle peak detection on/off
- Status shows as "ENABLED" (green) or "DISABLED" (gray)
- Press `S` to save the enabled state

**CLI Version:**
- Use `--enable-peak` flag to enable
- Use `--save-config` to persist the enabled state
- Example: `./bin/audio_analysis_cli -f level1.mp3 --enable-peak`

### How It Works

Peak detection identifies **sudden energy spikes** - dramatic increases in bass energy that often correspond to climactic moments, drops, or impacts in the music. Unlike bass level detection (which measures absolute energy), peak detection measures **rate of change**.

**Detection Algorithm:**
```c
// Calculate energy increase percentage
energyIncrease = (currentEnergy - previousEnergy) / previousEnergy

// Peak detected if:
// 1. Energy increases by peak_threshold percentage (e.g., 20%)
// 2. At least 0.5 seconds since last peak (debouncing)
if (energyIncrease >= peak_threshold) {
    // Log peak with timestamp and percentage increase
}
```

**Key Characteristics:**
- **Detects sudden spikes** - Not just high energy, but rapid increases
- **Debounced** - Minimum 0.5s between peaks to avoid over-detection
- **Percentage-based** - Adapts to different music dynamics
- **Independent** - Works alongside bass level detection

### Peak Threshold Tuning

**What the threshold means:**
- `0.20` (20%) = Energy must increase by 20% or more
- `0.50` (50%) = Energy must double (increase by 50%+)
- `0.10` (10%) = Very sensitive, catches small spikes
- `1.00` (100%) = Only extreme spikes (energy doubles)

**Tuning Guidelines:**

| Music Type | Recommended Peak | Reasoning |
|------------|------------------|-----------|
| EDM/Dance | 0.25 - 0.35 | Dramatic drops and builds |
| Rock/Metal | 0.20 - 0.30 | Sudden drum hits, crescendos |
| Classical | 0.15 - 0.25 | Subtle but meaningful swells |
| Hip-Hop | 0.20 - 0.30 | Bass drops, beat changes |
| Ambient | 0.30 - 0.50 | Sparse, only major events |

**Common Peak Tuning Issues:**

1. **Too many peaks detected?**
   - Increase threshold (e.g., 0.20 → 0.30)
   - Music might be very dynamic

2. **Missing important moments?**
   - Decrease threshold (e.g., 0.20 → 0.15)
   - Check if drops/impacts are gradual vs. sudden

3. **Peaks at wrong moments?**
   - Peak detection might not suit your music style
   - Consider focusing on bass levels instead

### Using Peaks in Level Design

**Peaks are perfect for:**
- 🎯 **Boss spawn times** - Peak = boss enters
- 💥 **Explosion triggers** - Visual effects on peaks
- 🎵 **Beat sync moments** - Match gameplay to music
- ⚡ **Power-up spawns** - Reward player at climactic moments
- 🌊 **Wave transitions** - Start new wave on peak

**Example Workflow:**

```bash
# Analyze track with peak detection enabled
./bin/audio_analysis_cli -f boss_level.mp3 --enable-peak --peak 0.25

# Check log file
cat assets/audio/boss_level.log

# Found peaks at: 12.5s, 28.3s, 45.1s, 60.8s

# Use in static_waves.c
addSpawn(&system->spawns, 12.5f, ENEMY_BOSS, 400, 50);  // Boss on first peak
addSpawn(&system->spawns, 28.3f, ENEMY_ELITE, 350, 50); // Elite wave on peak
addSpawn(&system->spawns, 45.1f, ENEMY_TANK, 400, 50);  // Tank on peak
```

**Peak Log Format:**
```
[12.50] PEAK detected - Energy: 0.650 (increase: +85.7%)
[28.30] PEAK detected - Energy: 0.720 (increase: +62.3%)
```

### Peak vs. Bass Level Detection

**When to use PEAKS:**
- Identifying climactic moments
- Sync to sudden changes
- Boss/special enemy spawns
- Beat-synchronized events

**When to use BASS LEVELS:**
- Overall intensity tracking
- Gradual difficulty scaling
- Sustained wave patterns
- Ambient intensity matching

**Best Practice:** Use **both together**
- Bass levels → Overall wave intensity
- Peaks → Special event timing

### Why Disabled by Default?

Peak detection is an **opt-in feature** for several reasons:

1. **Cleaner Logs** - Most users only need bass level detection for basic level design
2. **Focused Analysis** - Reduces log clutter when peaks aren't needed
3. **Opt-in Clarity** - Makes it clear that peaks are a specialized feature
4. **Simpler Workflow** - Default behavior is straightforward bass detection
5. **Performance** - Slightly reduced processing overhead (though minimal)

Users who need peak detection for boss fights, climactic moments, or beat-synchronized events can easily enable it with one keypress (`P`) or flag (`--enable-peak`).

### Migration Guide for Existing Users

If you previously used peak detection (when it was enabled by default), you need to explicitly enable it now:

**GUI Workflow:**
1. Press `P` to toggle peak detection ON
2. Status will show "Peak: ENABLED" in green
3. Press `S` to save the enabled state to config
4. Peak detection will remain enabled for future sessions

**CLI Workflow:**
```bash
# Old way (peaks were automatic)
./bin/audio_analysis_cli -f level1.mp3 --peak 0.25

# New way (must enable)
./bin/audio_analysis_cli -f level1.mp3 --enable-peak --peak 0.25

# Save the enabled state
./bin/audio_analysis_cli -f level1.mp3 --enable-peak --peak 0.25 --save-config

# Future runs use saved config
./bin/audio_analysis_cli -f level2.mp3  # Peaks still enabled from config
```

**Backward Compatibility:**
- ✅ Old config files work (peaks default to disabled)
- ✅ All existing bass detection features unchanged
- ✅ Existing commands work (just add `--enable-peak` to enable peaks)
- ✅ GUI shows clear status of peak detection state

---

## Log Files

### Automatic Naming

Every analysis creates a log file in `assets/audio/` with the same name as the MP3:

```
assets/audio/level1.mp3  →  assets/audio/level1.log
assets/audio/level2.mp3  →  assets/audio/level2.log
```

### Automatic Backup

When restarting or running a new session, existing logs are backed up:
```
level1.log  →  level1_backup_20251004_143022.log (backup)
            →  level1.log (new log)
```

### Log Contents

```
=== Bass Detection Log ===
Audio file: assets/audio/level1.mp3
Duration: 554.02 seconds
Thresholds: LOW=0.150, MEDIUM=0.350, HIGH=0.600
Peak Detection: ENABLED (threshold: 0.200 = 20% energy increase)

[10.50] Bass START - Level: HIGH (Energy: 0.650)
[10.50] PEAK detected - Energy: 0.650 (increase: +85.7%)
[12.20] Bass CHANGE - Level: MEDIUM (Energy: 0.380)
[14.15] Bass END - Duration: 3.65s

=== Session Summary ===
Total bass events detected: 42
Total peaks detected: 18
Session duration: 554.02 seconds
```

**Note:** When peak detection is disabled, the log will show `Peak Detection: DISABLED` and no peaks will be logged.

**Using Log Data for Level Design:**
- Timestamps → Spawn times in static_waves.c
- Energy levels → Enemy type selection
- Duration → Wave length/density
- Event count → Overall level intensity

---

## Visualization

### Spectrum Analyzer
- **Red/Orange bars** - Bass frequencies (0-250Hz)
- **Green bars** - Mid frequencies (250Hz-2kHz)
- **Blue bars** - High frequencies (2kHz+)
- **Red line** - 250Hz bass cutoff marker

### Spectrogram (Waterfall Display)
- **Time axis** - Scrolls right to left
- **Frequency axis** - Vertical (0-11kHz shown)
- **Color intensity** - Brighter = stronger signal
- **Color coding** - Red=bass, green=mid, blue=high

### Bass Indicator Panel
- **Energy bar** - Real-time bass energy (0.0-1.0)
- **Level display** - Current detection level
- **Threshold markers** - L/M/H lines on bar
- **Values display** - Current threshold settings
- **Save indicator** - Yellow warning for unsaved changes

---

## Technical Details

### Audio Processing
- **FFT size:** 2048 samples
- **Window function:** Hamming window (reduces spectral leakage)
- **Bass range:** 0-250Hz
- **Sample rate:** 44100 Hz
- **Smoothing:** 70% previous + 30% new (reduces jitter)
- **Debouncing:** 100ms minimum between level changes

### Performance
- **FFT rate:** ~46ms intervals
- **CPU usage:** Minimal (FFT only when buffer full)
- **Memory:** ~500KB for analysis buffers
- **Resolution:** 1200x800 pixels
- **Frame rate:** 60 FPS

---

## Build System

```bash
# Build tools
make audio_gui       # Build GUI version
make audio_cli       # Build CLI version

# Run tools
make run_audio_gui   # Build and run GUI
make run_audio_cli   # Build and run CLI

# Cleanup
make clean          # Remove binaries and build artifacts (preserves config files)
```

### Files Structure

**Source Files:**
- `src/demo/audio_analysis_gui.c` - GUI version with SDL/Raylib
- `src/demo/audio_analysis_cli.c` - Console version (silent analysis)

**Generated Files:**
- `bin/audio_analysis_gui` - GUI binary
- `bin/audio_analysis_cli` - CLI binary
- `.audio_analysis.conf` - Default configuration file (hidden file in repo root)
- `assets/audio/*.log` - Analysis logs
- `assets/audio/*_backup_*.log` - Automatic log backups

**Peak Detection Implementation:**

The peak detection feature is implemented in both programs:

1. **audio_analysis_gui.c** (GUI):
   - Added `peakEnabled` and `peakThreshold` to `BassConfig` struct
   - Added peak detection fields to `AudioAnalyzer` struct (`previousBassEnergy`, `lastPeakTime`, `peakCount`)
   - Implemented conditional peak detection in `analyzeAudioFrame()`
   - Added visual peak indicator with fade-out effect
   - Added keyboard toggle (`P` key) for enable/disable
   - Peak threshold adjustment (Hold `4` + UP/DOWN) only when enabled

2. **audio_analysis_cli.c** (CLI):
   - Added `peakEnabled` and `peakThreshold` to `BassConfig` struct
   - Added peak detection fields to `Analyzer` struct
   - Implemented conditional peak detection in `analyzeAudioFrame()`
   - Added `--enable-peak` command-line flag
   - Peaks logged only when enabled

Both programs share the same config file format and peak detection algorithm.

---

## Relationship to Game Systems

### What This Tool DOES

✅ **Design-Time Analysis**
- Analyzes music tracks to understand intensity patterns
- Generates timestamped logs with bass energy levels
- Provides visual feedback for threshold tuning
- Exports data for manual level design

✅ **Helps You Design**
- Identify peak moments for boss fights
- Find calm sections for recovery periods
- Match wave intensity to music energy
- Create synchronized feeling with static spawns

### What This Tool Does NOT Do

❌ **Runtime Systems**
- Does NOT run during gameplay
- Does NOT affect real-time enemy spawns
- Does NOT require the game to have audio analysis code
- Does NOT create automatic/dynamic waves

❌ **The Game Uses Static Waves**
- All spawns predefined in `static_waves.c`
- Same spawns every playthrough
- No runtime bass detection needed
- Spawns based on manual design (informed by this tool)

### Related Documentation

- **[FLIGHT_PLAN.md](FLIGHT_PLAN.md)** - Current static wave system and level design
- **[static_waves.c](../src/systems/static_waves.c)** - Where you implement wave designs

---

## Advanced Usage

### Batch Analysis

```bash
# Analyze multiple tracks with same settings
./bin/audio_analysis_cli -f level1.mp3 --low 0.12 --save-config
./bin/audio_analysis_cli -f level2.mp3  # Uses saved config
./bin/audio_analysis_cli -f level3.mp3  # Uses saved config

# Compare different threshold settings
./bin/audio_analysis_cli -f track.mp3 --low 0.10 --medium 0.25 --high 0.50 -t 60
./bin/audio_analysis_cli -f track.mp3 --low 0.15 --medium 0.35 --high 0.60 -t 60
```

### A/B Testing Thresholds

```bash
# GUI Workflow:
1. Start demo with level1.mp3
2. Set LOW=0.10, MED=0.25, HIGH=0.50
3. Press SPACE to analyze
4. Press R to restart
5. Set LOW=0.15, MED=0.35, HIGH=0.60
6. Press SPACE to analyze with new settings
7. Compare console output and logs
8. Save preferred settings with S
```

### Configuration Profiles

```bash
# Create genre-specific profiles
./bin/audio_analysis_cli -f edm_track.mp3 --low 0.10 --medium 0.25 --high 0.50 --save-config
cp bin/bass_analyzer.conf bin/bass_analyzer_edm.conf

./bin/audio_analysis_cli -f rock_track.mp3 --low 0.15 --medium 0.35 --high 0.60 --save-config
cp bin/bass_analyzer.conf bin/bass_analyzer_rock.conf

# Load specific profile
cp bin/bass_analyzer_edm.conf bin/bass_analyzer.conf
./bin/audio_analysis_cli -f new_edm.mp3
```

---

## Troubleshooting

### No Bass Events Detected
1. Music might be too quiet - check volume
2. Thresholds too high - try lowering them
3. Track might not have strong bass - try different music
4. Check console for warnings

### Too Many Detections
1. Thresholds too low - increase them
2. Track might be very bass-heavy - adjust accordingly
3. Use fine adjustment (SHIFT + arrows) to dial in

### Config Not Loading
- Check file exists: `ls -la bin/*.conf`
- Check permissions: `chmod 644 bin/*.conf`
- Check format (key=value, no spaces around =)
- Values must be 0.0-1.0

### Log Files Not Generated
- Check `assets/audio/` directory exists
- Check write permissions
- Look for error messages in console
- Verify MP3 file loaded successfully

---

## Summary

### Remember

🎯 **This is a game design tool, not a runtime system**

The Audio Analyzer helps you understand your music's intensity patterns so you can design static wave spawns that *feel* synchronized with the soundtrack. The game itself uses predetermined spawns - no real-time audio analysis happens during gameplay.

**Your Workflow:**
1. Analyze music → Get log with timestamps and intensities
2. Design waves → Use log data to create spawns in static_waves.c
3. Play game → Predetermined spawns create music-synchronized experience

**Result:** A game that feels rhythmic and musical, built on predictable, well-designed static wave patterns.

---

**For more information about the static wave system, see [FLIGHT_PLAN.md](FLIGHT_PLAN.md)**

---

## Future Enhancements (Optional)

Potential improvements for future versions of the audio analyzer:

### Peak Detection Enhancements
- Adjustable debounce time (currently fixed at 0.5s)
- Peak magnitude categories (small/medium/large peaks)
- Peak detection on other frequency ranges (mid, high frequencies)
- Visual waveform display with peak markers
- Export peak timestamps to JSON for automation

### General Improvements
- Multiple simultaneous file analysis
- Batch processing with progress bars
- Custom frequency range analysis (beyond bass)
- Real-time MIDI output for beat sync testing
- Integration with DAW software
- Machine learning-based intensity prediction

### UI/UX Enhancements
- Draggable timeline markers for peak timestamps
- Visual comparison of different threshold settings
- Undo/redo for configuration changes
- Preset management for different genres
- Export log data to CSV/JSON formats

These are optional enhancements and not required for the current static wave system workflow.

---

## Version History

**October 4, 2025** - Peak Detection Feature
- Added optional peak detection (disabled by default)
- Peak threshold configuration (0.0-1.0)
- Toggle control: `P` key (GUI) or `--enable-peak` flag (CLI)
- Conditional logging (peaks only logged when enabled)
- Visual indicators and status display
- Shared configuration between GUI and CLI

**Current Status**: ✅ Fully Implemented and Tested
- macOS (darwin 24.6.0)
- Both GUI and CLI versions working
- Documentation complete
