# Documentation Index

Welcome to the Capybara Shoot'em Up documentation!

## 📖 Documentation Overview

### Getting Started
- **[BUILD.md](BUILD.md)** - Installation, compilation, and platform-specific setup
- **[GAMEPLAY.md](GAMEPLAY.md)** - How to play, controls, and game mechanics

### Game Content
- **[ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md)** - All 10 enemy types, behaviors, and sprite designs
- **[WEAPONS_AND_PROJECTILES.md](WEAPONS_AND_PROJECTILES.md)** - Weapon systems, projectile types, and combat mechanics
- **[FLIGHT_PLAN.md](FLIGHT_PLAN.md)** - Wave system design and phase progression

### Technical Documentation
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Project structure, design patterns, and module organization
- **[API.md](API.md)** - Complete API reference for all modules and functions

### Development
- **[DEBUG_FEATURES.md](DEBUG_FEATURES.md)** - Testing tools, debug modes, and development utilities
- **[TODO.md](TODO.md)** - Roadmap and planned features

## 🚀 Quick Links

### For Players
1. Start with [BUILD.md](BUILD.md) to compile the game
2. Read [GAMEPLAY.md](GAMEPLAY.md) to learn the controls
3. Check [ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md) to understand enemy types

### For Developers
1. Review [ARCHITECTURE.md](ARCHITECTURE.md) for project structure
2. Consult [API.md](API.md) for function references
3. Use [DEBUG_FEATURES.md](DEBUG_FEATURES.md) for testing tools
4. See [TODO.md](TODO.md) for contribution opportunities

## 📁 Project Structure

```
capybara-project/
├── docs/           # This documentation
├── src/            # Source code
│   ├── core/      # Main game loop
│   ├── entities/  # Game objects
│   ├── systems/   # Game systems
│   └── utils/     # Utilities
├── include/        # Header files
├── assets/         # Sprites and resources
├── bin/           # Compiled executable
└── build/         # Build artifacts
```

## 🎮 Game Features

- **10 Enemy Types**: Each with unique behaviors and attack patterns
- **4 Projectile Types**: Laser, Plasma, Missile, and Energy Orb
- **17 Wave Phases**: Progressive difficulty with themed enemy encounters
- **Heat Management**: Weapon overheating system
- **Dynamic Difficulty**: Speed increases over time
- **Sprite System**: Both abstract and spaceship designs
- **Debug Mode**: Invulnerability, phase selection, pause, and hitbox visualization

## 🛠️ Building the Game

```bash
# Quick build
make

# Run the game
./bin/shootemup

# View enemy showcase
make showcase

# View sprite showcase
make showcase_sprites
```

## 📝 Contributing

The modular architecture makes it easy to extend the game. Check the [TODO.md](TODO.md) for planned features and the [API.md](API.md) for implementation details.

---

Return to [Main README](../README.md)
