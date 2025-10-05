// Shared audio analysis utilities for both GUI and CLI analyzers
#include "raylib.h"
#include "audio_analysis.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

// Simple FFT implementation (Cooley-Tukey radix-2)
void audio_fft(float complex *x, int n) {
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

// Apply Hamming window to reduce spectral leakage
void audio_applyWindow(float *samples, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * PI * i / (size - 1));
        samples[i] *= window;
    }
}

// Calculate bass energy (0-250Hz) with weighted sum
float audio_calculateBassEnergy(const float *magnitude, int magnitudeSize) {
    float bassEnergy = 0.0f;
    int bassEndBin = (int)(BASS_FREQ_MAX * FFT_SIZE / SAMPLE_RATE);
    float bassSum = 0.0f;
    float totalWeight = 0.0f;
    
    // Ensure we don't go beyond the magnitude array
    if (bassEndBin > magnitudeSize) {
        bassEndBin = magnitudeSize;
    }
    
    for (int i = 2; i < bassEndBin; i++) {  // Skip DC component
        // Weight lower frequencies more heavily
        float weight = 1.0f + (bassEndBin - i) / (float)bassEndBin;
        bassSum += magnitude[i] * magnitude[i] * weight;  // Use power
        totalWeight += weight;
    }
    
    // Normalize bass energy
    if (totalWeight > 0) {
        bassEnergy = sqrtf(bassSum / totalWeight);
    }
    
    return bassEnergy;
}

// Determine bass level based on energy and thresholds
BassLevel audio_getBassLevel(float energy, const BassConfig *config) {
    if (energy < config->thresholdLow) return BASS_NONE;
    if (energy < config->thresholdMedium) return BASS_LOW;
    if (energy < config->thresholdHigh) return BASS_MEDIUM;
    return BASS_HIGH;
}

// Get string representation of bass level
const char* audio_getBassLevelString(BassLevel level) {
    switch(level) {
        case BASS_LOW: return "LOW";
        case BASS_MEDIUM: return "MEDIUM";
        case BASS_HIGH: return "HIGH";
        default: return "NONE";
    }
}

// Load configuration from file
void audio_loadConfig(BassConfig *config, const char *configFilePath) {
    // Set defaults
    config->thresholdLow = BASS_THRESHOLD_LOW_DEFAULT;
    config->thresholdMedium = BASS_THRESHOLD_MEDIUM_DEFAULT;
    config->thresholdHigh = BASS_THRESHOLD_HIGH_DEFAULT;
    config->peakThreshold = PEAK_THRESHOLD_DEFAULT;
    config->peakEnabled = 0;  // Disabled by default
    
    // Use provided path or default
    const char *configPath = (configFilePath != NULL) ? configFilePath : CONFIG_FILE_DEFAULT;
    
    FILE *file = fopen(configPath, "r");
    if (file == NULL) {
        return;  // No config file, use defaults
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;
        
        // Parse key=value pairs
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
    printf("Configuration loaded from: %s\n", configPath);
}

// Save configuration to file
void audio_saveConfig(const BassConfig *config, const char *configFilePath) {
    // Use provided path or default
    const char *configPath = (configFilePath != NULL) ? configFilePath : CONFIG_FILE_DEFAULT;
    
    FILE *file = fopen(configPath, "w");
    if (file == NULL) {
        printf("Warning: Could not save configuration file: %s\n", configPath);
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
    printf("Configuration saved to: %s\n", configPath);
}

// Helper function to get log file path from MP3 path
void audio_getLogFilePath(const char *mp3Path, char *logPath, size_t logPathSize) {
    strncpy(logPath, mp3Path, logPathSize - 1);
    logPath[logPathSize - 1] = '\0';
    
    // Replace .mp3 extension with .log
    char *ext = strrchr(logPath, '.');
    if (ext != NULL && strcmp(ext, ".mp3") == 0) {
        strcpy(ext, ".log");
    } else {
        // If no .mp3 extension found, just append .log
        strncat(logPath, ".log", logPathSize - strlen(logPath) - 1);
    }
}

// Helper function to backup existing log file
void audio_backupLogFile(const char *logPath) {
    struct stat st;
    if (stat(logPath, &st) != 0) {
        // File doesn't exist, no backup needed
        return;
    }
    
    // Create backup filename with date
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char backupPath[512];
    char dateStr[64];
    
    strftime(dateStr, sizeof(dateStr), "%Y%m%d_%H%M%S", t);
    
    // Insert "_backup_DATE" before the .log extension
    strncpy(backupPath, logPath, sizeof(backupPath) - 1);
    backupPath[sizeof(backupPath) - 1] = '\0';
    
    char *ext = strrchr(backupPath, '.');
    if (ext != NULL) {
        *ext = '\0';
        snprintf(backupPath, sizeof(backupPath), "%s_backup_%s.log", backupPath, dateStr);
    } else {
        snprintf(backupPath, sizeof(backupPath), "%s_backup_%s", logPath, dateStr);
    }
    
    // Rename existing log file to backup
    if (rename(logPath, backupPath) == 0) {
        printf("Backed up existing log to: %s\n", backupPath);
    } else {
        printf("Warning: Could not backup existing log file\n");
    }
}

// Helper function to scan for MP3 files in a directory
int audio_scanMP3Files(const char *directory, char files[][MAX_FILENAME_LENGTH], int maxFiles) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        printf("Error: Could not open directory: %s\n", directory);
        return 0;
    }
    
    struct dirent *entry;
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL && count < maxFiles) {
        // Check if file has .mp3 extension
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
