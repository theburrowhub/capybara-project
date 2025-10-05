#!/bin/bash

# Quick test script for audio analysis CLI
# This will run the analyzer for the first 10 seconds of a selected file

echo "Testing audio analysis CLI..."
echo ""

# Check if binary exists
if [ ! -f "bin/audio_analysis_cli" ]; then
    echo "Building audio analysis CLI..."
    make audio_cli
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
fi

# Check for MP3 files
mp3_count=$(find assets/audio -maxdepth 1 -name "*.mp3" | wc -l)
if [ "$mp3_count" -eq 0 ]; then
    echo "Error: No MP3 files found in 'assets/audio'!"
    exit 1
fi

# Check if level1.mp3 exists, if so use it, otherwise use -f flag for first found file
if [ -f "assets/audio/level1.mp3" ]; then
    echo "Using default file: level1.mp3"
    echo "=================================================="
    echo ""
    ./bin/audio_analysis_cli -f level1.mp3
else
    echo "level1.mp3 not found, please select a file:"
    echo "=================================================="
    echo ""
    ./bin/audio_analysis_cli
fi

echo ""
echo "=================================================="
echo "Test complete. Check the output above for bass detection events."
echo ""
echo "To analyze a specific file, use:"
echo "  ./bin/audio_analysis_cli -f <filename.mp3>"
echo "  ./bin/audio_analysis_cli -f level1.mp3 -t 30  # Analyze first 30 seconds"
echo ""
echo "To run the full GUI version, use:"
echo "  ./run_audio_demo.sh"
echo "or:"
echo "  make run_audio_gui"

