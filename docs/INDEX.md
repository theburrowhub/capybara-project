# Documentation Index

Welcome to the Capybara Shoot'em Up documentation!

## 📖 Documentation Overview

### Getting Started
- **[USER MANUAL (PDF)](USER_MANUAL.pdf)** - 📖 Complete illustrated player guide (compile with `make manual`)
- **[BUILD.md](BUILD.md)** - Installation, compilation, and platform-specific setup
- **[GAMEPLAY.md](GAMEPLAY.md)** - How to play, controls, and game mechanics

### Game Content
- **[LEVEL_SYSTEM.md](LEVEL_SYSTEM.md)** - 🎵 Multiple levels with unique music and wave plans
- **[LEVEL1_PHASES.md](LEVEL1_PHASES.md)** - 🎓 Level 1 complete phase guide and design
- **[LEVEL2_PHASES.md](LEVEL2_PHASES.md)** - 🔥 Level 2 complete phase guide and design
- **[ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md)** - All 10 enemy types, behaviors, and sprite designs
- **[WEAPONS_AND_DAMAGE.md](WEAPONS_AND_DAMAGE.md)** - Comprehensive weapon systems, damage mechanics, projectile types, and combat
- **[POWERUPS.md](POWERUPS.md)** - ⭐ Complete powerup system with types, drop mechanics, visual design, and integration
- **[PLAYER_SHIP.md](PLAYER_SHIP.md)** - Player ship design, systems, and capabilities
- **[ENERGY_MODE_SYSTEM.md](ENERGY_MODE_SYSTEM.md)** - Offensive/Defensive energy modes and special abilities

### Technical Documentation
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Project structure, design patterns, and module organization
- **[API.md](API.md)** - Complete API reference for all modules and functions
- **[DATABASE_SCHEMA.md](DATABASE_SCHEMA.md)** - 💾 SQLite database schema, settings, and high scores
- **[HIGH_SCORES_PRESETS.md](HIGH_SCORES_PRESETS.md)** - 🏆 Legendary developer high score presets
- **[HIGH_SCORES_UI.md](HIGH_SCORES_UI.md)** - 🎮 High scores menu interface and navigation
- **[NAME_INPUT_DIALOG.md](NAME_INPUT_DIALOG.md)** - 💬 Name input dialog for high scores

### Development & Tools
- **[ENEMY_TEST_ARENA.md](ENEMY_TEST_ARENA.md)** - Interactive enemy testing showcase
- **[DEBUG_FEATURES.md](DEBUG_FEATURES.md)** - Testing tools, debug modes, and development utilities
- **[AUDIO_ANALYZER.md](AUDIO_ANALYZER.md)** - Audio analyzer with bass detection and configuration
- **[HIGH_SCORES_PRESETS.md](HIGH_SCORES_PRESETS.md)** - Tool to populate database with legendary developer scores
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

- **🎵 Multiple Levels**: 2 levels (extensible) with unique music tracks and wave plans
- **Bass-Driven Gameplay**: Enemy waves sync with music bass patterns
- **10 Enemy Types**: Each with unique behaviors and attack patterns
- **4 Projectile Types**: Laser, Plasma, Missile, and Energy Orb
- **⭐ Powerup System**: 4 types (energy, shield, hull, weapon) with smart drop mechanics
- **🏆 High Score System**: SQLite database with top 10 per difficulty + legendary developer presets
- **⚙️ Persistent Settings**: Video options (resolution, fullscreen, vsync) and audio saved to database
- **Music-Reactive Spawning**: Difficulty adapts to audio intensity in real-time
- **Epic Boss Battles**: Strategic boss encounters at musical climaxes
- **Dynamic Difficulty**: Progressive scaling synchronized with audio
- **Sprite System**: Both abstract and spaceship designs
- **Debug Mode**: Invulnerability, phase selection, pause, and hitbox visualization

## 🛠️ Building the Game

```bash
# Quick build
make

# Run the game
./bin/shootemup

# Populate high scores with legendary developers
make run_populate_highscores

# View enemy showcase
make showcase

# View sprite showcase
make showcase_sprites
```

## 📝 Contributing

The modular architecture makes it easy to extend the game. Check the [TODO.md](TODO.md) for planned features and the [API.md](API.md) for implementation details.

---

Return to [Main README](../README.md)
