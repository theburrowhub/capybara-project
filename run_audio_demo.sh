#!/bin/bash

# Audio Analysis GUI Runner
# This script builds and runs the audio analysis GUI with bass detection

echo "======================================"
echo "Audio Analysis GUI - Bass Detection"
echo "======================================"
echo ""

# Check if the audio directory exists
if [ ! -d "assets/audio" ]; then
    echo "Error: Audio directory 'assets/audio' not found!"
    echo "Please ensure the audio directory exists."
    exit 1
fi

# Check if there are any MP3 files
mp3_count=$(find assets/audio -maxdepth 1 -name "*.mp3" | wc -l)
if [ "$mp3_count" -eq 0 ]; then
    echo "Error: No MP3 files found in 'assets/audio'!"
    echo "Please add MP3 files to the audio directory."
    exit 1
fi

echo "Found $mp3_count MP3 file(s) in assets/audio/"
echo ""

# Clean and build the audio analysis GUI
echo "Building audio analysis GUI..."
make clean > /dev/null 2>&1
make audio_gui

if [ $? -ne 0 ]; then
    echo "Build failed! Please check for compilation errors."
    exit 1
fi

echo ""
echo "Starting audio analysis GUI..."
echo "======================================"
echo "File Selection:"
echo "  Use UP/DOWN arrows or mouse to select"
echo "  ENTER/SPACE to confirm selection"
echo ""
echo "During Playback:"
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
echo "Note: Log files will be saved in assets/audio/ with the same name as the MP3 file."
echo "      Existing log files will be backed up automatically."
echo ""

# Run the audio analysis GUI
./bin/audio_analysis_gui

echo ""
echo "Audio analysis GUI finished."
