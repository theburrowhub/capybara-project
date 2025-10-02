#!/bin/bash
# Test script to jump directly to boss phase

# Temporarily modify constants.h to start at boss phase (phase 11)
sed -i.bak 's/#define DEBUG_START_PHASE 0/#define DEBUG_START_PHASE 11/' include/constants.h

# Rebuild
make clean && make

# Run the game
echo "Starting game at boss phase (Phase 11)..."
echo "Boss should have:"
echo "  - 500 base health"
echo "  - 100 shield health (regenerates after 5 seconds if not hit)"
echo "  - Total 600 health to defeat"
echo ""
echo "Press Ctrl+C to exit the game"
./bin/shootemup

# Restore original constants.h
mv include/constants.h.bak include/constants.h
