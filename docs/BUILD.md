# Build Instructions

This project uses **CMake** as the primary build system, providing cross-platform support for Linux, macOS, and Windows. A traditional Makefile is also available for backward compatibility.

## Prerequisites

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install build tools and dependencies
brew install cmake raylib sqlite3 pkg-config
```

### Linux (Ubuntu/Debian)
```bash
# Update package list
sudo apt update

# Install CMake, Raylib, and dependencies
sudo apt install cmake build-essential pkg-config libraylib-dev libsqlite3-dev

# For older Ubuntu versions, you may need to build raylib from source
# See: https://github.com/raysan5/raylib
```

### Linux (Fedora)
```bash
# Install CMake, Raylib, and dependencies
sudo dnf install cmake gcc make pkg-config raylib-devel sqlite-devel
```

### Windows (MSYS2/MinGW)
```bash
# Open MSYS2 MinGW 64-bit terminal
# Install build tools and dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-raylib mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-pkg-config make
```

---

## Building the Game

### Method 1: Using Build Scripts (Recommended)

We provide convenient build scripts that handle CMake configuration and compilation:

#### Unix/Linux/macOS
```bash
# Simple build (Release mode)
./build.sh

# Clean build
./build.sh --clean

# Debug build
./build.sh --debug

# Build and run
./build.sh --run

# Create distribution package
./build.sh --package

# Build with static linking
./build.sh --static

# Build only the main game (no tools/demos)
./build.sh --game-only

# All options
./build.sh --help
```

#### Windows
```batch
REM Simple build (Release mode)
build.bat

REM Clean build
build.bat --clean

REM Debug build
build.bat --debug

REM Build and run
build.bat --run

REM Create distribution package
build.bat --package

REM All options
build.bat --help
```

### Method 2: Using CMake Directly

#### Configure and Build
```bash
# Create build directory
mkdir -p build
cd build

# Configure (Release mode)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)

# Run
./shootemup
```

#### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Build type: `Debug`, `Release` |
| `BUILD_STATIC` | OFF | Build static binaries (ON for Windows) |
| `BUILD_GAME` | ON | Build main game executable |
| `BUILD_TOOLS` | ON | Build sprite generators and utilities |
| `BUILD_DEMOS` | ON | Build demo/showcase programs |

Example with options:
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_STATIC=ON \
    -DBUILD_GAME=ON \
    -DBUILD_TOOLS=OFF \
    -DBUILD_DEMOS=OFF
```

### Method 3: Using Traditional Makefile

The original Makefile is still available:
```bash
# Build everything
make

# Build and run
make run

# The executable will be created in bin/shootemup
./bin/shootemup
```

---

## Built Executables

After building, all executables are located in the `build/` directory:

### Main Game
- `shootemup` - The main game executable

### Showcase Programs
Interactive demonstration and testing programs for each major system:

- `enemy_showcase` - Interactive enemy testing arena with all 10 enemy types
- `enemy_showcase_sprites` - Sprite-based enemy showcase
- `player_showcase` - Player ship controls and weapon testing
- `projectile_showcase` - Test all 4 projectile types
- `powerup_showcase` - Powerup system demonstration and testing

### Audio Tools
- `audio_analysis_gui` - Audio analysis GUI with visual bass detection
- `audio_analysis_cli` - Command-line audio analysis tool

### Sprite Generation Tools
- `generate_enemy_sprites` - Generate all enemy sprite images
- `generate_spaceship_sprites` - Generate spaceship sprite variants
- `generate_projectile_sprites` - Generate projectile sprite images
- `generate_player_sprite` - Generate player ship sprite

### Database Tools
- `populate_highscores` - Populate high score database with preset data

### Running Executables

```bash
# From build directory
cd build

# Run main game
./shootemup

# Run showcases
./enemy_showcase
./player_showcase
./projectile_showcase
./powerup_showcase

# Run tools
./generate_enemy_sprites
./populate_highscores

# Run audio analysis
./audio_analysis_gui ../assets/audio/music.mp3
```

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
│   └── utils/          # Logging, audio analysis, database
├── include/            # Header files (.h)
├── assets/             # Game assets
│   ├── sprites/        # Generated sprite images
│   └── audio/          # Audio files (not tracked in git)
├── build/              # CMake build directory (generated)
│   ├── shootemup       # Main game executable
│   ├── enemy_showcase  # Enemy testing
│   ├── powerup_showcase # Powerup testing
│   └── ...            # Other executables and object files
├── bin/                # Legacy Makefile build output (if using Makefile)
├── docs/               # Documentation
├── CMakeLists.txt      # CMake build configuration
├── Makefile            # Traditional Makefile (legacy)
├── build.sh            # Convenience build script (Unix/macOS)
└── build.bat           # Convenience build script (Windows)
```

---

## Showcase Programs

### Enemy Showcase
Interactive testing environment for all enemy types.

```bash
# If using build script
./build.sh

# Or with CMake
cd build
./enemy_showcase
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
cd build
./player_showcase
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
cd build
./projectile_showcase
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
cd build
./powerup_showcase
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
# Build all tools
./build.sh

# Navigate to build directory
cd build

# Generate all enemy sprites
./generate_enemy_sprites

# Generate projectile sprites
./generate_projectile_sprites

# Generate player ship sprite
./generate_player_sprite

# Generate spaceship variants
./generate_spaceship_sprites
```

Generated sprites are saved to `assets/sprites/`.

---

## Audio Tools

### Audio Analysis GUI
Visual tool for analyzing audio and configuring bass detection.

```bash
cd build
./audio_analysis_gui ../assets/audio/your_music.mp3
```

**Features:**
- Real-time bass waveform
- FFT spectrum analyzer
- Bass threshold configuration
- Visual intensity indicators

### Audio Analysis CLI
Command-line audio analysis tool.

```bash
cd build
./audio_analysis_cli ../assets/audio/your_music.mp3
```

---

## Troubleshooting

### CMake Not Found
```bash
# macOS
brew install cmake

# Linux (Ubuntu/Debian)
sudo apt install cmake

# Linux (Fedora)
sudo dnf install cmake

# Check installation
cmake --version
```

### Raylib Not Found
If CMake cannot find Raylib:
```bash
# Check if Raylib is installed
pkg-config --libs raylib

# If not found, install Raylib
brew install raylib  # macOS
sudo apt install libraylib-dev  # Linux (Ubuntu/Debian)
sudo dnf install raylib-devel  # Linux (Fedora)

# For MSYS2/Windows
pacman -S mingw-w64-x86_64-raylib
```

### SQLite3 Not Found
```bash
# macOS
brew install sqlite3

# Linux (Ubuntu/Debian)
sudo apt install libsqlite3-dev

# Linux (Fedora)
sudo dnf install sqlite-devel

# MSYS2/Windows
pacman -S mingw-w64-x86_64-sqlite3
```

### Permission Denied
```bash
# Make build scripts executable
chmod +x build.sh
chmod +x run_debug_game.sh
```

### Build Errors
```bash
# Clean and rebuild
./build.sh --clean

# Or manually
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Check for missing dependencies
pkg-config --cflags --libs raylib
pkg-config --cflags --libs sqlite3

# Verify all source files are present
ls -R src/
```

### CMake Configuration Fails
```bash
# See detailed error messages
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release --debug-output

# Check if pkg-config is installed
which pkg-config
```

### Linking Errors on Linux
```bash
# Install missing X11 development libraries
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev

# Update library cache
sudo ldconfig
```

---

## Development Workflow

### Standard Development Cycle
1. **Make changes** to source files
2. **Build** with `./build.sh` or `cmake --build build`
3. **Test** with `./build/shootemup`
4. **Debug** if needed with `./build.sh --debug` and debugger
5. **Clean** periodically with `./build.sh --clean`

### Testing Workflow
1. **Build all** with `./build.sh` (includes all demos and tools)
2. **Test specific system** with appropriate showcase (e.g., `./build/enemy_showcase`)
3. **Iterate** on code changes
4. **Verify** in main game

### Adding New Features
1. **Create** new .c and .h files in appropriate directories
2. **Update CMakeLists.txt** to include new source files
3. **Rebuild** with `./build.sh --clean`
4. **Test** with debug build
5. **Document** in appropriate docs file

---

## Adding New Files

When adding new source files, update `CMakeLists.txt`:

### Add to Entity Module
```cmake
# In CMakeLists.txt
set(ENTITY_SRCS
    src/entities/player_ship.c
    src/entities/enemy_types.c
    src/entities/your_new_entity.c
)
```

### Add to System Module
```cmake
# In CMakeLists.txt
set(SYSTEM_SRCS
    src/systems/weapon.c
    src/systems/collision.c
    src/systems/your_new_system.c
    # ... other systems
)
```

### Add to Utility Module
```cmake
# In CMakeLists.txt
set(UTIL_SRCS
    src/utils/logger.c
    src/utils/database.c
    src/utils/your_new_util.c
)
```

Then rebuild:
```bash
./build.sh --clean
```

---

## Platform-Specific Notes

### macOS
- Uses Homebrew's Raylib installation
- Frameworks: OpenGL, Cocoa, IOKit, CoreVideo
- Debug symbols in .dSYM format
- Supports universal binaries (x86_64 + arm64)

**Apple Silicon (M1/M2/M3/M4):**
```bash
# CMake automatically builds universal binaries
./build.sh
```

### Linux
- May need additional X11 libraries:
  ```bash
  sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
  ```
- Uses system package manager for Raylib
- Static linking supported for distribution

### Windows
- Use MSYS2 MinGW 64-bit terminal for building
- CMake uses MinGW Makefiles generator
- Static linking enabled by default for Windows builds
- Executables have `.exe` extension

---

## Compiler Flags

CMake automatically manages compiler flags based on build type:

### Debug Build (`-DCMAKE_BUILD_TYPE=Debug`)
- `-g` - Debug symbols
- `-Wall` - All warnings
- `-Wextra` - Extra warnings
- `-std=c99` - C99 standard
- `-DDEBUG` - Debug macro defined

### Release Build (`-DCMAKE_BUILD_TYPE=Release`)
- `-O3` - Maximum optimization
- `-Wall` - All warnings
- `-Wextra` - Extra warnings
- `-std=c99` - C99 standard

---

## Build Performance

### Compilation Time
- **Clean build**: ~10-20 seconds (all executables)
- **Incremental build**: < 1-2 seconds
- **Full rebuild**: ~15-25 seconds

### Optimization Tips
```bash
# Use parallel compilation (CMake does this automatically)
cmake --build build -j$(nproc)

# Or with build script
./build.sh  # Automatically uses all CPU cores

# Build only specific targets
cd build
cmake --build . --target shootemup  # Just the game
cmake --build . --target enemy_showcase  # Just one showcase
```

---

## Continuous Integration

The project uses GitHub Actions for CI/CD with CMake.

### Local CI Check
```bash
# Check if everything builds (mimics CI)
./build.sh --clean
./build.sh --static --package

# Or manually
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC=ON
cmake --build . -j$(nproc)
cpack
```

### GitHub Actions
The CI/CD pipeline automatically:
- Builds for Linux, macOS, and Windows
- Creates static binaries (where applicable)
- Runs on all pull requests
- Creates release packages on tags

---

## Dependencies Summary

### Required
- **CMake**: Build system (v3.15+)
- **Raylib**: Graphics and input (v5.0+)
- **SQLite3**: Database for high scores (v3.0+)
- **GCC/Clang**: C compiler with C99 support
- **pkg-config**: Dependency management

### Optional
- **LLDB/GDB**: Debugging tools
- **Strip**: Binary optimization (usually included with compiler)

### Assets
- **Sprites**: Generated procedurally (run sprite generator tools)
- **Audio files**: Place in `assets/audio/` (MP3, OGG, WAV) - optional for music
- **Database**: `highscores.db` - automatically created on first run

---

## Getting Help

If you encounter build issues:

1. **Check documentation**: Review this BUILD.md and ARCHITECTURE.md
2. **Clean build**: Run `./build.sh --clean` or `rm -rf build && mkdir build`
3. **Verify dependencies**: Ensure CMake, Raylib, and SQLite3 are installed
4. **Check logs**: Look for error messages in terminal output
5. **Test build system**: Try `cmake --version` and `pkg-config --libs raylib`
6. **Platform-specific**: Check platform notes above
7. **GitHub Actions**: Check `.github/workflows/` for reference configurations

---

## Next Steps

After successful build:

1. **Run the game**: `./build/shootemup`
2. **Read [GAMEPLAY.md](GAMEPLAY.md)** to learn controls
3. **Try showcases** to explore each system (`./build/enemy_showcase`, etc.)
4. **Review [ARCHITECTURE.md](ARCHITECTURE.md)** to understand code structure
5. **Check [API.md](API.md)** for function references
6. **See [TODO.md](TODO.md)** for contribution opportunities