#!/bin/bash

# Sprite-based Enemy Showcase Runner Script
# This script generates sprites and runs the sprite-based showcase demo

echo "================================"
echo "  SPRITE ENEMY SHOWCASE DEMO"
echo "================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if we're in the project directory
if [ ! -f "Makefile" ]; then
    echo -e "${RED}Error: Makefile not found. Please run this script from the project root.${NC}"
    exit 1
fi

# Check if sprites exist
if [ ! -f "assets/sprites/enemy_spritesheet.png" ]; then
    echo -e "${YELLOW}Sprites not found. Generating sprites first...${NC}"
    echo ""
    
    # Build and run the sprite generator
    if make generate_sprites; then
        echo -e "${GREEN}Sprites generated successfully!${NC}"
        echo ""
    else
        echo -e "${RED}Failed to generate sprites!${NC}"
        exit 1
    fi
else
    echo -e "${GREEN}Sprites already exist.${NC}"
    echo -e "${BLUE}To regenerate sprites, run: make generate_sprites${NC}"
    echo ""
fi

# Build the sprite showcase if needed
echo -e "${YELLOW}Building sprite showcase...${NC}"
if make -q bin/enemy_showcase_sprites 2>/dev/null; then
    echo -e "${GREEN}Sprite showcase is up to date.${NC}"
else
    if make bin/enemy_showcase_sprites; then
        echo -e "${GREEN}Build successful!${NC}"
    else
        echo -e "${RED}Build failed! Check the error messages above.${NC}"
        exit 1
    fi
fi

echo ""
echo "================================"
echo "Starting Sprite Enemy Showcase..."
echo "================================"
echo ""
echo "FEATURES:"
echo "  • 10 unique enemy sprites with distinct visual designs"
echo "  • Each enemy has unique stats and behaviors"
echo "  • Sprites are loaded from PNG files"
echo ""
echo "CONTROLS:"
echo "  • SPACE      - Pause/Resume animations"
echo "  • D          - Toggle enemy details"
echo "  • S          - Toggle between sprites and procedural graphics"
echo "  • R          - Reset camera"
echo "  • Arrow Keys - Pan camera"
echo "  • Mouse Wheel - Zoom in/out"
echo "  • Hover mouse over enemy for selection"
echo ""
echo "ENEMY TYPES:"
echo "  1. GRUNT    🔺 - Basic, small, fast (triangle)"
echo "  2. TANK     ■  - Large, slow, armored (square)"
echo "  3. SPEEDER  »  - Very fast, arrow-shaped"
echo "  4. ZIGZAG   ◊  - Diamond with erratic movement"
echo "  5. BOMBER   ◉  - Large with danger indicator"
echo "  6. SHIELD   ◎  - Hexagon with rotating shields"
echo "  7. SWARM    •  - Small, moves in groups"
echo "  8. ELITE    ★  - Star-shaped, strong stats"
echo "  9. GHOST    ◈  - Semi-transparent, phases in/out"
echo "  10. BOSS    ✦  - Large with spikes and rings"
echo ""
echo "Press any key to start..."
read -n 1

# Run the sprite showcase
./bin/enemy_showcase_sprites

echo ""
echo -e "${GREEN}Sprite showcase closed.${NC}"
echo ""
echo "Additional options:"
echo "  • Run 'make generate_sprites' to regenerate sprites"
echo "  • Run 'make showcase' for the procedural graphics version"
echo "  • Sprite files are in assets/sprites/"
