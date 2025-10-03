#!/bin/bash

# Quick test script for audio demo
# This will run the demo for 15 seconds to verify bass detection is working

echo "Testing audio spectrogram demo..."
echo "This will run for about 15 seconds to check bass detection"
echo ""

# Check if binary exists
if [ ! -f "bin/audio_spectrogram" ]; then
    echo "Building audio demo..."
    make audio_demo
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
fi

echo "Starting audio demo (will run for 15 seconds)..."
echo "Watch for bass detection events in the output below:"
echo "=================================================="
echo ""

# Run the demo in background and capture output
timeout 15s ./bin/audio_spectrogram 2>&1 | grep -E "(Bass START|Bass END|Bass CHANGE|successfully loaded|Total bass)" &

DEMO_PID=$!
wait $DEMO_PID

echo ""
echo "=================================================="
echo "Test complete. If you see bass events above, the demo is working correctly!"
echo ""
echo "To run the full demo with GUI, use:"
echo "  ./run_audio_demo.sh"
echo "or:"
echo "  make run_audio"

