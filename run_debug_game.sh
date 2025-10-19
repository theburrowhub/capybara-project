#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
INVULNERABLE="false"
LEVEL=1
PHASE=0
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
    "0-5 seconds - No enemies (warm-up)"
    "5-35 seconds - 40 Grunts in 5 lines (no firing)"
    "35-55 seconds - TANK ASSAULT (2 tanks, 20 health each) 🎯"
    "55-85 seconds - 30 Swarm enemies in groups"
    "85-120 seconds - TANK ASSAULT + Mixed enemies 🎯"
    "120-150 seconds - 3 Elite enemies"
    "150-180 seconds - Zigzag enemies"
    "180-220 seconds - Shield enemies"
    "220-260 seconds - Bomber enemies"
    "260-300 seconds - Ghost enemies"
    "300-345 seconds - TANK ASSAULT + Combined Arms (intense) 🎯🔥"
    "345-390 seconds - Mini-Boss encounter"
    "390-420 seconds - Light enemies (recovery)"
    "420-460 seconds - Elite and Shield combo"
    "460-500 seconds - Fast enemies (evasive)"
    "500-540 seconds - TANK ASSAULT + Heavy enemies (very intense) 🎯🔥🔥"
    "540-590 seconds - Final challenge"
)

# Level names
LEVEL_NAMES=(
    "Level 1: Initiation"
    "Level 2: Escalation"
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
    echo "  -i, --invulnerable         Enable invulnerability mode"
    echo "  -d, --debug-log            Enable debug logging to game.log"
    echo "  -p, --phase LEVEL-PHASE    Start at specific level and phase (e.g., 1-12, 2-3)"
    echo "      --phase NUMBER         Start at phase in Level 1 (legacy, e.g., 12)"
    echo "  -l, --list                 List all available phases"
    echo "  -h, --help                 Show this help message"
    echo ""
    echo -e "${YELLOW}Level-Phase Format:${NC}"
    echo "  Format: LEVEL-PHASE"
    echo "    1-0    = Level 1, Start from beginning"
    echo "    1-12   = Level 1, Phase 12 (Mini-Boss)"
    echo "    2-3    = Level 2, Phase 3 (Tank Squadron)"
    echo "    2-0    = Level 2, Start from beginning"
    echo ""
    echo -e "${YELLOW}Examples:${NC}"
    echo "  $0                         # Normal game (Level 1)"
    echo "  $0 -i                      # Invulnerable from start"
    echo "  $0 -d                      # Enable debug logging"
    echo "  $0 -p 1-3                  # Level 1, Tank Squadron"
    echo "  $0 -p 2-0                  # Level 2 from beginning"
    echo "  $0 -p 2-5                  # Level 2, Phase 5"
    echo "  $0 -i -p 1-3               # Tank Squadron with invulnerability"
    echo "  $0 -i -d -p 1-11           # Combined Arms (intense tanks) with all debug features"
    echo "  $0 -p 12                   # Legacy: Level 1, Phase 12"
    echo ""
    echo -e "${YELLOW}Available Levels:${NC}"
    echo "  Level 1 - Initiation (553.82s, ~9 minutes)"
    echo "  Level 2 - Escalation (612.13s, ~10 minutes)"
    echo ""
    echo -e "${YELLOW}Tank Testing Phases (Level 1):${NC}"
    echo "  1-3   - Tank Squadron (clean tank testing)"
    echo "  1-5   - Mixed Assault (tanks + other enemies)"
    echo "  1-11  - Combined Arms (high intensity tanks)"
    echo "  1-16  - Heavy Assault (extreme tank challenge)"
    echo ""
}

# Function to list phases
list_phases() {
    echo -e "${GREEN}================================${NC}"
    echo -e "${GREEN}Available Phases${NC}"
    echo -e "${GREEN}================================${NC}"
    echo ""
    echo -e "${BLUE}Note: Phases work for both Level 1 and Level 2${NC}"
    echo -e "${BLUE}Use format: LEVEL-PHASE (e.g., 1-12 or 2-5)${NC}"
    echo ""
    for i in "${!PHASE_NAMES[@]}"; do
        echo -e "${YELLOW}Phase $i:${NC} ${PHASE_NAMES[$i]}"
        echo -e "  ${BLUE}${PHASE_DESCRIPTIONS[$i]}${NC}"
        echo ""
    done
}

# Function to parse level-phase format
parse_level_phase() {
    local input=$1
    
    # Check if input contains a dash (level-phase format)
    if [[ $input =~ ^([0-9]+)-([0-9]+)$ ]]; then
        LEVEL=${BASH_REMATCH[1]}
        PHASE=${BASH_REMATCH[2]}
        return 0
    # Legacy format: just a number (assume Level 1)
    elif [[ $input =~ ^[0-9]+$ ]]; then
        LEVEL=1
        PHASE=$input
        return 0
    else
        return 1
    fi
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -i|--invulnerable)
            INVULNERABLE="true"
            shift
            ;;
        -d|--debug-log)
            LOGGING="true"
            shift
            ;;
        -p|--phase)
            if [[ -n $2 ]]; then
                if parse_level_phase "$2"; then
                    shift 2
                else
                    echo -e "${RED}Error: Invalid format for --phase${NC}"
                    echo -e "${YELLOW}Use: LEVEL-PHASE (e.g., 1-12, 2-3) or just PHASE for Level 1${NC}"
                    exit 1
                fi
            else
                echo -e "${RED}Error: --phase requires an argument${NC}"
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

# Validate level number
if [[ $LEVEL -lt 1 || $LEVEL -gt 2 ]]; then
    echo -e "${RED}Error: Level must be 1 or 2${NC}"
    echo "Available levels: 1 (Initiation), 2 (Escalation)"
    exit 1
fi

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
echo -e "  Debug Logging: ${LOGGING}"
echo -e "  Starting Level: $LEVEL - ${LEVEL_NAMES[$((LEVEL-1))]}"
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
    
    # First, try to restore from backup if it exists
    if [[ -f "include/constants.h.backup" ]]; then
        mv include/constants.h.backup include/constants.h
    fi
    
    # Explicitly ensure all debug values are set to defaults
    # This provides an extra layer of safety
    sed -i '' 's/#define DEBUG_INVULNERABILITY true/#define DEBUG_INVULNERABILITY false/' include/constants.h 2>/dev/null
    sed -i '' 's/#define DEBUG_LOGGING true/#define DEBUG_LOGGING false/' include/constants.h 2>/dev/null
    sed -i '' 's/#define DEBUG_START_LEVEL [0-9]*/#define DEBUG_START_LEVEL 1/' include/constants.h 2>/dev/null
    sed -i '' 's/#define DEBUG_START_PHASE [0-9]*/#define DEBUG_START_PHASE 0/' include/constants.h 2>/dev/null
    
    # Rebuild the game with restored defaults
    echo -e "${YELLOW}Rebuilding game with default settings...${NC}"
    make clean > /dev/null 2>&1
    if make > /dev/null 2>&1; then
        echo -e "${GREEN}Game rebuilt with default settings!${NC}"
    else
        echo -e "${RED}Warning: Rebuild failed. You may need to run 'make clean && make' manually.${NC}"
    fi
    
    echo -e "${GREEN}Settings restored to defaults!${NC}"
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

# Update logging setting
if [[ $LOGGING == "true" ]]; then
    sed -i '' 's/#define DEBUG_LOGGING false/#define DEBUG_LOGGING true/' include/constants.h
else
    sed -i '' 's/#define DEBUG_LOGGING true/#define DEBUG_LOGGING false/' include/constants.h
fi

# Update level setting
sed -i '' "s/#define DEBUG_START_LEVEL [0-9]*/#define DEBUG_START_LEVEL $LEVEL/" include/constants.h

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

if [[ $LOGGING == "true" ]]; then
    echo -e "${BLUE}[DEBUG LOGGING] game.log will be created${NC}"
fi

if [[ $LEVEL -ne 1 || $PHASE -gt 0 ]]; then
    echo -e "${YELLOW}[DEBUG: Level $LEVEL, Phase $PHASE] will appear in game${NC}"
fi

echo ""
echo -e "${GREEN}Press any key to start the game...${NC}"
read -n 1 -s

# Run the game
./build/shootemup

echo ""
if [[ $LOGGING == "true" && -f "game.log" ]]; then
    echo -e "${BLUE}Debug log saved to: game.log${NC}"
fi
echo -e "${GREEN}Game closed. Original settings have been restored.${NC}"