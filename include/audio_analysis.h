#ifndef AUDIO_ANALYSIS_H
#define AUDIO_ANALYSIS_H

#include <complex.h>
#include <stdio.h>

// FFT and analysis constants
#define FFT_SIZE 2048
#define BASS_FREQ_MAX 250.0f
#define SAMPLE_RATE 44100.0f
#define MAX_MP3_FILES 50
#define MAX_FILENAME_LENGTH 256

// Bass detection thresholds (defaults)
#define BASS_THRESHOLD_LOW_DEFAULT 0.15f
#define BASS_THRESHOLD_MEDIUM_DEFAULT 0.35f
#define BASS_THRESHOLD_HIGH_DEFAULT 0.60f
#define PEAK_THRESHOLD_DEFAULT 0.20f  // Detect peaks when energy jumps by 20%
#define PEAK_DEBOUNCE_TIME 0.5f       // Minimum time between peak detections (seconds)

// Default configuration file path (in repo root)
#define CONFIG_FILE_DEFAULT ".audio_analysis.conf"

// Configuration structure
typedef struct {
    float thresholdLow;
    float thresholdMedium;
    float thresholdHigh;
    float peakThreshold;  // Threshold for peak detection
    int peakEnabled;      // Enable/disable peak detection (0=disabled, 1=enabled)
} BassConfig;

// Bass level enumeration
typedef enum {
    BASS_NONE = 0,
    BASS_LOW,
    BASS_MEDIUM,
    BASS_HIGH
} BassLevel;

// Core FFT and windowing functions
void audio_fft(float complex *x, int n);
void audio_applyWindow(float *samples, int size);

// Bass detection functions
float audio_calculateBassEnergy(const float *magnitude, int magnitudeSize);
BassLevel audio_getBassLevel(float energy, const BassConfig *config);
const char* audio_getBassLevelString(BassLevel level);

// Configuration management
void audio_loadConfig(BassConfig *config, const char *configFilePath);
void audio_saveConfig(const BassConfig *config, const char *configFilePath);

// File utility functions
void audio_getLogFilePath(const char *mp3Path, char *logPath, size_t logPathSize);
void audio_backupLogFile(const char *logPath);
int audio_scanMP3Files(const char *directory, char files[][MAX_FILENAME_LENGTH], int maxFiles);

#endif // AUDIO_ANALYSIS_H
