# Player Ship Design Documentation

## Overview

The enhanced player ship system features a sophisticated multi-layered design with advanced capabilities, visual effects, and upgrade systems. This document details the complete player ship implementation.

## Ship Features

### 1. Core Systems

#### Health & Shield System
- **Hull Integrity**: 100 HP base health
- **Energy Shield**: 50 points regenerating shield
- **Shield Regeneration**: 5 points/second after 3-second delay
- **Damage Mitigation**: Shield absorbs damage before hull

#### Energy Management
- **Energy Pool**: 100 points for abilities
- **Regeneration Rate**: 10 points/second
- **Ability Costs**: Variable per ability

#### Movement System
- **Base Speed**: 5.0 units/frame
- **Boost Speed**: 3x base speed
- **Banking Animation**: Visual tilt when turning
- **Smooth Acceleration**: Physics-based movement

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

### 3. Special Abilities

#### Boost (SHIFT)
- **Cost**: 20 energy
- **Duration**: 2 seconds
- **Effect**: 3x movement speed
- **Cooldown**: 3 seconds
- **Visual**: Orange boost trail

#### Shield Burst (E)
- **Cost**: 40 energy
- **Duration**: 3 seconds
- **Effect**: Temporary invulnerability
- **Cooldown**: 10 seconds
- **Visual**: Pulsing hexagonal shield

#### EMP Blast (Q)
- **Cost**: 60 energy
- **Duration**: 0.5 seconds
- **Effect**: Clears nearby projectiles
- **Cooldown**: 15 seconds
- **Visual**: Expanding energy rings

#### Overdrive (R)
- **Cost**: 80 energy
- **Duration**: 5 seconds
- **Effect**: Unlimited firing, no heat
- **Cooldown**: 20 seconds
- **Visual**: Enhanced weapon effects

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
- 8 frames of animation
- Frames 0-3: Normal flight states
- Frames 4-7: Boosted/shielded states
- Dynamic sprite switching

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
- Better boost multiplier
- Improved acceleration

#### Energy Upgrades (Levels 1-5)
- Larger energy pool
- Faster regeneration
- Reduced ability costs

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
    
    // Abilities
    float abilityCooldowns[ABILITY_COUNT];
    bool abilityActive[ABILITY_COUNT];
    
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
| 1-6 | Switch weapon modes |
| SHIFT | Activate boost |
| E | Shield burst |
| Q | EMP blast |
| R | Overdrive mode |
| TAB | Toggle stats |
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

### Heat Management
The weapon heat system prevents continuous firing:
- Heat builds up: 8 units per shot
- Cooldown rate: 30 units/second
- Overheat threshold: 100 units
- Overheat penalty: 2-second cooldown

### Shield Mechanics
Shield regeneration follows these rules:
1. Damage stops regeneration
2. 3-second delay before regen starts
3. Regenerates at 5 points/second
4. Shield burst provides full shield + invulnerability

### Ability Balance
Abilities are balanced through:
- Energy costs (20-80 points)
- Cooldown timers (3-20 seconds)
- Duration limits (0.5-5 seconds)
- Visual telegraphing

## Credits

Designed and implemented as part of the Capybara Project space shooter game. The player ship features a sleek, modern design inspired by classic arcade shooters with contemporary visual effects and gameplay mechanics.
