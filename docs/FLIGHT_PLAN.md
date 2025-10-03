# Flight Plan - Static Wave Orchestration

## Overview
This game features a **predetermined, carefully designed wave system** with exact enemy spawn times, types, and positions. Each wave is meticulously crafted to create a balanced and consistent gameplay experience.

## Music Duration
- **Total Duration**: 553.82 seconds (~9 minutes, 14 seconds)
- **Sample Rate**: 44100 Hz
- **Format**: MP3

## Static Wave System

### Design Philosophy
All enemy waves are **predetermined and static**:
- **No randomization** - Same enemies spawn at same times every playthrough
- **Exact positions** - Each enemy has a specific spawn location
- **Designed formations** - Enemies spawn in crafted patterns
- **Balanced difficulty** - Carefully tuned progression

### Wave Types

#### Formation Waves
- Multiple enemies spawn at once in specific patterns
- Example: 2 Tanks in vertical formation (150px spacing)
- Coordinated attacks create tactical challenges

#### Single Spawns
- Individual high-value targets
- Elite enemies, Bosses
- Strategic placement for specific challenges

#### Sequential Waves
- Enemies spawn in quick succession
- Creates flowing combat encounters
- Varied enemy types for dynamic gameplay

## Wave Timeline (Redesigned for Continuous Intense Action)

### Wave Design Philosophy
**INTENSE PHASES**: Continuous enemy waves every 3-5 seconds - constant action, always something to fight
**CALM PHASES**: Minimal sparse enemies - time to breathe and recover



### Warm-Up (0-55s) 🟢 CALM
- **20 Grunts** in 5 waves (8s, 18s, 28s, 38s, 48s)
- **No firing** - Safe tutorial
- Purpose: Learn controls

### First Combat (55-85s) 🟢 CALM
- **3 Grunts** (58s)
- **4 Swarms** (70s)
- **Firing enabled** from here
- Purpose: Gentle intro to combat

### 🔴 INTENSE: Tank Assault (87-110s)
**Continuous waves every 5 seconds - no breaks:**
- 87s: **2 Tanks** 🎯 (assault pattern)
- 92s: **3 Speeders** (zigzag)
- 97s: **4 Grunts**
- 102s: **5 Swarms**
- 107s: **3 Speeders**
- **Purpose**: Non-stop action, tanks + support

### Calm (110-125s) 🟢 RECOVERY
- 118s: **3 Swarms** only
- Purpose: Breathe after intense assault

### 🔴 INTENSE: Swarm Assault (125-150s)
**6 waves of swarms every 4 seconds:**
- 127s, 131s, 135s, 139s, 143s, 147s: **5 Swarms** each wave
- **Purpose**: Constant dodging, non-stop movement

### Calm (150-165s) 🟢 RECOVERY
- 157s: **3 Grunts** only
- Purpose: Recovery after swarms

### 🔴 INTENSE: Mixed Assault (165-195s)
**6 waves every 5 seconds - varied enemy types:**
- 167s: **4 Speeders**
- 172s: **4 Zigzags**
- 177s: **4 Grunts**
- 182s: **3 Speeders**
- 187s: **6 Swarms**
- 192s: **3 Zigzags**
- **Purpose**: Constant variety, test all skills

### Calm (195-215s) 🟢 RECOVERY
- 205s: **3 Swarms** only
- Purpose: Breathe after intense mix

### 🔴 INTENSE: Elite Encounter (218-245s)
**5 waves every 5 seconds - with elite centerpiece:**
- 220s: **2 Elites** (advance to center) ⭐
- 225s: **4 Speeders** (support)
- 230s: **5 Swarms**
- 235s: **4 Grunts**
- 240s: **3 Zigzags**
- **Purpose**: Elite at center, constant support waves

### Calm (245-265s) 🟢 RECOVERY
- 255s: **3 Swarms** only
- Purpose: Recovery after elites

### 🔴 INTENSE: Shield Assault (265-295s)
**6 waves every 5 seconds:**
- 267s: **2 Shields**
- 272s: **4 Speeders**
- 277s: **5 Grunts**
- 282s: **4 Zigzags**
- 287s: **6 Swarms**
- 292s: **3 Speeders**
- **Purpose**: Defensive enemies + constant pressure

### Calm (295-310s) 🟢 RECOVERY
- 302s: **3 Swarms** only
- Purpose: Breathe before tanks

### 🔴 INTENSE: Tank Assault #2 (310-345s)
**7 waves every 5-6 seconds - maximum pressure:**
- 312s: **2 Tanks** 🎯 (assault pattern)
- 318s: **4 Zigzags**
- 323s: **5 Speeders**
- 328s: **2 Bombers**
- 333s: **5 Grunts**
- 338s: **6 Swarms**
- 343s: **4 Speeders**
- **Purpose**: Longest intense phase, tanks + variety

### Calm (345-365s) 🟢 RECOVERY
- 355s: **3 Grunts** only
- Purpose: Major recovery before elites

### 🔴 INTENSE: Elite Assault (368-398s)
**6 waves every 5 seconds - high difficulty:**
- 370s: **2 Elites** ⭐ (to center)
- 375s: **2 Shields**
- 380s: **4 Zigzags**
- 385s: **4 Speeders**
- 390s: **2 Bombers**
- 395s: **4 Grunts**
- **Purpose**: Elites + varied threats

### Calm (398-415s) 🟢 RECOVERY
- 407s: **4 Swarms** only
- Purpose: Final recovery before boss

### 🔴 INTENSE: Pre-Boss Build (415-427s)
**2 waves building intensity:**
- 417s: **4 Zigzags**
- 422s: **4 Speeders**
- **Purpose**: Build tension before boss

### 🔥 BOSS BATTLE (427-455s)
- 427s: **BOSS** spawns
- 432s, 440s, 448s: **3 Grunts** support waves
- **Purpose**: Epic finale

### Calm (455-475s) 🟢 RECOVERY
- 462s: **4 Swarms** only
- Purpose: Celebrate boss victory

### 🔴 INTENSE: Final Push (475-510s)
**6 waves every 5 seconds - last challenge:**
- 477s: **2 Elites** ⭐
- 483s: **5 Speeders**
- 488s: **2 Bombers**
- 493s: **4 Zigzags**
- 498s: **4 Grunts**
- 503s: **5 Swarms**
- 508s: **3 Speeders**
- **Purpose**: Final intense combat

### Calm: Victory Lap (510-550s) 🟢 EASY
- 520s: **3 Grunts**
- 535s: **4 Swarms**
- **Purpose**: Easy finish

## Wave Design Features

### Tank Assault Waves 🎯
Only 2 tank encounters in the entire game (92s, 303s):
- **2 Tanks** spawn in formation
- **150px vertical spacing** (no collision)
- **Tactical pattern**: Advance → Pause (5-10s) → Retreat
- **20 health each** - requires sustained focus
- **3 missiles per tank** (6 total per wave)
- Always followed by recovery period

### Difficulty Pattern
The game now follows a rhythm:
```
🔴 CHALLENGE → 🟢 RECOVERY → 🔴 CHALLENGE → 🟢 RECOVERY
```
- After every intense phase comes a calm recovery period
- Recovery uses only Swarms/Grunts (easy enemies)
- 10-20 seconds of calm between challenges
- Allows players to catch their breath

## Boss Mechanics

### Boss Appearance
- **Spawn Time**: 427 seconds (7min 7s)
- **Active Window**: 427-451 seconds (24 seconds)
- **Position**: Right side, center screen
- **Movement**: Hover pattern with slow approach
- **Minions**: 3 Grunts every 15 seconds

## Gameplay Statistics

- **Total Spawn Events**: ~100+ predetermined encounters
- **Total Enemies**: ~150-200 across all waves
- **Tank Encounters**: 6 (87s, 180s, 275s, 325s, 400s, 505s)
- **Boss Encounters**: 1 (427s)
- **Peak Difficulty**: 360-425s (pre-boss build) and 427-451s (boss)

## Design Philosophy

This static wave system creates a **consistent, skillful experience**:

1. **Designed**: Every wave is intentionally crafted
2. **Learnable**: Same waves allow skill improvement
3. **Balanced**: Difficulty curve is carefully tuned
4. **Fair**: No randomness means consistent challenge
5. **Replayable**: Master the exact enemy patterns

## Editing Waves

To modify enemy waves, edit `src/systems/static_waves.c`:

```c
events[index++] = (SpawnEvent){
    .time = 87.0f,              // When to spawn
    .type = ENEMY_TANK,          // Enemy type
    .x = SCREEN_WIDTH + 50,      // X position
    .y = 225,                    // Y position (center for formations)
    .count = 2,                  // Number of enemies
    .interval = 150,             // Vertical spacing (pixels)
    .pattern = "tank_assault"    // Movement pattern
};
```

Recompile after editing: `make clean && make`

