# Building on Linux

This guide provides detailed instructions for compiling and running **Capybara Shoot'em Up** on Linux distributions.

## Table of Contents

- [System Requirements](#system-requirements)
- [Tested Distributions](#tested-distributions)
- [Dependencies](#dependencies)
- [Quick Installation Guide](#quick-installation-guide)
- [Step-by-Step Installation](#step-by-step-installation)
- [Building the Game](#building-the-game)
- [Running the Game](#running-the-game)
- [Troubleshooting](#troubleshooting)
- [Differences from macOS](#differences-from-macos)

---

## System Requirements

### Minimum Requirements
- **CPU**: x86_64 (64-bit)
- **RAM**: 512 MB
- **GPU**: OpenGL 3.3+ compatible
- **Disk Space**: 100 MB (including dependencies)
- **Display**: 1200x600 minimum resolution

### Recommended
- **CPU**: Dual-core 2.0 GHz or better
- **RAM**: 2 GB
- **GPU**: OpenGL 4.3+ with hardware acceleration
- **Audio**: PulseAudio or ALSA

---

## Tested Distributions

This game has been successfully tested on:

| Distribution | Version | Status |
|--------------|---------|--------|
| **Ubuntu** | 25.10 (Questing Quokka) | ✅ Fully Working |
| **Ubuntu** | 24.04 LTS (Noble Numbat) | ✅ Expected to work |
| **Ubuntu** | 22.04 LTS (Jammy Jellyfish) | ✅ Expected to work |
| **Debian** | 12 (Bookworm) | ✅ Expected to work |
| **Fedora** | 40+ | ✅ Expected to work |
| **Arch Linux** | Rolling | ✅ Expected to work |

---

## Dependencies

### Core Build Tools
- **GCC/G++**: 9.0+ (C99 support required)
- **CMake**: 3.15+
- **Make**: GNU Make
- **pkg-config**: For dependency management
- **Git**: For cloning Raylib (if needed)

### Libraries
- **Raylib**: 5.0+ (graphics and input library)
- **SQLite3**: 3.0+ (database for high scores)
- **X11 Development Libraries**: For windowing system
- **OpenGL**: Mesa or proprietary drivers
- **ALSA/PulseAudio**: For audio support

---

## Quick Installation Guide

### Ubuntu/Debian (One-Command Install)

```bash
# Install all dependencies except Raylib
sudo apt update && sudo apt install -y \
    cmake build-essential pkg-config git \
    libsqlite3-dev \
    libx11-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libxi-dev libgl1-mesa-dev \
    libasound2-dev

# Build and install Raylib from source
cd /tmp
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git
cd raylib && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build . -j$(nproc)
sudo cmake --install .
sudo ldconfig

# Clone and build the game
cd ~/
git clone https://github.com/theburrowhub/capybara-project.git
cd capybara-project
./build.sh

# Run the game
./build/shootemup
```

### Fedora

```bash
# Install dependencies
sudo dnf install -y \
    cmake gcc gcc-c++ make pkg-config git \
    sqlite-devel \
    libX11-devel libXrandr-devel libXinerama-devel \
    libXcursor-devel libXi-devel mesa-libGL-devel \
    alsa-lib-devel

# Build and install Raylib (not in Fedora repos)
cd /tmp
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git
cd raylib && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build . -j$(nproc)
sudo cmake --install .
sudo ldconfig

# Build the game
cd ~/capybara-project
./build.sh
```

### Arch Linux

```bash
# Install dependencies (Raylib available in AUR)
sudo pacman -S cmake base-devel pkg-config git \
               sqlite libx11 libxrandr libxinerama \
               libxcursor libxi mesa alsa-lib

# Install Raylib from AUR
yay -S raylib  # or use your preferred AUR helper

# Build the game
cd ~/capybara-project
./build.sh
```

---

## Step-by-Step Installation

### 1. Install Core Build Tools

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y cmake build-essential pkg-config git
```

#### Fedora
```bash
sudo dnf install -y cmake gcc gcc-c++ make pkg-config git
```

#### Arch Linux
```bash
sudo pacman -S cmake base-devel pkg-config git
```

### 2. Install SQLite3 Development Libraries

#### Ubuntu/Debian
```bash
sudo apt install -y libsqlite3-dev
```

#### Fedora
```bash
sudo dnf install -y sqlite-devel
```

#### Arch Linux
```bash
sudo pacman -S sqlite
```

### 3. Install X11 and OpenGL Libraries

These libraries are required for windowing and graphics rendering.

#### Ubuntu/Debian
```bash
sudo apt install -y \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libasound2-dev
```

#### Fedora
```bash
sudo dnf install -y \
    libX11-devel \
    libXrandr-devel \
    libXinerama-devel \
    libXcursor-devel \
    libXi-devel \
    mesa-libGL-devel \
    alsa-lib-devel
```

#### Arch Linux
```bash
sudo pacman -S \
    libx11 \
    libxrandr \
    libxinerama \
    libxcursor \
    libxi \
    mesa \
    alsa-lib
```

### 4. Install Raylib

⚠️ **Important**: Raylib is **NOT available** in the official repositories of Ubuntu 25.10 and most Debian-based distributions. You must compile it from source.

#### Option A: Compile from Source (Recommended)

```bash
# Navigate to temporary directory
cd /tmp

# Clone Raylib repository (version 5.5)
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git

# Build Raylib
cd raylib
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build . -j$(nproc)

# Install system-wide
sudo cmake --install .

# Update library cache
sudo ldconfig

# Verify installation
pkg-config --modversion raylib
# Should output: 5.5.0
```

#### Option B: Use AUR (Arch Linux Only)

```bash
yay -S raylib
# or
paru -S raylib
```

### 5. Verify All Dependencies

Run this command to check if all dependencies are correctly installed:

```bash
echo "=== Dependency Verification ==="
echo "GCC: $(gcc --version 2>&1 | head -1)"
echo "G++: $(g++ --version 2>&1 | head -1)"
echo "CMake: $(cmake --version 2>&1 | head -1)"
echo "Raylib: $(pkg-config --modversion raylib 2>&1)"
echo "SQLite3: $(pkg-config --modversion sqlite3 2>&1)"
echo "System: $(lsb_release -d 2>/dev/null | cut -f2 || cat /etc/os-release | grep PRETTY_NAME | cut -d'"' -f2)"
```

Expected output should show versions without errors.

---

## Building the Game

### Using the Build Script (Recommended)

The project includes a convenient build script that handles CMake configuration:

```bash
# Navigate to project directory
cd capybara-project

# Build the game (Release mode)
./build.sh

# Other build options:
./build.sh --clean        # Clean build
./build.sh --debug        # Debug build with symbols
./build.sh --static       # Static linking
./build.sh --game-only    # Build only the main game
./build.sh --help         # Show all options
```

### Using CMake Directly

```bash
# Create build directory
mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with all CPU cores
cmake --build . -j$(nproc)

# Optionally strip binaries to reduce size
strip shootemup
```

### Build Output

After successful compilation, you'll find these executables in the `build/` directory:

```
build/
├── shootemup                      # Main game (167 KB)
├── enemy_showcase                 # Enemy testing arena (142 KB)
├── player_showcase                # Player ship showcase (70 KB)
├── powerup_showcase               # Powerup demonstration (79 KB)
├── projectile_showcase            # Projectile testing (66 KB)
├── enemy_showcase_sprites         # Enemy sprite showcase (66 KB)
├── audio_analysis_gui             # Audio analyzer GUI (44 KB)
├── audio_analysis_cli             # Audio analyzer CLI (35 KB)
├── generate_enemy_sprites         # Enemy sprite generator (34 KB)
├── generate_projectile_sprites    # Projectile sprite generator (35 KB)
├── generate_spaceship_sprites     # Spaceship sprite generator (39 KB)
├── generate_player_sprite         # Player sprite generator (21 KB)
└── populate_highscores            # High scores populator (30 KB)
```

---

## Running the Game

### Launch the Main Game

```bash
cd capybara-project
./build/shootemup
```

### First Run

On the first run, the game will:
1. Create configuration directory at `~/.config/capybara-project/`
2. Initialize the SQLite database with 40 legendary high scores
3. Create default input configuration

### Expected Output

```
Populating high scores with legendary game developers...
✓ Added 40 legendary developer high scores
Database initialized at: /home/username/.config/capybara-project/game.db
INFO: Initializing raylib 5.5
INFO: Platform backend: DESKTOP (GLFW)
INFO: DISPLAY: Device initialized successfully
INFO:     > Display size: 2560 x 1440
INFO:     > Screen size:  1200 x 600
INFO: GL: OpenGL device information:
INFO:     > Vendor:   Mesa
INFO:     > Renderer: Your GPU
INFO:     > Version:  4.3 (Core Profile) Mesa 25.2.3
INFO: AUDIO: Device initialized successfully
INFO:     > Backend:       miniaudio | PulseAudio
```

### Running Demo Programs

```bash
# Interactive enemy testing
./build/enemy_showcase

# Player ship controls and weapons
./build/player_showcase

# Projectile system testing
./build/projectile_showcase

# Powerup collection and effects
./build/powerup_showcase
```

---

## Troubleshooting

### CMake Cannot Find Raylib

**Error:**
```
CMake Error: Could not find a package configuration file provided by "raylib"
```

**Solution:**
```bash
# Verify Raylib is installed
pkg-config --modversion raylib

# If not found, reinstall Raylib
cd /tmp/raylib/build
sudo cmake --install .
sudo ldconfig

# Update pkg-config cache
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
```

### SQLite3 Not Found

**Error:**
```
Package 'sqlite3', required by 'virtual:world', not found
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt install libsqlite3-dev

# Fedora
sudo dnf install sqlite-devel

# Arch Linux
sudo pacman -S sqlite
```

### X11 Libraries Missing

**Error:**
```
fatal error: X11/Xlib.h: No such file or directory
```

**Solution:**
```bash
# Ubuntu/Debian
sudo apt install libx11-dev libxrandr-dev libxinerama-dev \
                 libxcursor-dev libxi-dev

# Fedora
sudo dnf install libX11-devel libXrandr-devel libXinerama-devel \
                 libXcursor-devel libXi-devel
```

### OpenGL Not Working

**Symptoms:**
- Black screen
- Game crashes on startup
- "Failed to create OpenGL context" error

**Solution:**

1. Check GPU drivers:
```bash
glxinfo | grep "OpenGL version"
# Should show OpenGL 3.3 or higher
```

2. Install Mesa drivers (if using integrated/AMD graphics):
```bash
sudo apt install mesa-utils libgl1-mesa-dri
```

3. Install proprietary drivers (NVIDIA):
```bash
sudo ubuntu-drivers autoinstall
# or
sudo apt install nvidia-driver-535
```

4. Verify hardware acceleration:
```bash
glxinfo | grep "direct rendering"
# Should output: direct rendering: Yes
```

### Audio Issues

**No sound or audio initialization fails:**

1. Check PulseAudio/PipeWire:
```bash
pactl info
# or
pw-cli info
```

2. Install missing audio libraries:
```bash
sudo apt install pulseaudio libasound2-dev
```

3. Restart audio service:
```bash
systemctl --user restart pulseaudio
# or
systemctl --user restart pipewire
```

### Permission Denied on Build Script

**Error:**
```
bash: ./build.sh: Permission denied
```

**Solution:**
```bash
chmod +x build.sh
chmod +x run_debug_game.sh
```

### Build Warnings

The compiler may show warnings like:
- Unused variables
- Unused parameters
- Missing switch cases

**These are non-critical** and don't affect functionality. They're code quality warnings that may be addressed in future releases.

---

## Differences from macOS

### Package Management

| Component | macOS | Linux |
|-----------|-------|-------|
| **Raylib** | `brew install raylib` | Must compile from source |
| **SQLite3** | `brew install sqlite3` | `apt install libsqlite3-dev` |
| **CMake** | `brew install cmake` | `apt install cmake` |

### System Libraries

**Linux requires additional libraries** that macOS includes by default:

- **X11**: Windowing system (macOS uses Cocoa)
- **Mesa**: OpenGL implementation (macOS uses Metal/OpenGL framework)
- **ALSA/PulseAudio**: Audio system (macOS uses CoreAudio)

### Graphics Backend

- **macOS**: Uses OpenGL/Metal frameworks
- **Linux**: Uses Mesa + X11/Wayland

### Audio Backend

- **macOS**: CoreAudio (built-in)
- **Linux**: miniaudio → PulseAudio/ALSA

### File Paths

| Type | macOS | Linux |
|------|-------|-------|
| **Config** | `~/Library/Application Support/` | `~/.config/capybara-project/` |
| **Database** | `~/Library/Application Support/capybara-project/` | `~/.config/capybara-project/` |

---

## Performance Notes

### Compilation Time

- **Full clean build**: ~30 seconds (on modern hardware)
- **Incremental build**: < 2 seconds
- **Raylib compilation**: ~3 minutes (one-time only)

### Runtime Performance

- **FPS**: 60 FPS (locked)
- **Memory usage**: ~50-100 MB
- **CPU usage**: 10-20% (single core)
- **GPU**: Minimal (2D sprites)

### Optimization Tips

1. **Use Release builds** for better performance:
   ```bash
   ./build.sh  # Already builds in Release mode
   ```

2. **Enable compiler optimizations**:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS="-O3 -march=native"
   ```

3. **Strip debug symbols** to reduce binary size:
   ```bash
   strip build/shootemup
   ```

---

## Distribution-Specific Notes

### Ubuntu 25.10+

- ✅ GCC 15.2.0 works perfectly
- ✅ CMake 3.31+ available
- ⚠️ Raylib must be compiled from source
- ✅ All X11 libraries available

### Ubuntu 22.04 LTS

- May have older CMake (3.22+) - still compatible
- GCC 11+ available
- Raylib not in repos - compile from source

### Fedora 40+

- Uses DNF package manager
- May have Raylib in COPR repositories
- GCC 14+ and CMake 3.28+ available

### Arch Linux

- Rolling release - always latest packages
- Raylib available in AUR
- Easiest to maintain

---

## Creating a Portable Build

To create a standalone version that doesn't require system Raylib:

```bash
# Build with static linking
./build.sh --static

# Create distribution package
./build.sh --package

# This creates a .tar.gz in build/ directory
```

---

## Automated Installation Script

Create a file named `install-linux.sh`:

```bash
#!/bin/bash

set -e  # Exit on error

echo "=================================="
echo "Capybara Shoot'em Up - Linux Setup"
echo "=================================="
echo ""

# Detect distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
else
    echo "Cannot detect Linux distribution"
    exit 1
fi

echo "Detected distribution: $DISTRO"
echo ""

# Install dependencies based on distribution
case $DISTRO in
    ubuntu|debian|linuxmint)
        echo "Installing dependencies via APT..."
        sudo apt update
        sudo apt install -y \
            cmake build-essential pkg-config git \
            libsqlite3-dev \
            libx11-dev libxrandr-dev libxinerama-dev \
            libxcursor-dev libxi-dev libgl1-mesa-dev \
            libasound2-dev
        ;;
    fedora|rhel|centos)
        echo "Installing dependencies via DNF..."
        sudo dnf install -y \
            cmake gcc gcc-c++ make pkg-config git \
            sqlite-devel \
            libX11-devel libXrandr-devel libXinerama-devel \
            libXcursor-devel libXi-devel mesa-libGL-devel \
            alsa-lib-devel
        ;;
    arch|manjaro)
        echo "Installing dependencies via Pacman..."
        sudo pacman -S --needed \
            cmake base-devel pkg-config git \
            sqlite libx11 libxrandr libxinerama \
            libxcursor libxi mesa alsa-lib
        ;;
    *)
        echo "Unsupported distribution: $DISTRO"
        exit 1
        ;;
esac

# Check if Raylib is installed
if ! pkg-config --exists raylib; then
    echo ""
    echo "Raylib not found. Compiling from source..."
    cd /tmp
    if [ -d raylib ]; then
        rm -rf raylib
    fi
    git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git
    cd raylib
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
    cmake --build . -j$(nproc)
    sudo cmake --install .
    sudo ldconfig
    echo "✓ Raylib installed successfully"
else
    echo "✓ Raylib already installed"
fi

echo ""
echo "=================================="
echo "All dependencies installed!"
echo "=================================="
echo ""
echo "To build the game, run:"
echo "  ./build.sh"
echo ""
echo "To run the game, execute:"
echo "  ./build/shootemup"
echo ""
```

Make it executable and run:
```bash
chmod +x install-linux.sh
./install-linux.sh
```

---

## Additional Resources

### Documentation
- [Main README](../README.md) - Project overview
- [Build Instructions](BUILD.md) - Cross-platform build guide
- [Architecture](ARCHITECTURE.md) - Code structure
- [Gameplay Guide](GAMEPLAY.md) - Controls and mechanics

### External Resources
- [Raylib Official Site](https://www.raylib.com/)
- [CMake Documentation](https://cmake.org/documentation/)
- [SQLite Documentation](https://www.sqlite.org/docs.html)

### Support
- Report issues: [GitHub Issues](https://github.com/theburrowhub/capybara-project/issues)
- Project repository: [GitHub](https://github.com/theburrowhub/capybara-project)

---

## License

This project uses Raylib, which is licensed under the zlib/libpng license. See the project's main LICENSE file for more information.

---

**Last Updated**: November 9, 2025  
**Tested On**: Ubuntu 25.10 (Questing Quokka)  
**Raylib Version**: 5.5.0  
**CMake Version**: 3.31.6
