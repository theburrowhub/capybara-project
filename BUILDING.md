# Building the Project

This guide covers building the Capybara Shoot'em Up project on all supported platforms.

## Quick Start

### Linux/macOS
```bash
./build_static.sh
./build/shootemup
```

### Windows (MinGW)
```cmd
build_static.bat
build\shootemup.exe
```

## Build Systems

The project supports two build systems:

### 1. Make (Traditional, Linux/macOS only)

**Quick build:**
```bash
make
```

**Available targets:**
```bash
make game              # Build only the main game
make all               # Build everything
make clean             # Clean build artifacts
make run               # Build and run
make debug             # Build with debug symbols
make release           # Build with optimizations
```

See the [Makefile](Makefile) for all targets.

### 2. CMake (Cross-platform, recommended for releases)

**Quick build:**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**CMake options:**
```bash
-DBUILD_STATIC=ON/OFF      # Enable static linking
-DBUILD_GAME=ON/OFF        # Build main game
-DBUILD_TOOLS=ON/OFF       # Build sprite generators
-DBUILD_DEMOS=ON/OFF       # Build showcase programs
-DCMAKE_BUILD_TYPE=Release # Release or Debug
```

## Platform-Specific Instructions

### Linux

#### Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  pkg-config \
  libasound2-dev \
  libx11-dev \
  libxrandr-dev \
  libxi-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libxcursor-dev \
  libxinerama-dev \
  sqlite3 \
  libsqlite3-dev
```

**Fedora:**
```bash
sudo dnf install -y \
  gcc \
  gcc-c++ \
  cmake \
  make \
  pkg-config \
  alsa-lib-devel \
  mesa-libGL-devel \
  libX11-devel \
  libXrandr-devel \
  libXi-devel \
  libXcursor-devel \
  libXinerama-devel \
  sqlite-devel
```

#### Install Raylib

**Option 1: From package manager**
```bash
# Ubuntu/Debian
sudo apt install libraylib-dev

# Fedora
sudo dnf install raylib-devel
```

**Option 2: Build from source (recommended for latest version)**
```bash
git clone --depth 1 --branch 5.0 https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
make -j$(nproc)
sudo make install
sudo ldconfig
```

#### Build the Game

**Using Make:**
```bash
make clean
make -j$(nproc)
./build/shootemup
```

**Using CMake (static):**
```bash
./build_static.sh
./build/shootemup
```

### macOS

#### Install Dependencies

**Install Homebrew** (if not already installed):
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

**Install packages:**
```bash
brew install cmake pkg-config sqlite3 raylib
```

#### Build the Game

**Using Make:**
```bash
make clean
make -j$(sysctl -n hw.ncpu)
./build/shootemup
```

**Using CMake:**
```bash
./build_static.sh
./build/shootemup
```

**For Apple Silicon (M1/M2/M3):**
```bash
mkdir build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
make -j$(sysctl -n hw.ncpu)
```

### Windows

#### Option 1: MSYS2/MinGW (Recommended)

**Install MSYS2:**
1. Download from: https://www.msys2.org/
2. Install to `C:\msys64`
3. Open "MSYS2 MinGW 64-bit" from Start menu

**Install dependencies:**
```bash
pacman -Syu
pacman -S --needed \
  mingw-w64-x86_64-gcc \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-make \
  mingw-w64-x86_64-pkg-config \
  mingw-w64-x86_64-sqlite3 \
  make git
```

**Install Raylib:**
```bash
git clone --depth 1 --branch 5.0 https://github.com/raysan5/raylib.git
cd raylib
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=OFF
mingw32-make -j$(nproc)
mingw32-make install
```

**Build the game:**
```bash
./build_static.bat
# or manually:
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC=ON
mingw32-make -j$(nproc)
```

#### Option 2: Visual Studio (Advanced)

**Requirements:**
- Visual Studio 2019 or later
- CMake 3.15+
- vcpkg (for dependencies)

**Install dependencies with vcpkg:**
```cmd
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install raylib:x64-windows sqlite3:x64-windows
```

**Build:**
```cmd
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\path\to\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build . --config Release
```

## Build Output

### Executables

After building, you'll find these executables:

**Main Game:**
- `build/shootemup` (both Make and CMake now use build/ directory)

**Tools:**
- `generate_enemy_sprites` - Generate enemy sprite assets
- `generate_projectile_sprites` - Generate projectile sprites
- `generate_player_sprite` - Generate player sprite
- `populate_highscores` - Populate high score database

**Showcases/Demos:**
- `enemy_showcase` - Interactive enemy testing
- `player_showcase` - Player ship testing
- `projectile_showcase` - Projectile testing
- `powerup_showcase` - Powerup system testing
- `audio_analysis_gui` - Audio analysis tool
- `audio_analysis_cli` - CLI audio analysis

### Directory Structure

```
build/                # CMake build output
├── shootemup         # Main game
├── enemy_showcase    # Showcases
└── ...               # Other executables

Note: Make and CMake both now use the build/ directory for consistency.
```

## Static Linking

### Why Static Linking?

Static linking creates a single binary with all dependencies included:
- **Portable** - Works on any system without installing dependencies
- **Simple** - Just copy the executable
- **Reliable** - No version conflicts

### Platform Support

| Platform | Static Linking | Notes |
|----------|----------------|-------|
| Linux | ✅ Full | Fully static binary |
| macOS | ⚠️ Partial | System frameworks still dynamic |
| Windows | ✅ Full | Static with MinGW |

### Building Static Binaries

**Linux:**
```bash
mkdir build && cd build
cmake .. -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**macOS:** (Limited static support)
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release  # Dynamic frameworks
make -j$(sysctl -n hw.ncpu)
```

**Windows:**
```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release
mingw32-make -j$(nproc)
```

## Troubleshooting

### Common Issues

#### "raylib not found"

**Solution:**
```bash
# Linux
sudo apt install libraylib-dev
# or build from source (see above)

# macOS
brew install raylib

# Windows (MSYS2)
# Build from source (see above)
```

#### "sqlite3 not found"

**Solution:**
```bash
# Linux
sudo apt install libsqlite3-dev

# macOS
brew install sqlite3

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-sqlite3
```

#### "OpenGL not found" (Linux)

**Solution:**
```bash
sudo apt install libgl1-mesa-dev libglu1-mesa-dev
```

#### Linking errors

**Solution:**
```bash
# Clean and rebuild
make clean && make
# or
rm -rf build && ./build_static.sh
```

#### "Permission denied" when running

**Solution:**
```bash
chmod +x build/shootemup
```

### Platform-Specific Issues

#### Linux: Missing X11 libraries

```bash
sudo apt install libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
```

#### macOS: Code signing issues

```bash
# Remove quarantine attribute
xattr -d com.apple.quarantine build/shootemup
```

#### Windows: DLL not found

If you get "missing DLL" errors:
```bash
# Copy MinGW DLLs to build directory
cp /mingw64/bin/libgcc_s_seh-1.dll build/
cp /mingw64/bin/libstdc++-6.dll build/
cp /mingw64/bin/libwinpthread-1.dll build/
```

Or rebuild with full static linking:
```bash
cmake .. -G "MinGW Makefiles" -DBUILD_STATIC=ON -DCMAKE_BUILD_TYPE=Release
```

## Development Builds

### Debug Build

**With Make:**
```bash
make debug
```

**With CMake:**
```bash
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

Features:
- Debug symbols included
- No optimizations
- Assertions enabled
- Larger binary size

### Release Build

**With Make:**
```bash
make release
```

**With CMake:**
```bash
mkdir build-release && cd build-release
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Features:
- Optimizations enabled (-O3)
- No debug symbols
- Assertions disabled
- Smaller binary size
- Better performance

## Performance Tips

### Parallel Builds

Speed up compilation:

```bash
# Linux
make -j$(nproc)

# macOS
make -j$(sysctl -n hw.ncpu)

# Windows (MSYS2)
mingw32-make -j$(nproc)
```

### Ccache (Linux/macOS)

Install ccache to speed up rebuilds:

```bash
# Linux
sudo apt install ccache

# macOS
brew install ccache

# Use with CMake
mkdir build && cd build
cmake .. -DCMAKE_C_COMPILER_LAUNCHER=ccache
make -j$(nproc)
```

## Verification

### Test the Build

```bash
# Run the game
./build/shootemup

# Run showcases
./build/enemy_showcase
./build/player_showcase

# Generate sprites
./build/generate_enemy_sprites
```

### Check Binary

**Linux:**
```bash
file build/shootemup
ldd build/shootemup  # Check dynamic dependencies
```

**macOS:**
```bash
file build/shootemup
otool -L build/shootemup  # Check dynamic libraries
```

**Windows:**
```bash
file build/shootemup.exe
ldd build/shootemup.exe  # Check DLL dependencies
```

## Clean Build

Remove all build artifacts:

**Make:**
```bash
make clean
```

**CMake:**
```bash
rm -rf build/
```

**All:**
```bash
make clean
rm -rf build/
```

## Next Steps

After building:
1. Read [GAMEPLAY.md](docs/GAMEPLAY.md) for controls
2. Try showcases to test each system
3. See [DEBUG_FEATURES.md](docs/DEBUG_FEATURES.md) for testing
4. Check [CI_CD_SETUP.md](docs/CI_CD_SETUP.md) for automated builds

## Resources

- [Build System Documentation](docs/BUILD.md)
- [CI/CD Setup](docs/CI_CD_SETUP.md)
- [Raylib Documentation](https://www.raylib.com/)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)

