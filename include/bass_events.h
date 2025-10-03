#ifndef BASS_EVENTS_H
#define BASS_EVENTS_H

#include <stdbool.h>

// Bass energy levels from audio analysis
typedef enum {
    BASS_NONE = 0,      // No bass detected
    BASS_LOW,           // Energy: 0.05-0.15
    BASS_MEDIUM,        // Energy: 0.15-0.30
    BASS_HIGH           // Energy: 0.30+
} BassLevel;

// Bass event from audio analysis
typedef struct {
    float startTime;    // When bass starts (seconds)
    float endTime;      // When bass ends (seconds)
    float duration;     // Duration of bass event
    BassLevel level;    // Intensity level
    float energy;       // Peak energy value
} BassEvent;

// Bass-driven wave configuration
typedef struct {
    BassEvent* events;
    int eventCount;
    int currentEventIndex;
    float songDuration;
} BassEventSystem;

// Initialize bass events from level1.mp3 analysis
void InitBassEvents(BassEventSystem* system);

// Get current bass level at given time
BassLevel GetBassLevelAtTime(const BassEventSystem* system, float time);

// Get bass level with anticipation/lead time (looks ahead)
BassLevel GetBassLevelWithAnticipation(const BassEventSystem* system, float time, float leadTime);

// Get current bass event
const BassEvent* GetCurrentBassEvent(const BassEventSystem* system, float time);

// Cleanup
void CleanupBassEvents(BassEventSystem* system);

#endif // BASS_EVENTS_H

