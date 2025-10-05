#!/bin/bash
# Run the enemy test arena showcase

cd "$(dirname "$0")"

echo "Building Enemy Test Arena..."
make enemy_showcase

if [ $? -eq 0 ]; then
    echo ""
    echo "==================================="
    echo "  ENEMY TEST ARENA - HOW TO USE"
    echo "==================================="
    echo ""
    echo "NAVIGATION:"
    echo "  Grid → Click Enemy → Arena → ESC → Grid → ESC → Exit"
    echo ""
    echo "GRID MODE (Initial Screen):"
    echo "  - View all enemy types in a grid"
    echo "  - Hover over enemies to see details"
    echo "  - CLICK on any enemy to test it"
    echo "  - ESC: Exit program (only works here)"
    echo ""
    echo "ARENA MODE (After clicking enemy):"
    echo "  - Player ship (invincible) on left"
    echo "  - Selected enemy spawns from right"
    echo "  - Enemies use their standard attack behavior"
    echo "  - Enemies FIRE PROJECTILES (same as main game!)"
    echo "  - WASD or Arrow Keys to move"
    echo "  - SPACE or Left Mouse to shoot (press each time)"
    echo "  - Behavior: 100% identical to main game"
    echo "  - ESC: Return to grid (test another enemy)"
    echo ""
    echo "Each enemy will continuously respawn to test behavior!"
    echo ""
    echo "Starting showcase..."
    echo ""
    
    ./bin/enemy_showcase
else
    echo "Build failed!"
    exit 1
fi
