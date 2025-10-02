#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
INVULNERABLE="false"
PHASE=0
OVERHEATING="false"
LOGGING="false"
SHOW_HELP=0

# Phase names array (matching the game's phase structure)
PHASE_NAMES=(
    "Normal Start"
    "Warm-Up"
    "First Wave"
    "Tank Squadron"
    "Swarm Attack"
    "Mixed Assault"
    "Elite Squadron"
    "Zigzag Chaos"
    "Shield Wall"
    "Bomber Run"
    "Ghost Ambush"
    "Combined Arms"
    "Mini-Boss"
    "Recovery"
    "Elite & Shield"
    "Evasive Maneuvers"
    "Heavy Assault"
    "Final Wave"
)

# Phase descriptions
PHASE_DESCRIPTIONS=(
    "Start from beginning normally"
    "0-5 seconds - No enemies"
    "5-35 seconds - 40 Grunts in 5 lines (no firing)"
    "35-55 seconds - 5 Tank enemies"
    "55-85 seconds - 30 Swarm enemies in groups"
    "85-120 seconds - Speeders and Grunts"
    "120-150 seconds - 3 Elite enemies"
    "150-180 seconds - Zigzag enemies"
    "180-220 seconds - Shield enemies"
    "220-260 seconds - Bomber enemies"
    "260-300 seconds - Ghost enemies"
    "300-345 seconds - Mixed enemy types"
    "345-390 seconds - Boss encounter"
    "390-420 seconds - Light enemies"
    "420-460 seconds - Elite and Shield combo"
    "460-500 seconds - Fast enemies"
    "500-540 seconds - Heavy enemies"
    "540-590 seconds - Final challenge"
)

# Function to display help
show_help() {
    echo -e "${GREEN}================================${NC}"
    echo -e "${GREEN}Debug Game Runner${NC}"
    echo -e "${GREEN}================================${NC}"
    echo ""
    echo -e "${YELLOW}Usage:${NC}"
    echo "  $0 [OPTIONS]"
    echo ""
    echo -e "${YELLOW}Options:${NC}"
    echo "  -i, --invulnerable    Enable invulnerability mode"
    echo "  -o, --overheating     Enable weapon overheating system"
    echo "  -d, --debug-log       Enable debug logging to game.log"
    echo "  -p, --phase NUMBER    Start at specific phase (0-17)"
    echo "  -l, --list           List all available phases"
    echo "  -h, --help           Show this help message"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  $0                    # Normal game"
    echo "  $0 -i                 # Invulnerable from start"
    echo "  $0 -o                 # Enable weapon overheating"
    echo "  $0 -d                 # Enable debug logging"
    echo "  $0 -p 3               # Start at Tank Squadron"
    echo "  $0 -i -o -d -p 3      # Tank Squadron with invulnerability, overheating, and logging"
    echo ""
}

# Function to list phases
list_phases() {
    echo -e "${GREEN}================================${NC}"
    echo -e "${GREEN}Available Phases${NC}"
    echo -e "${GREEN}================================${NC}"
    echo ""
    for i in "${!PHASE_NAMES[@]}"; do
        echo -e "${YELLOW}Phase $i:${NC} ${PHASE_NAMES[$i]}"
        echo -e "  ${BLUE}${PHASE_DESCRIPTIONS[$i]}${NC}"
        echo ""
    done
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -i|--invulnerable)
            INVULNERABLE="true"
            shift
            ;;
        -o|--overheating)
            OVERHEATING="true"
            shift
            ;;
        -d|--debug-log)
            LOGGING="true"
            shift
            ;;
        -p|--phase)
            if [[ -n $2 && $2 =~ ^[0-9]+$ ]]; then
                PHASE=$2
                shift 2
            else
                echo -e "${RED}Error: --phase requires a number (0-17)${NC}"
                exit 1
            fi
            ;;
        -l|--list)
            list_phases
            exit 0
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            exit 1
            ;;
    esac
done

# Validate phase number
if [[ $PHASE -lt 0 || $PHASE -gt 17 ]]; then
    echo -e "${RED}Error: Phase must be between 0 and 17${NC}"
    echo "Use -l or --list to see available phases"
    exit 1
fi

# Display configuration
echo -e "${GREEN}================================${NC}"
echo -e "${GREEN}Debug Game Configuration${NC}"
echo -e "${GREEN}================================${NC}"
echo ""
echo -e "${YELLOW}Settings:${NC}"
echo -e "  Invulnerability: ${INVULNERABLE}"
echo -e "  Weapon Overheating: ${OVERHEATING}"
echo -e "  Debug Logging: ${LOGGING}"
echo -e "  Starting Phase: $PHASE - ${PHASE_NAMES[$PHASE]}"
echo -e "  Description: ${PHASE_DESCRIPTIONS[$PHASE]}"
echo ""

# Backup the original constants.h
echo -e "${YELLOW}Backing up original constants.h...${NC}"
cp include/constants.h include/constants.h.backup

# Function to restore original settings
restore_settings() {
    echo ""
    echo -e "${YELLOW}Restoring original settings...${NC}"
    mv include/constants.h.backup include/constants.h
    echo -e "${GREEN}Settings restored!${NC}"
}

# Set up trap to restore settings on script exit
trap restore_settings EXIT

# Modify constants.h with the selected settings
echo -e "${YELLOW}Applying debug settings...${NC}"

# Update invulnerability setting
if [[ $INVULNERABLE == "true" ]]; then
    sed -i '' 's/#define DEBUG_INVULNERABILITY false/#define DEBUG_INVULNERABILITY true/' include/constants.h
else
    sed -i '' 's/#define DEBUG_INVULNERABILITY true/#define DEBUG_INVULNERABILITY false/' include/constants.h
fi

# Update overheating setting
if [[ $OVERHEATING == "true" ]]; then
    sed -i '' 's/#define WEAPON_OVERHEATING false/#define WEAPON_OVERHEATING true/' include/constants.h
else
    sed -i '' 's/#define WEAPON_OVERHEATING true/#define WEAPON_OVERHEATING false/' include/constants.h
fi

# Update logging setting
if [[ $LOGGING == "true" ]]; then
    sed -i '' 's/#define DEBUG_LOGGING false/#define DEBUG_LOGGING true/' include/constants.h
else
    sed -i '' 's/#define DEBUG_LOGGING true/#define DEBUG_LOGGING false/' include/constants.h
fi

# Update phase setting
sed -i '' "s/#define DEBUG_START_PHASE [0-9]*/#define DEBUG_START_PHASE $PHASE/" include/constants.h

# Recompile the game
echo -e "${YELLOW}Recompiling game with debug settings...${NC}"
make clean > /dev/null 2>&1
if make > /dev/null 2>&1; then
    echo -e "${GREEN}Compilation successful!${NC}"
else
    echo -e "${RED}Compilation failed!${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}Starting game with selected configuration${NC}"
echo ""
echo -e "${YELLOW}Controls:${NC}"
echo "  - Move: WASD/Arrow Keys"
echo "  - Shoot: Left Mouse Button"
echo "  - Pause: P"
echo "  - Show Hitboxes: H"
echo "  - Restart: R (when game over)"
echo "  - Exit: ESC or close window"
echo ""

if [[ $INVULNERABLE == "true" ]]; then
    echo -e "${GREEN}[INVULNERABLE] will appear in game${NC}"
fi

if [[ $OVERHEATING == "true" ]]; then
    echo -e "${YELLOW}[WEAPON OVERHEATING] is enabled${NC}"
fi

if [[ $LOGGING == "true" ]]; then
    echo -e "${BLUE}[DEBUG LOGGING] game.log will be created${NC}"
fi

if [[ $PHASE -gt 0 ]]; then
    echo -e "${YELLOW}[DEBUG: Started at Phase $PHASE] will appear in game${NC}"
fi

echo ""
echo -e "${GREEN}Press any key to start the game...${NC}"
read -n 1 -s

# Run the game
./bin/shootemup

echo ""
if [[ $LOGGING == "true" && -f "game.log" ]]; then
    echo -e "${BLUE}Debug log saved to: game.log${NC}"
fi
echo -e "${GREEN}Game closed. Original settings have been restored.${NC}"
