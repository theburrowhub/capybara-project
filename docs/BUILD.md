# Build Instructions

## Prerequisites

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install Raylib
brew install raylib

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
```

### Linux (Fedora)
```bash
# Install Raylib and dependencies
sudo dnf install raylib-devel
sudo dnf install gcc make pkg-config
```

### Windows (MinGW)
1. Download Raylib from https://github.com/raysan5/raylib/releases
2. Install MinGW-w64 from https://www.mingw-w64.org/
3. Add MinGW to PATH
4. Update Makefile paths as needed

## Building the Game

### Quick Build
```bash
# Build the game
make

# The executable will be created in bin/shootemup
```

### Build Options

#### Clean Build
```bash
# Remove all build artifacts
make clean

# Rebuild from scratch
make rebuild
```

#### Debug Build
```bash
# Build with debug symbols
make debug

# Use with debugger (gdb, lldb)
lldb bin/shootemup
```

#### Release Build
```bash
# Build with optimizations
make release
```

#### Build and Run
```bash
# Build and immediately run the game
make run

# Or use the shell script
./run.sh
```

## Project Structure

```
capybara-project/
├── src/          # Source files (.c)
├── include/      # Header files (.h)
├── build/        # Object files (.o) - generated
├── bin/          # Executable - generated
└── docs/         # Documentation
```

## Makefile Targets

| Target | Description |
|--------|-------------|
| `all` | Default target, builds the game |
| `clean` | Removes build artifacts and logs |
| `rebuild` | Clean + build |
| `run` | Build and run the game |
| `debug` | Build with debug symbols (-g) |
| `release` | Build with optimizations (-O3) |
| `help` | Show available targets |

## Troubleshooting

### Raylib Not Found
If you get "raylib.h not found":
```bash
# Check if Raylib is installed
pkg-config --libs raylib

# If not found, reinstall Raylib
brew reinstall raylib  # macOS
```

### Permission Denied
```bash
# Make scripts executable
chmod +x run.sh
```

### Build Errors
```bash
# Clean and rebuild
make clean
make

# Check for missing dependencies
pkg-config --cflags --libs raylib
```

## Development Workflow

1. **Make changes** to source files
2. **Build** with `make`
3. **Test** with `./bin/shootemup`
4. **Debug** if needed with `make debug` and debugger
5. **Clean** periodically with `make clean`

## Adding New Files

When adding new source files:

1. Add `.c` file to appropriate directory in `src/`
2. Add `.h` file to `include/`
3. Update Makefile `SRCS` variables:
   ```makefile
   ENTITY_SRCS = $(SRC_DIR)/entities/player.c \
                 $(SRC_DIR)/entities/enemy.c \
                 $(SRC_DIR)/entities/your_new_file.c
   ```
4. Rebuild with `make rebuild`

## Platform-Specific Notes

### macOS
- Uses Homebrew's Raylib installation
- Frameworks: OpenGL, Cocoa, IOKit, CoreVideo
- Debug symbols in .dSYM format

### Linux
- May need additional X11 libraries
- Uses system package manager for Raylib
- Core dumps enabled with `ulimit -c unlimited`

### Windows
- Consider using MSYS2 or WSL for Unix-like environment
- May need to adjust paths in Makefile
- Use `.exe` extension for executable
