// Simple console-based bass analyzer for testing
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#define SAMPLE_SIZE 1024
#define BASS_FREQ_MAX 250.0f
#define SAMPLE_RATE 44100.0f

// Bass detection thresholds
#define BASS_THRESHOLD_LOW 0.15f
#define BASS_THRESHOLD_MEDIUM 0.35f
#define BASS_THRESHOLD_HIGH 0.60f

typedef enum {
    BASS_NONE = 0,
    BASS_LOW,
    BASS_MEDIUM,
    BASS_HIGH
} BassLevel;

float calculateBassEnergy(float *samples, int sampleCount) {
    float energy = 0.0f;
    for (int i = 0; i < sampleCount && i < SAMPLE_SIZE; i++) {
        energy += fabsf(samples[i]);
    }
    return energy / (float)sampleCount;
}

const char* getBassLevelString(BassLevel level) {
    switch(level) {
        case BASS_LOW: return "LOW";
        case BASS_MEDIUM: return "MEDIUM";
        case BASS_HIGH: return "HIGH";
        default: return "NONE";
    }
}

BassLevel getBassLevel(float energy) {
    if (energy < BASS_THRESHOLD_LOW) return BASS_NONE;
    if (energy < BASS_THRESHOLD_MEDIUM) return BASS_LOW;
    if (energy < BASS_THRESHOLD_HIGH) return BASS_MEDIUM;
    return BASS_HIGH;
}

int main(void) {
    printf("=== Audio Bass Analyzer (Console Mode) ===\n");
    printf("Loading: assets/audio/level1.mp3\n\n");
    
    // Initialize audio
    InitAudioDevice();
    
    // Load music
    Music music = LoadMusicStream("assets/audio/level1.mp3");
    
    if (music.frameCount == 0) {
        printf("Error: Could not load audio file!\n");
        CloseAudioDevice();
        return 1;
    }
    
    float duration = GetMusicTimeLength(music);
    printf("Successfully loaded audio\n");
    printf("Duration: %.2f seconds\n", duration);
    printf("Bass Thresholds: LOW=%.2f, MEDIUM=%.2f, HIGH=%.2f\n\n", 
           BASS_THRESHOLD_LOW, BASS_THRESHOLD_MEDIUM, BASS_THRESHOLD_HIGH);
    
    printf("Starting analysis (first 10 seconds)...\n");
    printf("Time format: [seconds.ms]\n");
    printf("=========================================\n\n");
    
    // Play music
    PlayMusicStream(music);
    SetMusicVolume(music, 0.5f);
    
    BassLevel currentLevel = BASS_NONE;
    BassLevel previousLevel = BASS_NONE;
    float bassStartTime = 0.0f;
    int eventCount = 0;
    float samples[SAMPLE_SIZE];
    
    // Analyze for 10 seconds or until music ends
    float analysisTime = 0.0f;
    float maxTime = fminf(10.0f, duration);
    
    while (analysisTime < maxTime) {
        UpdateMusicStream(music);
        
        // Simulate bass energy based on time (for demo purposes)
        // In a real implementation, you'd extract actual audio samples
        float timePos = GetMusicTimePlayed(music);
        
        // Create varying bass pattern
        float bassEnergy = 0.0f;
        
        // Simulate bass hits at specific intervals
        float beatTime = fmodf(timePos, 1.0f);  // Get fractional second
        if (beatTime < 0.1f) {
            bassEnergy = 0.7f;  // Strong bass hit
        } else if (beatTime < 0.2f) {
            bassEnergy = 0.3f;  // Decay
        } else if (fmodf(timePos, 2.0f) < 0.5f) {
            bassEnergy = 0.2f;  // Background bass
        } else {
            bassEnergy = 0.05f; // Low/no bass
        }
        
        // Add some variation
        bassEnergy += sinf(timePos * 2.0f) * 0.1f;
        if (bassEnergy < 0) bassEnergy = 0;
        if (bassEnergy > 1) bassEnergy = 1;
        
        previousLevel = currentLevel;
        currentLevel = getBassLevel(bassEnergy);
        
        // Detect and log bass events
        if (previousLevel != currentLevel) {
            if (previousLevel == BASS_NONE && currentLevel != BASS_NONE) {
                // Bass started
                bassStartTime = timePos;
                eventCount++;
                printf("[%06.2f] Bass START - Level: %-6s (Energy: %.3f)\n",
                       timePos, getBassLevelString(currentLevel), bassEnergy);
            } else if (previousLevel != BASS_NONE && currentLevel == BASS_NONE) {
                // Bass ended
                float duration = timePos - bassStartTime;
                printf("[%06.2f] Bass END   - Duration: %.2fs\n", timePos, duration);
            } else if (previousLevel != BASS_NONE && currentLevel != BASS_NONE) {
                // Bass level changed
                printf("[%06.2f] Bass CHANGE - Level: %-6s (Energy: %.3f)\n",
                       timePos, getBassLevelString(currentLevel), bassEnergy);
            }
        }
        
        analysisTime = timePos;
        
        // Small delay to prevent CPU spinning
        WaitTime(0.05f);  // 50ms update rate
    }
    
    printf("\n=========================================\n");
    printf("Analysis complete!\n");
    printf("Total bass events detected: %d\n", eventCount);
    printf("Analysis duration: %.2f seconds\n", analysisTime);
    
    // Cleanup
    UnloadMusicStream(music);
    CloseAudioDevice();
    
    return 0;
}
