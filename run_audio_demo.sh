#!/bin/bash

# Audio Spectrogram Demo Runner
# This script builds and runs the audio spectrogram demo with bass detection

echo "======================================"
echo "Audio Spectrogram Demo - Bass Detection"
echo "======================================"
echo ""

# Check if the audio file exists
if [ ! -f "assets/audio/level1.mp3" ]; then
    echo "Error: Audio file 'assets/audio/level1.mp3' not found!"
    echo "Please ensure the audio file is in the correct location."
    exit 1
fi

# Clean and build the audio demo
echo "Building audio demo..."
make clean > /dev/null 2>&1
make audio_demo

if [ $? -ne 0 ]; then
    echo "Build failed! Please check for compilation errors."
    exit 1
fi

echo ""
echo "Starting audio demo..."
echo "======================================"
echo "Controls:"
echo "  SPACE - Pause/Resume playback"
echo "  ESC   - Exit demo"
echo "======================================"
echo ""
echo "Bass Detection Levels:"
echo "  LOW    - Energy > 0.15"
echo "  MEDIUM - Energy > 0.35"
echo "  HIGH   - Energy > 0.60"
echo "======================================"
echo ""

# Run the audio demo
./bin/audio_spectrogram

echo ""
echo "Audio demo finished."
