# Enemies and Sprites Documentation

This document combines all information about enemy types, their behaviors, and sprite designs.

## Enemy Types Overview

The game features 10 distinct enemy types, each with unique characteristics:

| Type | Name | Symbol | Power | Health | Speed | Size | Resistance | Color |
|------|------|--------|-------|--------|-------|------|------------|-------|
| 1 | **Grunt** | ▲ | 10 | 1 | 1.2x | 0.8x | 0% | Dark Red |
| 2 | **Tank** | ■ | 30 | 20 | 0.5x | 2.0x | 20% | Gray-Blue |
| 3 | **Speeder** | » | 15 | 1 | 2.5x | 0.6x | 0% | Yellow |
| 4 | **Zigzag** | ◊ | 20 | 2 | 1.0x | 1.0x | 10% | Green |
| 5 | **Bomber** | ◉ | 40 | 3 | 0.8x | 1.5x | 20% | Purple |
| 6 | **Shield** | ◎ | 25 | 4 | 0.9x | 1.2x | 70% | Cyan |
| 7 | **Swarm** | • | 8 | 1 | 1.5x | 0.5x | 0% | Orange |
| 8 | **Elite** | ★ | 35 | 4 | 1.3x | 1.3x | 30% | Gold |
| 9 | **Ghost** | ◈ | 22 | 2 | 1.1x | 1.1x | 40% | Light Blue |
| 10 | **Boss** | ✦ | 50 | 10 | 0.6x | 3.0x | 60% | Dark Red |

## Detailed Enemy Descriptions

### 1. Grunt (▲) - Light Fighter
- **Role**: Basic fodder enemy
- **Behavior**: Simple straight-line movement with slight vertical drift
- **Strategy**: Appears frequently, easy to defeat but can overwhelm in numbers
- **Visual Design**: Small arrow-shaped fighter with swept wings
- **Sprite Features**: Compact triangular hull, small cockpit, wing-tip weapons

### 2. Tank (■) - Heavy Assault Cruiser ⚠️⚠️
- **Role**: Heavy missile platform with tactical assault capability
- **Health**: 20 hits (very durable!)
- **Behavior**: Tactical assault pattern - advances, pauses at strategic position, then retreats
- **Weapons**: 3 medium-speed tracking missiles with 20° spread pattern, fires at 0.6/s
- **Spawning**: During intense bass phases (medium/high), exactly 2 tanks spawn in spaced formation (150px apart)
- **Movement Pattern**: 
  - Advances at variable speeds (50-100% speed)
  - Stops at random positions (not reaching left third of screen)
  - Pauses for 5-10 seconds while firing
  - Retreats back to right edge and disappears
  - Each tank has independent timing
- **Strategy**: EXTREMELY HIGH PRIORITY! Focus fire while they're paused. Avoid missile barrages. Takes sustained damage to destroy.
- **Visual Design**: Blocky, heavily armored battleship
- **Sprite Features**: Rectangular main hull, side engine pods, front armor plating, missile tubes

### 3. Speeder (») - Fast Interceptor
- **Role**: Fast attacker
- **Behavior**: Rapid horizontal movement with speed trails
- **Strategy**: Hard to hit due to speed, low health makes timing crucial
- **Visual Design**: Sleek, needle-like ship with swept-back wings
- **Sprite Features**: Long thin fuselage, swept wings, prominent thruster

### 4. Zigzag (◊) - Agile Fighter
- **Role**: Unpredictable movement
- **Behavior**: Moves in zigzag patterns, hard to predict
- **Strategy**: Requires leading shots due to unpredictable movement
- **Visual Design**: Diamond-shaped hull with angled wings
- **Sprite Features**: Angular design, zigzag wing pattern, twin engines

### 5. Bomber (◉) - Heavy Bomber
- **Role**: High-damage threat
- **Behavior**: Pulsing danger indicator, drops bombs (future implementation)
- **Strategy**: Priority target due to high damage
- **Visual Design**: Wide-body craft with visible bomb bay
- **Sprite Features**: Elliptical fuselage, wing-mounted engines, exposed bombs

### 6. Shield (◎) - Defensive Craft
- **Role**: Protected enemy
- **Behavior**: Rotating shield particles provide protection
- **Strategy**: Wait for shield gaps or use special weapons
- **Visual Design**: Hexagonal ship with shield generator nodes
- **Sprite Features**: Six shield projectors at vertices, energy field lines

### 7. Swarm (•) - Drone Ship
- **Role**: Group attacker
- **Behavior**: Circular movement pattern, coordinates with other swarm units
- **Strategy**: Area damage effective, can slip through defenses
- **Visual Design**: Tiny triangular drone with antenna
- **Sprite Features**: Compact design, communication antenna, mini thrusters

### 8. Elite (★) - Advanced Fighter
- **Role**: All-around strong enemy
- **Behavior**: Intelligent movement, adapts to player position
- **Strategy**: Balanced threat requiring focused attention
- **Visual Design**: Star-shaped configuration with weapon pods
- **Sprite Features**: Five-pointed star hull, weapon pod on each point

### 9. Ghost (◈) - Stealth Fighter
- **Role**: Phasing enemy
- **Behavior**: Alternates between visible and semi-transparent states
- **Strategy**: Time attacks when visible, can't be hit when phased
- **Visual Design**: Angular stealth bomber shape
- **Sprite Features**: Faceted surfaces, semi-transparent effect, cloaking field

### 10. Boss (✦) - Massive Battleship
- **Role**: Major threat
- **Health**: 150 hits + shield
- **Behavior**: Hovers in last third of screen with random movement, heavy weapon barrage
- **Strategy**: Requires sustained fire and pattern recognition
- **Visual Design**: Large multi-hull dreadnought
- **Sprite Features**: Complex hull design, multiple weapon turrets, command bridge
- **Death Effect**: Spectacular multi-explosion sequence (9 simultaneous explosions, 3-second duration)

## Enemy Properties Explained

### Power
- Damage dealt to player on collision
- Range: 8 (Swarm) to 50 (Boss)

### Health
- Number of hits required to destroy
- Range: 1 (basic enemies) to 150 (Boss)

### Speed Multiplier
- Movement speed relative to base speed
- Range: 0.5x (Tank) to 2.5x (Speeder)

### Size Multiplier
- Visual size relative to standard enemy
- Range: 0.5x (Swarm) to 3.0x (Boss)

### Damage Resistance
- Percentage of damage reduced
- Range: 0% (no resistance) to 70% (Shield)

## Explosion Effects

When enemies are destroyed, they create spectacular explosions that scale with their size:

### Explosion Tiers

| Enemy Size | Enemies | Explosion Type | Radius | Particles | Shockwave | Screen Shake |
|------------|---------|----------------|--------|-----------|-----------|--------------|
| **Small** (20-35px) | Swarm, Speeder, Grunt | Small | 30px | 10 | ❌ | ❌ |
| **Medium** (36-50px) | Zigzag, Ghost, Shield | Medium | 50px | 20 | ✅ | Light |
| **Medium+** (51-70px) | Elite, Bomber | Medium+ | 60px | 25 | ✅ | Medium |
| **Large** (71-100px) | Tank | Large | 80px | 40 | ✅ | Strong |
| **Boss** (100+px) | Boss | **Multi-Explosion** | 180px | 50 | ✅ | Very Strong |

### Special Boss Explosion

When the boss is destroyed, a spectacular **multi-stage explosion** occurs:
- **Main explosion**: 180px radius, 50 particles, lasts 3 seconds
- **8 secondary explosions**: Arranged in circle pattern (45° apart)
- **Chain reaction effect**: Multiple shockwave rings
- **Intense screen shake**: 25.0 intensity for 1.2 seconds
- **Total explosions**: 9 simultaneous (1 main + 8 secondary)

This creates the most spectacular death effect in the game!

## Sprite System

### Available Sprite Sets

1. **Abstract Sprites** - Geometric shapes and symbols
2. **Spaceship Sprites** - Detailed spaceship designs

### File Locations
```
assets/sprites/
├── enemy_spritesheet.png         # Abstract enemy sprites
├── spaceship_spritesheet.png     # Spaceship sprites
├── enemies/                      # Individual abstract sprites
│   ├── enemy_grunt.png
│   ├── enemy_tank.png
│   └── ... (all enemy types)
└── spaceships/                   # Individual spaceship sprites
    ├── spaceship_grunt.png
    ├── spaceship_tank.png
    └── ... (all enemy types)
```

### Technical Details
- **Sprite Size**: 64x64 pixels per enemy
- **Sprite Sheet**: 320x128 pixels (5x2 grid)
- **Format**: PNG with transparency
- **Background**: Transparent for easy integration

### Design Philosophy

The sprite designs follow these principles:

1. **Visual Hierarchy**: Size correlates with enemy strength
2. **Shape Language**: 
   - Sharp angles = aggressive/fast
   - Rounded = defensive/slow
   - Complex = advanced/elite
3. **Color Coding**: Maintains consistent color scheme
4. **Functional Details**: Visual elements hint at capabilities
5. **Readability**: Clear silhouettes for instant recognition

## Running the Enemy Showcase

### Build and Run
```bash
# Build and run the enemy showcase
make showcase
```

### Showcase Controls
- **SPACE**: Pause/Resume animations
- **D**: Toggle detail display
- **S**: Switch between sprites and procedural graphics
- **T**: Toggle between Abstract and Spaceship sprite sets
- **R**: Reset camera
- **Arrow Keys**: Pan camera
- **Mouse Wheel**: Zoom in/out
- **Mouse Hover**: Select enemy for detailed stats

## Integration with Main Game

To use these enemy types in the main game:

1. Select enemy types based on game progression
2. Adjust spawn rates for different types
3. Create waves with specific enemy compositions
4. Scale difficulty by modifying base stats
5. Load appropriate sprite sheets for visual representation

## Future Enhancements

### Enemy Behaviors
- Projectile attacks for certain enemy types
- Formation flying for swarm enemies
- Special abilities (teleportation, shields, etc.)
- Boss phases with different attack patterns
- Difficulty scaling for enemy stats

### Sprite Improvements
- Damage states showing battle damage
- Animation frames (engine glow, rotating parts)
- Faction variants with different color schemes
- Boss transformation states
- Matching projectile sprites for each ship design
