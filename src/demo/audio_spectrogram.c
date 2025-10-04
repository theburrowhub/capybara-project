#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>

// Constants
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define MAX_MP3_FILES 50
#define MAX_FILENAME_LENGTH 256
#define FFT_SIZE 2048
#define SPECTRUM_WIDTH 800
#define SPECTRUM_HEIGHT 400
#define SPECTROGRAM_HEIGHT 200
#define BASS_FREQ_MAX 250.0f  // Bass frequencies up to 250Hz
#define SAMPLE_RATE 44100.0f
#define HISTORY_SIZE 100      // Number of frames to keep in spectrogram history

// Bass detection thresholds (defaults)
#define BASS_THRESHOLD_LOW_DEFAULT 0.05f
#define BASS_THRESHOLD_MEDIUM_DEFAULT 0.15f
#define BASS_THRESHOLD_HIGH_DEFAULT 0.30f
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

typedef struct {
    float magnitude[FFT_SIZE/2];
    float history[HISTORY_SIZE][FFT_SIZE/2];  // Spectrogram history
    int historyIndex;
    float bassEnergy;
    BassLevel currentBassLevel;
    BassLevel previousBassLevel;
    double bassStartTime;
    double bassEndTime;
    int bassEventCount;
    char logBuffer[1024];
    double lastLogTime;
    float audioBuffer[FFT_SIZE];
    int audioBufferWritePos;
    int samplesCollected;
    FILE *logFile;  // File handle for logging
    BassConfig config;  // Configuration
    int configChanged;  // Flag to show config was changed
    // Peak detection
    float previousBassEnergy;
    double lastPeakTime;
    int peakCount;
    float peakEnergy;  // Energy of the last detected peak
    double peakDisplayTime;  // Time to display peak indicator
} AudioAnalyzer;

// Global analyzer for audio callback
AudioAnalyzer *g_analyzer = NULL;

// Load configuration from file
void loadConfig(BassConfig *config) {
    // Set defaults
    config->thresholdLow = BASS_THRESHOLD_LOW_DEFAULT;
    config->thresholdMedium = BASS_THRESHOLD_MEDIUM_DEFAULT;
    config->thresholdHigh = BASS_THRESHOLD_HIGH_DEFAULT;
    config->peakThreshold = PEAK_THRESHOLD_DEFAULT;
    config->peakEnabled = 0;  // Disabled by default
    
    FILE *file = fopen(CONFIG_FILE, "r");
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
    printf("Configuration loaded from: %s\n", CONFIG_FILE);
}

// Save configuration to file
void saveConfig(const BassConfig *config) {
    FILE *file = fopen(CONFIG_FILE, "w");
    if (file == NULL) {
        printf("Warning: Could not save configuration file: %s\n", CONFIG_FILE);
        return;
    }
    
    fprintf(file, "# Audio Spectrogram Configuration\n");
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
void backupLogFile(const char *logPath) {
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
int scanMP3Files(const char *directory, char files[][MAX_FILENAME_LENGTH], int maxFiles) {
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

// Helper function to display file selection screen and get selected file
int selectAudioFile(char *selectedPath, size_t pathSize) {
    const char *audioDir = "assets/audio";
    char mp3Files[MAX_MP3_FILES][MAX_FILENAME_LENGTH];
    int fileCount = scanMP3Files(audioDir, mp3Files, MAX_MP3_FILES);
    
    if (fileCount == 0) {
        printf("Error: No MP3 files found in %s\n", audioDir);
        return 0;
    }
    
    printf("Found %d MP3 file(s) in %s:\n", fileCount, audioDir);
    for (int i = 0; i < fileCount; i++) {
        printf("  %d. %s\n", i + 1, mp3Files[i]);
    }
    
    // Initialize window for file selection
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Spectrogram - Select Audio File");
    SetTargetFPS(60);
    
    int selectedIndex = 0;
    int confirmed = 0;
    
    while (!WindowShouldClose() && !confirmed) {
        // Handle input
        if (IsKeyPressed(KEY_UP)) {
            selectedIndex = (selectedIndex - 1 + fileCount) % fileCount;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            selectedIndex = (selectedIndex + 1) % fileCount;
        }
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            confirmed = 1;
        }
        
        // Mouse selection
        Vector2 mousePos = GetMousePosition();
        int listStartY = 200;
        int itemHeight = 40;
        
        for (int i = 0; i < fileCount; i++) {
            int itemY = listStartY + i * itemHeight;
            if (mousePos.x >= 100 && mousePos.x <= SCREEN_WIDTH - 100 &&
                mousePos.y >= itemY && mousePos.y <= itemY + itemHeight - 5) {
                selectedIndex = i;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    confirmed = 1;
                }
            }
        }
        
        // Draw
        BeginDrawing();
        ClearBackground((Color){20, 20, 30, 255});
        
        DrawText("Select Audio File", 50, 50, 32, WHITE);
        DrawText("Use UP/DOWN arrows or mouse to select, ENTER/SPACE to confirm", 50, 100, 16, GRAY);
        DrawText(TextFormat("Files in: %s", audioDir), 50, 140, 14, DARKGRAY);
        
        // Draw file list
        for (int i = 0; i < fileCount; i++) {
            int itemY = listStartY + i * itemHeight;
            Color bgColor = (i == selectedIndex) ? (Color){60, 120, 180, 255} : (Color){40, 40, 50, 255};
            Color textColor = (i == selectedIndex) ? WHITE : LIGHTGRAY;
            
            DrawRectangle(100, itemY, SCREEN_WIDTH - 200, itemHeight - 5, bgColor);
            DrawRectangleLines(100, itemY, SCREEN_WIDTH - 200, itemHeight - 5, 
                              (i == selectedIndex) ? SKYBLUE : GRAY);
            
            DrawText(mp3Files[i], 120, itemY + 10, 20, textColor);
        }
        
        // Instructions
        DrawText("ESC to exit", 50, SCREEN_HEIGHT - 50, 14, GRAY);
        
        EndDrawing();
    }
    
    if (confirmed) {
        snprintf(selectedPath, pathSize, "%s/%s", audioDir, mp3Files[selectedIndex]);
        printf("\nSelected file: %s\n", selectedPath);
        return 1;
    }
    
    CloseWindow();
    return 0;
}

// Audio callback to capture processed audio
void AudioProcessorCallback(void *buffer, unsigned int frames) {
    if (g_analyzer == NULL) return;
    
    float *data = (float *)buffer;
    
    // Collect samples for FFT analysis
    for (unsigned int i = 0; i < frames * 2; i += 2) {  // Stereo data
        if (g_analyzer->samplesCollected < FFT_SIZE) {
            // Use left channel, normalize from stereo
            float sample = data[i];
            g_analyzer->audioBuffer[g_analyzer->audioBufferWritePos] = sample;
            g_analyzer->audioBufferWritePos = (g_analyzer->audioBufferWritePos + 1) % FFT_SIZE;
            g_analyzer->samplesCollected++;
        }
    }
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

// Apply Hamming window to reduce spectral leakage
void applyWindow(float *samples, int size) {
    for (int i = 0; i < size; i++) {
        float window = 0.54f - 0.46f * cosf(2.0f * PI * i / (size - 1));
        samples[i] *= window;
    }
}

// Analyze audio frame and extract spectrum
void analyzeAudioFrame(AudioAnalyzer *analyzer) {
    if (analyzer->samplesCollected < FFT_SIZE) {
        return;  // Not enough samples yet
    }
    
    // Prepare complex array for FFT
    float complex fftData[FFT_SIZE];
    
    // Copy samples and apply window
    float windowedSamples[FFT_SIZE];
    for (int i = 0; i < FFT_SIZE; i++) {
        windowedSamples[i] = analyzer->audioBuffer[i];
    }
    applyWindow(windowedSamples, FFT_SIZE);
    
    // Convert to complex
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
        
        // Apply smoothing to reduce jitter
        analyzer->magnitude[i] = analyzer->magnitude[i] * 0.7f + newMag * 0.3f;
    }
    
    // Update spectrogram history
    memcpy(analyzer->history[analyzer->historyIndex], analyzer->magnitude, sizeof(float) * (FFT_SIZE/2));
    analyzer->historyIndex = (analyzer->historyIndex + 1) % HISTORY_SIZE;
    
    // Calculate bass energy (0-250Hz) with logarithmic weighting
    analyzer->bassEnergy = 0.0f;
    int bassEndBin = (int)(BASS_FREQ_MAX * FFT_SIZE / SAMPLE_RATE);
    float bassSum = 0.0f;
    float totalWeight = 0.0f;
    
    for (int i = 2; i < bassEndBin && i < FFT_SIZE/2; i++) {  // Skip DC component
        // Weight lower frequencies more heavily
        float weight = 1.0f + (bassEndBin - i) / (float)bassEndBin;
        bassSum += analyzer->magnitude[i] * analyzer->magnitude[i] * weight;  // Use power
        totalWeight += weight;
    }
    
    // Normalize bass energy
    if (totalWeight > 0) {
        analyzer->bassEnergy = sqrtf(bassSum / totalWeight);
    }
    
    // Determine bass level using configured thresholds
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
    
    // Detect peaks - only if enabled
    double currentTime = GetTime();
    
    if (analyzer->config.peakEnabled) {
        // Peak detection: sudden energy increases
        float energyDelta = analyzer->bassEnergy - analyzer->previousBassEnergy;
        float energyIncrease = (analyzer->previousBassEnergy > 0.01f) ? 
                               (energyDelta / analyzer->previousBassEnergy) : 0.0f;
        
        // Detect peak if energy increases by threshold percentage and enough time has passed
        if (energyIncrease >= analyzer->config.peakThreshold && 
            (currentTime - analyzer->lastPeakTime) >= PEAK_DEBOUNCE_TIME) {
            analyzer->lastPeakTime = currentTime;
            analyzer->peakCount++;
            analyzer->peakEnergy = analyzer->bassEnergy;
            analyzer->peakDisplayTime = currentTime;
            
            snprintf(analyzer->logBuffer, sizeof(analyzer->logBuffer),
                    "[%.2f] PEAK detected - Energy: %.3f (increase: +%.1f%%)", 
                    currentTime, analyzer->bassEnergy, energyIncrease * 100.0f);
            
            printf("%s\n", analyzer->logBuffer);
            if (analyzer->logFile) {
                fprintf(analyzer->logFile, "%s\n", analyzer->logBuffer);
                fflush(analyzer->logFile);
            }
        }
        
        // Update previous energy for next peak detection
        analyzer->previousBassEnergy = analyzer->bassEnergy;
    }
    
    // Log bass events (with debouncing to avoid rapid toggles)
    
    // Detect bass start
    if (analyzer->previousBassLevel == BASS_NONE && analyzer->currentBassLevel != BASS_NONE) {
        analyzer->bassStartTime = currentTime;
        analyzer->bassEventCount++;
        
        const char *levelStr = (analyzer->currentBassLevel == BASS_LOW) ? "LOW" :
                               (analyzer->currentBassLevel == BASS_MEDIUM) ? "MEDIUM" : "HIGH";
        
        snprintf(analyzer->logBuffer, sizeof(analyzer->logBuffer),
                "[%.2f] Bass START - Level: %s (Energy: %.3f)", 
                currentTime, levelStr, analyzer->bassEnergy);
        analyzer->lastLogTime = currentTime;
        
        printf("%s\n", analyzer->logBuffer);
        if (analyzer->logFile) {
            fprintf(analyzer->logFile, "%s\n", analyzer->logBuffer);
            fflush(analyzer->logFile);
        }
    }
    // Detect bass level change (log all transitions for accurate level design data)
    else if (analyzer->previousBassLevel != BASS_NONE && 
             analyzer->currentBassLevel != BASS_NONE &&
             analyzer->previousBassLevel != analyzer->currentBassLevel) {
        
        const char *levelStr = (analyzer->currentBassLevel == BASS_LOW) ? "LOW" :
                               (analyzer->currentBassLevel == BASS_MEDIUM) ? "MEDIUM" : "HIGH";
        
        snprintf(analyzer->logBuffer, sizeof(analyzer->logBuffer),
                "[%.2f] Bass CHANGE - Level: %s (Energy: %.3f)", 
                currentTime, levelStr, analyzer->bassEnergy);
        analyzer->lastLogTime = currentTime;
        
        printf("%s\n", analyzer->logBuffer);
        if (analyzer->logFile) {
            fprintf(analyzer->logFile, "%s\n", analyzer->logBuffer);
            fflush(analyzer->logFile);
        }
    }
    // Detect bass end
    else if (analyzer->previousBassLevel != BASS_NONE && analyzer->currentBassLevel == BASS_NONE) {
        analyzer->bassEndTime = currentTime;
        float duration = analyzer->bassEndTime - analyzer->bassStartTime;
        
        snprintf(analyzer->logBuffer, sizeof(analyzer->logBuffer),
                "[%.2f] Bass END - Duration: %.2fs", 
                currentTime, duration);
        analyzer->lastLogTime = currentTime;
        
        printf("%s\n", analyzer->logBuffer);
        if (analyzer->logFile) {
            fprintf(analyzer->logFile, "%s\n", analyzer->logBuffer);
            fflush(analyzer->logFile);
        }
    }
    
    // Reset sample counter for next batch
    analyzer->samplesCollected = 0;
    analyzer->audioBufferWritePos = 0;
}

// Draw spectrum analyzer
void drawSpectrum(AudioAnalyzer *analyzer, int x, int y, int width, int height) {
    // Draw background
    DrawRectangle(x, y, width, height, (Color){20, 20, 30, 255});
    DrawRectangleLines(x, y, width, height, GRAY);
    
    // Draw frequency spectrum bars
    int barCount = width / 4;
    float binStep = (FFT_SIZE/4.0f) / barCount;  // Show only lower half for better visibility
    
    for (int i = 0; i < barCount; i++) {
        int binIndex = (int)(i * binStep);
        if (binIndex >= FFT_SIZE/2) break;
        
        float magnitude = analyzer->magnitude[binIndex];
        float barHeight = magnitude * height * 50.0f;  // Scale for visibility
        if (barHeight > height) barHeight = height;
        
        // Color based on frequency range
        Color barColor;
        float freq = (binIndex * SAMPLE_RATE) / FFT_SIZE;
        if (freq < BASS_FREQ_MAX) {
            // Bass frequencies - red to orange gradient
            int intensity = (int)(magnitude * 2000);
            if (intensity > 255) intensity = 255;
            barColor = (Color){255, intensity, 0, 200};
        } else if (freq < 2000) {
            // Mid frequencies - green
            barColor = (Color){0, 200, 100, 200};
        } else {
            // High frequencies - blue
            barColor = (Color){100, 150, 255, 200};
        }
        
        DrawRectangle(x + i * 4, y + height - (int)barHeight, 3, (int)barHeight, barColor);
    }
    
    // Draw labels
    DrawText("Spectrum Analyzer", x + 5, y + 5, 12, WHITE);
    DrawText("0Hz", x + 5, y + height - 15, 10, GRAY);
    
    // Draw bass frequency marker
    int bassMarkerX = (int)((BASS_FREQ_MAX / (SAMPLE_RATE / 2)) * width);
    DrawLine(x + bassMarkerX, y + 20, x + bassMarkerX, y + height, RED);
    DrawText("250Hz", x + bassMarkerX + 2, y + 20, 10, RED);
    DrawText("11kHz", x + width - 45, y + height - 15, 10, GRAY);
}

// Draw spectrogram (waterfall display)
void drawSpectrogram(AudioAnalyzer *analyzer, int x, int y, int width, int height) {
    // Draw background
    DrawRectangle(x, y, width, height, (Color){10, 10, 20, 255});
    DrawRectangleLines(x, y, width, height, GRAY);
    
    // Draw spectrogram history
    int barWidth = width / HISTORY_SIZE;
    if (barWidth < 1) barWidth = 1;
    
    for (int t = 0; t < HISTORY_SIZE; t++) {
        int histIndex = (analyzer->historyIndex - t - 1 + HISTORY_SIZE) % HISTORY_SIZE;
        int xPos = x + width - (t + 1) * barWidth;
        
        // Draw frequency bins as vertical lines
        int binCount = height;
        float binStep = (FFT_SIZE/4.0f) / binCount;  // Only show lower frequencies
        
        for (int f = 0; f < binCount; f++) {
            int binIndex = (int)(f * binStep);
            if (binIndex >= FFT_SIZE/2) break;
            
            float magnitude = analyzer->history[histIndex][binIndex];
            
            // Color intensity based on magnitude
            int intensity = (int)(magnitude * 3000.0f);
            if (intensity > 255) intensity = 255;
            
            // Color based on frequency range
            float freq = (binIndex * SAMPLE_RATE) / FFT_SIZE;
            Color pixelColor;
            if (freq < BASS_FREQ_MAX) {
                // Bass - red/orange
                pixelColor = (Color){intensity, intensity/2, 0, 255};
            } else if (freq < 2000) {
                // Mid - green
                pixelColor = (Color){0, intensity, intensity/2, 255};
            } else {
                // High - blue
                pixelColor = (Color){intensity/2, intensity/2, intensity, 255};
            }
            
            DrawRectangle(xPos, y + height - f - 1, barWidth, 1, pixelColor);
        }
    }
    
    // Draw labels
    DrawText("Spectrogram (Time →)", x + 5, y + 5, 12, WHITE);
}

// Draw bass level indicator
void drawBassIndicator(AudioAnalyzer *analyzer, int x, int y) {
    // Draw background
    DrawRectangle(x, y, 350, 220, (Color){30, 30, 40, 255});
    DrawRectangleLines(x, y, 350, 220, GRAY);
    
    DrawText("Bass Detection", x + 10, y + 10, 16, WHITE);
    
    // Draw bass energy bar
    int barWidth = (int)(300 * analyzer->bassEnergy);
    if (barWidth > 300) barWidth = 300;
    
    Color barColor;
    const char *levelText;
    
    switch (analyzer->currentBassLevel) {
        case BASS_NONE:
            barColor = DARKGRAY;
            levelText = "NONE";
            break;
        case BASS_LOW:
            barColor = GREEN;
            levelText = "LOW";
            break;
        case BASS_MEDIUM:
            barColor = ORANGE;
            levelText = "MEDIUM";
            break;
        case BASS_HIGH:
            barColor = RED;
            levelText = "HIGH";
            break;
    }
    
    // Draw energy bar
    DrawRectangle(x + 25, y + 45, barWidth, 35, barColor);
    DrawRectangleLines(x + 25, y + 45, 300, 35, WHITE);
    
    // Peak indicator - flash for 0.3 seconds after peak detected (only if enabled)
    if (analyzer->config.peakEnabled) {
        double timeSincePeak = GetTime() - analyzer->peakDisplayTime;
        if (timeSincePeak < 0.3) {
            // Draw flashing peak indicator
            int alpha = (int)(255 * (1.0 - timeSincePeak / 0.3));
            DrawRectangle(x + 25, y + 42, 300, 3, (Color){255, 255, 0, alpha});
            DrawText("PEAK!", x + 280, y + 27, 12, (Color){255, 255, 0, alpha});
        }
    }
    
    // Draw level text
    DrawText(TextFormat("Level: %s", levelText), x + 25, y + 90, 14, WHITE);
    DrawText(TextFormat("Energy: %.3f", analyzer->bassEnergy), x + 25, y + 110, 12, LIGHTGRAY);
    if (analyzer->config.peakEnabled) {
        DrawText(TextFormat("Peaks: %d", analyzer->peakCount), x + 180, y + 110, 12, YELLOW);
    }
    
    // Draw threshold markers
    int lowMark = (int)(300 * analyzer->config.thresholdLow);
    int medMark = (int)(300 * analyzer->config.thresholdMedium);
    int highMark = (int)(300 * analyzer->config.thresholdHigh);
    
    DrawLine(x + 25 + lowMark, y + 40, x + 25 + lowMark, y + 85, GREEN);
    DrawLine(x + 25 + medMark, y + 40, x + 25 + medMark, y + 85, ORANGE);
    DrawLine(x + 25 + highMark, y + 40, x + 25 + highMark, y + 85, RED);
    
    DrawText("L", x + 22 + lowMark, y + 30, 10, GREEN);
    DrawText("M", x + 21 + medMark, y + 30, 10, ORANGE);
    DrawText("H", x + 22 + highMark, y + 30, 10, RED);
    
    // Display threshold values
    DrawText("Thresholds (hold 1/2/3/4 + UP/DOWN, S to save):", x + 10, y + 130, 9, DARKGRAY);
    DrawText(TextFormat("L:%.3f M:%.3f H:%.3f", 
             analyzer->config.thresholdLow, 
             analyzer->config.thresholdMedium, 
             analyzer->config.thresholdHigh), 
             x + 10, y + 145, 10, LIGHTGRAY);
    
    // Peak detection status and settings
    if (analyzer->config.peakEnabled) {
        DrawText(TextFormat("Peak: ENABLED %.2f (%.0f%% increase)", 
                 analyzer->config.peakThreshold,
                 analyzer->config.peakThreshold * 100.0f), 
                 x + 10, y + 160, 10, GREEN);
    } else {
        DrawText("Peak: DISABLED (press P to enable)", x + 10, y + 160, 10, DARKGRAY);
    }
    
    if (analyzer->configChanged) {
        DrawText("* Config changed - press S to save", x + 10, y + 175, 9, YELLOW);
    }
    
    // Peak detection controls
    DrawText("P: toggle peak detection | Hold 4 + UP/DOWN: adjust", x + 10, y + 195, 8, DARKGRAY);
}

// Draw info panel
void drawInfoPanel(AudioAnalyzer *analyzer, Music music, const char *audioFileName, const char *logFileName, int x, int y, bool isPaused) {
    DrawRectangle(x, y, 350, 170, (Color){30, 30, 40, 255});
    DrawRectangleLines(x, y, 350, 170, GRAY);
    
    DrawText("Audio Info", x + 10, y + 10, 16, WHITE);
    
    float musicLength = GetMusicTimeLength(music);
    float musicPlayed = GetMusicTimePlayed(music);
    float progress = musicPlayed / musicLength;
    
    // Show status
    if (isPaused && musicPlayed == 0.0f) {
        DrawText("[Ready - Press SPACE to start]", x + 10, y + 55, 11, YELLOW);
    } else if (isPaused) {
        DrawText("[Paused - Press SPACE to resume]", x + 10, y + 55, 11, ORANGE);
    } else {
        DrawText("[Playing]", x + 10, y + 55, 11, GREEN);
    }
    
    // Extract just the filename from the full path
    const char *displayName = strrchr(audioFileName, '/');
    if (displayName == NULL) {
        displayName = audioFileName;
    } else {
        displayName++; // Skip the '/'
    }
    
    DrawText(TextFormat("File: %s", displayName), x + 10, y + 35, 12, LIGHTGRAY);
    
    DrawText(TextFormat("Time: %.1f / %.1f s", musicPlayed, musicLength), x + 10, y + 75, 12, LIGHTGRAY);
    DrawText(TextFormat("Bass Events: %d", analyzer->bassEventCount), x + 10, y + 95, 12, LIGHTGRAY);
    
    // Log file info
    if (analyzer->logFile && logFileName) {
        DrawText("Logging to file:", x + 10, y + 113, 10, DARKGRAY);
        DrawText(logFileName, x + 10, y + 125, 9, (Color){150, 150, 150, 255});
    }
    
    // Progress bar
    DrawRectangle(x + 10, y + 140, (int)(330 * progress), 20, isPaused ? GRAY : GREEN);
    DrawRectangleLines(x + 10, y + 140, 330, 20, WHITE);
}

int main(void) {
    // Select audio file
    char audioFile[512];
    if (!selectAudioFile(audioFile, sizeof(audioFile))) {
        printf("No file selected. Exiting.\n");
        return 0;
    }
    
    // Initialize window and audio
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Spectrogram Demo - Bass Detection");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Load audio file
    Music music = LoadMusicStream(audioFile);
    
    // Check if music was loaded successfully
    if (music.frameCount == 0) {
        printf("Error: Could not load audio file: %s\n", audioFile);
        CloseAudioDevice();
        CloseWindow();
        return 1;
    }
    
    printf("Successfully loaded: %s\n", audioFile);
    printf("Music duration: %.2f seconds\n", GetMusicTimeLength(music));
    printf("Sample rate: %u Hz\n", music.stream.sampleRate);
    printf("Channels: %u\n", music.stream.channels);
    // Initialize audio analyzer
    AudioAnalyzer analyzer = {0};
    g_analyzer = &analyzer;
    
    // Load configuration
    loadConfig(&analyzer.config);
    
    printf("\n=== Bass Detection Log ===\n");
    printf("Thresholds: LOW=%.3f, MEDIUM=%.3f, HIGH=%.3f\n", 
           analyzer.config.thresholdLow, analyzer.config.thresholdMedium, analyzer.config.thresholdHigh);
    printf("Peak detection: %s", analyzer.config.peakEnabled ? "ENABLED" : "DISABLED");
    if (analyzer.config.peakEnabled) {
        printf(" (threshold: %.3f = %.0f%% energy increase)", 
               analyzer.config.peakThreshold, analyzer.config.peakThreshold * 100.0f);
    }
    printf("\n");
    printf("Bass frequency range: 0-%.0f Hz\n\n", BASS_FREQ_MAX);
    
    // Generate log file path from MP3 path
    char logFilePath[512];
    getLogFilePath(audioFile, logFilePath, sizeof(logFilePath));
    
    // Backup existing log file if it exists
    backupLogFile(logFilePath);
    
    // Open new log file
    analyzer.logFile = fopen(logFilePath, "w");
    
    if (analyzer.logFile) {
        fprintf(analyzer.logFile, "=== Bass Detection Log ===\n");
        fprintf(analyzer.logFile, "Audio file: %s\n", audioFile);
        fprintf(analyzer.logFile, "Duration: %.2f seconds\n", GetMusicTimeLength(music));
        fprintf(analyzer.logFile, "Sample rate: %u Hz\n", music.stream.sampleRate);
        fprintf(analyzer.logFile, "Thresholds: LOW=%.3f, MEDIUM=%.3f, HIGH=%.3f\n", 
               analyzer.config.thresholdLow, analyzer.config.thresholdMedium, analyzer.config.thresholdHigh);
        fprintf(analyzer.logFile, "Peak detection: %s", analyzer.config.peakEnabled ? "ENABLED" : "DISABLED");
        if (analyzer.config.peakEnabled) {
            fprintf(analyzer.logFile, " (threshold: %.3f = %.0f%% energy increase)", 
                   analyzer.config.peakThreshold, analyzer.config.peakThreshold * 100.0f);
        }
        fprintf(analyzer.logFile, "\n");
        fprintf(analyzer.logFile, "Bass frequency range: 0-%.0f Hz\n\n", BASS_FREQ_MAX);
        fprintf(analyzer.logFile, "Time format: [seconds from start]\n");
        fprintf(analyzer.logFile, "=====================================\n\n");
        fflush(analyzer.logFile);
        printf("Log file created: %s\n", logFilePath);
    } else {
        printf("Warning: Could not create log file: %s\n", logFilePath);
    }
    
    // Attach audio processor to capture audio data
    AttachAudioStreamProcessor(music.stream, AudioProcessorCallback);
    
    // Load music but don't start playing yet
    SetMusicVolume(music, 0.7f);
    
    printf("Music loaded. Press SPACE to start, R to restart, ESC to exit\n");
    printf("Adjust thresholds with 1/2/3 + UP/DOWN before starting\n");
    printf("  1: LOW, 2: MEDIUM, 3: HIGH bass thresholds\n");
    printf("  P: Toggle peak detection (currently: %s)\n", 
           analyzer.config.peakEnabled ? "ENABLED" : "DISABLED");
    if (analyzer.config.peakEnabled) {
        printf("  4: PEAK detection threshold (hold 4 + UP/DOWN)\n");
    }
    printf("\n");
    
    // Track pause state - start paused
    bool isPaused = true;
    
    // Main loop
    while (!WindowShouldClose()) {
        // Update music
        UpdateMusicStream(music);
        
        // Analyze audio if we have enough samples
        if (analyzer.samplesCollected >= FFT_SIZE) {
            analyzeAudioFrame(&analyzer);
        }
        
        // Restart music if it ends (but not if manually paused)
        if (!isPaused && !IsMusicStreamPlaying(music)) {
            StopMusicStream(music);
            PlayMusicStream(music);
            printf("\n=== Music Restarted ===\n\n");
        }
        
        // Controls - Play/Pause toggle
        if (IsKeyPressed(KEY_SPACE)) {
            if (isPaused) {
                // Start or resume music
                if (!IsMusicStreamPlaying(music)) {
                    PlayMusicStream(music);
                    printf("[%.2f] Music STARTED\n", GetTime());
                } else {
                    ResumeMusicStream(music);
                    printf("[%.2f] Music RESUMED\n", GetTime());
                }
                isPaused = false;
            } else {
                PauseMusicStream(music);
                isPaused = true;
                printf("[%.2f] Music PAUSED\n", GetTime());
            }
        }
        
        // Restart hotkey (R) - Reset song and log data
        if (IsKeyPressed(KEY_R)) {
            StopMusicStream(music);
            SeekMusicStream(music, 0.0f);
            
            // Reset analyzer state
            analyzer.bassEventCount = 0;
            analyzer.bassStartTime = 0;
            analyzer.bassEndTime = 0;
            analyzer.currentBassLevel = BASS_NONE;
            analyzer.previousBassLevel = BASS_NONE;
            analyzer.historyIndex = 0;
            memset(analyzer.history, 0, sizeof(analyzer.history));
            memset(analyzer.magnitude, 0, sizeof(analyzer.magnitude));
            analyzer.samplesCollected = 0;
            analyzer.audioBufferWritePos = 0;
            // Reset peak detection
            analyzer.peakCount = 0;
            analyzer.previousBassEnergy = 0.0f;
            analyzer.lastPeakTime = 0.0;
            analyzer.peakEnergy = 0.0f;
            analyzer.peakDisplayTime = 0.0;
            
            // Reopen log file
            if (analyzer.logFile) {
                fclose(analyzer.logFile);
            }
            backupLogFile(logFilePath);
            analyzer.logFile = fopen(logFilePath, "w");
            if (analyzer.logFile) {
                fprintf(analyzer.logFile, "=== Bass Detection Log ===\n");
                fprintf(analyzer.logFile, "Audio file: %s\n", audioFile);
                fprintf(analyzer.logFile, "Duration: %.2f seconds\n", GetMusicTimeLength(music));
                fprintf(analyzer.logFile, "Sample rate: %u Hz\n", music.stream.sampleRate);
                fprintf(analyzer.logFile, "Thresholds: LOW=%.3f, MEDIUM=%.3f, HIGH=%.3f\n", 
                       analyzer.config.thresholdLow, analyzer.config.thresholdMedium, analyzer.config.thresholdHigh);
                fprintf(analyzer.logFile, "Peak detection: %s", analyzer.config.peakEnabled ? "ENABLED" : "DISABLED");
                if (analyzer.config.peakEnabled) {
                    fprintf(analyzer.logFile, " (threshold: %.3f = %.0f%% energy increase)", 
                           analyzer.config.peakThreshold, analyzer.config.peakThreshold * 100.0f);
                }
                fprintf(analyzer.logFile, "\n");
                fprintf(analyzer.logFile, "Bass frequency range: 0-%.0f Hz\n\n", BASS_FREQ_MAX);
                fprintf(analyzer.logFile, "Time format: [seconds from start]\n");
                fprintf(analyzer.logFile, "=====================================\n\n");
                fflush(analyzer.logFile);
            }
            
            printf("\n=== RESTART - Song and log data reset ===\n");
            printf("Press SPACE to start playback\n\n");
            isPaused = true;
        }
        
        // Threshold adjustment controls
        // Hold 1/2/3 and press UP/DOWN to adjust thresholds
        float step = 0.01f;  // Adjustment step
        if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
            step = 0.001f;  // Fine adjustment with shift
        }
        
        // Adjust LOW threshold (hold 1, then press UP/DOWN)
        if (IsKeyDown(KEY_ONE)) {
            if (IsKeyPressed(KEY_UP)) {
                analyzer.config.thresholdLow += step;
                if (analyzer.config.thresholdLow > 1.0f) analyzer.config.thresholdLow = 1.0f;
                analyzer.configChanged = 1;
                printf("LOW threshold: %.3f\n", analyzer.config.thresholdLow);
            } else if (IsKeyPressed(KEY_DOWN)) {
                analyzer.config.thresholdLow -= step;
                if (analyzer.config.thresholdLow < 0.0f) analyzer.config.thresholdLow = 0.0f;
                analyzer.configChanged = 1;
                printf("LOW threshold: %.3f\n", analyzer.config.thresholdLow);
            }
        }
        
        // Adjust MEDIUM threshold (hold 2, then press UP/DOWN)
        if (IsKeyDown(KEY_TWO)) {
            if (IsKeyPressed(KEY_UP)) {
                analyzer.config.thresholdMedium += step;
                if (analyzer.config.thresholdMedium > 1.0f) analyzer.config.thresholdMedium = 1.0f;
                analyzer.configChanged = 1;
                printf("MEDIUM threshold: %.3f\n", analyzer.config.thresholdMedium);
            } else if (IsKeyPressed(KEY_DOWN)) {
                analyzer.config.thresholdMedium -= step;
                if (analyzer.config.thresholdMedium < 0.0f) analyzer.config.thresholdMedium = 0.0f;
                analyzer.configChanged = 1;
                printf("MEDIUM threshold: %.3f\n", analyzer.config.thresholdMedium);
            }
        }
        
        // Adjust HIGH threshold (hold 3, then press UP/DOWN)
        if (IsKeyDown(KEY_THREE)) {
            if (IsKeyPressed(KEY_UP)) {
                analyzer.config.thresholdHigh += step;
                if (analyzer.config.thresholdHigh > 1.0f) analyzer.config.thresholdHigh = 1.0f;
                analyzer.configChanged = 1;
                printf("HIGH threshold: %.3f\n", analyzer.config.thresholdHigh);
            } else if (IsKeyPressed(KEY_DOWN)) {
                analyzer.config.thresholdHigh -= step;
                if (analyzer.config.thresholdHigh < 0.0f) analyzer.config.thresholdHigh = 0.0f;
                analyzer.configChanged = 1;
                printf("HIGH threshold: %.3f\n", analyzer.config.thresholdHigh);
            }
        }
        
        // Adjust PEAK threshold (hold 4, then press UP/DOWN) - only if enabled
        if (IsKeyDown(KEY_FOUR) && analyzer.config.peakEnabled) {
            if (IsKeyPressed(KEY_UP)) {
                analyzer.config.peakThreshold += step;
                if (analyzer.config.peakThreshold > 1.0f) analyzer.config.peakThreshold = 1.0f;
                analyzer.configChanged = 1;
                printf("PEAK threshold: %.3f (%.0f%% energy increase)\n", 
                       analyzer.config.peakThreshold, analyzer.config.peakThreshold * 100.0f);
            } else if (IsKeyPressed(KEY_DOWN)) {
                analyzer.config.peakThreshold -= step;
                if (analyzer.config.peakThreshold < 0.0f) analyzer.config.peakThreshold = 0.0f;
                analyzer.configChanged = 1;
                printf("PEAK threshold: %.3f (%.0f%% energy increase)\n", 
                       analyzer.config.peakThreshold, analyzer.config.peakThreshold * 100.0f);
            }
        }
        
        // Toggle peak detection (P key)
        if (IsKeyPressed(KEY_P)) {
            analyzer.config.peakEnabled = !analyzer.config.peakEnabled;
            analyzer.configChanged = 1;
            printf("Peak detection: %s\n", analyzer.config.peakEnabled ? "ENABLED" : "DISABLED");
        }
        
        // Save configuration
        if (IsKeyPressed(KEY_S)) {
            saveConfig(&analyzer.config);
            analyzer.configChanged = 0;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Title
        DrawText("Audio Spectrogram Demo with Real-Time Bass Detection", 20, 20, 22, WHITE);
        
        // Show appropriate instructions based on state
        if (isPaused && GetMusicTimePlayed(music) == 0.0f) {
            DrawText("SPACE: start | Hold 1/2/3 + UP/DOWN: adjust | P: toggle peak | S: save | ESC: exit", 20, 48, 11, YELLOW);
        } else {
            DrawText("SPACE: pause/resume | R: restart | Hold 1/2/3 + UP/DOWN: adjust | P: peak | S: save | ESC: exit", 20, 48, 10, GRAY);
        }
        
        // Draw visualizations
        drawSpectrum(&analyzer, 50, 90, SPECTRUM_WIDTH, SPECTRUM_HEIGHT);
        drawSpectrogram(&analyzer, 50, 510, SPECTRUM_WIDTH, SPECTROGRAM_HEIGHT);
        drawBassIndicator(&analyzer, 870, 90);
        drawInfoPanel(&analyzer, music, audioFile, logFilePath, 870, 250, isPaused);
        
        EndDrawing();
    }
    
    // Cleanup
    printf("\n=== Session Summary ===\n");
    printf("Total bass events detected: %d\n", analyzer.bassEventCount);
    if (analyzer.config.peakEnabled) {
        printf("Total peaks detected: %d\n", analyzer.peakCount);
    }
    printf("Session duration: %.2f seconds\n", GetTime());
    
    // Write summary to log file and close it
    if (analyzer.logFile) {
        fprintf(analyzer.logFile, "\n=====================================\n");
        fprintf(analyzer.logFile, "=== Session Summary ===\n");
        fprintf(analyzer.logFile, "Total bass events detected: %d\n", analyzer.bassEventCount);
        if (analyzer.config.peakEnabled) {
            fprintf(analyzer.logFile, "Total peaks detected: %d\n", analyzer.peakCount);
        }
        fprintf(analyzer.logFile, "Session duration: %.2f seconds\n", GetTime());
        fclose(analyzer.logFile);
        printf("Log file closed: %s\n", logFilePath);
    }
    
    DetachAudioStreamProcessor(music.stream, AudioProcessorCallback);
    g_analyzer = NULL;
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
