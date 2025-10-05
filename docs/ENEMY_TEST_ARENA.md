# Enemy Test Arena

An interactive testing environment for debugging and analyzing enemy behavior in the game.

## Overview

The Enemy Test Arena is a two-mode showcase system that allows you to:
1. **Select** any enemy type from a visual grid
2. **Test** the enemy in a live combat arena with an invincible player ship

## How to Run

```bash
./run_enemy_test_arena.sh
```

Or manually:
```bash
make enemy_showcase
./bin/enemy_showcase
```

### Navigation Flow
```
Grid Selection → Click Enemy → Test Arena → ESC → Grid Selection → ESC → Exit
     ↑_______________________________________________|
```

## Features

### Grid Selection Mode (Initial Screen)

- **Visual Grid**: All 10 enemy types displayed in a grid layout
- **Live Animations**: Each enemy shows its characteristic animations
- **Hover Details**: Mouse over enemies to see their stats
- **Click to Test**: Click any enemy to enter the test arena

**Controls:**
- `Mouse Hover` - Highlight and view enemy details
- `Left Click` - Select enemy and enter test arena
- `ESC` - **Exit program** (only works in grid mode)

### Test Arena Mode

After clicking an enemy, you enter an interactive combat arena where:

- **Invincible Player**: Test without worrying about dying
- **Continuous Spawning**: Enemies respawn automatically when destroyed or leave screen
- **Standard Behaviors**: Each enemy uses its authentic attack pattern from the main game
- **Live Combat**: Shoot and destroy enemies to test weapon effectiveness
- **Statistics Tracking**: View spawned, destroyed, and active enemy counts

**Controls:**
- `WASD` or `Arrow Keys` - Move player ship
- `SPACE` or `Left Mouse Button` - Fire weapons (press each time, same as main game)
- `ESC` - **Return to grid selection** (select another enemy or exit)

**Gameplay:**
- **Identical to main game behavior:**
  - Fire rate: `0.12s` between shots (`WEAPON_FIRE_RATE`)
  - Bullet speed: `12.0f` pixels per frame (`BULLET_SPEED`)
  - Shooting: Press button each time (not continuous hold)
  - **Enemy firing**: Enemies fire projectiles using their weapon configurations
  - **Enemy projectiles**: Same types, speeds, and patterns as main game
- **Arena differences:**
  - Player is invincible - enemy projectiles pass through (for safe testing)
  - No weapon overheating
  - Focus on observing enemy attack patterns

## Enemy Behaviors

Each enemy type in the test arena uses its **authentic attack pattern from the main game**.

For detailed information about each enemy type including:
- Combat roles and strategies
- Movement patterns and behaviors  
- Stats (health, speed, power, resistance)
- Visual design and sprite features
- Tactical considerations

**See: [Enemies and Sprites Documentation](ENEMIES_AND_SPRITES.md)**

### Movement Patterns Used in Arena

The test arena applies the following patterns to enemies:

| Enemy Type | Pattern Applied | Reference |
|------------|----------------|-----------|
| Grunt | `straight` | [→ Grunt Details](ENEMIES_AND_SPRITES.md) |
| Tank | `tank_assault` | [→ Tank Details](ENEMIES_AND_SPRITES.md) |
| Speeder | `rush` | [→ Speeder Details](ENEMIES_AND_SPRITES.md) |
| Zigzag | `zigzag` | [→ Zigzag Details](ENEMIES_AND_SPRITES.md) |
| Bomber | `slow_advance` | [→ Bomber Details](ENEMIES_AND_SPRITES.md) |
| Shield | `hover` | [→ Shield Details](ENEMIES_AND_SPRITES.md) |
| Swarm | `erratic` | [→ Swarm Details](ENEMIES_AND_SPRITES.md) |
| Elite | `elite_retreat` | [→ Elite Details](ENEMIES_AND_SPRITES.md) |
| Ghost | `phasing` | [→ Ghost Details](ENEMIES_AND_SPRITES.md) |
| Boss | `boss` | [→ Boss Details](ENEMIES_AND_SPRITES.md) |

These patterns are defined in `src/systems/wave_system.c` in the `ApplyMovementPattern()` function.

## Use Cases

### 1. Behavior Testing
Test how each enemy type moves and attacks to ensure proper implementation.

### 2. Weapon Pattern Testing
- Observe enemy firing patterns (burst count, spread angles, projectile types)
- Verify Tank's 3-missile barrage, Shield's 360° spread, Elite's 4-shot pattern
- Test timing: Grunt fires 1/s, Speeder fires 3/s, Tank fires 0.6/s

### 3. Balance Adjustment
Evaluate enemy difficulty and time-to-kill to inform balance changes.

### 4. Visual Debugging
Observe enemy animations, special effects, and attack patterns in isolation.

### 5. Projectile Verification
- See which projectile types each enemy uses (lasers, plasma, missiles, orbs)
- Verify projectile speeds and behaviors
- Test homing missiles and energy orbs

### 6. Movement Pattern Verification
Ensure complex movement patterns (tank assault, elite retreat, boss hover) work correctly while firing.

## Implementation Details

### Architecture

The showcase uses a state-based architecture with two modes:

```c
typedef enum {
    MODE_GRID_SELECT,   // Grid selection screen
    MODE_TEST_ARENA     // Interactive test arena
} ShowcaseMode;
```

### Enemy Movement Patterns

Enemies use the same movement system as the main game:
- `ApplyMovementPattern()` - Sets initial movement parameters
- `UpdateEnemyMovement()` - Updates position each frame based on type-specific behavior

### Collision Detection

Simplified collision system:
- Player bullets vs enemies
- No player damage (invincible mode)
- Enemies removed when destroyed or off-screen

### Spawning Logic

- **Initial spawn**: 0.5 seconds after entering arena
- **Respawn delay**: 1.0 seconds after last enemy dies/escapes
- **Spawn location**: Random Y position on right edge
- **Maximum enemies**: Limited by MAX_ENEMIES constant

## Troubleshooting

### Enemy not moving correctly
- Check that the enemy's attack pattern matches the main game
- Verify `UpdateEnemyMovement()` is being called each frame

### Collision not working
- Ensure bullets have proper bounds
- Verify enemy bounds are updated each frame

### Performance issues
- Reduce MAX_ENEMIES if too many spawn
- Check for memory leaks in spawn/despawn cycle

## Future Enhancements

Potential additions:
- [ ] Weapon switching to test different projectile types
- [ ] Enemy firing enabled/disabled toggle
- [ ] Slow motion mode for detailed observation
- [ ] Detailed hit feedback (damage numbers)
- [ ] Wave pattern testing (spawn multiple enemies)
- [ ] Recording/playback of test sessions
- [ ] Side-by-side comparison of multiple enemy types

## Related Files

- `src/demo/enemy_showcase.c` - Main implementation
- `src/entities/enemy_types.c` - Enemy definitions and behaviors
- `src/systems/wave_system.c` - Movement pattern system
- `include/enemy_types.h` - Enemy type definitions
- `run_enemy_test_arena.sh` - Convenient run script
