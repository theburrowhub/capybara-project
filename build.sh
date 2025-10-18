#!/bin/bash
# Convenience build script for CMake

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN=false
STATIC=false
BUILD_GAME=true
BUILD_TOOLS=true
BUILD_DEMOS=true
RUN_GAME=false
INSTALL=false
PACKAGE=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -s|--static)
            STATIC=true
            shift
            ;;
        --game-only)
            BUILD_TOOLS=false
            BUILD_DEMOS=false
            shift
            ;;
        -r|--run)
            RUN_GAME=true
            shift
            ;;
        -i|--install)
            INSTALL=true
            shift
            ;;
        -p|--package)
            PACKAGE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [options]"
            echo ""
            echo "Options:"
            echo "  -d, --debug      Build in debug mode (default: Release)"
            echo "  -c, --clean      Clean build directory before building"
            echo "  -s, --static     Build static binaries"
            echo "  --game-only      Build only the main game (no tools/demos)"
            echo "  -r, --run        Run the game after building"
            echo "  -i, --install    Install files to build/install"
            echo "  -p, --package    Create distribution package"
            echo "  -h, --help       Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                    # Build in release mode"
            echo "  $0 -d                 # Build in debug mode"
            echo "  $0 -c -r              # Clean build and run"
            echo "  $0 --static -p        # Build static and create package"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}===============================================${NC}"
echo -e "${BLUE}Capybara Shoot'em Up - Build Script${NC}"
echo -e "${BLUE}===============================================${NC}"

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}Configuring CMake...${NC}"
CMAKE_ARGS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DBUILD_GAME=$BUILD_GAME"
    "-DBUILD_TOOLS=$BUILD_TOOLS"
    "-DBUILD_DEMOS=$BUILD_DEMOS"
)

if [ "$STATIC" = true ]; then
    CMAKE_ARGS+=("-DBUILD_STATIC=ON")
fi

# macOS deployment target (don't force universal binaries - build for native arch)
if [[ "$OSTYPE" == "darwin"* ]]; then
    CMAKE_ARGS+=(
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.15"
    )
    # Only build universal binaries if explicitly requested via environment variable
    if [ -n "$BUILD_UNIVERSAL" ] && [ "$BUILD_UNIVERSAL" = "true" ]; then
        CMAKE_ARGS+=("-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64")
    fi
fi

cmake .. "${CMAKE_ARGS[@]}"

# Build
echo -e "${YELLOW}Building...${NC}"
CPU_COUNT=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j$CPU_COUNT

echo -e "${GREEN}✓ Build completed successfully!${NC}"

# Install if requested
if [ "$INSTALL" = true ]; then
    echo -e "${YELLOW}Installing...${NC}"
    cmake --install . --prefix install_temp
    echo -e "${GREEN}✓ Installation completed in build/install_temp${NC}"
fi

# Package if requested
if [ "$PACKAGE" = true ]; then
    echo -e "${YELLOW}Creating package...${NC}"
    cpack
    echo -e "${GREEN}✓ Package created successfully!${NC}"
    ls -lh *.tar.gz *.zip 2>/dev/null || true
fi

# Run if requested
if [ "$RUN_GAME" = true ]; then
    if [ -f "shootemup" ]; then
        echo -e "${GREEN}Starting game...${NC}"
        cd ..
        ./build/shootemup
    else
        echo -e "${RED}Error: shootemup executable not found${NC}"
        exit 1
    fi
fi

echo -e "${BLUE}===============================================${NC}"
echo -e "${GREEN}Build process complete!${NC}"
echo -e "${BLUE}===============================================${NC}"

if [ "$RUN_GAME" = false ]; then
    echo ""
    echo "To run the game:"
    echo "  ./build/shootemup"
    echo ""
fi
