# Enemy Test Arena

An interactive testing environment for debugging and analyzing enemy behavior in the game.

## Overview

The Enemy Test Arena is a two-mode showcase system that allows you to:
1. **Select** any enemy type from a visual grid
2. **Test** the enemy in a live combat arena with an invincible player ship

## How to Run

```bash
make showcase
```

Or build and run manually:
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
- **Visual Effects**: Explosions, particle effects, screen shake (shared from main game)

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
  - **Explosions**: Same explosion effects when enemies destroyed
- **Arena differences:**
  - Player is invincible - enemy projectiles pass through (for safe testing)
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

### 7. Explosion Testing
- Verify explosion sizes match enemy sizes
- Test particle effects and colors
- Observe shockwave rings (medium/large enemies)
- Experience screen shake effects
- **Test boss multi-explosion** (9 simultaneous explosions, 3-second duration)

## Quick Testing Guide

### Essential Tests (5 minutes)

1. **Basic Functionality**
   ```bash
   ./bin/enemy_showcase
   ```
   - Click **Grunt** → Shoot → Verify small explosion appears

2. **Enemy Firing (Critical)**
   - Click **Shield** → Verify it fires **6 projectiles in 360° circle**
   - Click **Tank** → Verify it fires **3-missile spread** when paused
   - Click **Ghost** → Verify it **only fires when visible** (not when phased)

3. **Boss Explosion (Spectacular)**
   - Click **Boss** → Shoot ~150 times → Watch for **9 simultaneous explosions**
   - Should create massive chain reaction lasting 3 seconds
   - Very strong screen shake

### Full Test (15 minutes)

Test all 10 enemy types:
- Verify each enemy fires projectiles
- Verify explosion size matches enemy size
- Check that behaviors match main game

### What to Report

If something doesn't work:
```
ENEMY: [which enemy]
ISSUE: [what's wrong]
EXPECTED: [what should happen]
ACTUAL: [what happened]
```

## Implementation Details

### Shared Modules (100% Code Reuse)

The arena uses the **exact same systems** as the main game:

1. **Combat System** (`combat_system.c`)
   - `Combat_FireEnemyProjectile()` - Enemy firing logic
   - `Combat_UpdateEnemyFiring()` - Firing timer management

2. **Projectile Manager** (`projectile_manager.c`)
   - `ProjectileManager_UpdateAll()` - Update all projectiles
   - `ProjectileManager_DrawAll()` - Draw all projectiles
   - `ProjectileManager_InitAll()` - Initialize projectiles

3. **Collision System** (`collision.c`)
   - `Collision_CheckBulletEnemyGeneric()` - Bullet-enemy collisions
   - Damage calculation with resistance
   - Ghost phasing logic
   - Boss shield handling

4. **Explosion System** (`explosion.c`)
   - `InitExplosionSystem()` - Initialize explosions
   - `UpdateExplosionSystem()` - Update all explosions
   - `DrawExplosions()` - Draw all explosions
   - `CreateEnemyExplosion()` - Create explosion on enemy death

5. **Enemy Systems** (`enemy_types.c`, `wave_system.c`)
   - Enemy definitions and stats
   - Movement patterns
   - Drawing routines

6. **Weapon System** (`weapon.c`)
   - Bullet initialization
   - Bullet firing logic

**Result:** The showcase is a **thin wrapper** around the game's systems with ~500 lines, while reusing ~2000+ lines of game logic!

### Architecture

```
                    Shared Game Systems
                    ──────────────────
┌────────────────────────────────────────────────────┐
│  combat_system.c  │  projectile_manager.c          │
│  collision.c      │  explosion.c                   │
│  enemy_types.c    │  wave_system.c                 │
│  weapon.c         │  player_ship.c                 │
└────────┬──────────────────────┬────────────────────┘
         ↓                      ↓
    game.c (~300 lines)    showcase.c (~500 lines)
    Uses all systems       Uses all systems
```

---

## Troubleshooting

### No explosions appear
**Fix:** Verify `state->explosionSystem` is initialized in `InitArenaState()`

### Explosions look different from game
**Issue:** Should be identical - they use the same functions
**Fix:** Report the difference

### Performance issues with many explosions
**Check:** Should handle up to 50 simultaneous explosions
**Fix:** May need to reduce MAX_EXPLOSIONS

---

## Related Files

**Main Implementation:**
- `src/demo/enemy_showcase.c` - Arena implementation

**Shared Modules (Reused):**
- `src/systems/combat_system.c` - Enemy firing
- `src/systems/projectile_manager.c` - Projectile lifecycle
- `src/systems/collision.c` - Collision detection
- `src/systems/explosion.c` - Explosion effects
- `src/systems/wave_system.c` - Movement patterns
- `src/entities/enemy_types.c` - Enemy definitions
- `src/entities/player_ship.c` - Player ship
- `src/systems/weapon.c` - Weapon system

**Headers:**
- `include/combat_system.h`
- `include/projectile_manager.h`
- `include/collision.h`
- `include/explosion.h`

**Documentation:**
- `EXPLOSION_INTEGRATION_TEST.md` - Testing steps
- `REFACTOR_TEST_STEPS.md` - Full refactoring tests

---

## Benefits

✅ **100% code reuse** - Zero explosion code duplication  
✅ **Identical effects** - Same as main game  
✅ **Easy maintenance** - Fix once, works everywhere  
✅ **Full features** - Particles, shockwaves, screen shake  
✅ **Automatic sizing** - Explosion size matches enemy size  
✅ **Color matching** - Explosions use enemy colors  

The test arena now provides a **complete and authentic** enemy testing experience! 🎮💥