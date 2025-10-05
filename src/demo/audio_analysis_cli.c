// Console-based bass analyzer with real FFT analysis
#include "raylib.h"
#include "audio_analysis.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Note: CLI uses shared default thresholds defined in audio_analysis.h
// (0.15 LOW, 0.35 MEDIUM, 0.60 HIGH)

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
    audio_applyWindow(windowedSamples, FFT_SIZE);
    
    for (int i = 0; i < FFT_SIZE; i++) {
        fftData[i] = windowedSamples[i];
    }
    
    // Perform FFT
    audio_fft(fftData, FFT_SIZE);
    
    // Calculate magnitude spectrum with smoothing
    for (int i = 0; i < FFT_SIZE/2; i++) {
        float real = crealf(fftData[i]);
        float imag = cimagf(fftData[i]);
        float newMag = sqrtf(real * real + imag * imag) / (FFT_SIZE/4);
        
        // Apply smoothing
        analyzer->magnitude[i] = analyzer->magnitude[i] * 0.7f + newMag * 0.3f;
    }
    
    // Calculate bass energy using shared function
    analyzer->bassEnergy = audio_calculateBassEnergy(analyzer->magnitude, FFT_SIZE/2);
    
    // Determine bass level
    analyzer->previousBassLevel = analyzer->currentBassLevel;
    analyzer->currentBassLevel = audio_getBassLevel(analyzer->bassEnergy, &analyzer->config);
    
    // Verbose output - show all energy readings
    if (verbose && ((int)(currentTime * 10) % 5 == 0)) {  // Every 0.5 seconds
        printf("[%06.2f] Energy: %.3f Level: %s\n", 
               currentTime, analyzer->bassEnergy, audio_getBassLevelString(analyzer->currentBassLevel));
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
        
        const char *levelStr = audio_getBassLevelString(analyzer->currentBassLevel);
        
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
        const char *levelStr = audio_getBassLevelString(analyzer->currentBassLevel);
        
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


void printUsage(const char *programName) {
    printf("Usage: %s [OPTIONS]\n", programName);
    printf("\nOptions:\n");
    printf("  -f, --file <filename>    Specify MP3 file to analyze (relative to assets/audio/)\n");
    printf("  -c, --config <path>      Specify configuration file path (default: %s)\n", CONFIG_FILE_DEFAULT);
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
    printf("  %s -f level1.mp3 --config myconfig.conf  # Use custom config file\n", programName);
    printf("  %s -f level1.mp3 --enable-peak --peak 0.25 --save-config  # Enable peak detection\n", programName);
    printf("  %s --file level2.mp3                  # Use saved config if available\n", programName);
    printf("\nConfiguration:\n");
    printf("  Default config file: %s\n", CONFIG_FILE_DEFAULT);
    printf("  Settings are loaded from config file if it exists\n");
    printf("  Use --config to specify a different config file location\n");
    printf("  Use --save-config to persist changes\n");
    printf("  Peak detection is disabled by default - use --enable-peak to activate\n");
    printf("\nNote: Analysis runs silently in background (no audio playback)\n");
}

int main(int argc, char *argv[]) {
    printf("=== Audio Analysis CLI (Silent Background Analysis) ===\n\n");
    
    const char *audioDir = "assets/audio";
    char audioFile[512] = {0};
    const char *configFilePath = NULL;  // NULL means use default
    float timeLimit = 0.0f;  // 0 means no limit (use full song duration)
    int shouldSaveConfig = 0;
    int verbose = 0;
    
    // First pass: check for --help or --config argument
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printUsage(argv[0]);
            return 0;
        }
        if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
            configFilePath = argv[i + 1];
        }
    }
    
    // Load configuration (with custom path if specified)
    BassConfig config;
    audio_loadConfig(&config, configFilePath);
    
    // Parse command-line arguments (second pass for other options)
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if ((strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) && i + 1 < argc) {
                snprintf(audioFile, sizeof(audioFile), "%s/%s", audioDir, argv[i + 1]);
                i++;
            } else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) && i + 1 < argc) {
                // Already handled in first pass, skip
                i++;
            } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                // Already handled in first pass, skip
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
            audio_saveConfig(&config, configFilePath);
        }
        
        if (audioFile[0] == '\0') {
            printf("Error: No file specified\n\n");
            printUsage(argv[0]);
            return 1;
        }
    } else {
        // Interactive file selection
        char mp3Files[MAX_MP3_FILES][MAX_FILENAME_LENGTH];
        int fileCount = audio_scanMP3Files(audioDir, mp3Files, MAX_MP3_FILES);
        
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
    audio_getLogFilePath(audioFile, logFilePath, sizeof(logFilePath));
    audio_backupLogFile(logFilePath);
    
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
