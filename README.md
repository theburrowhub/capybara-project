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
- [Level System](docs/LEVEL_SYSTEM.md) - Multiple levels with unique wave plans and music
- [Enemies and Sprites](docs/ENEMIES_AND_SPRITES.md) - All 10 enemy types and sprite designs
- [Weapons and Damage](docs/WEAPONS_AND_DAMAGE.md) - Comprehensive weapon systems, damage mechanics, and projectile types
- [Powerup System](docs/POWERUPS.md) - 4 powerup types with drop mechanics
- [Flight Plan](docs/FLIGHT_PLAN.md) - Level 1 wave system and phase progression

### Development
- [Debug Features](docs/DEBUG_FEATURES.md) - Testing tools and debug modes
- [TODO List](docs/TODO.md) - Planned features and improvements

### CI/CD & Releases
- [Complete CI/CD Guide](docs/CI_CD_COMPLETE_GUIDE.md) - Version management, releases, and automation

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

## 🚀 Creating Releases

Multi-platform releases (Linux, macOS, Windows) are automated via GitHub Actions.

### Quick Release

```bash
# 1. Install commitizen
make cz-install

# 2. Make commits (conventional format)
make cz-commit

# 3. Create release
make cz-alpha     # Alpha: v1.0.0-alpha.0
# or
make cz-beta      # Beta: v1.0.0-beta.0
# or
make cz-release   # Stable: v1.0.0

# 4. Push to trigger CI/CD
git push origin main
git push --tags

# → Builds for Linux, macOS, Windows (~15 min)
# → Check: https://github.com/YOUR_USERNAME/capybara-project/releases
```

### Release Commands

| Command | Purpose | Example |
|---------|---------|---------|
| `make cz-alpha` | Create alpha release | v1.0.0-alpha.0 |
| `make cz-beta` | Create beta release | v1.0.0-beta.0 |
| `make cz-rc` | Create release candidate | v1.0.0-rc.0 |
| `make cz-release` | Create stable release | v1.0.0 |

**Complete guide**: [docs/CI_CD_COMPLETE_GUIDE.md](docs/CI_CD_COMPLETE_GUIDE.md)

## Key Features

- 🎵 **Multiple Levels**: 2 levels with unique music tracks and wave plans (extensible)
- 🎮 **Dynamic Gameplay**: Speed increases every 20 seconds
- 🔫 **Heat Management**: Weapon overheats with continuous fire
- 👾 **10 Enemy Types**: Each with unique behaviors and attack patterns
- 🌊 **Wave System**: Synchronized with music bass events for dynamic difficulty
- ⭐ **Powerup System**: 4 types (energy, shield, hull, weapon) with smart drop mechanics
- 📊 **Score System**: Points for destroying enemies
- 🐛 **Debug Mode**: Invulnerability, phase selection, and hitbox visualization

## Controls

### In-Game
| Action | Keys |
|--------|------|
| Move | WASD / Arrow Keys |
| Shoot | Left Click (hold for continuous fire) |
| Pause | P |
| Debug | H (show hitboxes) |
| Restart | R (when game over) |

### Menu
| Action | Keys |
|--------|------|
| Navigate | UP/DOWN or W/S |
| Select | ENTER or SPACE |
| Adjust | LEFT/RIGHT or A/D |
| Back | ESC or BACKSPACE |
| Quick Fullscreen | F11 or ALT+ENTER |

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

### High Score System
**Complete high score system with personalization!**

Features:
- **Automatically populated** with 40 legendary game developers on first run
- **Name input dialog** when you achieve a high score - personalize your entry!
- **Navigate difficulties** with LEFT/RIGHT arrows to see all leaderboards
- 10 scores per difficulty level (EASY, NORMAL, HARD, INSANE)
- Based on gaming industry legends (Miyamoto, Kojima, Carmack, etc.)
- Optional reset tool: `make force_populate_highscores`
- See [High Scores Documentation](docs/HIGH_SCORES_PRESETS.md) | [Name Input Dialog](docs/NAME_INPUT_DIALOG.md)

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