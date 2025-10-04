# Weapons and Projectiles Documentation

This document describes the complete weapon system, projectile types, and combat mechanics.

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

## Weapon Configurations by Enemy Type

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

**Note**: Tank enemies fire 3 medium-speed tracking missiles in a spread pattern. During intense bass phases (medium/high), multiple tanks spawn simultaneously creating challenging missile barrages.

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
- **Projectile Type**: Currently laser (future: upgradeable)
- **Input**: Space bar or left mouse button
- **Auto-fire**: Hold for continuous shooting (until overheat)

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

### Future Patterns
- **Spiral**: Corkscrew movement for area coverage
- **Spread**: Expands cone over distance
- **Boomerang**: Returns if miss
- **Chain**: Jumps between targets

## Special Properties

### Piercing
- Projectile continues through targets
- Can hit multiple enemies in a line
- Currently: Energy Orb only
- Future: Upgradeable for other types

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

## Damage Calculation

### Base Formula
```
Final Damage = Base Damage × (1 - Enemy Resistance) × Difficulty Modifier
```

### Damage Resistance by Enemy
- **No Resistance (0%)**: Grunt, Speeder, Swarm
- **Low (10-20%)**: Zigzag, Bomber
- **Medium (30-40%)**: Elite, Ghost
- **High (50-70%)**: Tank, Shield, Boss

### Effective Strategies
- **vs Fast Enemies**: Use homing projectiles (Plasma/Missile)
- **vs Groups**: Use explosive projectiles (Energy Orb)
- **vs Tanks**: Focus fire with high-damage weapons (Missile)
- **vs Shields**: Time shots between shield rotations

## Sprite Information

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

## Performance Considerations

- **Max Active Projectiles**: 100 simultaneous
- **Auto-despawn**: When leaving screen bounds
- **Lifetime Limits**: Prevent memory leaks
- **Trail Effects**: Can be toggled for performance
- **Collision Checks**: Optimized spatial partitioning

## Using the Weapon System

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

## Code Integration Examples

### Initialize Projectile System
```c
// Initialize projectile types
InitProjectileTypes();

// Create projectile pool
ProjectilePool* pool = CreateProjectilePool(MAX_PROJECTILES);
```

### Fire Player Projectile
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

### Enemy Firing Pattern
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

## Future Enhancements

### New Projectile Types
- **Beam Weapons**: Continuous damage laser
- **Cluster Bombs**: Split into multiple projectiles
- **Bouncing Shots**: Ricochet off obstacles
- **Chain Lightning**: Jump between nearby enemies

### Weapon Upgrades
- **Power Ups**: Temporary weapon enhancements
- **Permanent Upgrades**: Unlock new projectile types
- **Combo Weapons**: Combine projectile effects
- **Charge Shots**: Hold to charge for more damage

### Visual Improvements
- **Impact Animations**: Unique per projectile type
- **Particle Effects**: Trails, sparks, explosions
- **Screen Effects**: Shake, flash on big impacts
- **Damage Numbers**: Show damage dealt

### Gameplay Features
- **Deflection**: Reflect enemy projectiles
- **Absorption**: Convert projectiles to energy
- **Weapon Switching**: Quick swap between types
- **Special Abilities**: Ultimate attacks
