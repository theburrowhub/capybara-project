# Player Ship Design Documentation

## Overview

The enhanced player ship system features a sophisticated multi-layered design with advanced capabilities, visual effects, and upgrade systems. This document details the complete player ship implementation.

## Ship Features

### 1. Core Systems

#### Health & Shield System
- **Hull Integrity**: 100 HP base health
- **Energy Shield**: 25 points (Offensive Mode) / 50 points (Defensive Mode)
- **Shield Regeneration**: 2 points/second after 5-second delay (4 points/sec in Defensive with full energy)
- **Damage Mitigation**: Shield absorbs damage before hull

#### Energy Management
- **Energy Pool**: 100 points for special abilities
- **Regeneration Rate**: 2 points/second (strategic resource management)

#### Movement System
- **Base Speed**: 5.0 units/frame
- **Banking Animation**: Visual tilt when turning (-30° to +30°)
- **Smooth Movement**: Physics-based acceleration

### 2. Weapon Systems

The player ship features 6 distinct weapon modes:

#### Single Shot (Mode 1)
- Standard forward projectile
- Balanced damage and fire rate
- Low heat generation

#### Double Shot (Mode 2)
- Parallel twin projectiles
- Increased damage output
- Moderate heat generation

#### Spread Shot (Mode 3)
- 3-way spread pattern
- Area coverage
- Higher heat generation

#### Rapid Fire (Mode 4)
- Very high fire rate
- Lower damage per shot
- High heat generation

#### Charge Beam (Mode 5)
- Hold to charge
- Massive damage on release
- Charge indicator display
- Variable damage based on charge level

#### Wave Shot (Mode 6)
- 5 projectiles in wave pattern
- Sine wave movement
- Area denial capability

### 3. Energy Mode System

#### Offensive Mode (Default)
- **Max Shield**: 25 points
- **Energy Bonus (when full)**: 2× weapon damage multiplier
- **Special Ability (Hold E)**: Devastating Attack
  - Drains energy continuously while held
  - Fires continuous spread of bullets
  - Drains 40 energy/second in Offensive mode
  - Visual: Massive bullet spread pattern

#### Defensive Mode (Press Q to toggle)
- **Max Shield**: 50 points (2× capacity)
- **Energy Bonus (when full)**: 2× shield regeneration (4 points/sec)
- **Special Ability (Hold E)**: Enhanced Shield
  - Drains energy continuously while held
  - Larger shield radius (60px vs 40px)
  - Absorbs 50% of all damage
  - Drains 20 energy/second in Defensive mode
  - Visual: Brighter, larger hexagonal shield

#### Energy Management
- **Energy Pool**: 100 points
- **Regeneration**: 2 points/second (slow regeneration encourages strategic use)
- **Regen Delay**: Energy stops regenerating for 5 seconds after depletion
- **Toggle**: Press Q to switch between Offensive and Defensive modes

### 4. Visual Design

#### Ship Appearance
- **Primary Color**: Cyan blue (#0096FF)
- **Secondary Color**: Gold accents (#FFC800)
- **Tertiary Color**: Light blue-gray details
- **Engine Color**: Light blue glow

#### Visual Effects
- **Engine Trail**: 20-point particle trail
- **Engine Pulse**: Animated engine glow
- **Banking Effect**: Ship tilts when turning
- **Shield Bubble**: Hexagonal pattern overlay
- **Weapon Ports**: Visible firing positions

#### Sprite Animation
- Simplified sprite system
- Banking animation when turning
- Energy mode visual indicators (orange/cyan)

### 5. Upgrade System

#### Weapon Upgrades (Levels 1-5)
- Increased damage output
- Reduced heat generation
- Improved fire rate

#### Shield Upgrades (Levels 1-5)
- Higher maximum shield
- Faster regeneration
- Shorter regen delay

#### Engine Upgrades (Levels 1-5)
- Increased base speed
- Improved acceleration

#### Energy Upgrades (Levels 1-5)
- Larger energy pool
- Faster regeneration

## Implementation Details

### File Structure

```
include/
  player_ship.h        - Enhanced player ship header
  
src/entities/
  player_ship.c        - Complete implementation
  
src/demo/
  player_ship_showcase.c - Interactive demonstration
  
src/tools/
  generate_player_sprite.c - Sprite generator
  
assets/sprites/
  player_ship.png      - Individual sprite
  player_ship_spritesheet.png - Animation sheet
```

### Key Data Structures

```c
typedef struct PlayerShip {
    // Position & Physics
    Vector2 position;
    Vector2 velocity;
    Rectangle bounds;
    float rotation;
    
    // Health & Shield
    int health, maxHealth;
    float shield, maxShield;
    float shieldRegenRate;
    
    // Energy System
    float energy, maxEnergy;
    float energyRegenRate;
    
    // Weapons
    WeaponMode weaponMode;
    float weaponHeat, maxHeat;
    bool overheated;
    
    // Energy mode system
    EnergyMode energyMode;
    bool specialAbilityActive;
    
    // Visual Effects
    Vector2 trailPositions[20];
    float engineGlow, shieldGlow;
    Color primaryColor, engineColor;
    
    // Upgrades
    ShipUpgrades upgrades;
} PlayerShip;
```

## Usage Guide

### Building the Player Ship Demo

```bash
# Generate player sprite
make generate_player

# Run player showcase
make player_showcase

# Or do both at once
make player
```

### Controls

| Key | Action |
|-----|--------|
| WASD/Arrows | Movement |
| SPACE | Fire weapon |
| 1-6 | Select specific weapon mode |
| R | Cycle through weapon modes |
| Q | Toggle energy mode (Offensive/Defensive) |
| E (hold) | Activate special ability |
| P | Pause game |
| H | Toggle hitboxes |
| H | Toggle help |

### Debug Controls (Showcase Only)

| Key | Action |
|-----|--------|
| - | Damage ship (10 HP) |
| = | Repair ship (10 HP) |
| U | Apply random upgrade |

## Integration with Main Game

To integrate the enhanced player ship into the main game:

1. Replace `player.h/c` references with `player_ship.h/c`
2. Update game state to use `PlayerShip` instead of `Player`
3. Modify collision system to handle new bounds
4. Update HUD rendering to use `DrawShipHUD()`
5. Integrate ability effects with game logic

## Performance Considerations

- Trail system uses circular buffer (no allocations)
- Particle effects are procedurally generated
- Sprite batching for projectiles
- Efficient collision detection with spatial partitioning
- Heat system prevents spam while maintaining gameplay

## Future Enhancements

### Planned Features
- [ ] Weapon combinations (dual-mode firing)
- [ ] Shield absorption (converts damage to energy)
- [ ] Homing missiles
- [ ] Deployable drones
- [ ] Warp drive ability
- [ ] Customizable ship colors
- [ ] Persistent upgrade progression
- [ ] Achievement system

### Visual Improvements
- [ ] Damage state animations
- [ ] Shield impact effects
- [ ] Muzzle flash effects
- [ ] Engine afterburner
- [ ] Cockpit interior view
- [ ] Dynamic lighting

## Technical Notes

### Shield Mechanics
Shield regeneration follows these rules:
1. Damage stops regeneration
2. 5-second delay before regen starts
3. Regenerates at 2 points/second (base rate)
4. Regenerates at 4 points/second (Defensive mode with full energy)
5. Max shield depends on energy mode: 25 (Offensive) or 50 (Defensive)

### Energy System Balance
The energy system is balanced for strategic decision-making:
- **Slow Regeneration**: 2 points/second (50 seconds for full recharge)
- **Energy Drain**: Special abilities drain energy while active
  - Offensive mode: 40 energy/second when using Devastating Attack
  - Defensive mode: 20 energy/second when using Enhanced Shield
- **Depletion Penalty**: 5-second delay before energy regeneration after running out
- **Mode Switching**: Instant switching between Offensive/Defensive modes with Q key

## Credits

Designed and implemented as part of the Capybara Project space shooter game. The player ship features a sleek, modern design inspired by classic arcade shooters with contemporary visual effects and gameplay mechanics.
