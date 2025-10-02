#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Constants
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define FFT_SIZE 2048
#define SPECTRUM_WIDTH 800
#define SPECTRUM_HEIGHT 400
#define SPECTROGRAM_HEIGHT 200
#define BASS_FREQ_MAX 250.0f  // Bass frequencies up to 250Hz
#define SAMPLE_RATE 44100.0f
#define HISTORY_SIZE 100      // Number of frames to keep in spectrogram history

// Bass detection thresholds (normalized 0-1)
#define BASS_THRESHOLD_LOW 0.05f
#define BASS_THRESHOLD_MEDIUM 0.15f
#define BASS_THRESHOLD_HIGH 0.30f

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
} AudioAnalyzer;

// Global analyzer for audio callback
AudioAnalyzer *g_analyzer = NULL;

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
    
    // Determine bass level
    analyzer->previousBassLevel = analyzer->currentBassLevel;
    
    if (analyzer->bassEnergy < BASS_THRESHOLD_LOW) {
        analyzer->currentBassLevel = BASS_NONE;
    } else if (analyzer->bassEnergy < BASS_THRESHOLD_MEDIUM) {
        analyzer->currentBassLevel = BASS_LOW;
    } else if (analyzer->bassEnergy < BASS_THRESHOLD_HIGH) {
        analyzer->currentBassLevel = BASS_MEDIUM;
    } else {
        analyzer->currentBassLevel = BASS_HIGH;
    }
    
    // Log bass events (with debouncing to avoid rapid toggles)
    double currentTime = GetTime();
    
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
    // Detect bass level change (only if significant and stable for a moment)
    else if (analyzer->previousBassLevel != BASS_NONE && 
             analyzer->currentBassLevel != BASS_NONE &&
             analyzer->previousBassLevel != analyzer->currentBassLevel &&
             currentTime - analyzer->lastLogTime > 0.1) {  // Debounce 100ms
        
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
    DrawRectangle(x, y, 350, 140, (Color){30, 30, 40, 255});
    DrawRectangleLines(x, y, 350, 140, GRAY);
    
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
    
    // Draw level text
    DrawText(TextFormat("Level: %s", levelText), x + 25, y + 90, 14, WHITE);
    DrawText(TextFormat("Energy: %.3f", analyzer->bassEnergy), x + 25, y + 110, 12, LIGHTGRAY);
    
    // Draw threshold markers
    int lowMark = (int)(300 * BASS_THRESHOLD_LOW);
    int medMark = (int)(300 * BASS_THRESHOLD_MEDIUM);
    int highMark = (int)(300 * BASS_THRESHOLD_HIGH);
    
    DrawLine(x + 25 + lowMark, y + 40, x + 25 + lowMark, y + 85, GREEN);
    DrawLine(x + 25 + medMark, y + 40, x + 25 + medMark, y + 85, ORANGE);
    DrawLine(x + 25 + highMark, y + 40, x + 25 + highMark, y + 85, RED);
    
    DrawText("L", x + 22 + lowMark, y + 30, 10, GREEN);
    DrawText("M", x + 21 + medMark, y + 30, 10, ORANGE);
    DrawText("H", x + 22 + highMark, y + 30, 10, RED);
}

// Draw info panel
void drawInfoPanel(AudioAnalyzer *analyzer, Music music, const char *logFileName, int x, int y) {
    DrawRectangle(x, y, 350, 170, (Color){30, 30, 40, 255});
    DrawRectangleLines(x, y, 350, 170, GRAY);
    
    DrawText("Audio Info", x + 10, y + 10, 16, WHITE);
    
    float musicLength = GetMusicTimeLength(music);
    float musicPlayed = GetMusicTimePlayed(music);
    float progress = musicPlayed / musicLength;
    
    DrawText(TextFormat("File: level1.mp3"), x + 10, y + 35, 12, LIGHTGRAY);
    DrawText(TextFormat("Time: %.1f / %.1f s", musicPlayed, musicLength), x + 10, y + 55, 12, LIGHTGRAY);
    DrawText(TextFormat("Bass Events: %d", analyzer->bassEventCount), x + 10, y + 75, 12, LIGHTGRAY);
    
    // Log file info
    if (analyzer->logFile && logFileName) {
        DrawText("Logging to file:", x + 10, y + 93, 10, DARKGRAY);
        DrawText(logFileName, x + 10, y + 105, 9, (Color){150, 150, 150, 255});
    }
    
    // Progress bar
    DrawRectangle(x + 10, y + 120, (int)(330 * progress), 20, GREEN);
    DrawRectangleLines(x + 10, y + 120, 330, 20, WHITE);
    
    // Last log message (show for 2 seconds)
    if (strlen(analyzer->logBuffer) > 0 && GetTime() - analyzer->lastLogTime < 2.0) {
        DrawText(analyzer->logBuffer, x + 10, y + 148, 9, YELLOW);
    }
}

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Audio Spectrogram Demo - Bass Detection");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Load audio file
    const char *audioFile = "assets/audio/level1.mp3";
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
    printf("\n=== Bass Detection Log ===\n");
    printf("Thresholds: LOW=%.2f, MEDIUM=%.2f, HIGH=%.2f\n", 
           BASS_THRESHOLD_LOW, BASS_THRESHOLD_MEDIUM, BASS_THRESHOLD_HIGH);
    printf("Bass frequency range: 0-%.0f Hz\n\n", BASS_FREQ_MAX);
    
    // Initialize audio analyzer
    AudioAnalyzer analyzer = {0};
    g_analyzer = &analyzer;
    
    // Open log file with timestamp
    char logFileName[256];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(logFileName, sizeof(logFileName), "bass_detection_%Y%m%d_%H%M%S.log", t);
    analyzer.logFile = fopen(logFileName, "w");
    
    if (analyzer.logFile) {
        fprintf(analyzer.logFile, "=== Bass Detection Log ===\n");
        fprintf(analyzer.logFile, "Audio file: %s\n", audioFile);
        fprintf(analyzer.logFile, "Duration: %.2f seconds\n", GetMusicTimeLength(music));
        fprintf(analyzer.logFile, "Sample rate: %u Hz\n", music.stream.sampleRate);
        fprintf(analyzer.logFile, "Thresholds: LOW=%.2f, MEDIUM=%.2f, HIGH=%.2f\n", 
               BASS_THRESHOLD_LOW, BASS_THRESHOLD_MEDIUM, BASS_THRESHOLD_HIGH);
        fprintf(analyzer.logFile, "Bass frequency range: 0-%.0f Hz\n\n", BASS_FREQ_MAX);
        fprintf(analyzer.logFile, "Time format: [seconds from start]\n");
        fprintf(analyzer.logFile, "=====================================\n\n");
        fflush(analyzer.logFile);
        printf("Log file created: %s\n", logFileName);
    } else {
        printf("Warning: Could not create log file\n");
    }
    
    // Attach audio processor to capture audio data
    AttachAudioStreamProcessor(music.stream, AudioProcessorCallback);
    
    // Start playing music
    PlayMusicStream(music);
    SetMusicVolume(music, 0.7f);
    
    printf("Music playing... Press SPACE to pause, ESC to exit\n\n");
    
    // Main loop
    while (!WindowShouldClose()) {
        // Update music
        UpdateMusicStream(music);
        
        // Analyze audio if we have enough samples
        if (analyzer.samplesCollected >= FFT_SIZE) {
            analyzeAudioFrame(&analyzer);
        }
        
        // Restart music if it ends
        if (!IsMusicStreamPlaying(music)) {
            StopMusicStream(music);
            PlayMusicStream(music);
            printf("\n=== Music Restarted ===\n\n");
        }
        
        // Controls
        if (IsKeyPressed(KEY_SPACE)) {
            if (IsMusicStreamPlaying(music)) {
                PauseMusicStream(music);
                printf("[%.2f] Music PAUSED\n", GetTime());
            } else {
                ResumeMusicStream(music);
                printf("[%.2f] Music RESUMED\n", GetTime());
            }
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Title
        DrawText("Audio Spectrogram Demo with Real-Time Bass Detection", 20, 20, 22, WHITE);
        DrawText("Press SPACE to pause/resume | ESC to exit", 20, 48, 14, GRAY);
        
        // Draw visualizations
        drawSpectrum(&analyzer, 50, 90, SPECTRUM_WIDTH, SPECTRUM_HEIGHT);
        drawSpectrogram(&analyzer, 50, 510, SPECTRUM_WIDTH, SPECTROGRAM_HEIGHT);
        drawBassIndicator(&analyzer, 870, 90);
        drawInfoPanel(&analyzer, music, logFileName, 870, 250);
        
        EndDrawing();
    }
    
    // Cleanup
    printf("\n=== Session Summary ===\n");
    printf("Total bass events detected: %d\n", analyzer.bassEventCount);
    printf("Session duration: %.2f seconds\n", GetTime());
    
    // Write summary to log file and close it
    if (analyzer.logFile) {
        fprintf(analyzer.logFile, "\n=====================================\n");
        fprintf(analyzer.logFile, "=== Session Summary ===\n");
        fprintf(analyzer.logFile, "Total bass events detected: %d\n", analyzer.bassEventCount);
        fprintf(analyzer.logFile, "Session duration: %.2f seconds\n", GetTime());
        fclose(analyzer.logFile);
        printf("Log file closed: %s\n", logFileName);
    }
    
    DetachAudioStreamProcessor(music.stream, AudioProcessorCallback);
    g_analyzer = NULL;
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
