#include "bass_events.h"
#include <stdlib.h>
#include <stdio.h>

// Parsed bass events from assets/audio/level1.mp3 analysis
// These are the major bass sections from the detection log
static BassEvent bassEventsData[] = {
    // Early game - warm up period
    {55.85f, 83.02f, 27.17f, BASS_MEDIUM, 0.190f},      // First bass section
    {83.27f, 106.53f, 23.26f, BASS_MEDIUM, 0.189f},     // Second wave
    {110.68f, 135.27f, 24.59f, BASS_MEDIUM, 0.179f},    // Third wave
    {138.14f, 163.55f, 25.41f, BASS_MEDIUM, 0.190f},    // Fourth wave
    {165.55f, 190.53f, 24.99f, BASS_MEDIUM, 0.189f},    // Fifth wave
    
    // Mid game - intensity builds
    {222.12f, 247.97f, 25.85f, BASS_MEDIUM, 0.190f},    // Complex rhythmic section
    {249.56f, 250.79f, 1.23f, BASS_MEDIUM, 0.175f},     // Quick burst
    {250.84f, 254.22f, 3.38f, BASS_MEDIUM, 0.180f},     // Short wave
    {254.27f, 257.65f, 3.38f, BASS_MEDIUM, 0.180f},     // Quick wave
    {257.70f, 261.08f, 3.38f, BASS_MEDIUM, 0.171f},     // Rapid fire
    {261.13f, 264.51f, 3.38f, BASS_MEDIUM, 0.175f},     // Continued intensity
    {264.56f, 267.94f, 3.38f, BASS_MEDIUM, 0.186f},     // Building up
    {267.99f, 271.31f, 3.32f, BASS_MEDIUM, 0.166f},     // Final burst before break
    {271.42f, 272.91f, 1.49f, BASS_LOW, 0.097f},        // Brief respite
    
    // Late game - maximum intensity
    {276.69f, 305.45f, 28.76f, BASS_MEDIUM, 0.183f},    // Extended intense section
    
    // Boss section - epic finale
    {372.99f, 417.28f, 44.29f, BASS_MEDIUM, 0.188f},    // Long boss intro
    {427.83f, 451.38f, 23.55f, BASS_MEDIUM, 0.188f},    // Boss battle peak
    {454.82f, 479.53f, 24.71f, BASS_MEDIUM, 0.151f},    // Boss final phase
    {482.69f, 510.36f, 27.67f, BASS_MEDIUM, 0.182f},    // Victory/final wave
};

void InitBassEvents(BassEventSystem* system) {
    // Calculate total number of events
    system->eventCount = sizeof(bassEventsData) / sizeof(BassEvent);
    
    // Allocate and copy bass events
    system->events = (BassEvent*)malloc(sizeof(BassEvent) * system->eventCount);
    for (int i = 0; i < system->eventCount; i++) {
        system->events[i] = bassEventsData[i];
    }
    
    system->currentEventIndex = 0;
    system->songDuration = 553.82f; // From audio analysis
    
    printf("[BASS EVENTS] Initialized %d bass events for music-driven gameplay\n", system->eventCount);
}

BassLevel GetBassLevelAtTime(const BassEventSystem* system, float time) {
    // Check if we're within any bass event
    for (int i = 0; i < system->eventCount; i++) {
        const BassEvent* event = &system->events[i];
        if (time >= event->startTime && time <= event->endTime) {
            return event->level;
        }
    }
    
    return BASS_NONE;
}

BassLevel GetBassLevelWithAnticipation(const BassEventSystem* system, float time, float leadTime) {
    // Look ahead by leadTime seconds to anticipate incoming bass changes
    float anticipatedTime = time + leadTime;
    
    // Check if we're about to enter a bass event or already in one
    for (int i = 0; i < system->eventCount; i++) {
        const BassEvent* event = &system->events[i];
        
        // Check if we're approaching this event or already in it
        if (time >= event->startTime - leadTime && time <= event->endTime) {
            return event->level;
        }
        
        // Also check the anticipated time
        if (anticipatedTime >= event->startTime && anticipatedTime <= event->endTime) {
            return event->level;
        }
    }
    
    return BASS_NONE;
}

const BassEvent* GetCurrentBassEvent(const BassEventSystem* system, float time) {
    // Find the current bass event
    for (int i = 0; i < system->eventCount; i++) {
        const BassEvent* event = &system->events[i];
        if (time >= event->startTime && time <= event->endTime) {
            return event;
        }
    }
    
    return NULL;
}

void CleanupBassEvents(BassEventSystem* system) {
    if (system->events) {
        free(system->events);
        system->events = NULL;
    }
    system->eventCount = 0;
}

