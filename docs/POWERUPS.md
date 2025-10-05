# Powerup System

This document provides comprehensive information about the powerup system, including types, drop mechanics, visual design, and integration.

## Table of Contents
- [Overview](#overview)
- [Powerup Types](#powerup-types)
- [Drop Mechanics](#drop-mechanics)
- [Powerup Behavior](#powerup-behavior)
- [Visual Design](#visual-design)
- [Resource Economy](#resource-economy)
- [Integration Guide](#integration-guide)
- [Wave Design Strategy](#wave-design-strategy)
- [Code Reference](#code-reference)
- [Testing and Showcase](#testing-and-showcase)

---

## Overview

The powerup system provides players with critical resources and upgrades during gameplay. Powerups are dropped by defeated enemies and drift leftward across the screen. Players can collect them by flying into them, with a magnetic attraction effect when nearby.

### Key Features
- ✅ Automatic drops from defeated enemies
- ✅ Magnetic attraction to player (80px range)
- ✅ 15-second lifetime with fade-out
- ✅ Smooth animations (pulse + rotation)
- ✅ Instant effect application on collection
- ✅ Score bonus (+50 points per powerup)
- ✅ Maximum 20 active powerups (performance cap)

---

## Powerup Types

### 1. Energy Cell (⚡)
- **Visual**: Yellow circle with white lightning bolt
- **Color**: YELLOW `RGB(255, 255, 0)`
- **Effect**: Restores 20% of maximum energy
- **Rarity**: COMMON
- **Drop Rate**: 15-35% depending on enemy
- **Description**: Energy is consumed by player abilities (boost, shield burst, EMP blast, overdrive)
- **Purpose**: Frequent resource for abilities, no passive regeneration

### 2. Shield Booster (🛡️)
- **Visual**: Cyan hexagon with rotating shield design
- **Color**: SKYBLUE `RGB(135, 206, 235)`
- **Effect**: Fully restores shields (100%)
- **Rarity**: UNCOMMON
- **Drop Rate**: 2-25% depending on enemy
- **Description**: Shields regenerate over time but shield powerups provide instant full recovery
- **Purpose**: Emergency shield recovery

### 3. Repair Kit (+)
- **Visual**: Red circle with white medical cross
- **Color**: RED `RGB(255, 0, 0)`
- **Effect**: Repairs 20% of maximum hull damage
- **Rarity**: VERY RARE
- **Drop Rate**: 0.2-20% depending on enemy
- **Description**: Hull damage is critical and difficult to restore - this is the only way to repair hull
- **Purpose**: Critical hull repair (no passive regeneration)

### 4. Weapon Upgrade (⭐)
- **Visual**: Orange rotating star (4/5/6 points for levels 1/2/3)
- **Color**: ORANGE `RGB(255, 165, 0)`
- **Effect**: Increases weapon level by 1/2/3 (max level 5)
- **Rarity**: Level 1: UNCOMMON, Level 2: RARE, Level 3: VERY RARE
- **Drop Rate**: 1-25% depending on enemy
- **Description**: Weapon upgrades improve firepower and damage output
- **Purpose**: Power progression throughout game

#### Weapon Upgrade Distribution
When a weapon powerup drops, it randomly selects level:
- **Level 1** (+1): 60% chance - Common upgrade
- **Level 2** (+2): 30% chance - Rare upgrade
- **Level 3** (+3): 10% chance - Very rare upgrade

Maximum weapon level is 5. Collecting upgrades beyond level 5 has no effect.

---

## Drop Mechanics

### Drop System
- Powerups are dropped when enemies are destroyed
- Drop chance is determined by enemy type
- Each enemy rolls once on death to determine if/what powerup drops
- Powerups spawn at the enemy's death position
- Drops are **automatic** - integrated with collision system

### Drop Rate Table

| Enemy Type | Energy | Shield | Hull | Weapon | Nothing |
|-----------|--------|--------|------|--------|---------|
| **GRUNT** | 25% | 3% | 0.5% | 2% | 69.5% |
| **SWARM** | 15% | 2% | 0.2% | 1% | 81.8% |
| **SPEEDER** | 20% | 5% | 1% | 4% | 70% |
| **ZIGZAG** | 22% | 8% | 2% | 5% | 63% |
| **BOMBER** | 25% | 12% | 4% | 9% | 50% |
| **SHIELD** | 20% | 25% | 3% | 7% | 45% |
| **TANK** | 30% | 15% | 5% | 10% | 40% |
| **GHOST** | 28% | 10% | 3% | 8% | 51% |
| **ELITE** | 35% | 20% | 8% | 15% | 22% |
| **BOSS** | 30% | 25% | 20% | 25% | 0% |

### Drop Pattern Philosophy

#### Weak Enemies (Grunt, Swarm, Speeder)
- **Low drop rates** (15-25% for any drop)
- **Mostly energy** when they do drop
- **Rare valuable items** (hull < 1%, weapon < 5%)
- **Purpose**: Consistent but small energy supply

#### Medium Enemies (Zigzag, Bomber, Shield)
- **Medium drop rates** (37-55% for any drop)
- **Balanced drops** with focus on enemy specialty
  - Shield enemies drop shield powerups more often (25%)
  - Bomber enemies drop various items (50% drop rate)
- **Occasional weapon upgrades** (5-9%)
- **Purpose**: Reward tactical play with meaningful resources

#### Strong Enemies (Tank, Ghost, Elite)
- **High drop rates** (49-78% for any drop)
- **Better quality items** (hull 3-8%, weapon 8-15%)
- **Substantial energy/shield** (28-35% energy, 10-20% shield)
- **Purpose**: Major resource injection for defeating challenging enemies

#### Boss Enemy
- **Guaranteed drop** (100% drop rate)
- **High-value items only** (no common energy spam)
- **Equal chance** for all powerup types
- **Purpose**: Epic reward for major accomplishment

---

## Powerup Behavior

### Movement
- **Initial velocity**: Drifts left at 25 pixels/second (half of fall speed)
- **Vertical drift**: None (maintains spawn height)
- **Magnet effect**: When player is within 80 pixels, powerup is pulled toward player
- **Magnet speed**: 200 pixels/second (scales with distance)

### Lifetime
- **Maximum lifetime**: 15 seconds
- **Fade effect**: Powerups fade out during final 3 seconds
- **Despawn**: Automatically removed if not collected within 15 seconds
- **Off-screen**: Removed if they drift off the left edge

### Visual Effects
- **Pulse animation**: Size pulses between 0.7x and 1.0x scale
- **Rotation**: Rotates 90 degrees per second (clockwise)
- **Glow**: Outer glow effect that pulses with the powerup
- **Magnet indicator**: White circle appears when being magneted to player

### Collection
- **Detection**: Rectangle collision between player and powerup
- **Auto-collect**: Player automatically collects on touch
- **Bonus score**: +50 points for collecting any powerup
- **Instant effect**: Effects apply immediately on collection

---

## Visual Design

### Size Specifications
- **Base Size**: 20 pixels radius
- **Collision Bounds**: 20x20 pixel square
- **Glow Effect**: 30 pixel radius (1.5x base size)
- **Magnet Indicator**: 40 pixel radius circle (when active)
- **Magnetic Range**: 80 pixels from powerup center

### Color Coding Philosophy

The powerup colors match the HUD display colors for instant visual recognition:

```
HUD Element     Color       Powerup
----------------------------------------
HULL bar        RED         Repair Kit (Red +)
SHIELD bar      SKYBLUE     Shield Booster (Cyan hexagon)
ENERGY bar      YELLOW      Energy Cell (Yellow ⚡)
Weapon stat     ORANGE      Weapon Upgrade (Orange ⭐)
```

### Detailed Visual Design

#### Energy Cell ⚡
```
Shape: Circle
Primary: Yellow circle (18px radius)
Secondary: Lighter yellow inner circle (14px radius)
Symbol: White lightning bolt
  - Top triangle: 3 points
  - Bottom triangle: 3 points
  - Total height: ~20px
  - Width: ~6px at widest
Glow: Pulsing yellow aura
Animation: Gentle pulse, rotates 90°/sec
```

#### Shield Booster 🛡️
```
Shape: Hexagon (6 sides)
Primary: Sky blue hexagon
Interior: Lighter cyan inner circle (10px radius)
Symbol: None (hexagon itself represents shield)
Glow: Cyan pulsing aura
Animation: Rotates continuously at 90°/sec
```

**Hexagon Vertices**: 18px radius at 0°, 60°, 120°, 180°, 240°, 300°

#### Repair Kit +
```
Shape: Circle with medical cross
Primary: Red circle (18px radius)
Secondary: Darker red inner circle (14px radius)
Symbol: White medical cross
  - Vertical bar: 16px height, 3px width
  - Horizontal bar: 16px width, 3px height
Glow: Red pulsing aura
Animation: Gentle pulse, rotates 90°/sec
```

#### Weapon Upgrade ⭐
```
Shape: Star (4/5/6 points based on level)
Primary: Orange star
Center: White circle with level number
Symbol: Black number (1, 2, or 3)
Glow: Orange pulsing aura
Animation: Continuous rotation at 90°/sec
```

**Star Specifications:**
- **Level 1** (4-pointed star): Outer radius 18px, inner 8px, points at 0°, 90°, 180°, 270°
- **Level 2** (5-pointed star): Outer radius 18px, inner 8px, points at 0°, 72°, 144°, 216°, 288°
- **Level 3** (6-pointed star): Outer radius 18px, inner 8px, points at 0°, 60°, 120°, 180°, 240°, 300°

### Animation Specifications

#### Pulse Animation
- **Frequency**: 3.0 Hz (3 cycles per second)
- **Range**: 0.7x to 1.0x scale (70% to 100%)
- **Function**: Sine wave
- **Applies to**: All glow effects

#### Rotation Animation
- **Speed**: 90 degrees per second
- **Direction**: Clockwise
- **Applies to**: All powerup types

#### Magnet Animation
When player is within 80 pixels:
- **Indicator**: White circle outline appears at 40px radius
- **Alpha**: 50% transparency
- **Movement**: Powerup accelerates toward player
- **Speed**: 200 px/s (scales with distance)

#### Fade-Out Animation
Last 3 seconds of lifetime (12-15 seconds):
- **Initial**: 100% alpha
- **Final**: 0% alpha
- **Function**: Linear fade
- **Applies to**: All visual elements

### Drawing Order (Back to Front)
1. Background glow (large, faded, pulsing)
2. Magnet indicator (if player nearby)
3. Primary shape (circle/hexagon/star)
4. Inner elements (smaller circles, patterns)
5. Symbol/Icon (lightning, cross, number)
6. Outline (white circle, 80% alpha)

### Accessibility Considerations

#### Shape Differentiation
Each powerup has a unique shape for color-blind accessibility:
- **Circle** (Energy, Hull) - Distinguished by symbol
- **Hexagon** (Shield) - Unique shape
- **Star with variable points** (Weapon) - Unique shape with number

#### Visual Clarity
- **High contrast**: Bright colors against dark space background
- **White outlines**: Help powerups stand out
- **Pulsing glow**: Movement draws eye attention
- **Large size**: 20px radius visible at distance

---

## Resource Economy

### Energy Balance
- **Primary source**: Powerup drops (no passive regeneration)
- **Consumption**: Abilities (10-25 energy per use)
- **Strategy**: Frequent drops from common enemies maintain supply
- **Drop rate**: ~20-30% from most enemies
- **Average drops**: 3-5 per minute during normal gameplay

### Shield Balance
- **Primary source**: Passive regeneration over time
- **Secondary source**: Shield powerups for instant recovery
- **Strategy**: Powerups for emergency situations
- **Drop rate**: ~10-15% from medium/strong enemies
- **Average drops**: 1-2 per minute during normal gameplay

### Hull Balance
- **Primary source**: Hull powerups ONLY
- **No regeneration**: Hull damage is permanent without powerups
- **Rarity**: < 1% from weak enemies, 5-8% from strong enemies
- **Strategy**: Creates tension and rewards skillful play
- **Average drops**: 0-1 per 2-3 minutes (very rare)

### Weapon Upgrades
- **Source**: Powerup drops only
- **Progression**: Gradual throughout game
- **Cap**: Maximum level 5
- **Drop rate**: 5-15% from medium/strong enemies, 25% from boss
- **Average drops**: 1-2 per minute during normal gameplay

### Statistics & Balance

#### Expected Collection Rate
- **Magnetic range**: 80 pixels (generous)
- **Expected collection rate**: 80-90% of spawned powerups
- **Missed powerups**: 10-20% drift off screen
- **Strategy**: Position to collect during combat

---

## Integration Guide

The powerup system is **automatically integrated** with enemy destruction. No manual spawn events are needed - drops are organic based on combat.

### Step 1: Add PowerupSystem to Game State

Update `include/types.h` to include the powerup system:

```c
#include "powerup.h"  // Add at top with other includes

struct Game {
    // ... existing fields ...
    
    // Add powerup system
    PowerupSystem* powerupSystem;
};
```

### Step 2: Initialize Powerup System

Update `src/core/game.c` initialization:

```c
#include "powerup.h"  // Add at top

void InitGame(Game* game) {
    // ... existing initialization ...
    
    // Initialize powerup system
    game->powerupSystem = (PowerupSystem*)malloc(sizeof(PowerupSystem));
    InitPowerupSystem(game->powerupSystem);
}
```

### Step 3: Update Game Loop

Update `src/core/game.c` main loop:

```c
void UpdateGame(Game* game, float deltaTime) {
    // ... existing update code ...
    
    // Update powerups
    UpdatePowerups(game->powerupSystem, game->playerShip, deltaTime);
    
    // Check powerup collisions
    CheckPowerupCollisions(game->powerupSystem, game->playerShip, &game->score);
}

void DrawGame(const Game* game) {
    // ... existing draw code ...
    
    // Draw powerups (after enemies, before HUD)
    DrawPowerups(game->powerupSystem);
}
```

### Step 4: Drop Powerups on Enemy Death

Update `src/systems/collision.c` where enemies are destroyed:

```c
#include "powerup.h"  // Add at top

// In CheckBulletEnemyCollisions()
if (ctx->enemies[e].health <= 0) {
    // Create explosion if system is provided
    if (ctx->explosionSystem) {
        Color enemyColor = GetEnemyTypeColor(ctx->enemies[e].type);
        float enemySize = ctx->enemies[e].bounds.width;
        CreateEnemyExplosion(ctx->explosionSystem, ctx->enemies[e].position, 
                           enemyColor, enemySize);
    }
    
    // Drop powerup from enemy
    if (ctx->game && ctx->game->powerupSystem) {
        DropPowerupFromEnemy(ctx->game->powerupSystem, &ctx->enemies[e]);
    }
    
    // ... rest of existing code (score, deactivate, etc.) ...
}
```

### Step 5: Cleanup on Game Exit

Update `src/core/game.c` cleanup:

```c
void CleanupGame(Game* game) {
    // ... existing cleanup ...
    
    // Cleanup powerup system
    if (game->powerupSystem) {
        CleanupPowerupSystem(game->powerupSystem);
        free(game->powerupSystem);
        game->powerupSystem = NULL;
    }
}
```

### Expected Behavior After Integration

#### Energy Powerups (Yellow ⚡)
- Most common drop
- Restores yellow energy bar by 20%
- Should drop frequently from grunts and common enemies

#### Shield Powerups (Cyan 🛡️)
- Uncommon drop
- Fully restores cyan shield bar
- Should drop from medium/strong enemies

#### Hull Powerups (Red +)
- Very rare drop
- Restores red health bar by 20%
- Should mostly drop from elite/tank/boss enemies

#### Weapon Powerups (Orange ⭐)
- Uncommon/rare drop
- Visible weapon level should increase
- Stars have 4/5/6 points for levels 1/2/3

---

## Wave Design Strategy

### Strategic Wave Design

Since powerup drops are automatic based on enemy deaths, design waves strategically to control resource flow:

#### Energy-Focused Phases
For phases where players need energy:
- Spawn **Grunt/Tank waves** (25-30% energy drops)
- Frequent, easy-to-kill enemies
- Example: Warm-up phases, recovery periods

```c
// Spawn lots of grunts (25% energy drop rate)
SpawnWave(ENEMY_GRUNT, count: 10);
// Expected: ~2-3 energy powerups
```

#### Shield-Focused Phases
For shield recovery opportunities:
- Spawn **Shield enemies** (25% shield drops)
- **Elite/Boss encounters** (20-25% shield drops)
- Example: After intense combat phases

```c
// Spawn shield enemies (25% shield drop rate)
SpawnWave(ENEMY_SHIELD, count: 2);
// Expected: ~50% chance for shield powerup
```

#### Hull Recovery Opportunities
Hull is **intentionally rare**:
- Only **Elite/Boss/Tank** enemies have meaningful hull drop rates (5-20%)
- Creates tension: hull damage is serious
- Forces careful play to avoid hull damage

#### Weapon Upgrade Opportunities
For power progression:
- **Elite enemies** (15% weapon drop, highest non-boss)
- **Tank waves** (10% weapon drop)
- **Boss** (25% weapon drop, guaranteed)
- Spread throughout game for gradual power increase

```c
// Spawn elites or tanks for weapon upgrades
SpawnWave(ENEMY_ELITE, count: 2);
SpawnWave(ENEMY_TANK, count: 2);
// Expected: ~30-40% chance for weapon upgrade
```

### Example Wave Designs

#### Recovery Phase (Low intensity, high energy)
```
Spawn 5 Grunts every 2 seconds
Expected drops: 1-2 energy cells
Purpose: Let player recover energy for abilities
```

#### Shield Training Phase
```
Spawn 2 Shield enemies
Expected drops: ~50% chance for shield powerup
Purpose: Teach importance of shield management
```

#### Boss Preparation Phase
```
Spawn 2 Tanks + 2 Elites
Expected drops: 1-2 weapon upgrades, energy/shield
Purpose: Power up player before boss fight
```

#### Boss Reward
```
Boss defeated
Guaranteed drop: One of energy/shield/hull/weapon (25% each)
Purpose: Major reward for major accomplishment
```

---

## Code Reference

### File Structure
- **Header**: `include/powerup.h` - Types, structures, function declarations
- **Implementation**: `src/systems/powerup.c` - All powerup logic (~550 lines)
- **Integration**: Called from collision system when enemies die
- **Showcase**: `src/demo/powerup_showcase.c` - Interactive demo program

### Key Functions

```c
// Initialize system
void InitPowerupSystem(PowerupSystem* system);

// Drop from enemy (called on enemy death)
void DropPowerupFromEnemy(PowerupSystem* system, const EnemyEx* enemy);

// Update (called every frame)
void UpdatePowerups(PowerupSystem* system, PlayerShip* player, float dt);

// Draw (called every frame)
void DrawPowerups(const PowerupSystem* system);

// Check collisions (called every frame)
void CheckPowerupCollisions(PowerupSystem* system, PlayerShip* player, int* score);

// Cleanup
void CleanupPowerupSystem(PowerupSystem* system);
```

### Memory Usage
- **Maximum active powerups**: 20 (MAX_POWERUPS)
- **Size per powerup**: ~120 bytes
- **Total memory**: ~2.4 KB
- **Performance**: Negligible impact

### Performance Considerations
- **Draw calls per powerup**: ~6-10 draw calls
- **Expected active powerups**: 3-8 average, 20 maximum
- **Total draw calls**: ~60-200 per frame (negligible)
- **CPU impact**: Minimal - simple physics and rendering

---

## Testing and Showcase

### Build the Showcase

```bash
make powerup_showcase
```

This launches an interactive demo where you can:
- Test all powerup types
- See drop rate tables
- Adjust player stats
- Watch animations and effects
- Test collection mechanics

### Showcase Controls
- **Arrow Keys**: Move ship
- **SPACE**: Spawn random powerup
- **1-6**: Spawn specific powerup type
- **A**: Toggle auto-spawn
- **R**: Reset player stats
- **I**: Toggle instructions
- **ESC**: Exit

### Balance Adjustments

**If energy too scarce:**
- Increase grunt drop rate in `src/systems/powerup.c`
- Or increase `ENERGY_RECOVERY_PERCENT`

**If hull too common:**
- Hull should be rare! Keep drop rates low
- Only elites/tanks/boss should drop frequently

**If weapons level too fast:**
- Decrease weapon drop chances
- Cap at level 3 instead of 5

**Edit drop rates** in `src/systems/powerup.c` at the `DROP_RATES` array.

### Troubleshooting

#### Powerups Not Appearing
- Check: Is `DropPowerupFromEnemy()` being called?
- Check: Is `DrawPowerups()` being called in draw loop?
- Check: Are enemies being destroyed? (check collision system)

#### Powerups Not Being Collected
- Check: Is `CheckPowerupCollisions()` being called?
- Check: Player bounds correct? Powerup bounds correct?

#### Compilation Errors
- Check: Did you add `powerup.c` to Makefile `SYSTEM_SRCS`?
- Check: Did you include `powerup.h` in necessary files?
- Run `make clean && make` to rebuild everything

#### Powerups Dropping Too Often/Rarely
- Adjust: Edit drop rates in `src/systems/powerup.c`
- See: `DROP_RATES` array
- Tune: Increase/decrease percentages for each enemy type

---

## Advanced Features

### Display Powerup Notifications

Add to HUD drawing code:

```c
void DrawPowerupNotification(Game* game) {
    if (game->lastPowerupCollectedTime > 0 && 
        GetTime() - game->lastPowerupCollectedTime < 2.0f) {
        
        const char* msg = GetPowerupName(game->lastPowerupType);
        int msgWidth = MeasureText(msg, 20);
        
        DrawText(msg, SCREEN_WIDTH/2 - msgWidth/2, 100, 20, YELLOW);
    }
}
```

### Add Sound Effects

```c
// Load sounds in InitGame()
Sound powerupSound = LoadSound("assets/audio/powerup.wav");

// Play in CheckPowerupCollisions() when collected
PlaySound(powerupSound);
```

### Powerup Statistics

Add to game state and display:

```c
// Track collection stats
typedef struct {
    int energyCollected;
    int shieldCollected;
    int hullCollected;
    int weaponCollected;
} PowerupStats;

// Display in end game screen
void DrawGameStats(Game* game) {
    DrawText(TextFormat("Energy Collected: %d", stats.energyCollected), ...);
    DrawText(TextFormat("Shield Collected: %d", stats.shieldCollected), ...);
    // ... etc
}
```

---

## Summary

You now have a complete, polished powerup system that:

- ✅ **Designed** with detailed drop patterns tuned to enemy difficulty
- ✅ **Implemented** with clean, efficient code
- ✅ **Automatic** - drops happen organically from combat
- ✅ **Balanced** - resource scarcity creates meaningful choices
- ✅ **Visual** - clear, recognizable, color-coded designs
- ✅ **Fair** - magnetic collection prevents missed pickups
- ✅ **Strategic** - wave design controls resource flow
- ✅ **Performant** - negligible performance impact
- ✅ **Flexible** - easy to tune drop rates and effects
- ✅ **Documented** - comprehensive guides and references

### Why This System Works

1. **Smart Drop System**: Stronger enemies drop better items
2. **Color Coding**: Powerup colors match HUD for instant recognition
3. **Magnetic Collection**: 80px range prevents frustration
4. **Shape Variety**: Accessible design for color-blind players
5. **Hull Scarcity**: Creates tension - hull damage matters
6. **Boss Guarantees**: Major accomplishments always rewarded
7. **Automatic Integration**: No manual spawning - just destroy enemies