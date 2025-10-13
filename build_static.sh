#!/bin/bash
# Build script for static linking (Linux/macOS)

set -e

echo "==================================="
echo "Capybara Shoot'em Up - Static Build"
echo "==================================="
echo ""

# Detect platform
OS="$(uname -s)"
case "${OS}" in
    Linux*)     PLATFORM=Linux;;
    Darwin*)    PLATFORM=macOS;;
    *)          PLATFORM="UNKNOWN:${OS}"
esac

echo "Platform: ${PLATFORM}"
echo ""

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed."
    echo "Please install CMake:"
    echo "  macOS: brew install cmake"
    echo "  Linux: sudo apt install cmake"
    exit 1
fi

# Clean previous build
if [ -d "build" ]; then
    echo "Cleaning previous build..."
    rm -rf build
fi

mkdir -p build
cd build

# Configure based on platform
echo "Configuring CMake..."
if [ "${PLATFORM}" = "Linux" ]; then
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_STATIC=ON \
        -DBUILD_GAME=ON \
        -DBUILD_TOOLS=ON \
        -DBUILD_DEMOS=ON
elif [ "${PLATFORM}" = "macOS" ]; then
    # macOS: static linking is problematic, build with dynamic linking
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_STATIC=OFF \
        -DBUILD_GAME=ON \
        -DBUILD_TOOLS=ON \
        -DBUILD_DEMOS=ON \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15
else
    echo "Error: Unsupported platform ${PLATFORM}"
    exit 1
fi

# Build
echo ""
echo "Building..."
if [ "${PLATFORM}" = "macOS" ]; then
    make -j$(sysctl -n hw.ncpu)
else
    make -j$(nproc)
fi

echo ""
echo "==================================="
echo "Build complete!"
echo "==================================="
echo ""
echo "Binaries are in: build/"
echo ""
echo "Main game: ./build/shootemup"
echo "Enemy showcase: ./build/enemy_showcase"
echo ""
echo "To run the game:"
echo "  ./build/shootemup"
echo ""

