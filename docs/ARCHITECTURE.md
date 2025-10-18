# Architecture Overview

## Project Structure

The Capybara Shoot'em Up game follows a modular architecture with clear separation of concerns.

```
capybara-project/
├── include/              # Header files (interfaces)
├── src/                 # Implementation files (organized by domain)
│   ├── core/           # Core game logic (main, game)
│   ├── entities/       # Game objects (player, enemies)
│   ├── input/          # Input handling (manager, config)
│   ├── gameplay/       # Game mechanics (waves, weapons, powerups)
│   ├── rendering/      # Rendering pipeline
│   ├── physics/        # Collision and combat
│   ├── effects/        # Visual effects (explosions, projectiles)
│   ├── ui/             # User interface (menus)
│   ├── demo/           # Showcase/demo programs + shared utilities
│   ├── tools/          # Sprite generation tools
│   └── utils/          # General utilities (logging, database)
├── assets/             # Game assets
│   ├── sprites/        # Generated sprite images
│   └── audio/          # Audio files (not tracked)
├── bin/                # Executable output
├── build/              # Compilation artifacts
└── docs/               # Documentation
```

## Module Organization

### Core Modules (`src/core/`)
- **main.c**: Entry point, window initialization, main game loop
- **game.c**: Game state management, update coordination, system orchestration

### Entity Modules (`src/entities/`)
- **player_ship.c**: Player ship logic, movement, energy/shield management, HUD rendering
- **enemy_types.c**: 10 enemy types with unique behaviors, AI patterns, and visual designs

### Input Modules (`src/input/`)
- **input_manager.c**: Unified input handling for keyboard and gamepad
- **input_config.c**: Input configuration, key binding, save/load settings

### Gameplay Modules (`src/gameplay/`)
- **weapon.c**: Weapon modes (Single, Double, Spread, Rapid, Dual, Charge), heat management, bullet physics
- **powerup.c**: Powerup system with 4 types, drop mechanics, collection, and visual effects
- **wave_system.c**: Dynamic enemy wave spawning, bass-driven difficulty scaling
- **level_system.c**: Level management and progression
- **level1_waves.c**: Level 1 wave patterns and phase definitions
- **level2_waves.c**: Level 2 wave patterns and phase definitions

### Rendering Modules (`src/rendering/`)
- **renderer.c**: Main rendering pipeline, HUD, background, game over screen, level transitions

### Physics Modules (`src/physics/`)
- **collision.c**: Collision detection between all entities, damage application, powerup collection
- **combat_system.c**: Generic combat logic, damage calculation, projectile firing

### Effects Modules (`src/effects/`)
- **explosion.c**: Explosion visual effects for enemy destruction, screen shake
- **projectile_manager.c**: Projectile pooling and lifecycle management
- **projectile_types.c**: 4 projectile types (Laser, Plasma, Missile, Energy Orb) with behavior definitions

### UI Modules (`src/ui/`)
- **menu.c**: Complete menu system (main, options, high scores, credits, pause, controls configuration)

### Utility Modules (`src/utils/`)
- **logger.c**: Debug logging, collision tracking, event logging
- **database.c**: SQLite database for high scores and settings persistence
- **audio_analysis.c**: Bass detection, audio analysis, music-reactive gameplay

### Demo Programs (`src/demo/`)
- **demo_common.c**: Shared utilities for demo programs (camera init, system init, starfield)
- **enemy_showcase.c**: Interactive enemy testing arena with grid and combat modes
- **enemy_showcase_sprites.c**: Sprite-based enemy showcase
- **player_ship_showcase.c**: Player ship testing environment
- **projectile_showcase.c**: Projectile type testing
- **powerup_showcase.c**: Powerup system demonstration
- **audio_analysis_gui.c**: Audio analysis visualization tool
- **audio_analysis_cli.c**: Command-line audio analyzer

### Tools (`src/tools/`)
- **generate_enemy_sprites.c**: Generate enemy sprite images
- **generate_spaceship_sprites.c**: Generate spaceship sprite variants
- **generate_projectile_sprites.c**: Generate projectile sprite images
- **generate_player_sprite.c**: Generate player ship sprite

## Data Flow

### Initialization
```
main.c
└── InitGame (game.c)
    ├── InitPlayerShip (player_ship.c)
    ├── InitEnemyTypes (enemy_types.c)
    ├── InitWeaponSystem (weapon.c)
    ├── InitPowerupSystem (powerup.c)
    ├── InitWaveSystem (wave_system.c)
    ├── InitProjectileTypes (projectile_types.c)
    ├── InitExplosionSystem (explosion.c)
    └── InitAudioAnalysis (audio_analysis.c)
```

### Game Loop
```
main.c (60 FPS)
└── UpdateGame (game.c)
    ├── UpdateAudioAnalysis (audio_analysis.c)
    ├── UpdatePlayerShip (player_ship.c)
    │   ├── Handle input
    │   ├── Update physics
    │   ├── Update energy/shield
    │   └── Update energy mode
    ├── UpdateWeapon (weapon.c)
    │   ├── Handle shooting
    │   ├── Update heat
    │   └── Update bullets
    ├── UpdateWaveSystem (wave_system.c)
    │   ├── Spawn enemies based on audio
    │   └── Progress through phases
    ├── UpdateEnemyBehaviors (enemy_types.c)
    │   ├── Movement patterns
    │   ├── AI logic
    │   └── Firing patterns
    ├── UpdatePowerups (powerup.c)
    │   ├── Movement and lifetime
    │   └── Magnet attraction
    ├── CheckCollisions (collision.c)
    │   ├── Player vs Enemy
    │   ├── Bullet vs Enemy
    │   ├── Enemy Projectile vs Player
    │   └── Player vs Powerup
    └── UpdateExplosions (explosion.c)
└── DrawGame (renderer.c)
    ├── DrawBackground
    ├── DrawEnemies (enemy_types.c)
    ├── DrawBullets (weapon.c)
    ├── DrawPowerups (powerup.c)
    ├── DrawExplosions (explosion.c)
    ├── DrawPlayerShip (player_ship.c)
    └── DrawHUD
        ├── Top HUD (wave, progress, time, enemy count)
        └── Bottom HUD (ship status, score, weapon info)
```

## Key Design Patterns

### 1. Module Pattern with Domain Organization
Each module exposes a clean interface through header files while hiding implementation details. Modules are organized by domain (input, gameplay, rendering, physics, effects, ui) for clear separation of concerns.

**Example:**
```c
// powerup.h (interface)
void InitPowerupSystem(PowerupSystem* system);
void UpdatePowerups(PowerupSystem* system, PlayerShip* player, float dt);
void DrawPowerups(const PowerupSystem* system);

// powerup.c (implementation - in src/gameplay/)
static void UpdatePowerupPhysics(Powerup* p, float dt);
static void CheckPowerupMagnet(Powerup* p, const PlayerShip* ship);
```

### 2. Unified Input System
All input handling goes through a single `InputManager` system with configurable key bindings:
- No duplicate input detection code
- Consistent behavior across keyboard and gamepad
- Configurable bindings saved to disk
- Support for menu navigation and gameplay actions

### 3. Entity-Component Pattern
Game objects share common properties:
- Position (Vector2)
- Bounds (Rectangle)
- Active state
- Health/durability
- Visual properties

### 4. State Management
Central `Game` struct contains all game state, passed to functions that need it:
```c
typedef struct Game {
    PlayerShip* playerShip;
    EnemyEx* enemies;
    PowerupSystem* powerupSystem;
    WaveSystem* waveSystem;
    AudioAnalysisData* audioData;
    // ... etc
} Game;
```

### 5. Object Pooling
Fixed-size arrays for bullets, enemies, projectiles, powerups with active/inactive states:
- **Bullets**: 50 max
- **Enemies**: Variable per wave
- **Powerups**: 20 max active
- **Explosions**: 50 max

### 6. Data-Driven Design
Enemy types, projectile types, and wave patterns are defined as data structures:
```c
static const EnemyTypeData ENEMY_DATA[] = {
    [ENEMY_GRUNT] = {
        .health = 20,
        .speed = 150,
        .color = LIME,
        // ... etc
    },
    // ...
};
```

## Memory Management

### Static Allocation
- Constants and configuration in `constants.h`
- Enemy type data tables
- Projectile type definitions

### Dynamic Allocation
- Game struct and subsystems allocated at initialization
- Arrays allocated once with fixed sizes
- No runtime memory allocation during gameplay

### Cleanup
Proper cleanup in `CleanupGame()` frees all allocated resources.

## Collision System

Uses axis-aligned bounding box (AABB) collision detection with multiple collision contexts:

### Collision Types
1. **Player Bullets vs Enemies**: Damage enemies, track kills
2. **Enemy Projectiles vs Player**: Damage player ship (shield → hull)
3. **Player vs Enemies**: Collision damage to player
4. **Player vs Powerups**: Collection with instant effect
5. **Devastating Attack vs Enemies**: Special high-damage player attack

### Damage System
- Base damage per weapon mode
- Weapon powerup multipliers (1.0x, 1.5x, 2.0x, 2.5x)
- Offensive mode bonus (2.0x when energy full)
- Per-bullet damage displayed in HUD

## Rendering Pipeline

### Order of Operations
1. **BeginDrawing()**
2. **ClearBackground(BLACK)**
3. **Draw Play Zone** (30px to 500px)
   - Background stars
   - Enemy projectiles
   - Enemies with visual effects
   - Player bullets/projectiles
   - Powerups with glow effects
   - Explosions
   - Player ship with shield/energy effects
4. **Draw Top HUD** (0px to 30px)
   - Phase name
   - Wave progress bar
   - Game time
   - Enemy count
5. **Draw Bottom HUD** (500px to 600px)
   - Ship status (hull, shield, energy, mode)
   - Score
   - Weapon info (mode, power level, damage)
   - Controls reference
6. **Draw Overlays**
   - Game over screen
   - Pause screen
   - Debug info
7. **EndDrawing()**

## Audio-Driven Gameplay

### Bass Detection System
- Real-time audio analysis with BASS library
- Bass intensity detection (Low, Medium, High)
- Configurable thresholds and smoothing
- Wave spawning synchronized to bass patterns

### Music-Reactive Features
- Enemy spawn rates scale with bass intensity
- Difficulty progression through 9 phases
- Boss appears at musical climax (427 seconds)
- Visual effects synchronized to audio

## Configuration System

Key constants defined in `constants.h`:

### Screen Layout
```c
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600
#define TOP_HUD_HEIGHT 30
#define BOTTOM_HUD_HEIGHT 100
#define PLAY_ZONE_TOP 30
#define PLAY_ZONE_BOTTOM 500
#define PLAY_ZONE_HEIGHT 470
```

### Object Limits
```c
#define MAX_BULLETS 50
#define MAX_ENEMIES 50
#define MAX_PROJECTILES 100
#define MAX_POWERUPS 20
#define MAX_EXPLOSIONS 50
```

### Weapon Parameters
```c
#define WEAPON_HEAT_PER_SHOT 8.0f
#define WEAPON_COOLDOWN_RATE 30.0f
#define WEAPON_OVERHEAT_TIME 3.0f
#define WEAPON_FIRE_RATE 0.12f
```

### Powerup Parameters
```c
#define POWERUP_MAX_LIFETIME 15.0f
#define POWERUP_MAGNET_RANGE 80.0f
#define POWERUP_MAGNET_SPEED 200.0f
#define ENERGY_RECOVERY_PERCENT 0.20f
#define HULL_RECOVERY_PERCENT 0.20f
```

## Extension Points

The modular architecture makes it easy to extend:

### Add New Enemy Types
1. Add enum value to `EnemyType` in `enemy_types.h`
2. Add data entry to `ENEMY_DATA` array in `enemy_types.c`
3. Implement behavior in `UpdateEnemyBehavior()`
4. Add visual design in `DrawEnemyEx()`

### Add New Weapon Modes
1. Add enum value to `WeaponMode` in `weapon.h`
2. Implement firing pattern in `ShootBulletsForMode()`
3. Add HUD display in `DrawShipHUD()`

### Add New Powerup Types
1. Add enum value to `PowerupType` in `powerup.h`
2. Add drop rates to `DROP_RATES` array
3. Implement effect in `ApplyPowerupEffect()`
4. Add visual design in `DrawPowerup()`

### Add New Projectile Types
1. Add enum value to `ProjectileType` in `projectile_types.h`
2. Add data entry to projectile data array
3. Implement movement in `UpdateProjectiles()`
4. Add visual design in `DrawProjectiles()`

## Documentation Structure

Comprehensive documentation in `docs/`:
- **WEAPONS_AND_DAMAGE.md**: Complete weapon and damage system
- **POWERUPS.md**: Complete powerup system
- **ENEMIES_AND_SPRITES.md**: All enemy types and behaviors
- **GAMEPLAY.md**: Player guide and controls
- **FLIGHT_PLAN.md**: Wave system and phase progression
- **BUILD.md**: Build instructions and troubleshooting
- **API.md**: Function reference for all modules
- **DEBUG_FEATURES.md**: Testing and debug tools

## Performance Considerations

### Optimization Strategies
- Object pooling eliminates runtime allocation
- Fixed update rate (60 FPS) for consistent physics
- Efficient AABB collision detection
- Sprite caching and reuse
- Minimal draw calls per frame

### Target Performance
- **Frame Rate**: 60 FPS constant
- **Memory Usage**: ~10-20 MB
- **CPU Usage**: < 10% on modern hardware
- **Startup Time**: < 1 second

## Testing Infrastructure

### Showcase Programs
- Interactive testing environments for each major system
- Real-time parameter adjustment
- Visual debugging tools
- Performance monitoring

### Debug Features
- Toggle-able invulnerability
- Phase selection
- Pause functionality
- Hitbox visualization
- Collision logging
- FPS counter

## Recent Refactoring (v0.3.0 - October 2025)

The codebase underwent major reorganization to eliminate code duplication and improve maintainability:

### Code Quality Improvements
- **Eliminated ~350+ lines** of duplicate code
- **Unified input system**: Single `InputManager` with no duplication
- **Demo utilities**: Created `demo_common.{c,h}` with 9 reusable functions
- **Zero breaking changes**: All refactoring was internal reorganization

### Directory Structure Evolution
**Before:** Mixed `src/systems/` with 16 files of varying abstraction levels  
**After:** 8 domain-specific directories (input, gameplay, rendering, physics, effects, ui, etc.)

This refactoring provides:
- Clear separation of concerns by domain
- Easier navigation and maintenance
- Consistent patterns for future development
- Foundation for further modularization

See `CHANGELOG.md` for detailed changes.

## Future Architecture Considerations

### Potential Improvements
1. **ECS Architecture**: Full Entity-Component-System for better modularity
2. **Scene System**: Menu, gameplay, game over as separate scenes
3. **Resource Manager**: Centralized asset loading and caching
4. **Network Play**: Multiplayer support with client-server model
5. **Scripting**: Lua integration for wave patterns and enemy AI
6. **Particle System**: More sophisticated visual effects
7. **Sound Engine**: Full sound effect and music management