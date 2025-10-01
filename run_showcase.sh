#!/bin/bash

# Enemy Showcase Runner Script
# This script builds and runs the enemy showcase demo

echo "================================"
echo "    ENEMY SHOWCASE DEMO"
echo "================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the project directory
if [ ! -f "Makefile" ]; then
    echo -e "${RED}Error: Makefile not found. Please run this script from the project root.${NC}"
    exit 1
fi

# Clean previous builds (optional)
echo -e "${YELLOW}Cleaning previous builds...${NC}"
make clean > /dev/null 2>&1

# Build the enemy showcase
echo -e "${YELLOW}Building enemy showcase...${NC}"
if make enemy_showcase; then
    echo -e "${GREEN}Build successful!${NC}"
    echo ""
    echo "================================"
    echo "Starting Enemy Showcase..."
    echo "================================"
    echo ""
    echo "CONTROLS:"
    echo "  • SPACE     - Pause/Resume animations"
    echo "  • D         - Toggle enemy details"
    echo "  • R         - Reset camera"
    echo "  • Arrow Keys - Pan camera"
    echo "  • Mouse Wheel - Zoom in/out"
    echo "  • Hover mouse over enemy for selection"
    echo ""
    echo "ENEMY TYPES:"
    echo "  1. GRUNT    - Basic, small, fast"
    echo "  2. TANK     - Large, slow, high health"
    echo "  3. SPEEDER  - Very fast, low health"
    echo "  4. ZIGZAG   - Erratic movement pattern"
    echo "  5. BOMBER   - Large, dangerous"
    echo "  6. SHIELD   - Protected by shields"
    echo "  7. SWARM    - Small, group behavior"
    echo "  8. ELITE    - Strong all-around"
    echo "  9. GHOST    - Phases in and out"
    echo "  10. BOSS    - Very powerful"
    echo ""
    echo "Press any key to start..."
    read -n 1
    
    # Run the showcase
    ./bin/enemy_showcase
    
    echo ""
    echo -e "${GREEN}Enemy showcase closed.${NC}"
else
    echo -e "${RED}Build failed! Check the error messages above.${NC}"
    exit 1
fi
