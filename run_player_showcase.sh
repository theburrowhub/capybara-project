#!/bin/bash

echo "==================================="
echo "   PLAYER SHIP SHOWCASE"
echo "==================================="
echo ""
echo "Building and running the enhanced player ship demo..."
echo ""

# Build the player sprite generator and showcase
make generate_player
make player_showcase

echo ""
echo "==================================="
echo "Demo launched! Check the game window."
echo ""
echo "Controls:"
echo "  Movement: WASD/Arrow Keys"
echo "  Fire: SPACE"
echo "  Boost: LEFT SHIFT"
echo "  Shield Burst: E"
echo "  EMP Blast: Q"
echo "  Overdrive: R"
echo "  Weapon Modes: 1-6 Keys"
echo "  Toggle Stats: TAB"
echo "  Toggle Help: H"
echo ""
echo "==================================="
