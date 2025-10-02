# Audio Spectrogram Demo with Bass Detection

## Overview

Audio analysis demo that loads and plays `assets/audio/level1.mp3` with real-time bass detection, visualization, and automatic logging.

**Features:**
- Real-time spectrum analyzer and spectrogram visualization
- Three-level bass detection (LOW, MEDIUM, HIGH)
- Automatic timestamped log files
- Interactive playback controls

## Quick Start

```bash
# Build and run GUI demo
make run_audio

# Or use the automated script
./run_audio_demo.sh

# Console-only version (first 10 seconds)
make run_analyzer
```

**Controls:**
- `SPACE` - Pause/Resume
- `ESC` - Exit

## Bass Detection Thresholds

| Level  | Energy Threshold | Description |
|--------|-----------------|-------------|
| NONE   | < 0.05          | No significant bass |
| LOW    | 0.05 - 0.15     | Light bass presence |
| MEDIUM | 0.15 - 0.30     | Moderate bass |
| HIGH   | > 0.30          | Heavy bass |

**Adjusting Sensitivity:** Edit these constants in `src/demo/audio_spectrogram.c`:
```c
#define BASS_THRESHOLD_LOW 0.05f
#define BASS_THRESHOLD_MEDIUM 0.15f
#define BASS_THRESHOLD_HIGH 0.30f
```

## Automatic Logging

Every session creates a timestamped log file: `bass_detection_YYYYMMDD_HHMMSS.log`

**Log file includes:**
- Audio file metadata (duration, sample rate)
- Detection thresholds
- All bass events with timestamps
- Session summary

**Example log entries:**
```
[0.50] Bass START - Level: HIGH (Energy: 0.450)
[1.20] Bass CHANGE - Level: MEDIUM (Energy: 0.280)
[2.15] Bass END - Duration: 1.65s
```

## Visualization

### Spectrum Analyzer
- **Red/Orange bars**: Bass frequencies (0-250Hz)
- **Green bars**: Mid frequencies (250Hz-2kHz)
- **Blue bars**: High frequencies (2kHz+)
- **Red line**: 250Hz bass cutoff marker

### Spectrogram
- **Time axis**: Scrolls right to left (waterfall)
- **Frequency axis**: Vertical (0-11kHz shown)
- **Color intensity**: Brighter = stronger signal
- **Color coding**: Same as spectrum (red=bass, green=mid, blue=high)

### Bass Indicator
- **Energy bar**: Real-time bass energy level
- **Level display**: Current detection level (NONE/LOW/MEDIUM/HIGH)
- **Threshold markers**: L/M/H lines showing detection boundaries

## Technical Details

### Audio Processing
- **Capture method**: `AttachAudioStreamProcessor()` for real PCM data
- **FFT size**: 2048 samples
- **Window function**: Hamming window (reduces spectral leakage)
- **Bass range**: 0-250Hz
- **Algorithm**: Power-weighted energy calculation with frequency weighting
- **Smoothing**: 70% previous + 30% new (reduces jitter)
- **Debouncing**: 100ms minimum between level changes

### Bass Energy Calculation
```c
// Weight lower frequencies more heavily
for (int i = 2; i < bassEndBin; i++) {
    float weight = 1.0 + (bassEndBin - i) / (float)bassEndBin;
    bassSum += magnitude[i] * magnitude[i] * weight;  // Power
}
bassEnergy = sqrt(bassSum / totalWeight);
```

### Detection Logic
- **START**: Previous level = NONE, Current level ≠ NONE
- **CHANGE**: Both levels ≠ NONE, different levels, >100ms since last event
- **END**: Previous level ≠ NONE, Current level = NONE

## Build Targets

```bash
# Main demos
make audio_demo      # Build GUI spectrogram demo
make audio_analyzer  # Build console analyzer
make run_audio       # Build and run GUI
make run_analyzer    # Build and run console

# Cleanup
make clean          # Remove all build artifacts
```

## Files

- `src/demo/audio_spectrogram.c` - Main GUI demo with spectrogram
- `src/demo/audio_bass_analyzer.c` - Console-based analyzer
- `run_audio_demo.sh` - Automated run script
- `test_audio.sh` - Quick test script (15 seconds)
- `bass_detection_*.log` - Auto-generated log files (gitignored)

## Requirements

- **Raylib library** (handles audio, graphics, windowing)
- **C99 compiler** (gcc recommended)
- **Audio file**: `assets/audio/level1.mp3`

## Performance

- **FFT rate**: ~46ms intervals @ 44.1kHz
- **CPU usage**: Minimal (FFT only runs when buffer full)
- **Memory**: ~500KB for analysis buffers
- **Resolution**: 1200x800 pixels
- **Frame rate**: 60 FPS

## Troubleshooting

### No bass events detected
1. Check if audio file is playing
2. Verify volume is not muted
3. Lower thresholds in source code if needed
4. Check terminal for warning messages

### Buffer overflow warnings
Normal behavior - Raylib internal warnings, can be ignored

### Low FPS or lag
- Close other applications
- Check CPU usage
- Reduce screen resolution if needed

## Implementation Notes

### Why AttachAudioStreamProcessor?
Raylib's `Music` type doesn't expose raw PCM data directly. Using `AttachAudioStreamProcessor()` allows us to tap into the audio stream as it's being played, capturing real samples for FFT analysis.

### Why only left channel?
Simplifies processing and bass is typically centered in stereo mixes. Can be extended to average both channels if needed.

### Why Hamming window?
Reduces spectral leakage in FFT, giving cleaner frequency response. Alternative windows (Hanning, Blackman) can be used for different characteristics.

### Why power-weighted calculation?
Bass energy is better represented by power (magnitude²) than amplitude. Frequency weighting emphasizes lower frequencies where bass fundamentals exist (40-80Hz).

## Extension Ideas

- Add configurable frequency bands
- Export analysis results to CSV
- Real-time threshold adjustment via UI
- Beat detection based on bass events
- Multiple channel analysis (stereo)
- Custom audio file selection
- Visualization presets/themes

---

**Note:** This demo is designed for development and testing. Log files are automatically gitignored.
