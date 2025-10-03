# Flight Plan - Bass-Driven Wave Orchestration

## Overview
This game features a **dynamic, music-driven wave system** that synchronizes enemy spawns with the bass patterns detected in `assets/audio/level1.mp3`. The difficulty and enemy types adapt in real-time based on the music's intensity.

## Music Duration
- **Total Duration**: 553.82 seconds (~9 minutes, 14 seconds)
- **Sample Rate**: 44100 Hz
- **Format**: MP3

## Bass-Driven Difficulty System

### Bass Levels
The audio analysis detects bass energy at three levels:
- **BASS_NONE** (No bass detected): Calm periods, light enemy waves
- **BASS_LOW** (Energy: 0.05-0.15): Building intensity, mixed enemy types
- **BASS_MEDIUM** (Energy: 0.15-0.30): High intensity, challenging enemy combinations
- **BASS_HIGH** (Energy: 0.30+): Maximum difficulty, elite enemies (rare in this track)

### Enemy Spawn Rules by Bass Level

#### No Bass / Calm Periods
- **Enemy Types**: Grunts (70%), Swarm (30%)
- **Spawn Interval**: 2-4 seconds
- **Difficulty**: Easiest
- **Purpose**: Recovery periods, allow player to breathe

#### Low Bass (Building)
- **Enemy Types**: 
  - Grunts (40%), Swarm (30%), Speeders (20%), Zigzag (10%)
- **Spawn Interval**: 1-5 seconds depending on type
- **Difficulty**: Easy to Medium

#### Medium Bass (Intense)
- **Enemy Types**:
  - Tanks (20%), Speeders (20%), Zigzag (15%), Shields (15%)
  - Bombers (10%), Grunts (10%), Swarm (10%)
- **Spawn Interval**: 0.8-12 seconds depending on type
- **Difficulty**: Medium to Hard

#### High Bass (Maximum)
- **Enemy Types**:
  - Elite (30%), Bombers (25%), Ghost (20%), Tanks (15%), Shields (10%)
- **Spawn Interval**: 3-12 seconds
- **Difficulty**: Hardest

## Timeline of Major Bass Events

### Phase 1: Warm-Up (0:00 - 55:85s)
**Bass**: NONE | **Duration**: 55.85s
- **Tutorial/Practice Period**
- Easy enemies spawn every 3 seconds (Grunts 70%, Swarm 30%)
- Enemies do NOT fire - safe practice time
- Straight movement patterns only
- Perfect for learning controls
- ~18 enemies total in this phase

### Phase 2: First Wave (55:85 - 83:02s)
**Bass**: MEDIUM (0.190) | **Duration**: 27.17s
- First major bass drop! Enemy firing enabled

### Phase 3: Building Intensity (83:27 - 190:53s)
**Bass**: MEDIUM (0.179-0.190) | **4 waves**, ~25s each
- Consistent challenging gameplay with brief recovery periods

### Phase 4: Rhythmic Assault (222:12 - 272:91s)
**Bass**: MEDIUM (0.150-0.190) | Multiple short bursts
- Rapid enemy spawn cycles with very short recovery windows

### Phase 5: Extended Intensity (276:69 - 305:45s)
**Bass**: MEDIUM (0.183) | **Duration**: 28.76s
- Longest continuous intense section

### Phase 6: Pre-Boss Calm (305:45 - 372:99s)
**Bass**: NONE to LOW | **Duration**: 67s
- Recovery period before boss

### Phase 7: Boss Intro (372:99 - 417:28s)
**Bass**: MEDIUM (0.188) | **Duration**: 44.29s
- Heavy enemy waves increase, building tension

### Phase 8: ⚔️ BOSS BATTLE (427:83 - 451:38s)
**Bass**: MEDIUM (0.188) | **Duration**: 23.55s
- **BOSS SPAWNS at 427 seconds**
- Support minions every 15 seconds (3 Grunts)
- Most intense section of the game

### Phase 9: Victory Wave (482:69 - 510:36s)
**Bass**: MEDIUM (0.182) | **Duration**: 27.67s
- Final enemy waves, high score opportunity

### Phase 10: Cooldown (510:36 - 553:82s)
**Bass**: NONE | **Duration**: 43.46s
- Victory lap, score tally

## Dynamic Difficulty Scaling

### Progressive Difficulty
Spawn intervals decrease as game progresses:
- **0% Progress**: Base spawn intervals
- **50% Progress**: 15% faster spawns
- **100% Progress**: 30% faster spawns

Formula: `interval *= (1.0 - progress * 0.3)`

## Boss Mechanics

### Boss Appearance
- **Spawn Time**: 427 seconds (7min 7s)
- **Active Window**: 427-451 seconds (24 seconds)
- **Position**: Right side, center screen
- **Movement**: Hover pattern with slow approach
- **Minions**: 3 Grunts every 15 seconds

## Gameplay Statistics

- **Total Enemies**: 300-400+ (varies based on player survival)
- **Boss Encounters**: 1
- **Peak Difficulty**: 427-451 seconds
- **Recovery Periods**: 6 major calm sections
- **Intense Sections**: 12+ bass-driven waves

## Design Philosophy

This bass-driven system creates a **unique experience for every playthrough**:

1. **Music-First**: Gameplay follows the music's natural flow
2. **Dynamic**: Never the same enemy sequence twice
3. **Responsive**: Difficulty matches music intensity
4. **Cinematic**: Boss appears at perfect dramatic moment
5. **Replayable**: Random variety within structured framework

