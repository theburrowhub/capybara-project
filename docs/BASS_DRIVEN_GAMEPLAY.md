# Bass-Driven Gameplay System

## Overview

The game now features a **revolutionary bass-driven wave orchestration system** that synchronizes enemy spawns directly with the bass patterns detected in your music file (`assets/audio/level1.mp3`). This creates a unique, music-reactive experience where gameplay intensity follows the rhythm and energy of the soundtrack.

## System Architecture

### Components

1. **`bass_events.h/c`** - Bass event detection and management
2. **`wave_system.h/c`** - Updated to use bass-driven spawning
3. **`FLIGHT_PLAN.md`** - New documentation of bass-synchronized phases

### How It Works

```
Music Audio → Bass Detection Analysis → Bass Events → Anticipation System → Enemy Spawns
                                                            ↓
                                         Spawns 1.5s BEFORE bass hits
```

### Anticipation System ⏱️

Enemies don't spawn exactly when the bass hits—they spawn **1.5 seconds earlier**! This creates perfect synchronization:

1. **Bass approaching** → Enemies start spawning
2. **Enemy enters screen** → Takes ~1.5 seconds to reach combat zone
3. **Bass hits** → Enemies are now in position and the action peaks!

This makes the gameplay feel perfectly timed with the music rather than delayed.

## Bass Detection Data

The system uses **20 major bass events** extracted from your audio analysis:

- **Total Duration**: 553.82 seconds (~9 minutes)
- **Bass Events**: 20 significant sections
- **Energy Levels**: LOW (0.05-0.15), MEDIUM (0.15-0.30), HIGH (0.30+)

## Difficulty Mapping

### Bass Level → Enemy Types

**No Bass (Calm Periods)**
- Grunts (70%), Swarm (30%)
- Spawn: 2-4 second intervals
- Purpose: Recovery and breathing room

**Low Bass (Building)**
- Grunts (40%), Swarm (30%), Speeders (20%), Zigzag (10%)
- Spawn: 1-5 second intervals
- Purpose: Gradual difficulty increase

**Medium Bass (Intense)** ⚡
- Tanks (20%), Speeders (20%), Zigzag (15%), Shields (15%)
- Bombers (10%), Grunts (10%), Swarm (10%)
- Spawn: 0.8-12 second intervals (type-dependent)
- Purpose: Main combat intensity

**High Bass (Maximum)** 🔥
- Elite (30%), Bombers (25%), Ghost (20%), Tanks (15%), Shields (10%)
- Spawn: 3-12 second intervals
- Purpose: Maximum challenge during peaks

## Key Gameplay Moments

### 🎮 Warm-Up Phase (0-55.85s)
- **Easy practice period** before the music starts
- Enemies spawn every 3 seconds starting at 5s
- Only Grunts (70%) and Swarm (30%) - easiest enemies
- Enemies **do NOT fire** - completely safe practice
- Straight movement patterns only
- Learn controls and get comfortable
- ~18 enemies total to warm up with

### 🎵 First Bass Drop (55.85s)
- Transition from calm to intense!
- Enemy difficulty jumps with the music
- Enemies **start firing** when bass hits
- First MEDIUM bass section
- Intensity ramps dramatically - perfectly synced!

### ⚔️ Boss Battle (427-451s)
- Boss spawns at **~425.5s** (1.5s anticipation)
- Boss enters screen dramatically as bass hits at 427 seconds
- Synced with epic bass section
- 24-second window for boss fight
- Support minions every 15 seconds
- Most dramatic moment in the game - **perfect timing**!

### 🏁 Victory Lap (510-553s)
- Music fades
- Final enemies clear
- Score tally

## Dynamic Features

### Progressive Difficulty
Spawn intervals decrease over time:
```c
interval *= (1.0 - progress * 0.3)
```
- 0% complete: Base intervals
- 50% complete: 15% faster
- 100% complete: 30% faster spawns

### Weighted Random Selection
Each bass level has weighted enemy probabilities:
- Ensures variety
- Prevents predictable patterns
- Maintains difficulty curve

### Real-Time Adaptation
- Bass level checked every frame
- Enemy selection adapts instantly
- Movement patterns match intensity

## Code Structure

### Bass Event Definition
```c
typedef struct {
    float startTime;
    float endTime;
    float duration;
    BassLevel level;
    float energy;
} BassEvent;
```

### Enemy Spawn Configuration
```c
typedef struct {
    EnemyType type;
    int weight;
    float minInterval;
    float maxInterval;
} EnemySpawnConfig;
```

## Benefits of This System

1. **🎵 Music-First Design**
   - Gameplay naturally follows music flow
   - Dramatic moments feel earned

2. **🎲 High Replayability**
   - Never same enemy sequence twice
   - Weighted randomness within structure

3. **⚡ Responsive Intensity**
   - Difficulty matches music energy
   - Automatic pacing

4. **🎬 Cinematic Timing**
   - Boss appears at perfect moment
   - Victory feels synchronized

5. **🎮 Player Experience**
   - Natural difficulty curve
   - Recovery periods prevent fatigue
   - Boss timing feels epic

## Testing the System

Run the game and observe:

```bash
./bin/shootemup
```

### What to Notice:

1. **0-5s**: No enemies, learn controls
2. **5-55s**: Warm-up enemies (easy, non-firing) spawn every 3s
3. **55.85s**: 🎵 **FIRST BASS DROP** - enemies start firing, difficulty jumps!
4. **Throughout**: Enemy types/frequency match music intensity
5. **427s**: 💀 Boss appears during epic bass section
6. **510s+**: Cool down period as music fades

## Configuration

### Adjust Anticipation Time
In `wave_system.c`:
```c
// Change this value for different feel
#define BASS_ANTICIPATION_TIME 1.5f  // 1.0-2.0 seconds recommended
```

**Effects of different values:**
- **1.0s**: Tighter sync, enemies appear closer to bass hit
- **1.5s**: Default, perfect for most situations  
- **2.0s**: More anticipation, enemies on screen well before bass
- **0.0s**: No anticipation (not recommended - feels delayed)

### Adjust Boss Timing
In `wave_system.c`:
```c
// Boss spawns with anticipation
float bossSpawnTime = 427.0f - BASS_ANTICIPATION_TIME;
```

### Modify Difficulty Scaling
```c
// Progressive difficulty
float progressFactor = waveSystem->waveTimer / waveSystem->totalDuration;
interval *= (1.0f - progressFactor * 0.3f);  // Adjust 0.3f for faster/slower
```

### Change Enemy Weights
In `wave_system.c`, modify spawn config arrays:
```c
static EnemySpawnConfig mediumBassSpawns[] = {
    {ENEMY_TANK, 20, 4.0f, 8.0f},  // weight, minInterval, maxInterval
    // ...
};
```

## Debug Features

The system logs all spawns:
```
[55.85] Enemy spawned - Type:Grunt ID:1 Pattern:straight BassLevel:2
```

Check the game log file to see:
- When enemies spawn
- Current bass level
- Boss spawn timing
- Total enemies spawned

## Performance

- **Memory**: ~2KB for bass events (20 events)
- **CPU**: Negligible overhead (simple lookup per frame)
- **Spawn Check**: Every frame, O(1) complexity
- **Bass Level Lookup**: O(n) where n=20 (negligible)

## Future Enhancements

Potential improvements:
- Multiple music tracks with different bass patterns
- Boss variants based on music section
- Visual effects synced to bass
- Combo multipliers during intense sections
- Power-ups timed to specific beats

## Technical Notes

### Bass Event Extraction
Bass events were extracted using audio analysis of `level1.mp3`:
- Frequency range: 0-250 Hz
- Sample rate: 44100 Hz
- Threshold-based detection
- Manual curation of major sections

### Integration with Existing Systems
- No changes to enemy AI
- Compatible with all enemy types
- Works with existing collision/projectile systems
- Audio system independent (spawns work without music)

## Conclusion

This bass-driven system transforms your shoot'em up into a **rhythm-action experience** where the music doesn't just accompany the gameplay—it **drives** it. Every playthrough feels unique while maintaining musical synchronization, creating an engaging, replayable experience that feels both fresh and perfectly timed.

**The game is no longer just played—it's performed! 🎵🎮**

