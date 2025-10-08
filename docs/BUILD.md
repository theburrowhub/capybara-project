# Build Instructions

## Prerequisites

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Raylib
brew install raylib

# Install BASS audio library (for audio-reactive features)
# Download from: https://www.un4seen.com/bass.html
# Place libbass.dylib in /usr/local/lib/

# Install build tools (if needed)
brew install gcc make pkg-config
```

### Linux (Ubuntu/Debian)
```bash
# Update package list
sudo apt update

# Install Raylib and dependencies
sudo apt install libraylib-dev
sudo apt install build-essential pkg-config

# Install BASS audio library
# Download from: https://www.un4seen.com/bass.html
# Place libbass.so in /usr/local/lib/
```

### Linux (Fedora)
```bash
# Install Raylib and dependencies
sudo dnf install raylib-devel
sudo dnf install gcc make pkg-config

# Install BASS audio library
# Download from: https://www.un4seen.com/bass.html
# Place libbass.so in /usr/local/lib/
```

### Windows (MinGW)
1. Download Raylib from https://github.com/raysan5/raylib/releases
2. Download BASS from https://www.un4seen.com/bass.html
3. Install MinGW-w64 from https://www.mingw-w64.org/
4. Add MinGW to PATH
5. Update Makefile paths as needed

---

## Building the Game

### Quick Build
```bash
# Build the main game
make

# The executable will be created in bin/shootemup
./bin/shootemup
```

### Build Options

#### Clean Build
```bash
# Remove all build artifacts
make clean

# Rebuild from scratch
make clean && make
```

#### Debug Build
```bash
# Build with debug symbols and no optimization
make debug

# Use with debugger (gdb, lldb)
lldb bin/shootemup
```

#### Release Build
```bash
# Build with optimizations (-O3)
make release
```

#### Build and Run
```bash
# Build and immediately run the game
make run

# Or use the debug script
./run_debug_game.sh
```

---

## Makefile Targets

### Main Targets

| Target | Description |
|--------|-------------|
| `all` | Default target, builds the main game |
| `clean` | Removes all build artifacts, logs, and binaries |
| `rebuild` | Clean + build |
| `run` | Build and run the main game |
| `debug` | Build with debug symbols (-g) |
| `release` | Build with optimizations (-O3) |

### Showcase Programs

Interactive demonstration and testing programs for each major system:

| Target | Description |
|--------|-------------|
| `enemy_showcase` | Interactive enemy testing arena with all 10 enemy types |
| `showcase_sprites` | Sprite-based enemy showcase |
| `player_showcase` | Player ship controls and weapon testing |
| `projectile_showcase` | Test all 4 projectile types |
| `powerup_showcase` | Powerup system demonstration and testing |

### Audio Tools

| Target | Description |
|--------|-------------|
| `audio_gui` | Audio analysis GUI with visual bass detection |
| `audio_cli` | Command-line audio analysis tool |

### Sprite Generation Tools

| Target | Description |
|--------|-------------|
| `generate_enemies` | Generate all enemy sprite images |
| `generate_spaceships` | Generate spaceship sprite variants |
| `generate_projectiles` | Generate projectile sprite images |
| `generate_player` | Generate player ship sprite |

### All-in-One Targets

| Target | Description |
|--------|-------------|
| `all_showcases` | Build all showcase programs at once |
| `all_generators` | Build all sprite generation tools |
| `everything` | Build game, showcases, tools, and generators |

---

## Project Structure

```
capybara-project/
├── src/                 # Source files (.c)
│   ├── core/           # Main game loop and state
│   ├── entities/       # Player ship, enemy types
│   ├── systems/        # Game systems (weapons, powerups, collision, etc.)
│   ├── demo/           # Showcase programs
│   ├── tools/          # Sprite generation tools
│   └── utils/          # Logging, audio analysis
├── include/            # Header files (.h)
├── assets/             # Game assets
│   ├── sprites/        # Generated sprite images
│   └── audio/          # Audio files (not tracked in git)
├── build/              # Object files (.o) - generated
├── bin/                # Executables - generated
│   ├── shootemup       # Main game
│   ├── enemy_showcase  # Enemy testing
│   ├── powerup_showcase # Powerup testing
│   └── ...            # Other programs
└── docs/               # Documentation
```

---

## Showcase Programs

### Enemy Showcase
Interactive testing environment for all enemy types.

```bash
make enemy_showcase
./bin/enemy_showcase
```

**Controls:**
- Arrow Keys / WASD: Move player
- Space: Shoot
- Left/Right Arrows: Select enemy type
- S: Spawn selected enemy
- C: Clear all enemies
- R: Toggle auto-spawn
- H: Toggle hitboxes
- I: Toggle invulnerability
- P: Pause

### Player Ship Showcase
Test player ship controls, weapons, and energy modes.

```bash
make player_showcase
./bin/player_ship_showcase
```

**Controls:**
- Arrow Keys / WASD: Move
- Space: Shoot
- 1-6: Select weapon mode
- R: Cycle energy mode (Offensive/Defensive)
- +/-: Adjust weapon power level

### Projectile Showcase
Test all projectile types and enemy firing patterns.

```bash
make projectile_showcase
./bin/projectile_showcase
```

**Controls:**
- Arrow Keys: Move
- Left Mouse: Fire single projectile
- Right Mouse: Fire enemy pattern
- 1-4: Select projectile type
- Left/Right: Select enemy type
- A: Toggle auto-fire
- T: Toggle trails
- C: Clear all projectiles

### Powerup Showcase
Test powerup system, collection, and drop mechanics.

```bash
make powerup_showcase
./bin/powerup_showcase
```

**Controls:**
- Arrow Keys: Move ship
- Space: Spawn random powerup
- 1-4: Spawn specific powerup (Energy/Shield/Hull/Weapon)
- A: Toggle auto-spawn
- R: Reset player stats
- I: Toggle instructions

---

## Sprite Generation

Generate sprite images for enemies, projectiles, and player:

```bash
# Generate all enemy sprites
make generate_enemies
./bin/generate_enemy_sprites

# Generate projectile sprites
make generate_projectiles
./bin/generate_projectile_sprites

# Generate player ship sprite
make generate_player
./bin/generate_player_sprite

# Generate spaceship variants
make generate_spaceships
./bin/generate_spaceship_sprites
```

Generated sprites are saved to `assets/sprites/`.

---

## Audio Tools

### Audio Analysis GUI
Visual tool for analyzing audio and configuring bass detection.

```bash
make audio_gui
./bin/audio_analysis_gui assets/audio/your_music.mp3
```

**Features:**
- Real-time bass waveform
- FFT spectrum analyzer
- Bass threshold configuration
- Visual intensity indicators

### Audio Analysis CLI
Command-line audio analysis tool.

```bash
make audio_cli
./bin/audio_analysis_cli assets/audio/your_music.mp3
```

---

## Troubleshooting

### Raylib Not Found
If you get "raylib.h not found":
```bash
# Check if Raylib is installed
pkg-config --libs raylib

# If not found, reinstall Raylib
brew reinstall raylib  # macOS
sudo apt install --reinstall libraylib-dev  # Linux
```

### BASS Library Not Found
If you get BASS-related linking errors:
```bash
# Check if BASS library is present
ls -l /usr/local/lib/libbass*

# Download and install BASS from https://www.un4seen.com/bass.html
# macOS: Place libbass.dylib in /usr/local/lib/
# Linux: Place libbass.so in /usr/local/lib/
```

### Permission Denied
```bash
# Make scripts executable
chmod +x run_debug_game.sh
chmod +x bin/*
```

### Build Errors
```bash
# Clean and rebuild
make clean
make

# Check for missing dependencies
pkg-config --cflags --libs raylib

# Verify all source files are present
ls -R src/
```

### Audio File Not Found
The game expects audio in `assets/audio/` (not tracked in git):
```bash
# Create audio directory
mkdir -p assets/audio

# Place your music file
cp /path/to/music.mp3 assets/audio/

# Update audio path in game.c if needed
```

---

## Development Workflow

### Standard Development Cycle
1. **Make changes** to source files
2. **Build** with `make`
3. **Test** with `./bin/shootemup`
4. **Debug** if needed with `make debug` and debugger
5. **Clean** periodically with `make clean`

### Testing Workflow
1. **Build showcases** with `make all_showcases`
2. **Test specific system** with appropriate showcase
3. **Iterate** on code changes
4. **Verify** in main game

### Adding New Features
1. **Create** new .c and .h files
2. **Update Makefile** source variables
3. **Build** with `make rebuild`
4. **Test** with debug build
5. **Document** in appropriate docs file

---

## Adding New Files

When adding new source files:

### Add to Entity Module
```makefile
# In Makefile
ENTITY_SRCS = $(SRC_DIR)/entities/player_ship.c \
              $(SRC_DIR)/entities/enemy_types.c \
              $(SRC_DIR)/entities/your_new_entity.c
```

### Add to System Module
```makefile
# In Makefile
SYSTEM_SRCS = $(SRC_DIR)/systems/weapon.c \
              $(SRC_DIR)/systems/collision.c \
              $(SRC_DIR)/systems/your_new_system.c \
              ...
```

### Add to Utility Module
```makefile
# In Makefile
UTIL_SRCS = $(SRC_DIR)/utils/logger.c \
            $(SRC_DIR)/utils/your_new_util.c
```

Then rebuild:
```bash
make rebuild
```

---

## Platform-Specific Notes

### macOS
- Uses Homebrew's Raylib installation
- Frameworks: OpenGL, Cocoa, IOKit, CoreVideo
- Debug symbols in .dSYM format
- BASS library: `libbass.dylib`

**Apple Silicon (M1/M2/M3):**
```bash
# May need to specify architecture
arch -arm64 make
```

### Linux
- May need additional X11 libraries:
  ```bash
  sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
  ```
- Uses system package manager for Raylib
- BASS library: `libbass.so`
- Core dumps enabled with `ulimit -c unlimited`

### Windows
- Consider using MSYS2 or WSL for Unix-like environment
- May need to adjust paths in Makefile
- Use `.exe` extension for executable
- BASS library: `bass.dll` (place in same directory as executable)

---

## Compiler Flags

### Debug Build Flags
```
-g              # Debug symbols
-O0             # No optimization
-Wall           # All warnings
-Wextra         # Extra warnings
-std=c99        # C99 standard
```

### Release Build Flags
```
-O3             # Maximum optimization
-Wall           # All warnings
-Wextra         # Extra warnings
-std=c99        # C99 standard
-DNDEBUG        # Disable asserts
```

---

## Build Performance

### Compilation Time
- **Clean build**: ~5-10 seconds
- **Incremental build**: < 1 second
- **Full rebuild**: ~10-15 seconds

### Optimization Tips
```bash
# Use parallel compilation (if supported)
make -j4

# Build only what you need
make shootemup  # Just the game
make enemy_showcase  # Just one showcase
```

---

## Continuous Integration

### Automated Build Check
```bash
# Check if everything builds
make clean
make everything

# Run quick tests (if you add test targets)
make test
```

---

## Dependencies Summary

### Required
- **Raylib**: Graphics and input (v4.0+)
- **GCC/Clang**: C compiler
- **Make**: Build automation
- **pkg-config**: Dependency management

### Optional
- **BASS Audio Library**: Audio analysis and music-reactive features (v2.4+)
- **LLDB/GDB**: Debugging

### Assets (Not Included)
- **Music files**: Place in `assets/audio/` (MP3, OGG, WAV)
- The game generates sprites procedurally, so no sprite files are required

---

## Getting Help

If you encounter build issues:

1. **Check documentation**: Review this BUILD.md and ARCHITECTURE.md
2. **Clean build**: Run `make clean && make`
3. **Verify dependencies**: Ensure Raylib and BASS are installed
4. **Check logs**: Look for error messages in terminal output
5. **Test showcases**: Build and run showcases to isolate issues
6. **Platform-specific**: Check platform notes above

---

## Next Steps

After successful build:

1. **Read [GAMEPLAY.md](GAMEPLAY.md)** to learn controls
2. **Try showcases** to explore each system
3. **Review [ARCHITECTURE.md](ARCHITECTURE.md)** to understand code structure
4. **Check [API.md](API.md)** for function references
5. **See [TODO.md](TODO.md)** for contribution opportunities