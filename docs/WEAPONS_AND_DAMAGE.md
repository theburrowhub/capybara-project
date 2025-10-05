# Weapons and Damage System

This document provides comprehensive information about the weapon system, damage mechanics, projectile types, and combat gameplay.

## Table of Contents
- [Projectile Types](#projectile-types)
- [Weapon Configurations by Enemy](#weapon-configurations-by-enemy)
- [Player Weapon System](#player-weapon-system)
- [Damage System](#damage-system)
- [Weapon Power Levels](#weapon-power-levels)
- [Weapon Powerup Revive System](#weapon-powerup-revive-system)
- [Movement Patterns](#movement-patterns)
- [Special Properties](#special-properties)
- [HUD Display](#hud-display)
- [Sprites and Assets](#sprites-and-assets)
- [Code Integration](#code-integration)

---

## Projectile Types

The game features 4 distinct projectile types with unique characteristics:

### 1. 🔴 Laser
- **Damage**: 10
- **Speed**: 500 (fastest)
- **Pattern**: Straight line
- **Special**: Basic, rapid-fire capable
- **Visual**: Red beam with bright tip and trail
- **Best for**: Quick, precise shots against fast enemies

### 2. 🔵 Plasma
- **Damage**: 20
- **Speed**: 300 (medium)
- **Pattern**: Slight homing (30% tracking)
- **Special**: Electric effect, semi-guided
- **Visual**: Cyan energy ball with electric arcs
- **Best for**: Medium-range engagements, moving targets

### 3. 🚀 Missile
- **Damage**: 50 (highest)
- **Speed**: 280 (medium-fast)
- **Pattern**: Strong homing (70% tracking)
- **Special**: Explosive (30 radius), high damage, long lifetime
- **Visual**: Gray missile with orange exhaust flame
- **Best for**: Heavy targets, guaranteed hits on tanks/bosses

### 4. 💜 Energy Orb
- **Damage**: 35
- **Speed**: 150 (slowest)
- **Pattern**: Wave motion
- **Special**: Piercing, explosive (40 radius), infinite range
- **Visual**: Purple orb with rotating energy rings
- **Best for**: Area denial, crowd control, multiple targets

---

## Weapon Configurations by Enemy

Each enemy type has specific weapon loadouts:

| Enemy | Primary | Secondary | Burst | Spread | Fire Rate | Pattern |
|-------|---------|-----------|-------|--------|-----------|---------|
| **Grunt** | Laser | Laser | 1 | 0° | 1.0/s | Single shot |
| **Tank** | Missile | Energy Orb | 3 | 20° | 0.6/s | Missile barrage ⚠️ |
| **Speeder** | Laser | Laser | 3 | 15° | 3.0/s | Burst fire |
| **Zigzag** | Plasma | Laser | 2 | 30° | 1.5/s | Spread shot |
| **Bomber** | Energy Orb | Missile | 1 | 0° | 0.8/s | Area attack |
| **Shield** | Plasma | Plasma | 6 | 360° | 1.2/s | Radial burst |
| **Swarm** | Laser | Laser | 1 | 0° | 2.0/s | Rapid fire |
| **Elite** | Plasma | Missile | 4 | 45° | 2.0/s | Multi-shot |
| **Ghost** | Plasma | Energy Orb | 2 | 20° | 1.0/s | Phase fire |
| **Boss** | Missile | Energy Orb | 8 | 360° | 2.5/s | Barrage |

**Note**: Tank enemies fire 3 medium-speed tracking missiles in a spread pattern. During intense bass phases, multiple tanks spawn simultaneously creating challenging missile barrages.

---

## Player Weapon System

### Heat Management
The player's weapon system includes heat management to prevent spam:

- **Heat Generation**: 8% per shot
- **Cooling Rate**: 30% per second when not firing
- **Overheat Threshold**: 100%
- **Overheat Penalty**: 3-second forced cooldown
- **Visual Indicators**:
  - 🟢 Green (0-25%): Cool, safe to fire
  - 🟡 Yellow (25-50%): Warming up
  - 🟠 Orange (50-75%): Warning zone
  - 🔴 Red (75-100%): Critical heat
  - 🟣 Purple: Overheated, cooling down

### Firing Mechanics
- **Fire Rate**: 0.12 seconds between shots
- **Projectile Type**: Energy-based projectiles
- **Input**: Space bar or left mouse button
- **Auto-fire**: Hold for continuous shooting (until overheat)

---

## Damage System

### Core Principle
**The damage indicator shows damage PER INDIVIDUAL BULLET, not total volley damage.**

If you fire 3 bullets and each deals 6.0 damage, the indicator shows "6.0" (not 18.0).

### Damage Calculation Formula

```
Damage Per Bullet = Base Damage × Weapon Powerup Multiplier × Offensive Mode Bonus
```

### Base Damage by Weapon Mode

Each weapon mode has a base damage value per bullet:

| Weapon Mode | Bullets Fired | Base Damage/Bullet |
|-------------|---------------|--------------------|
| SINGLE      | 1             | 3.0                |
| DOUBLE      | 2             | 1.5                |
| SPREAD      | 3             | 1.0                |
| RAPID       | 1             | 1.5                |
| DUAL        | 2             | 1.5                |
| CHARGE      | Varies        | 1.0                |

**Note**: The indicator shows the damage of each individual bullet, not the combined total.

### Weapon Powerup Multipliers

Applied to base damage per bullet:

| Powerup Level | Multiplier | Effect |
|---------------|------------|--------|
| 0 (Standard)  | ×1.0       | No bonus |
| 1 (+1)        | ×1.5       | 50% more damage |
| 2 (+2)        | ×2.0       | Double damage |
| 3 (+3)        | ×2.5       | 2.5x damage |

### Offensive Mode Bonus
If in Offensive mode with full energy:
- **×2.0** additional multiplier
- Only applies when energy bar is at 100%

### Practical Examples

#### Example 1: Starting Loadout
- **Mode**: SINGLE (fires 1 bullet)
- **Powerup Level**: 0
- **Energy Mode**: Offensive (not full)
- **Calculation**: 3.0 × 1.0 × 1 = **3.0**
- **Effect**: 1 bullet dealing 3.0 damage

#### Example 2: Mid-Game Power
- **Mode**: SPREAD (fires 3 bullets)
- **Powerup Level**: 2 (+2)
- **Energy Mode**: Defensive
- **Calculation**: 1.0 × 2.0 × 1 = **2.0**
- **Effect**: 3 bullets dealing 2.0 damage each (6.0 total per volley)

#### Example 3: Maximum Single Shot Power
- **Mode**: SINGLE (fires 1 bullet)
- **Powerup Level**: 3 (+3)
- **Energy Mode**: Offensive (full energy)
- **Calculation**: 3.0 × 2.5 × 2 = **15.0**
- **Effect**: 1 bullet dealing 15.0 damage

#### Example 4: Maximum Spread Power
- **Mode**: SPREAD (fires 3 bullets)
- **Powerup Level**: 3 (+3)
- **Energy Mode**: Offensive (full energy)
- **Calculation**: 1.0 × 2.5 × 2 = **5.0**
- **Effect**: 3 bullets dealing 5.0 damage each (15.0 total per volley)

### Enemy Damage Resistance

Different enemies have varying resistance to damage:

- **No Resistance (0%)**: Grunt, Speeder, Swarm
- **Low (10-20%)**: Zigzag, Bomber
- **Medium (30-40%)**: Elite, Ghost
- **High (50-70%)**: Tank, Shield, Boss

### Effective Combat Strategies

- **vs Fast Enemies**: Use homing projectiles (Plasma/Missile)
- **vs Groups**: Use explosive projectiles (Energy Orb)
- **vs Tanks**: Focus fire with high-damage weapons (Missile)
- **vs Shields**: Time shots between shield rotations
- **Single Shot vs Multi-Shot**:
  - **SINGLE**: High damage per bullet (max 15.0), best for precision
  - **SPREAD**: Lower damage per bullet (max 5.0), best for area coverage
  - **DOUBLE**: Balanced damage (max 7.5), best for general purpose

### Accuracy Guarantee

The damage value shown in the indicator **exactly matches** the damage dealt to enemies per bullet:
- No hidden multipliers
- No random variation
- What you see is what you get

---

## Weapon Power Levels

The weapon system includes 4 progressive power levels based on collected weapon powerups.

| Level | Name | Damage Multiplier | Visual Scale | Description |
|-------|------|-------------------|--------------|-------------|
| 0 | Standard | 1.0x | 1.0x | Base weapon damage, standard projectile size |
| 1 | +1 | 1.5x | 1.2x | 50% more damage, slightly larger and brighter projectiles |
| 2 | +2 | 2.0x | 1.4x | Double damage, noticeably larger with extra shockwave ring |
| 3 | +3 | 2.5x | 1.6x | 2.5x damage, maximum size with pulsing aura effect |

### Visual Progression

#### Level 0 (Standard)
- Standard green energy projectile
- 3 lightning bolts
- 8 trail segments
- Base size and brightness

#### Level 1 (+1)
- 20% larger size
- 15% brighter colors
- 4 lightning bolts
- 10 trail segments
- More intense glow

#### Level 2 (+2)
- 40% larger size
- 30% brighter colors
- 5 lightning bolts
- 12 trail segments
- Additional outer shockwave ring
- Enhanced particle effects

#### Level 3 (+3 - Maximum Power)
- 60% larger size
- 45% brighter colors
- 6 lightning bolts
- 14 trail segments
- Extra shockwave ring
- Pulsing aura effect
- Maximum visual intensity

### Player Progression

Players collect weapon powerups to advance through levels:
- Start at Level 0 (standard)
- Collect 1st weapon powerup → Level 1 (+1)
- Collect 2nd weapon powerup → Level 2 (+2)
- Collect 3rd weapon powerup → Level 3 (+3, maximum)

The weapon level indicator in the HUD shows progress: `X/3` where X is the current level.

---

## Weapon Powerup Revive System

Weapon powerups serve a dual purpose: they increase damage output AND act as **extra lives**.

### Revive Mechanism

When the player's hull reaches 0:

1. **Check for Weapon Powerups**: If `weaponPowerupCount > 0`, trigger revive
2. **Consume One Level**: Reduce `weaponPowerupCount` by 1
3. **Restore Hull**: Recover 1/3 of maximum hull health
4. **Restore Shield**: Recover 50% of maximum shield
5. **Visual Effect**: Display golden aura and "REVIVED!" notification for 2 seconds

### Death Condition

The ship is **only destroyed** when hull reaches 0 **AND** no weapon powerups are active (`weaponPowerupCount == 0`).

### Progression Example
```
Start: 3 weapon powerups collected (Level 3)
├─ First "death" → Revive! Now at Level 2, with 33% hull + 50% shield
├─ Second "death" → Revive! Now at Level 1, with 33% hull + 50% shield  
├─ Third "death" → Revive! Now at Level 0, with 33% hull + 50% shield
└─ Fourth "death" → Game Over (no more powerups)
```

### Visual Feedback

#### Golden Aura Effect (2 seconds)
When revived, the ship displays a special golden aura:
- **Expanding Rings**: Two pulsing golden rings
- **Energy Particles**: 8 golden particles swirling around the ship
- **Golden Shield**: Enhanced shield with golden tint
- **Fade Out**: Effect gradually fades over 2 seconds

#### HUD Notification
A large, pulsing message appears in the center of the screen:
```
⚡ REVIVED! ⚡
Weapon Level: X/3 | Hull: YY
```

### Strategic Implications

#### Risk vs Reward
- **Collect Early**: Getting weapon powerups early gives you both power AND survivability
- **Save for Critical Moments**: Each powerup is effectively an extra life
- **Aggressive Play**: Players can play more aggressively knowing they have multiple lives

#### Weapon Power Trade-off
Each revive reduces weapon power, creating a downward spiral:
- Level 3 → 2: From 2.5x to 2.0x damage (20% reduction)
- Level 2 → 1: From 2.0x to 1.5x damage (25% reduction)  
- Level 1 → 0: From 1.5x to 1.0x damage (33% reduction)

#### Maximum Lives
With 3 weapon powerups collected: **4 total deaths** before game over
  - 1 initial life (Level 3)
  - 3 revives (Level 2 → 1 → 0)
- Each revive gives you a fighting chance with restored health and shield

### Balance Considerations

- **1/3 Hull**: Enough to survive but not overpowered
- **50% Shield**: Provides immediate protection
- Combined recovery gives player a real chance to recover

---

## Movement Patterns

### Straight Line
- Direct path from origin to target
- No deviation or tracking
- Used by: Laser projectiles

### Homing
- Actively tracks and follows targets
- **Weak Homing (30%)**: Gentle curve toward target (Plasma)
- **Strong Homing (70%)**: Aggressive tracking (Missile)
- Turn rate varies by projectile type

### Wave Motion
- Sine wave movement pattern
- Creates unpredictable, weaving path
- Used by: Energy Orb
- Makes dodging more challenging

---

## Special Properties

### Piercing
- Projectile continues through targets
- Can hit multiple enemies in a line
- Currently: Energy Orb only

### Explosive
- Creates area damage on impact
- Damages all enemies in radius
- **Missile**: 30 pixel radius
- **Energy Orb**: 40 pixel radius
- Visual: Explosion effect on impact

### Tracking/Homing
- Automatically adjusts trajectory
- **Plasma**: 30% tracking strength
- **Missile**: 70% tracking strength
- Requires line of sight to target

---

## HUD Display

### Damage Indicator Location
The damage indicator appears in the bottom-left HUD panel:
- **Label**: "DMG/BULLET"
- **Position**: Below the weapon mode display
- **Size**: Large, prominent number with visual bar

### Color Coding

The damage number changes color based on the current damage level:

| Damage Range | Color | Description |
|--------------|-------|-------------|
| < 3.0        | Gray  | Low damage - weak |
| 3.0 - 5.9    | Yellow | Medium damage - standard |
| 6.0 - 9.9    | Orange | Good damage - strong |
| 10.0 - 14.9  | Orange-Red | High damage - powerful |
| ≥ 15.0       | Bright Red | Massive damage - devastating |

### Glow Effect
For damage values ≥ 6.0, a subtle glow/shadow effect is added behind the number for emphasis.

### Active Multiplier Badges
Small indicators appear next to the damage number showing active bonuses:
- **×1.5**, **×2.0**, or **×2.5** (Orange): Weapon powerup multiplier
- **×2 !** (Red): Offensive mode bonus with full energy

### Progress Bar
A horizontal bar below the number provides a visual representation:
- **Width**: 150 pixels maximum
- **Scaling**: Calibrated to 20 damage (100% = 20 damage)
- **Color**: Matches the damage number color
- **Background**: Semi-transparent version of the damage color

### Bottom HUD Layout

The bottom HUD (100px height) displays:
1. **Left Section**: Ship status bars (hull, shield, energy, mode)
2. **Middle-Left**: Score (large, gold)
3. **Middle**: Weapon info (mode, power level, damage with bonuses/multipliers, charge)
4. **Right**: Controls reference

---

## Sprites and Assets

### Projectile Sprites
```
assets/sprites/
├── projectile_spritesheet.png    # All projectiles
└── projectiles/
    ├── projectile_laser.png      # Individual sprites
    ├── projectile_plasma.png
    ├── projectile_missile.png
    └── projectile_energy_orb.png
```

### Sprite Specifications
- **Individual Size**: 32x32 pixels
- **Sprite Sheet**: 128x32 pixels (4 projectiles)
- **Format**: PNG with transparency
- **Animation**: Rotation and glow effects

---

## Code Integration

### Technical Implementation

#### Function: `CalculateDamagePerShot()`
**Location**: `src/entities/player_ship.c`

**Parameters**: 
- `const PlayerShip* ship` - Player ship state

**Returns**: 
- `float` - Damage per individual bullet/projectile

**Logic**:
1. Determine base damage per bullet from weapon mode
2. Apply weapon powerup multiplier (1x, 1.5x, 2x, or 2.5x)
3. Apply offensive mode ×2 bonus if active with full energy
4. Return damage per individual bullet

#### Function: `DrawShipHUD()`
**Location**: `src/entities/player_ship.c`

**Features**:
- Displays "DMG/BULLET" label
- Calls `CalculateDamagePerShot()` to get current per-bullet damage
- Determines color based on damage level
- Formats number (1 decimal for < 10, integer for ≥ 10)
- Draws glow effect for high damage values
- Displays active multiplier badges (weapon powerup and offensive mode)
- Renders progress bar scaled to max 20 damage per bullet

### Modified Files

1. **include/types.h**
   - Added `int powerLevel` field to `Bullet` structure

2. **src/systems/weapon.c**
   - `InitBullets()`: Initialize powerLevel to 0
   - `ShootBulletsForMode()`: Apply damage multiplier and set bullet powerLevel
   - `UpdateBullets()`: Apply multipliers to devastating attack and charge mode bullets
   - `DrawBullets()`: Render bullets with progressive visual scaling

3. **include/player_ship.h**
   - Added `bool justRevived` - flag when revive happens
   - Added `float reviveEffectTimer` - timer for visual effect

4. **src/entities/player_ship.c**
   - `DamagePlayerShip()`: Check for revive when hull reaches 0
   - `UpdatePlayerShip()`: Update revive timer
   - `DrawShieldEffect()`: Render golden aura during revive
   - `DrawShipHUD()`: Display revive notification

### Code Examples

#### Initialize Projectile System
```c
// Initialize projectile types
InitProjectileTypes();

// Create projectile pool
ProjectilePool* pool = CreateProjectilePool(MAX_PROJECTILES);
```

#### Fire Player Projectile
```c
// Check heat and cooldown
if (!player->overheated && player->weaponHeat < 100) {
    Projectile* proj = GetFreeProjectile(pool);
    if (proj) {
        InitializeProjectile(proj, PROJECTILE_LASER, 
                           player->position, targetPos, false);
        player->weaponHeat += WEAPON_HEAT_PER_SHOT;
    }
}
```

#### Enemy Firing Pattern
```c
// Get enemy weapon configuration
const EnemyWeaponConfig* config = GetEnemyWeaponConfig(enemy->type);

// Fire burst pattern
for (int i = 0; i < config->burstCount; i++) {
    float angle = CalculateSpreadAngle(i, config->burstCount, 
                                       config->spreadAngle);
    FireProjectileWithAngle(enemy->position, angle, 
                           config->primaryWeapon);
}
```

#### Revive Logic
```c
// In DamagePlayerShip()
if (ship->health <= 0 && ship->weaponPowerupCount > 0) {
    ship->weaponPowerupCount--;           // Consume one level
    ship->health = ship->maxHealth / 3;   // Restore 1/3 hull
    ship->shield = ship->maxShield * 0.5f; // Restore 50% shield
    ship->justRevived = true;
    ship->reviveEffectTimer = 2.0f;
}
```

#### Power Multiplier Calculation
```c
float powerMultiplier = 1.0f;
switch (playerShip->weaponPowerupCount) {
    case 0: powerMultiplier = 1.0f; break;  // Standard
    case 1: powerMultiplier = 1.5f; break;  // +1
    case 2: powerMultiplier = 2.0f; break;  // +2
    case 3: powerMultiplier = 2.5f; break;  // +3
}

// Visual scaling
float sizeScale = 1.0f + (powerLevel * 0.2f);
float colorIntensity = 1.0f + (powerLevel * 0.15f);
```

---

## Performance Considerations

- **Max Active Projectiles**: 100 simultaneous
- **Auto-despawn**: When leaving screen bounds
- **Lifetime Limits**: Prevent memory leaks
- **Trail Effects**: Can be toggled for performance
- **Collision Checks**: Optimized spatial partitioning

---

## Usage Commands

### Generate Projectile Sprites
```bash
make generate_projectiles
```

### Run Projectile Showcase
```bash
make projectile_showcase
```

### Showcase Controls
- **Left Mouse**: Fire single projectile
- **Right Mouse**: Fire enemy pattern
- **[1-4]**: Select projectile type
- **[←→]**: Select enemy type
- **[A]**: Toggle auto-fire
- **[SPACE]**: Pause
- **[T]**: Toggle trails
- **[C]**: Clear all projectiles
