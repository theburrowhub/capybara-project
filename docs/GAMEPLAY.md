# Gameplay Guide

## Controls

### Keyboard Controls
- **Movement**: WASD or Arrow Keys
- **Shoot**: Left Mouse Button (hold for continuous fire)
- **Pause**: P
- **Debug Mode**: H (show hitboxes)
- **Restart**: R (when game over)
- **Exit**: ESC

### Planned Controller Support
- Xbox/PlayStation controller support (see TODO.md)

## Game Mechanics

### Player Ship
- **Health**: 10 hit points
- **Movement**: Limited to left half of screen
- **Weapon**: Rapid-fire projectiles with heat management

### Pause Feature
- Press **P** to pause the game at any time
- While paused:
  - All game updates are frozen (enemies, projectiles, timers)
  - "PAUSED" overlay appears on screen
  - Press P again to resume
- Useful for taking breaks or analyzing the game state

### Weapon Heat System
The weapon system prevents continuous spam shooting:
- Heat builds up with each shot (8% per shot)
- Weapon cools down when not firing (30% per second)
- **Overheat** at 100% heat triggers 3-second cooldown
- Visual indicator shows heat level with color coding:
  - 🟢 Green: 0-25% (Cool)
  - 🟡 Yellow: 25-50% (Warm)
  - 🟠 Orange: 50-75% (Warning)
  - 🔴 Red: 75-100% (Critical)
  - 🟣 Purple: Overheated

### Enemies
- 10 unique enemy types with different behaviors
- Each type has unique stats (health, speed, damage, resistance)
- Spawn from the right side of screen
- **Points**: Varies by enemy type (8-50 damage on collision)
- **Full Details**: See [Enemies and Sprites Documentation](ENEMIES_AND_SPRITES.md)

### Obstacles
Two types of obstacles spawn randomly:

#### Destructible (Orange)
- Can be destroyed with 2 hits
- Changes color when damaged
- Worth 50 points when destroyed

#### Indestructible (Gray)
- Cannot be destroyed
- Must be avoided
- Instant death on collision

### Difficulty Progression
The game increases in difficulty over time:
- Speed increases every 20 seconds
- Affects:
  - Enemy movement speed
  - Obstacle movement speed
  - Background scroll speed
  - Spawn rates
- Visual warning 5 seconds before speed increase

## Scoring System

- **Enemy Kills**: Points vary by enemy type
  - Swarm: 80 points
  - Grunt/Speeder: 100-150 points
  - Tank/Elite/Boss: 300-500 points
- **Obstacle Destroyed**: 50 points
- **Survival Bonus**: Points accumulate over time
- **Combo Multiplier**: Chain kills for bonus points

## Game Over Conditions

The game ends when:
- Player health reaches 0 (from enemy collisions)
- Player collides with any obstacle (instant death)

## Tips & Strategies

1. **Heat Management**: Fire in bursts to avoid overheating
2. **Positioning**: Stay mobile but avoid the right side
3. **Priority Targets**: Clear destructible obstacles for points
4. **Pattern Recognition**: Learn enemy movement patterns
5. **Risk vs Reward**: Destroying obstacles gives points but risks collision

## Debug Features

Press **H** to enable hitbox visualization:
- Green outline: Player hitbox
- Pink outline: Enemy hitboxes
- Magenta outline: Obstacle hitboxes

Collision events are logged to `collision_log.txt` for analysis.

## Advanced Features

### Available Enemy Types
The game includes 10 different enemy types:
- **Grunt, Tank, Speeder, Zigzag, Bomber**
- **Shield, Swarm, Elite, Ghost, Boss**
- Each with unique behaviors and weapon configurations
- See [Enemies and Sprites](ENEMIES_AND_SPRITES.md) for details

### Projectile System
4 projectile types with different properties:
- **Laser**: Fast, straight shots
- **Plasma**: Medium speed with homing
- **Missile**: Slow but high damage with strong homing
- **Energy Orb**: Area damage with piercing
- See [Weapons and Projectiles](WEAPONS_AND_PROJECTILES.md) for details

## Planned Features

See [TODO List](TODO.md) for upcoming features including:
- Power-ups and upgrades
- Boss battle phases
- Controller support
- Capybara-themed sprites
- Local co-op mode
