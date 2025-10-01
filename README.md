# Shoot'em Up Game - Capybara Project

A side-scrolling shoot'em up game built with Raylib in C, featuring dynamic difficulty scaling, weapon heat management, and procedurally generated obstacles.

## Project Structure

The project has been organized into a modular architecture for better maintainability:

```
capybara-project/
├── include/              # Header files
│   ├── types.h          # Core data structures
│   ├── constants.h      # Game constants
│   ├── player.h         # Player module header
│   ├── enemy.h          # Enemy module header
│   ├── obstacle.h       # Obstacle module header
│   ├── weapon.h         # Weapon/bullet module header
│   ├── collision.h      # Collision system header
│   ├── game.h           # Game state management header
│   ├── renderer.h       # Rendering system header
│   └── utils.h          # Utility functions header
│
├── src/                 # Source files
│   ├── core/           # Core game logic
│   │   ├── main.c      # Entry point
│   │   └── game.c      # Game state management
│   │
│   ├── entities/       # Game entities
│   │   ├── player.c    # Player logic
│   │   ├── enemy.c     # Enemy logic
│   │   └── obstacle.c  # Obstacle logic
│   │
│   ├── systems/        # Game systems
│   │   ├── weapon.c    # Weapon and bullet management
│   │   ├── collision.c # Collision detection
│   │   └── renderer.c  # Rendering system
│   │
│   └── utils/          # Utilities
│       └── logger.c    # Logging system
│
├── bin/                # Executable output directory
│   └── shootemup       # Game executable (generated)
├── build/              # Build artifacts (generated)
├── Makefile           # Build configuration
└── collision_log.txt  # Collision debug log (generated)
```

## Features

- **Modular Architecture**: Clean separation of concerns with distinct modules for each game component
- **Dynamic Difficulty**: Speed increases every 20 seconds
- **Weapon Heat System**: Prevents spam shooting with overheat mechanics
- **Enemy Variety**: Enemies with random movement patterns
- **Obstacle Types**: Destructible and indestructible obstacles
- **Debug Features**: Collision logging and hitbox visualization
- **Score System**: Points for destroying enemies and obstacles

## Building

### Prerequisites

- GCC compiler
- Raylib library (installed via Homebrew on macOS: `brew install raylib`)
- Make build tool

### Compilation

```bash
# Build the game (executable will be in bin/shootemup)
make

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Build with debug symbols
make debug

# Build with optimizations
make release

# Build and run
make run
```

The executable will be generated in the `bin/` directory as `bin/shootemup`.

## Controls

- **WASD/Arrow Keys**: Move player
- **Space/Left Click**: Shoot
- **H**: Show hitboxes (debug)
- **R**: Restart when game over
- **ESC**: Exit game

## Game Mechanics

### Player
- 3 health points
- Limited to left half of screen
- Loses health on enemy collision
- Instant death on obstacle collision

### Weapon System
- Heat builds up with continuous firing
- Automatic cooldown when not shooting
- 3-second penalty cooldown on overheat
- Visual heat indicator with color coding

### Enemies
- Spawn from the right side
- Random sizes and movement patterns
- Worth 100 points when destroyed
- Various AI behaviors (zigzag, dive, smooth movement)

### Obstacles
- **Destructible (Orange)**: Can be destroyed with 2 hits, worth 50 points
- **Indestructible (Gray)**: Cannot be destroyed, must be avoided
- Randomly generated sizes and positions

### Difficulty Progression
- Speed increases every 20 seconds
- Affects enemy spawn rate, movement speed, and scroll speed
- Visual warning before speed increases

## Debug Features

The game includes a comprehensive logging system that tracks:
- Collision events with detailed position data
- Player movement (sampled)
- Enemy and obstacle spawning
- Weapon overheat events

Logs are written to `collision_log.txt` for post-game analysis.

## Module Responsibilities

- **Core**: Main game loop and state management
- **Entities**: Individual game object logic (player, enemies, obstacles)
- **Systems**: Game-wide systems (collision, rendering, weapons)
- **Utils**: Helper functions and logging

## Development

The modular structure makes it easy to:
- Add new enemy types
- Implement new weapons
- Create additional obstacle varieties
- Extend the collision system
- Add new visual effects

Each module has clear interfaces defined in the header files, promoting loose coupling and high cohesion.