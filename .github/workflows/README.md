# GitHub Actions CI/CD

This directory contains the GitHub Actions workflows for building and releasing the Capybara Shoot'em Up game across multiple platforms.

## Workflows

### 1. Build Workflow (`build.yml`)

Triggers on:
- Pull requests to `main`, `master`, `develop`, or `pipeline` branches
- Manual workflow dispatch only

**Note**: Does NOT run on push to save CI minutes. Use release workflow for production builds.

**Platforms:**
- **Linux** (Ubuntu latest) - Static linking
- **macOS** (latest) - Dynamic linking with system frameworks
- **Windows** (MinGW64) - Static linking

**What it does:**
- Installs all required dependencies
- Builds raylib from source (for consistent versions)
- Compiles the game and tools using CMake
- Creates platform-specific artifacts (tar.gz for Linux/macOS, zip for Windows)
- Uploads artifacts for 30-day retention

### 2. Release Workflow (`release.yml`)

Triggers on:
- Push of version tags (e.g., `v1.0.0`)
- Manual workflow dispatch with version input

**What it does:**
- Creates a GitHub release
- Builds optimized binaries for all platforms
- Strips debug symbols for smaller binaries
- Packages binaries with assets and documentation
- Uploads release assets to the GitHub release

## Usage

### Testing Builds Locally

Use the provided build scripts to test locally before pushing:

**Linux/macOS:**
```bash
chmod +x build_static.sh
./build_static.sh
```

**Windows (MinGW):**
```cmd
build_static.bat
```

### Creating a Release

To create a new release:

1. **Tag your commit:**
   ```bash
   git tag -a v1.0.0 -m "Release version 1.0.0"
   git push origin v1.0.0
   ```

2. **Wait for the workflow:**
   - Go to the Actions tab in your GitHub repository
   - Watch the "Release Multi-Platform" workflow
   - It will create a release with all platform binaries

3. **Manual release (alternative):**
   - Go to Actions → Release Multi-Platform
   - Click "Run workflow"
   - Enter version tag (e.g., `v1.0.0`)
   - Click "Run workflow"

## Build Configuration

### Static Linking

**Linux:**
- Fully static linking enabled
- Raylib built as static library
- SQLite3 statically linked
- Minimal external dependencies

**macOS:**
- Dynamic linking (macOS doesn't support full static linking well)
- System frameworks used (OpenGL, Cocoa, IOKit, CoreVideo)
- Universal binaries (x86_64 + arm64) for Intel and Apple Silicon

**Windows:**
- Static linking with MinGW
- All dependencies statically linked
- Single executable with no DLL dependencies (except system)

### Dependencies Built from Source

Both workflows build **raylib 5.0** from source to ensure:
- Consistent versions across platforms
- Static linking support
- Latest features and bug fixes

### CMake Options

The workflows use these CMake options:

```cmake
-DCMAKE_BUILD_TYPE=Release      # Optimized release build
-DBUILD_STATIC=ON/OFF           # Static linking (platform-dependent)
-DBUILD_GAME=ON                 # Build main game executable
-DBUILD_TOOLS=ON                # Build sprite generators and tools
-DBUILD_DEMOS=ON                # Build showcase programs
```

## Artifacts

### Build Artifacts

Available for 30 days after each build:

- `capybara-shootemup-linux-x86_64.tar.gz`
- `capybara-shootemup-macos-universal.tar.gz`
- `capybara-shootemup-windows-x86_64.zip`

### Release Assets

Permanently available on the GitHub release page:

- Same filenames as build artifacts
- Include main game, tools, assets, and documentation
- Optimized and stripped binaries

## Platform-Specific Notes

### Linux
- Builds on Ubuntu latest (currently 22.04)
- Static binary with minimal dependencies
- Requires OpenGL 3.3+ at runtime
- Should work on most modern Linux distributions

### macOS
- Builds universal binaries (Intel + Apple Silicon)
- Minimum deployment target: macOS 10.15 (Catalina)
- Uses native system frameworks
- Requires macOS 10.15 or later to run

### Windows
- Builds with MinGW64 (GCC for Windows)
- Static linking for portability
- Requires Windows 7 or later
- No additional runtime needed (except DirectX for OpenGL)

## Troubleshooting

### Build Fails on Dependency Installation

If the workflow fails during dependency installation:
1. Check if package names have changed
2. Update the apt/brew/msys2 package names in the workflow
3. Check the workflow run logs for specific errors

### Build Fails During Compilation

If the workflow fails during compilation:
1. Check for syntax errors in your code
2. Ensure all source files are committed
3. Verify include paths are correct
4. Check that all required headers are present

### Artifacts Not Uploaded

If artifacts aren't uploaded:
1. Check that the build step succeeded
2. Verify artifact paths exist
3. Check GitHub Actions logs for upload errors
4. Ensure artifact names don't contain invalid characters

### Release Creation Fails

If release creation fails:
1. Verify the tag format is correct (e.g., `v1.0.0`)
2. Check that you haven't already created a release for this tag
3. Ensure you have proper permissions in the repository
4. Check that `GITHUB_TOKEN` has write permissions

## Customization

### Adding a New Platform

To add support for another platform (e.g., ARM Linux):

1. Add a new job in `build.yml`:
   ```yaml
   build-arm-linux:
     runs-on: ubuntu-latest
     # Add cross-compilation steps
   ```

2. Install cross-compilation tools
3. Configure CMake for the target architecture
4. Update release workflow with the new platform

### Changing Raylib Version

To use a different raylib version:

1. Update the clone command in both workflows:
   ```bash
   git clone --depth 1 --branch 4.5 https://github.com/raysan5/raylib.git
   ```

2. Test locally to ensure compatibility

### Adding Build Options

To add new CMake options:

1. Add the option to `CMakeLists.txt`:
   ```cmake
   option(NEW_OPTION "Description" OFF)
   ```

2. Use it in your CMake configuration
3. Add it to the workflow cmake command:
   ```yaml
   -DNEW_OPTION=ON
   ```

## Local Testing

Before pushing changes that might affect the build:

1. **Test with CMake locally:**
   ```bash
   ./build_static.sh  # Linux/macOS
   build_static.bat   # Windows
   ```

2. **Test with Make:**
   ```bash
   make clean
   make all
   ```

3. **Verify binary works:**
   ```bash
   ./build/shootemup
   ```

4. **Check dependencies (Linux):**
   ```bash
   ldd build/shootemup
   ```

5. **Check dependencies (macOS):**
   ```bash
   otool -L build/shootemup
   ```

## Performance

Typical workflow execution times:

- **Linux build**: 5-8 minutes
- **macOS build**: 6-10 minutes
- **Windows build**: 8-12 minutes
- **Total (all platforms)**: ~10-15 minutes (parallel)

The workflows run in parallel, so total time is approximately the longest individual build time.

## Security

- Workflows use official GitHub Actions
- Dependencies installed from official repositories
- No third-party code executed
- Artifacts are scanned by GitHub
- Release assets are checksummed

## Resources

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [CMake Documentation](https://cmake.org/documentation/)
- [Raylib Documentation](https://www.raylib.com/)
- [MinGW-w64](https://www.mingw-w64.org/)

