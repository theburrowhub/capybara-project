#!/bin/bash

# Build and run the game
echo "Building Shoot'em Up..."
make clean
make

if [ $? -eq 0 ]; then
    echo "Build successful! Launching game..."
    ./shootemup
else
    echo "Build failed. Please check the error messages above."
    exit 1
fi
