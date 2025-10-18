# CMake Migration Guide

This document explains the migration from Makefile to CMake as the primary build system.

## Summary

The project now uses **CMake** as the primary build system while maintaining the original Makefile for backward compatibility. This change provides:

- ✅ **Cross-platform support**: Consistent builds on Linux, macOS, and Windows
- ✅ **GitHub Actions integration**: Automated CI/CD pipelines
- ✅ **Better dependency management**: Automatic detection of raylib and SQLite3
- ✅ **Easier configuration**: Build options via CMake variables
- ✅ **Modern build system**: Industry-standard tooling

## What Changed

### New Files

1. **`CMakeLists.txt`** - CMake build configuration
2. **`build.sh`** - Convenience build script for Unix/macOS
3. **`build.bat`** - Convenience build script for Windows
4. **`CMAKE_MIGRATION.md`** - This migration guide

### Modified Files

1. **`docs/BUILD.md`** - Updated with CMake instructions
2. **`README.md`** - Updated quick start to mention CMake
3. **`.github/workflows/build.yml`** - Already configured for CMake
4. **`.github/workflows/release.yml`** - Already configured for CMake

### Unchanged

- All source code (`src/`, `include/`)
- Original `Makefile` (still works)
- Assets and documentation
- Project structure

## Migration for Developers

### Before (Makefile)

```bash
# Build
make

# Clean
make clean

# Run
make run

# Debug build
make debug
```

### After (CMake - Recommended)

```bash
# Build
./build.sh

# Clean build
./build.sh --clean

# Run
./build/shootemup

# Debug build
./build.sh --debug

# Build and run
./build.sh --run
```

### Build Script Options

The new `build.sh` script supports various options:

| Option | Description |
|--------|-------------|
| `-d, --debug` | Build in debug mode |
| `-c, --clean` | Clean before building |
| `-s, --static` | Build static binaries |
| `--game-only` | Build only main game (no tools/demos) |
| `-r, --run` | Run game after building |
| `-i, --install` | Install files to build/install |
| `-p, --package` | Create distribution package |
| `-h, --help` | Show help |

### CMake Direct Usage

If you prefer using CMake directly:

```bash
# Configure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . -j$(nproc)

# Run
./shootemup
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `CMAKE_BUILD_TYPE` | Release | Debug or Release |
| `BUILD_STATIC` | OFF | Build static binaries |
| `BUILD_GAME` | ON | Build main game |
| `BUILD_TOOLS` | ON | Build tools (generators, etc.) |
| `BUILD_DEMOS` | ON | Build demo programs |

Example:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_DEMOS=OFF
```

## Executable Locations

### Makefile (Old)
- Executables in: `bin/`
- Build artifacts in: `build/`

### CMake (New)
- Executables in: `build/`
- Build artifacts in: `build/`
- All-in-one location

## For GitHub Actions / CI

The GitHub Actions workflows (`.github/workflows/build.yml` and `.github/workflows/release.yml`) were already configured for CMake and will continue to work seamlessly.

## Common Tasks Comparison

| Task | Makefile | CMake (build.sh) |
|------|----------|------------------|
| Build all | `make` | `./build.sh` |
| Build game only | `make game` | `./build.sh --game-only` |
| Clean build | `make clean && make` | `./build.sh --clean` |
| Debug build | `make debug` | `./build.sh --debug` |
| Build and run | `make run` | `./build.sh --run` |
| Build static | N/A | `./build.sh --static` |
| Create package | N/A | `./build.sh --package` |

## Platform-Specific Notes

### macOS
- CMake builds for native architecture by default (arm64 on Apple Silicon, x86_64 on Intel)
- Universal binaries (arm64 + x86_64) only built in CI/CD (set `BUILD_UNIVERSAL=true` environment variable if needed locally)

### Linux
- Static linking supported with `--static` flag
- Requires `cmake`, `libraylib-dev`, `libsqlite3-dev`

### Windows
- Use `build.bat` instead of `build.sh`
- Requires MSYS2/MinGW environment
- Static linking enabled by default

## Troubleshooting

### "CMake not found"
```bash
# macOS
brew install cmake

# Linux (Ubuntu/Debian)
sudo apt install cmake

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-cmake
```

### "raylib not found"
```bash
# macOS
brew install raylib

# Linux (Ubuntu/Debian)
sudo apt install libraylib-dev

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-raylib
```

### Architecture mismatch on macOS
- The build script now builds for native architecture by default
- If you see "ignoring file: found architecture 'arm64', required architecture 'x86_64'", this is normal for CI builds
- Local builds should work without universal binary support

## Why CMake?

1. **Industry Standard**: CMake is widely used in professional C/C++ development
2. **Cross-Platform**: Works identically on Linux, macOS, and Windows
3. **Better Dependency Management**: Automatically finds libraries using pkg-config and other methods
4. **IDE Integration**: Supported by CLion, VS Code, Visual Studio, etc.
5. **CI/CD Integration**: GitHub Actions, GitLab CI, and other CI systems have built-in CMake support
6. **Packaging**: Built-in CPack support for creating distribution packages

## Backward Compatibility

The original Makefile is still available and functional. You can continue using it if preferred:

```bash
make        # Still works
make run    # Still works
make clean  # Still works
```

However, we recommend migrating to CMake for:
- New features (static builds, packaging)
- Better cross-platform support
- Consistency with CI/CD pipelines

## Questions?

- Check [docs/BUILD.md](docs/BUILD.md) for detailed build instructions
- See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for project structure
- Review `.github/workflows/` for CI/CD examples

## Summary of Benefits

| Feature | Makefile | CMake |
|---------|----------|-------|
| Linux support | ✅ | ✅ |
| macOS support | ✅ | ✅ |
| Windows support | ⚠️ (limited) | ✅ |
| Static builds | ❌ | ✅ |
| Distribution packages | ❌ | ✅ |
| IDE integration | ⚠️ (limited) | ✅ |
| Dependency detection | Manual | Automatic |
| CI/CD integration | ⚠️ (limited) | ✅ |
| Universal binaries (macOS) | ❌ | ✅ |

---

**Note**: This migration does not affect the source code, game functionality, or assets. It only changes the build system.

