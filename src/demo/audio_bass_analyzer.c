// Console-based bass analyzer with real FFT analysis
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <complex.h>

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

// Configuration file path
#define CONFIG_FILE "bin/audio_spectrogram.conf"

// Configuration structure
typedef struct {
    float thresholdLow;
    float thresholdMedium;
    float thresholdHigh;
    float peakThreshold;  // Threshold for peak detection
    int peakEnabled;      // Enable/disable peak detection (0=disabled, 1=enabled)
} BassConfig;

typedef enum {
    BASS_NONE = 0,
    BASS_LOW,
    BASS_MEDIUM,
    BASS_HIGH
} BassLevel;

// Global analyzer state
typedef struct {
    float audioBuffer[FFT_SIZE];
    int audioBufferWritePos;
    int samplesCollected;
    float magnitude[FFT_SIZE/2];
    float bassEnergy;
    BassLevel currentBassLevel;
    BassLevel previousBassLevel;
    double bassStartTime;
    int bassEventCount;
    FILE *logFile;
    BassConfig config;
    double lastLogTime;
    // Peak detection
    float previousBassEnergy;
    double lastPeakTime;
    int peakCount;
} Analyzer;

Analyzer *g_analyzer = NULL;

const char* getBassLevelString(BassLevel level) {
    switch(level) {
        case BASS_LOW: return "LOW";
        case BASS_MEDIUM: return "MEDIUM";
        case BASS_HIGH: return "HIGH";
        default: return "NONE";
    }
}

BassLevel getBassLevel(float energy, const BassConfig *config) {
    if (energy < config->thresholdLow) return BASS_NONE;
    if (energy < config->thresholdMedium) return BASS_LOW;
    if (energy < config->thresholdHigh) return BASS_MEDIUM;
    return BASS_HIGH;
}

// Simple FFT implementation (Cooley-Tukey radix-2)
void fft(float complex *x, int n) {
    if (n <= 1) return;
    
    // Bit reversal
    int j = 0;
    for (int i = 1; i < n - 1; i++) {
        int bit = n >> 1;
        while (j & bit) {
            j ^= bit;
            bit >>= 1;
        }
        j ^= bit;
        if (i < j) {
            float complex temp = x[i];
            x[i] = x[j];
            x[j] = temp;
        }
    }
    
    // Cooley-Tukey FFT
    for (int len = 2; len <= n; len <<= 1) {
        float angle = -2 * PI / len;
        float complex wlen = cexp(I * angle);
        
        for (int i = 0; i < n; i += len) {
            float complex w = 1;
            for (int j = 0; j < len / 2; j++) {
                float complex u = x[i + j];
                float complex v = x[i + j + len/2] * w;
                x[i + j] = u + v;
                x[i + j + len/2] = u - v;
                w *= wlen;
            }
        }
    }
}

// Apply Hamming window
void applyWindow(float *samples, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * PI * i / (size - 1));
        samples[i] *= window;
    }
}

// Audio callback to capture processed audio
void AudioProcessorCallback(void *buffer, unsigned int frames) {
    if (g_analyzer == NULL) return;
    
    float *data = (float *)buffer;
    
    // Collect samples for FFT analysis
    for (unsigned int i = 0; i < frames * 2; i += 2) {  // Stereo data
        if (g_analyzer->samplesCollected < FFT_SIZE) {
            // Use left channel
            float sample = data[i];
            g_analyzer->audioBuffer[g_analyzer->audioBufferWritePos] = sample;
            g_analyzer->audioBufferWritePos = (g_analyzer->audioBufferWritePos + 1) % FFT_SIZE;
            g_analyzer->samplesCollected++;
        }
    }
}

// Analyze audio frame
void analyzeAudioFrame(Analyzer *analyzer, double currentTime, int verbose) {
    if (analyzer->samplesCollected < FFT_SIZE) {
        return;  // Not enough samples yet
    }
    
    // Prepare FFT
    float complex fftData[FFT_SIZE];
    float windowedSamples[FFT_SIZE];
    
    for (int i = 0; i < FFT_SIZE; i++) {
        windowedSamples[i] = analyzer->audioBuffer[i];
    }
    applyWindow(windowedSamples, FFT_SIZE);
    
    for (int i = 0; i < FFT_SIZE; i++) {
        fftData[i] = windowedSamples[i];
    }
    
    // Perform FFT
    fft(fftData, FFT_SIZE);
    
    // Calculate magnitude spectrum with smoothing
    for (int i = 0; i < FFT_SIZE/2; i++) {
        float real = crealf(fftData[i]);
        float imag = cimagf(fftData[i]);
        float newMag = sqrtf(real * real + imag * imag) / (FFT_SIZE/4);
        
        // Apply smoothing
        analyzer->magnitude[i] = analyzer->magnitude[i] * 0.7f + newMag * 0.3f;
    }
    
    // Calculate bass energy (0-250Hz)
    analyzer->bassEnergy = 0.0f;
    int bassEndBin = (int)(BASS_FREQ_MAX * FFT_SIZE / SAMPLE_RATE);
    float bassSum = 0.0f;
    float totalWeight = 0.0f;
    
    for (int i = 2; i < bassEndBin && i < FFT_SIZE/2; i++) {
        float weight = 1.0f + (bassEndBin - i) / (float)bassEndBin;
        bassSum += analyzer->magnitude[i] * analyzer->magnitude[i] * weight;
        totalWeight += weight;
    }
    
    if (totalWeight > 0) {
        analyzer->bassEnergy = sqrtf(bassSum / totalWeight);
    }
    
    // Determine bass level
    analyzer->previousBassLevel = analyzer->currentBassLevel;
    
    if (analyzer->bassEnergy < analyzer->config.thresholdLow) {
        analyzer->currentBassLevel = BASS_NONE;
    } else if (analyzer->bassEnergy < analyzer->config.thresholdMedium) {
        analyzer->currentBassLevel = BASS_LOW;
    } else if (analyzer->bassEnergy < analyzer->config.thresholdHigh) {
        analyzer->currentBassLevel = BASS_MEDIUM;
    } else {
        analyzer->currentBassLevel = BASS_HIGH;
    }
    
    // Verbose output - show all energy readings
    if (verbose && ((int)(currentTime * 10) % 5 == 0)) {  // Every 0.5 seconds
        printf("[%06.2f] Energy: %.3f Level: %s\n", 
               currentTime, analyzer->bassEnergy, getBassLevelString(analyzer->currentBassLevel));
    }
    
    // Detect peaks - only if enabled
    if (analyzer->config.peakEnabled) {
        float energyDelta = analyzer->bassEnergy - analyzer->previousBassEnergy;
        float energyIncrease = (analyzer->previousBassEnergy > 0.01f) ? 
                               (energyDelta / analyzer->previousBassEnergy) : 0.0f;
        
        // Detect peak if energy increases by threshold percentage and enough time has passed
        if (energyIncrease >= analyzer->config.peakThreshold && 
            (currentTime - analyzer->lastPeakTime) >= PEAK_DEBOUNCE_TIME) {
            analyzer->lastPeakTime = currentTime;
            analyzer->peakCount++;
            
            printf("[%06.2f] PEAK detected - Energy: %.3f (increase: +%.1f%%)\n",
                   currentTime, analyzer->bassEnergy, energyIncrease * 100.0f);
            if (analyzer->logFile) {
                fprintf(analyzer->logFile, "[%06.2f] PEAK detected - Energy: %.3f (increase: +%.1f%%)\n",
                       currentTime, analyzer->bassEnergy, energyIncrease * 100.0f);
                fflush(analyzer->logFile);
            }
        }
        
        // Update previous energy for next peak detection
        analyzer->previousBassEnergy = analyzer->bassEnergy;
    }
    
    // Log bass events (with debouncing)
    if (analyzer->previousBassLevel == BASS_NONE && analyzer->currentBassLevel != BASS_NONE) {
        // Bass started
        analyzer->bassStartTime = currentTime;
        analyzer->bassEventCount++;
        
        const char *levelStr = getBassLevelString(analyzer->currentBassLevel);
        
        printf("[%06.2f] Bass START - Level: %-6s (Energy: %.3f)\n",
               currentTime, levelStr, analyzer->bassEnergy);
        if (analyzer->logFile) {
            fprintf(analyzer->logFile, "[%06.2f] Bass START - Level: %-6s (Energy: %.3f)\n",
                   currentTime, levelStr, analyzer->bassEnergy);
            fflush(analyzer->logFile);
        }
        analyzer->lastLogTime = currentTime;
    }
    else if (analyzer->previousBassLevel != BASS_NONE && 
             analyzer->currentBassLevel != BASS_NONE &&
             analyzer->previousBassLevel != analyzer->currentBassLevel) {
        // Bass level changed (no debouncing for CHANGE events - log all transitions)
        const char *levelStr = getBassLevelString(analyzer->currentBassLevel);
        
        printf("[%06.2f] Bass CHANGE - Level: %-6s (Energy: %.3f)\n",
               currentTime, levelStr, analyzer->bassEnergy);
        if (analyzer->logFile) {
            fprintf(analyzer->logFile, "[%06.2f] Bass CHANGE - Level: %-6s (Energy: %.3f)\n",
                   currentTime, levelStr, analyzer->bassEnergy);
            fflush(analyzer->logFile);
        }
        analyzer->lastLogTime = currentTime;
    }
    else if (analyzer->previousBassLevel != BASS_NONE && analyzer->currentBassLevel == BASS_NONE) {
        // Bass ended
        float duration = currentTime - analyzer->bassStartTime;
        
        printf("[%06.2f] Bass END   - Duration: %.2fs\n", currentTime, duration);
        if (analyzer->logFile) {
            fprintf(analyzer->logFile, "[%06.2f] Bass END   - Duration: %.2fs\n", currentTime, duration);
            fflush(analyzer->logFile);
        }
        analyzer->lastLogTime = currentTime;
    }
    
    // Reset sample counter for next batch
    analyzer->samplesCollected = 0;
    analyzer->audioBufferWritePos = 0;
}

// Load configuration from file
void loadConfig(BassConfig *config) {
    config->thresholdLow = BASS_THRESHOLD_LOW_DEFAULT;
    config->thresholdMedium = BASS_THRESHOLD_MEDIUM_DEFAULT;
    config->thresholdHigh = BASS_THRESHOLD_HIGH_DEFAULT;
    config->peakThreshold = PEAK_THRESHOLD_DEFAULT;
    config->peakEnabled = 0;  // Disabled by default
    
    FILE *file = fopen(CONFIG_FILE, "r");
    if (file == NULL) {
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char key[64];
        float value;
        if (sscanf(line, "%63[^=]=%f", key, &value) == 2) {
            if (strcmp(key, "threshold_low") == 0) {
                config->thresholdLow = value;
            } else if (strcmp(key, "threshold_medium") == 0) {
                config->thresholdMedium = value;
            } else if (strcmp(key, "threshold_high") == 0) {
                config->thresholdHigh = value;
            } else if (strcmp(key, "peak_threshold") == 0) {
                config->peakThreshold = value;
            } else if (strcmp(key, "peak_enabled") == 0) {
                config->peakEnabled = (int)value;
            }
        }
    }
    
    fclose(file);
    printf("Configuration loaded from: %s\n", CONFIG_FILE);
}

// Save configuration to file
void saveConfig(const BassConfig *config) {
    FILE *file = fopen(CONFIG_FILE, "w");
    if (file == NULL) {
        printf("Warning: Could not save configuration file: %s\n", CONFIG_FILE);
        return;
    }
    
    fprintf(file, "# Bass Analyzer Configuration\n");
    fprintf(file, "# Bass detection thresholds (0.0 - 1.0)\n");
    fprintf(file, "# These values determine when bass is detected at different levels\n\n");
    fprintf(file, "threshold_low=%.3f\n", config->thresholdLow);
    fprintf(file, "threshold_medium=%.3f\n", config->thresholdMedium);
    fprintf(file, "threshold_high=%.3f\n", config->thresholdHigh);
    fprintf(file, "\n# Peak detection configuration\n");
    fprintf(file, "# peak_enabled: 0=disabled, 1=enabled (default: 0)\n");
    fprintf(file, "# peak_threshold: energy increase percentage (e.g., 0.20 = 20%% jump)\n");
    fprintf(file, "peak_enabled=%d\n", config->peakEnabled);
    fprintf(file, "peak_threshold=%.3f\n", config->peakThreshold);
    
    fclose(file);
    printf("Configuration saved to: %s\n", CONFIG_FILE);
}

// Helper function to get log file path from MP3 path
void getLogFilePath(const char *mp3Path, char *logPath, size_t logPathSize) {
    strncpy(logPath, mp3Path, logPathSize - 1);
    logPath[logPathSize - 1] = '\0';
    
    char *ext = strrchr(logPath, '.');
    if (ext != NULL && strcmp(ext, ".mp3") == 0) {
        strcpy(ext, ".log");
    } else {
        strncat(logPath, ".log", logPathSize - strlen(logPath) - 1);
    }
}

// Helper function to backup existing log file
void backupLogFile(const char *logPath) {
    struct stat st;
    if (stat(logPath, &st) != 0) {
        return;  // File doesn't exist
    }
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char backupPath[512];
    char dateStr[64];
    
    strftime(dateStr, sizeof(dateStr), "%Y%m%d_%H%M%S", t);
    
    strncpy(backupPath, logPath, sizeof(backupPath) - 1);
    backupPath[sizeof(backupPath) - 1] = '\0';
    
    char *ext = strrchr(backupPath, '.');
    if (ext != NULL) {
        *ext = '\0';
        snprintf(backupPath, sizeof(backupPath), "%s_backup_%s.log", backupPath, dateStr);
    } else {
        snprintf(backupPath, sizeof(backupPath), "%s_backup_%s", logPath, dateStr);
    }
    
    if (rename(logPath, backupPath) == 0) {
        printf("Backed up existing log to: %s\n", backupPath);
    } else {
        printf("Warning: Could not backup existing log file\n");
    }
}

// Helper function to scan for MP3 files
int scanMP3Files(const char *directory, char files[][MAX_FILENAME_LENGTH], int maxFiles) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        printf("Error: Could not open directory: %s\n", directory);
        return 0;
    }
    
    struct dirent *entry;
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL && count < maxFiles) {
        size_t len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".mp3") == 0) {
            strncpy(files[count], entry->d_name, MAX_FILENAME_LENGTH - 1);
            files[count][MAX_FILENAME_LENGTH - 1] = '\0';
            count++;
        }
    }
    
    closedir(dir);
    return count;
}

void printUsage(const char *programName) {
    printf("Usage: %s [OPTIONS]\n", programName);
    printf("\nOptions:\n");
    printf("  -f, --file <filename>    Specify MP3 file to analyze (relative to assets/audio/)\n");
    printf("  -t, --time <seconds>     Limit analysis to first N seconds (default: full song)\n");
    printf("  -v, --verbose            Show all energy readings (for debugging)\n");
    printf("  --low <value>            Set LOW threshold (default: %.2f)\n", BASS_THRESHOLD_LOW_DEFAULT);
    printf("  --medium <value>         Set MEDIUM threshold (default: %.2f)\n", BASS_THRESHOLD_MEDIUM_DEFAULT);
    printf("  --high <value>           Set HIGH threshold (default: %.2f)\n", BASS_THRESHOLD_HIGH_DEFAULT);
    printf("  --enable-peak            Enable peak detection (disabled by default)\n");
    printf("  --peak <value>           Set PEAK threshold (default: %.2f = %.0f%% increase)\n", 
           PEAK_THRESHOLD_DEFAULT, PEAK_THRESHOLD_DEFAULT * 100.0f);
    printf("  --save-config            Save current settings to config file\n");
    printf("  -h, --help               Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -f level1.mp3                      # Analyze complete level1.mp3\n", programName);
    printf("  %s -f level2.mp3 -t 30                # Analyze first 30 seconds\n", programName);
    printf("  %s -f level2.mp3 -v                   # Verbose mode - show all readings\n", programName);
    printf("  %s -f level1.mp3 --enable-peak --peak 0.25 --save-config  # Enable peak detection\n", programName);
    printf("  %s --file level2.mp3                  # Use saved config if available\n", programName);
    printf("\nConfiguration:\n");
    printf("  Config file: %s\n", CONFIG_FILE);
    printf("  Settings are loaded from config file if it exists\n");
    printf("  Use --save-config to persist changes\n");
    printf("  Peak detection is disabled by default - use --enable-peak to activate\n");
    printf("\nNote: Analysis runs silently in background (no audio playback)\n");
}

int main(int argc, char *argv[]) {
    printf("=== Audio Bass Analyzer (Console Mode - Fast Silent Analysis) ===\n\n");
    
    const char *audioDir = "assets/audio";
    char audioFile[512] = {0};
    float timeLimit = 0.0f;  // 0 means no limit (use full song duration)
    int shouldSaveConfig = 0;
    int verbose = 0;
    
    // Load configuration
    BassConfig config;
    loadConfig(&config);
    
    // Parse command-line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        }
        
        for (int i = 1; i < argc; i++) {
            if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) && i + 1 < argc) {
                snprintf(audioFile, sizeof(audioFile), "%s/%s", audioDir, argv[i + 1]);
                i++;
            } else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) && i + 1 < argc) {
                timeLimit = atof(argv[i + 1]);
                if (timeLimit <= 0) {
                    printf("Error: Time limit must be positive\n");
                    return 1;
                }
                i++;
            } else if (strcmp(argv[i], "--low") == 0 && i + 1 < argc) {
                config.thresholdLow = atof(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "--medium") == 0 && i + 1 < argc) {
                config.thresholdMedium = atof(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "--high") == 0 && i + 1 < argc) {
                config.thresholdHigh = atof(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "--enable-peak") == 0) {
                config.peakEnabled = 1;
            } else if (strcmp(argv[i], "--peak") == 0 && i + 1 < argc) {
                config.peakThreshold = atof(argv[i + 1]);
                i++;
            } else if (strcmp(argv[i], "--save-config") == 0) {
                shouldSaveConfig = 1;
            } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
                verbose = 1;
            } else if (argv[i][0] == '-') {
                printf("Error: Unknown option '%s'\n\n", argv[i]);
                printUsage(argv[0]);
                return 1;
            }
        }
        
        if (shouldSaveConfig) {
            saveConfig(&config);
        }
        
        if (audioFile[0] == '\0') {
            printf("Error: No file specified\n\n");
            printUsage(argv[0]);
            return 1;
        }
    } else {
        // Interactive file selection
        char mp3Files[MAX_MP3_FILES][MAX_FILENAME_LENGTH];
        int fileCount = scanMP3Files(audioDir, mp3Files, MAX_MP3_FILES);
        
        if (fileCount == 0) {
            printf("Error: No MP3 files found in %s\n", audioDir);
            return 1;
        }
        
        printf("Found %d MP3 file(s) in %s:\n", fileCount, audioDir);
        for (int i = 0; i < fileCount; i++) {
            printf("  %d. %s\n", i + 1, mp3Files[i]);
        }
        
        printf("\nEnter file number (1-%d): ", fileCount);
        int selection;
        if (scanf("%d", &selection) != 1 || selection < 1 || selection > fileCount) {
            printf("Invalid selection. Exiting.\n");
            return 1;
        }
        
        snprintf(audioFile, sizeof(audioFile), "%s/%s", audioDir, mp3Files[selection - 1]);
    }
    
    printf("Loading: %s\n", audioFile);
    printf("Mode: Silent background analysis (no audio playback)\n\n");
    
    // Initialize audio
    InitAudioDevice();
    
    // Load music
    Music music = LoadMusicStream(audioFile);
    
    if (music.frameCount == 0) {
        printf("Error: Could not load audio file!\n");
        CloseAudioDevice();
        return 1;
    }
    
    float duration = GetMusicTimeLength(music);
    printf("Successfully loaded audio\n");
    printf("Duration: %.2f seconds\n", duration);
    
    // Validate and set analysis duration
    float analysisDuration;
    
    if (timeLimit > 0) {
        // User specified a time limit - validate it
        if (timeLimit > duration) {
            printf("Error: Time limit (%.2fs) exceeds song duration (%.2fs)\n", timeLimit, duration);
            printf("       Time limit must be between 0 and %.2f seconds\n", duration);
            UnloadMusicStream(music);
            CloseAudioDevice();
            return 1;  // Exit with error
        }
        analysisDuration = timeLimit;
    } else {
        // No limit specified - use full song duration
        analysisDuration = duration;
    }
    
    printf("Bass Thresholds: LOW=%.3f, MEDIUM=%.3f, HIGH=%.3f\n", 
           config.thresholdLow, config.thresholdMedium, config.thresholdHigh);
    printf("Peak Detection: %s", config.peakEnabled ? "ENABLED" : "DISABLED");
    if (config.peakEnabled) {
        printf(" (threshold: %.3f = %.0f%% energy increase)", 
               config.peakThreshold, config.peakThreshold * 100.0f);
    }
    printf("\n");
    
    if (timeLimit > 0 && timeLimit <= duration) {
        printf("Analysis limit: %.2f seconds (first %.1f%% of song)\n\n", 
               analysisDuration, (analysisDuration / duration) * 100);
    } else {
        printf("Analysis: Complete song (%.2f seconds)\n\n", analysisDuration);
    }
    
    // Setup log file
    char logFilePath[512];
    getLogFilePath(audioFile, logFilePath, sizeof(logFilePath));
    backupLogFile(logFilePath);
    
    FILE *logFile = fopen(logFilePath, "w");
    if (logFile) {
        fprintf(logFile, "=== Audio Bass Analyzer Log ===\n");
        fprintf(logFile, "Audio file: %s\n", audioFile);
        fprintf(logFile, "Duration: %.2f seconds\n", duration);
        fprintf(logFile, "Bass Thresholds: LOW=%.3f, MEDIUM=%.3f, HIGH=%.3f\n", 
               config.thresholdLow, config.thresholdMedium, config.thresholdHigh);
        fprintf(logFile, "Peak Detection: %s", config.peakEnabled ? "ENABLED" : "DISABLED");
        if (config.peakEnabled) {
            fprintf(logFile, " (threshold: %.3f = %.0f%% energy increase)", 
                   config.peakThreshold, config.peakThreshold * 100.0f);
        }
        fprintf(logFile, "\n\n");
        fprintf(logFile, "Time format: [seconds from start]\n");
        fprintf(logFile, "=====================================\n\n");
        fflush(logFile);
        printf("Log file created: %s\n", logFilePath);
    } else {
        printf("Warning: Could not create log file: %s\n", logFilePath);
    }
    
    // Initialize analyzer
    Analyzer analyzer = {0};
    analyzer.logFile = logFile;
    analyzer.config = config;
    g_analyzer = &analyzer;
    
    // Attach audio processor
    AttachAudioStreamProcessor(music.stream, AudioProcessorCallback);
    
    // Play music SILENTLY (very low volume)
    PlayMusicStream(music);
    SetMusicVolume(music, 0.001f);  // Nearly inaudible but keeps stream active
    
    printf("\nStarting silent analysis...\n");
    printf("Note: Analysis runs at ~1x speed (%.0f seconds of audio ≈ %.0f seconds to process)\n", 
           analysisDuration, analysisDuration);
    printf("      Will exit when reaching %.2f seconds or when stream ends\n", analysisDuration);
    printf("      Progress updates every 30 seconds\n");
    printf("Time format: [seconds.ms]\n");
    printf("=========================================\n\n");
    
    // Process as fast as possible until we reach the target duration
    float analysisTime = 0.0f;
    float maxTime = analysisDuration;  // This is set to song duration when no timeout specified
    float lastProgressUpdate = 0.0f;
    time_t startWallTime = time(NULL);
    
    while (1) {  // Loop until we explicitly break
        // Update music stream
        UpdateMusicStream(music);
        
        float currentTime = GetMusicTimePlayed(music);
        analysisTime = currentTime;
        
        // Primary exit condition: reached the target duration (with small margin)
        if (currentTime >= (maxTime - 0.1f)) {
            printf("\n[Completed: analyzed %.2f of %.2f seconds]\n", currentTime, maxTime);
            break;
        }
        
        // Secondary exit condition: stream stopped playing
        if (!IsMusicStreamPlaying(music)) {
            printf("\n[Music stream ended at %.2f seconds]\n", analysisTime);
            break;
        }
        
        // Show progress every 30 seconds
        if (currentTime - lastProgressUpdate >= 30.0f) {
            float percent = (currentTime / maxTime) * 100.0f;
            time_t elapsed = time(NULL) - startWallTime;
            printf("Progress: %.1f%% (%.1f/%.1f seconds) - %ld seconds elapsed\n", 
                   percent, currentTime, maxTime, (long)elapsed);
            lastProgressUpdate = currentTime;
        }
        
        // Analyze if we have enough samples
        if (analyzer.samplesCollected >= FFT_SIZE) {
            analyzeAudioFrame(&analyzer, currentTime, verbose);
        }
        
        // NO WAIT - process as fast as possible
    }
    
    time_t totalWallTime = time(NULL) - startWallTime;
    
    printf("\n=========================================\n");
    printf("Analysis complete!\n");
    printf("Total bass events detected: %d\n", analyzer.bassEventCount);
    if (config.peakEnabled) {
        printf("Total peaks detected: %d\n", analyzer.peakCount);
    }
    printf("Analysis duration: %.2f seconds\n", analysisTime);
    printf("Wall clock time: %ld seconds (%.1fx speed)\n", 
           (long)totalWallTime, analysisTime / (totalWallTime > 0 ? totalWallTime : 1));
    
    // Write summary to log file and close it
    if (logFile) {
        fprintf(logFile, "\n=====================================\n");
        fprintf(logFile, "=== Analysis Summary ===\n");
        fprintf(logFile, "Total bass events detected: %d\n", analyzer.bassEventCount);
        if (config.peakEnabled) {
            fprintf(logFile, "Total peaks detected: %d\n", analyzer.peakCount);
        }
        fprintf(logFile, "Analysis duration: %.2f seconds\n", analysisTime);
        fclose(logFile);
        printf("Log file closed: %s\n", logFilePath);
    }
    
    // Cleanup
    DetachAudioStreamProcessor(music.stream, AudioProcessorCallback);
    g_analyzer = NULL;
    UnloadMusicStream(music);
    CloseAudioDevice();
    
    return 0;
}
