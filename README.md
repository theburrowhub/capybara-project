# Shoot'em Up Game - Capybara Project

A side-scrolling shoot'em up game built with Raylib in C, featuring dynamic difficulty scaling, weapon heat management, and procedurally generated obstacles.

## 📚 Documentation

Complete documentation is available in the [`docs/`](docs/INDEX.md) folder:

### Core Documentation
- [Build Instructions](docs/BUILD.md) - How to compile and run the game
- [Gameplay Guide](docs/GAMEPLAY.md) - Controls, mechanics, and strategies
- [Architecture Overview](docs/ARCHITECTURE.md) - Project structure and design
- [API Reference](docs/API.md) - Module interfaces and functions

### Game Content
- [Enemies and Sprites](docs/ENEMIES_AND_SPRITES.md) - All 10 enemy types and sprite designs
- [Weapons and Projectiles](docs/WEAPONS_AND_PROJECTILES.md) - Weapon system and projectile types
- [Flight Plan](docs/FLIGHT_PLAN.md) - Wave system and phase progression

### Development
- [Debug Features](docs/DEBUG_FEATURES.md) - Testing tools and debug modes
- [TODO List](docs/TODO.md) - Planned features and improvements

## Quick Start

```bash
# Clone the repository
git clone <repository-url>
cd capybara-project

# Build the game
make

# Run the game
make run

# For advanced debugging (invulnerability, phase selection, etc.)
./run_debug_game.sh --help
```

## Key Features

- 🎮 **Dynamic Gameplay**: Speed increases every 20 seconds
- 🔫 **Heat Management**: Weapon overheats with continuous fire
- 👾 **10 Enemy Types**: Each with unique behaviors and attack patterns
- 🌊 **Wave System**: 17 phases with progressive difficulty
- 📊 **Score System**: Points for destroying enemies
- 🐛 **Debug Mode**: Invulnerability, phase selection, and hitbox visualization

## Controls

| Action | Keys |
|--------|------|
| Move | WASD / Arrow Keys |
| Shoot | Left Click (hold for continuous fire) |
| Pause | P |
| Debug | H (show hitboxes) |
| Restart | R (when game over) |
| Exit | ESC |

## Requirements

- C compiler (GCC/Clang)
- [Raylib](https://www.raylib.com/) graphics library
- Make build tool
- pkg-config

### macOS Installation
```bash
brew install raylib
```

### Linux Installation
```bash
# Ubuntu/Debian
sudo apt install libraylib-dev

# Fedora
sudo dnf install raylib-devel
```

## Demo & Testing Tools

### Enemy Showcase
Interactive showcase for testing enemy behavior:
```bash
make showcase
```
- Click any enemy to test it in a combat arena
- Invincible player ship for safe testing
- Enemies use their standard attack patterns
- Continuous spawning for behavior observation
- See [Enemy Test Arena Documentation](docs/ENEMY_TEST_ARENA.md)

### Audio Analysis Tools
```bash
make run_audio_gui   # Audio analysis GUI
make run_audio_cli   # Audio analysis CLI
```

## Debug Features

The game includes several debug features for testing and development:

- **Invulnerability Mode**: Set `DEBUG_INVULNERABILITY` to `true` in `constants.h`
- **Start at Any Phase**: Set `DEBUG_START_PHASE` (1-17) in `constants.h`
- **Pause Function**: Press P during gameplay
- **Hitbox Visualization**: Press H to show collision boxes

See [Debug Features Documentation](docs/DEBUG_FEATURES.md) for detailed usage.

## Project Structure

```
capybara-project/
├── src/        # Source code (.c files)
├── include/    # Headers (.h files)
├── docs/       # Documentation
├── assets/     # Sprites and graphics
├── bin/        # Executable output
└── build/      # Compilation artifacts
```

## Contributing

The modular architecture makes it easy to extend:
- See [Architecture Overview](docs/ARCHITECTURE.md) for design details
- Check [API Reference](docs/API.md) for module interfaces
- Review [TODO List](docs/TODO.md) for planned features

## License

[Add your license here]