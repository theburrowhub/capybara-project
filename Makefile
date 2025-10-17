# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include $(shell pkg-config --cflags raylib)
LIBS = $(shell pkg-config --libs raylib) -lm -lsqlite3

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

# Source files
CORE_SRCS = $(SRC_DIR)/core/main.c \
            $(SRC_DIR)/core/game.c

ENTITY_SRCS = $(SRC_DIR)/entities/player_ship.c \
              $(SRC_DIR)/entities/enemy_types.c

SYSTEM_SRCS = $(SRC_DIR)/systems/weapon.c \
              $(SRC_DIR)/systems/collision.c \
              $(SRC_DIR)/systems/renderer.c \
              $(SRC_DIR)/systems/wave_system.c \
              $(SRC_DIR)/systems/level_system.c \
              $(SRC_DIR)/systems/level1_waves.c \
              $(SRC_DIR)/systems/level2_waves.c \
              $(SRC_DIR)/systems/projectile_types.c \
              $(SRC_DIR)/systems/explosion.c \
              $(SRC_DIR)/systems/combat_system.c \
              $(SRC_DIR)/systems/projectile_manager.c \
              $(SRC_DIR)/systems/powerup.c \
              $(SRC_DIR)/systems/menu.c \
              $(SRC_DIR)/systems/input_config.c \
              $(SRC_DIR)/systems/input_manager.c

UTIL_SRCS = $(SRC_DIR)/utils/logger.c \
            $(SRC_DIR)/utils/database.c

# Shared audio analysis utilities
AUDIO_ANALYSIS_SRCS = $(SRC_DIR)/utils/audio_analysis.c
AUDIO_ANALYSIS_OBJS = $(AUDIO_ANALYSIS_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# All source files for main game
SRCS = $(CORE_SRCS) $(ENTITY_SRCS) $(SYSTEM_SRCS) $(UTIL_SRCS)

# Object files for main game
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Enemy showcase source files
SHOWCASE_SRCS = $(SRC_DIR)/demo/enemy_showcase.c \
                $(SRC_DIR)/entities/enemy_types.c \
                $(SRC_DIR)/entities/player_ship.c \
                $(SRC_DIR)/systems/projectile_types.c \
                $(SRC_DIR)/systems/weapon.c \
                $(SRC_DIR)/systems/wave_system.c \
                $(SRC_DIR)/systems/level_system.c \
                $(SRC_DIR)/systems/level1_waves.c \
                $(SRC_DIR)/systems/level2_waves.c \
                $(SRC_DIR)/systems/combat_system.c \
                $(SRC_DIR)/systems/projectile_manager.c \
                $(SRC_DIR)/systems/collision.c \
                $(SRC_DIR)/systems/explosion.c \
                $(SRC_DIR)/systems/powerup.c \
                $(SRC_DIR)/systems/input_config.c \
                $(SRC_DIR)/systems/input_manager.c \
                $(SRC_DIR)/utils/logger.c

# Sprite showcase source files
SPRITE_SHOWCASE_SRCS = $(SRC_DIR)/demo/enemy_showcase_sprites.c \
                       $(SRC_DIR)/entities/enemy_types.c \
                       $(SRC_DIR)/systems/projectile_types.c

# Sprite generator source files
SPRITE_GEN_SRCS = $(SRC_DIR)/tools/generate_enemy_sprites.c \
                  $(SRC_DIR)/entities/enemy_types.c \
                  $(SRC_DIR)/systems/projectile_types.c

# Spaceship sprite generator source files
SPACESHIP_GEN_SRCS = $(SRC_DIR)/tools/generate_spaceship_sprites.c \
                     $(SRC_DIR)/entities/enemy_types.c \
                     $(SRC_DIR)/systems/projectile_types.c

# Projectile generator source files
PROJECTILE_GEN_SRCS = $(SRC_DIR)/tools/generate_projectile_sprites.c \
                      $(SRC_DIR)/systems/projectile_types.c \
                      $(SRC_DIR)/entities/enemy_types.c

# Projectile showcase source files
PROJECTILE_SHOWCASE_SRCS = $(SRC_DIR)/demo/projectile_showcase.c \
                           $(SRC_DIR)/systems/projectile_types.c \
                           $(SRC_DIR)/entities/enemy_types.c

# Player ship showcase source files
PLAYER_SHOWCASE_SRCS = $(SRC_DIR)/demo/player_ship_showcase.c \
                       $(SRC_DIR)/entities/player_ship.c \
                       $(SRC_DIR)/systems/projectile_types.c \
                       $(SRC_DIR)/entities/enemy_types.c \
                       $(SRC_DIR)/systems/input_config.c \
                       $(SRC_DIR)/systems/input_manager.c

# Player sprite generator source files
PLAYER_GEN_SRCS = $(SRC_DIR)/tools/generate_player_sprite.c

# High score populator source files
HIGHSCORE_POPULATOR_SRCS = $(SRC_DIR)/tools/populate_highscores.c \
                           $(SRC_DIR)/utils/database.c

# Powerup showcase source files
POWERUP_SHOWCASE_SRCS = $(SRC_DIR)/demo/powerup_showcase.c \
                        $(SRC_DIR)/systems/powerup.c \
                        $(SRC_DIR)/entities/player_ship.c \
                        $(SRC_DIR)/entities/enemy_types.c \
                        $(SRC_DIR)/systems/projectile_types.c \
                        $(SRC_DIR)/systems/input_config.c \
                        $(SRC_DIR)/systems/input_manager.c

# Audio analysis GUI source files
AUDIO_GUI_SRCS = $(SRC_DIR)/demo/audio_analysis_gui.c

# Audio analysis CLI source files  
AUDIO_CLI_SRCS = $(SRC_DIR)/demo/audio_analysis_cli.c

# Object files for showcases
SHOWCASE_OBJS = $(SHOWCASE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
SPRITE_SHOWCASE_OBJS = $(SPRITE_SHOWCASE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
SPRITE_GEN_OBJS = $(SPRITE_GEN_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
SPACESHIP_GEN_OBJS = $(SPACESHIP_GEN_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PROJECTILE_GEN_OBJS = $(PROJECTILE_GEN_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PROJECTILE_SHOWCASE_OBJS = $(PROJECTILE_SHOWCASE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PLAYER_SHOWCASE_OBJS = $(PLAYER_SHOWCASE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
PLAYER_GEN_OBJS = $(PLAYER_GEN_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
POWERUP_SHOWCASE_OBJS = $(POWERUP_SHOWCASE_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
AUDIO_GUI_OBJS = $(AUDIO_GUI_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
AUDIO_CLI_OBJS = $(AUDIO_CLI_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
HIGHSCORE_POPULATOR_OBJS = $(HIGHSCORE_POPULATOR_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executable names
TARGET = $(BIN_DIR)/shootemup
SHOWCASE_TARGET = $(BIN_DIR)/enemy_showcase
SPRITE_SHOWCASE_TARGET = $(BIN_DIR)/enemy_showcase_sprites
SPRITE_GEN_TARGET = $(BIN_DIR)/generate_enemy_sprites
SPACESHIP_GEN_TARGET = $(BIN_DIR)/generate_spaceship_sprites
PROJECTILE_GEN_TARGET = $(BIN_DIR)/generate_projectile_sprites
PROJECTILE_SHOWCASE_TARGET = $(BIN_DIR)/projectile_showcase
PLAYER_SHOWCASE_TARGET = $(BIN_DIR)/player_showcase
PLAYER_GEN_TARGET = $(BIN_DIR)/generate_player_sprite
POWERUP_SHOWCASE_TARGET = $(BIN_DIR)/powerup_showcase
AUDIO_GUI_TARGET = $(BIN_DIR)/audio_analysis_gui
AUDIO_CLI_TARGET = $(BIN_DIR)/audio_analysis_cli
HIGHSCORE_POPULATOR_TARGET = $(BIN_DIR)/populate_highscores

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS += -lGL -lpthread -ldl -lrt -lX11
endif
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Default target - build all binaries
all: directories $(TARGET) $(SHOWCASE_TARGET) $(SPRITE_SHOWCASE_TARGET) $(SPRITE_GEN_TARGET) $(SPACESHIP_GEN_TARGET) $(PROJECTILE_GEN_TARGET) $(PROJECTILE_SHOWCASE_TARGET) $(PLAYER_SHOWCASE_TARGET) $(PLAYER_GEN_TARGET) $(POWERUP_SHOWCASE_TARGET) $(AUDIO_GUI_TARGET) $(AUDIO_CLI_TARGET) $(HIGHSCORE_POPULATOR_TARGET)

# Build only the main game
game: directories $(TARGET)

# Create build and bin directories
directories:
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/entities
	@mkdir -p $(BUILD_DIR)/systems
	@mkdir -p $(BUILD_DIR)/utils
	@mkdir -p $(BUILD_DIR)/demo
	@mkdir -p $(BUILD_DIR)/tools
	@mkdir -p $(BIN_DIR)
	@mkdir -p assets/sprites/enemies

# Link the main executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

# Link the showcase executable
$(SHOWCASE_TARGET): $(SHOWCASE_OBJS)
	$(CC) $(SHOWCASE_OBJS) -o $@ $(LIBS)

# Link the sprite showcase executable
$(SPRITE_SHOWCASE_TARGET): $(SPRITE_SHOWCASE_OBJS)
	$(CC) $(SPRITE_SHOWCASE_OBJS) -o $@ $(LIBS)

# Link the sprite generator executable
$(SPRITE_GEN_TARGET): $(SPRITE_GEN_OBJS)
	$(CC) $(SPRITE_GEN_OBJS) -o $@ $(LIBS)

# Link the spaceship sprite generator executable
$(SPACESHIP_GEN_TARGET): $(SPACESHIP_GEN_OBJS)
	$(CC) $(SPACESHIP_GEN_OBJS) -o $@ $(LIBS)

# Link the projectile generator executable
$(PROJECTILE_GEN_TARGET): $(PROJECTILE_GEN_OBJS)
	$(CC) $(PROJECTILE_GEN_OBJS) -o $@ $(LIBS)

# Link the projectile showcase executable
$(PROJECTILE_SHOWCASE_TARGET): $(PROJECTILE_SHOWCASE_OBJS)
	$(CC) $(PROJECTILE_SHOWCASE_OBJS) -o $@ $(LIBS)

# Link the player showcase executable
$(PLAYER_SHOWCASE_TARGET): $(PLAYER_SHOWCASE_OBJS)
	$(CC) $(PLAYER_SHOWCASE_OBJS) -o $@ $(LIBS)

# Link the player sprite generator executable
$(PLAYER_GEN_TARGET): $(PLAYER_GEN_OBJS)
	$(CC) $(PLAYER_GEN_OBJS) -o $@ $(LIBS)

# Link the powerup showcase executable
$(POWERUP_SHOWCASE_TARGET): $(POWERUP_SHOWCASE_OBJS)
	$(CC) $(POWERUP_SHOWCASE_OBJS) -o $@ $(LIBS)

# Link the audio analysis GUI executable
$(AUDIO_GUI_TARGET): $(AUDIO_GUI_OBJS) $(AUDIO_ANALYSIS_OBJS)
	$(CC) $(AUDIO_GUI_OBJS) $(AUDIO_ANALYSIS_OBJS) -o $@ $(LIBS) -lm

# Link the audio analysis CLI executable
$(AUDIO_CLI_TARGET): $(AUDIO_CLI_OBJS) $(AUDIO_ANALYSIS_OBJS)
	$(CC) $(AUDIO_CLI_OBJS) $(AUDIO_ANALYSIS_OBJS) -o $@ $(LIBS) -lm

# Link the high score populator executable
$(HIGHSCORE_POPULATOR_TARGET): $(HIGHSCORE_POPULATOR_OBJS)
	$(CC) $(HIGHSCORE_POPULATOR_OBJS) -o $@ -lsqlite3

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts (preserves config files)
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	rm -f $(TARGET)
	rm -f *.log *.txt

# Rebuild everything
rebuild: clean all

# Run the game
run: game
	./$(TARGET)

# Run the enemy showcase
showcase: $(SHOWCASE_TARGET)
	./$(SHOWCASE_TARGET)

# Run the sprite showcase
showcase_sprites: sprites $(SPRITE_SHOWCASE_TARGET)
	./$(SPRITE_SHOWCASE_TARGET)

# Build only the showcase
enemy_showcase: directories $(SHOWCASE_TARGET)

# Generate sprites
generate_sprites: directories $(SPRITE_GEN_TARGET)
	./$(SPRITE_GEN_TARGET)

# Generate spaceship sprites
generate_spaceships: directories $(SPACESHIP_GEN_TARGET)
	./$(SPACESHIP_GEN_TARGET)

# Build sprites then run sprite showcase
sprites: generate_sprites showcase_sprites

# Build spaceship sprites then run sprite showcase
spaceships: generate_spaceships showcase_sprites

# Generate projectile sprites
generate_projectiles: directories $(PROJECTILE_GEN_TARGET)
	./$(PROJECTILE_GEN_TARGET)

# Run projectile showcase
projectile_showcase: directories $(PROJECTILE_SHOWCASE_TARGET)
	./$(PROJECTILE_SHOWCASE_TARGET)

# Build and run projectiles demo
projectiles: generate_projectiles projectile_showcase

# Generate player sprite
generate_player: directories $(PLAYER_GEN_TARGET)
	./$(PLAYER_GEN_TARGET)

# Run player showcase
player_showcase: directories $(PLAYER_SHOWCASE_TARGET)
	./$(PLAYER_SHOWCASE_TARGET)

# Build and run player demo
player: generate_player player_showcase

# Run powerup showcase
powerup_showcase: directories $(POWERUP_SHOWCASE_TARGET)
	./$(POWERUP_SHOWCASE_TARGET)

# Build only powerup showcase
build_powerup_showcase: directories $(POWERUP_SHOWCASE_TARGET)

# Build audio analysis GUI
audio_gui: directories $(AUDIO_GUI_TARGET)

# Run audio analysis GUI
run_audio_gui: audio_gui
	./$(AUDIO_GUI_TARGET)

# Build audio analysis CLI
audio_cli: directories $(AUDIO_CLI_TARGET)

# Run audio analysis CLI
run_audio_cli: audio_cli
	./$(AUDIO_CLI_TARGET)

# Build high score populator
populate_highscores: directories $(HIGHSCORE_POPULATOR_TARGET)

# Run high score populator
run_populate_highscores: populate_highscores
	./$(HIGHSCORE_POPULATOR_TARGET)

# Force populate (clear and repopulate)
force_populate_highscores: populate_highscores
	./$(HIGHSCORE_POPULATOR_TARGET) --force

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build with optimizations
release: CFLAGS += -O3
release: all

# Show help
help:
	@echo "Available targets:"
	@echo ""
	@echo "MAIN TARGETS:"
	@echo "  all              - Build all binaries (default)"
	@echo "  game             - Build only the main game"
	@echo "  run              - Build and run the game"
	@echo "  clean            - Remove build artifacts"
	@echo "  rebuild          - Clean and rebuild all"
	@echo "  debug            - Build all with debug symbols"
	@echo "  release          - Build all with optimizations"
	@echo ""
	@echo "VERSION MANAGEMENT (Commitizen):"
	@echo "  cz-help          - Show commitizen help"
	@echo "  cz-commit        - Interactive commit"
	@echo "  cz-bump          - Auto-bump version"
	@echo "  cz-alpha         - Create alpha release"
	@echo "  cz-beta          - Create beta release"
	@echo "  cz-release       - Promote to stable"
	@echo ""
	@echo "SHOWCASE TARGETS:"
	@echo "  showcase         - Build and run enemy showcase"
	@echo "  enemy_showcase   - Build only the enemy showcase"
	@echo "  player_showcase  - Build player ship showcase"
	@echo "  powerup_showcase - Build and run powerup showcase"
	@echo "  showcase_sprites - Run sprite-based enemy showcase"
	@echo ""
	@echo "AUDIO TARGETS:"
	@echo "  audio_gui        - Build audio analysis GUI"
	@echo "  audio_cli        - Build audio analysis CLI"
	@echo "  run_audio_gui    - Build and run audio analysis GUI"
	@echo "  run_audio_cli    - Build and run audio analysis CLI"
	@echo ""
	@echo "DATABASE TOOLS:"
	@echo "  populate_highscores       - Build high score populator"
	@echo "  run_populate_highscores   - Populate database with preset scores"
	@echo "  force_populate_highscores - Clear and repopulate high scores"
	@echo ""
	@echo "GENERATOR TARGETS:"
	@echo "  generate_sprites     - Generate enemy sprites"
	@echo "  generate_spaceships  - Generate spaceship sprites"
	@echo "  generate_projectiles - Generate projectile sprites"
	@echo "  generate_player      - Generate player sprite"
	@echo ""
	@echo "COMBO TARGETS:"
	@echo "  sprites          - Generate sprites and run sprite showcase"
	@echo "  spaceships       - Generate spaceships and run showcase"
	@echo "  projectiles      - Generate projectiles and run showcase"
	@echo "  player           - Generate player sprite and run showcase"
	@echo ""
	@echo "OTHER:"
	@echo "  help             - Show this help message"
	@echo "  manual           - Compile user manual PDF"
	@echo "  clean-manual     - Remove manual build artifacts"

# User Manual targets
manual:
	@echo "Compiling simplified user manual..."
	@if ! command -v pdflatex >/dev/null 2>&1; then \
		echo "Error: pdflatex not found. Please install LaTeX:"; \
		echo "  macOS: brew install --cask mactex"; \
		echo "  Linux: sudo apt install texlive-latex-extra texlive-fonts-extra"; \
		exit 1; \
	fi
	@cd docs && pdflatex -interaction=nonstopmode USER_MANUAL_SIMPLE.tex
	@cd docs && pdflatex -interaction=nonstopmode USER_MANUAL_SIMPLE.tex
	@mv docs/USER_MANUAL_SIMPLE.pdf docs/USER_MANUAL.pdf 2>/dev/null || true
	@echo ""
	@echo "✓ User manual compiled successfully: docs/USER_MANUAL.pdf"

manual-full:
	@echo "Compiling full user manual (requires additional packages)..."
	@echo "Installing missing packages via MacPorts..."
	@echo "Run: sudo port install texlive-latex-extra texlive-fonts-extra"
	@echo ""
	@cd docs && pdflatex -interaction=nonstopmode USER_MANUAL.tex
	@cd docs && pdflatex -interaction=nonstopmode USER_MANUAL.tex
	@echo ""
	@echo "✓ Full user manual compiled: docs/USER_MANUAL.pdf"

clean-manual:
	@echo "Cleaning manual build artifacts..."
	@rm -f docs/USER_MANUAL.aux docs/USER_MANUAL.log docs/USER_MANUAL.out docs/USER_MANUAL.toc
	@rm -f docs/USER_MANUAL_SIMPLE.aux docs/USER_MANUAL_SIMPLE.log docs/USER_MANUAL_SIMPLE.out docs/USER_MANUAL_SIMPLE.toc
	@echo "Manual artifacts cleaned."

clean-manual-all: clean-manual
	@rm -f docs/USER_MANUAL.pdf docs/USER_MANUAL_SIMPLE.pdf
	@echo "Manual PDFs removed."

# ============================================================================
# Commitizen (Version Management) Targets
# ============================================================================

# Check if commitizen is installed
cz-check:
	@command -v cz >/dev/null 2>&1 || { \
		echo "❌ Commitizen is not installed."; \
		echo ""; \
		echo "Install with:"; \
		echo "  pip install commitizen"; \
		echo "  or"; \
		echo "  brew install commitizen"; \
		echo ""; \
		echo "Or run: make cz-install"; \
		exit 1; \
	}
	@echo "✓ Commitizen is installed"
	@cz version

# Install commitizen
cz-install:
	@echo "Installing commitizen..."
	@pip install --user commitizen
	@echo ""
	@echo "✓ Commitizen installed successfully!"
	@echo ""
	@echo "Usage: make cz-commit  # Interactive commit"
	@echo "       make cz-bump    # Auto bump version"
	@echo "       make cz-alpha   # Create alpha release"
	@echo "       make cz-beta    # Create beta release"

# Interactive commit with commitizen
cz-commit: cz-check
	@cz commit

# Show current version
cz-version: cz-check
	@echo "Current version:"
	@cz version --project

# Automatic version bump (determines version from commits)
cz-bump: cz-check
	@echo "🔍 Analyzing commits to determine version bump..."
	@cz bump --yes --changelog
	@echo ""
	@echo "✓ Version bumped and changelog updated!"
	@echo ""
	@echo "Next steps:"
	@echo "  1. Review CHANGELOG.md"
	@echo "  2. git push origin main"
	@echo "  3. git push --tags"

# Manual version bumps
cz-bump-major: cz-check
	@echo "📈 Bumping MAJOR version (breaking changes)..."
	@cz bump --increment MAJOR --yes --changelog
	@echo ""
	@echo "✓ MAJOR version bump complete!"
	@echo "Review changes and push: git push && git push --tags"

cz-bump-minor: cz-check
	@echo "📈 Bumping MINOR version (new features)..."
	@cz bump --increment MINOR --yes --changelog
	@echo ""
	@echo "✓ MINOR version bump complete!"
	@echo "Review changes and push: git push && git push --tags"

cz-bump-patch: cz-check
	@echo "📈 Bumping PATCH version (bug fixes)..."
	@cz bump --increment PATCH --yes --changelog
	@echo ""
	@echo "✓ PATCH version bump complete!"
	@echo "Review changes and push: git push && git push --tags"

# Pre-release versions
cz-alpha: cz-check
	@echo "🔬 Creating ALPHA pre-release..."
	@cz bump --prerelease alpha --yes --changelog
	@echo ""
	@echo "✓ Alpha version created!"
	@echo "Review changes and push: git push && git push --tags"

cz-beta: cz-check
	@echo "🧪 Creating BETA pre-release..."
	@cz bump --prerelease beta --yes --changelog
	@echo ""
	@echo "✓ Beta version created!"
	@echo "Review changes and push: git push && git push --tags"

cz-rc: cz-check
	@echo "🚀 Creating RELEASE CANDIDATE..."
	@cz bump --prerelease rc --yes --changelog
	@echo ""
	@echo "✓ Release candidate created!"
	@echo "Review changes and push: git push && git push --tags"

# Promote pre-release to stable
cz-release: cz-check
	@echo "🎉 Promoting to STABLE release..."
	@cz bump --yes --changelog
	@echo ""
	@echo "✓ Stable release created!"
	@echo "Review changes and push: git push && git push --tags"

# Generate/update changelog only
cz-changelog: cz-check
	@echo "📝 Generating changelog..."
	@cz changelog
	@echo ""
	@echo "✓ Changelog updated!"

# Show commitizen help
cz-help:
	@echo "=========================================="
	@echo "Commitizen Version Management"
	@echo "=========================================="
	@echo ""
	@echo "SETUP:"
	@echo "  make cz-install       - Install commitizen"
	@echo "  make cz-check         - Check if installed"
	@echo "  make cz-version       - Show current version"
	@echo ""
	@echo "COMMITS:"
	@echo "  make cz-commit        - Interactive commit (follows conventions)"
	@echo ""
	@echo "VERSION BUMPS (Automatic):"
	@echo "  make cz-bump          - Auto-detect version from commits"
	@echo ""
	@echo "VERSION BUMPS (Manual):"
	@echo "  make cz-bump-major    - v1.0.0 → v2.0.0 (breaking changes)"
	@echo "  make cz-bump-minor    - v1.0.0 → v1.1.0 (new features)"
	@echo "  make cz-bump-patch    - v1.0.0 → v1.0.1 (bug fixes)"
	@echo ""
	@echo "PRE-RELEASES:"
	@echo "  make cz-alpha         - v1.0.0 → v1.1.0-alpha.0"
	@echo "  make cz-beta          - v1.0.0-alpha.0 → v1.0.0-beta.0"
	@echo "  make cz-rc            - v1.0.0-beta.0 → v1.0.0-rc.0"
	@echo "  make cz-release       - v1.0.0-rc.0 → v1.0.0"
	@echo ""
	@echo "OTHER:"
	@echo "  make cz-changelog     - Update CHANGELOG.md only"
	@echo ""
	@echo "WORKFLOW:"
	@echo "  1. make cz-commit     # Interactive commit"
	@echo "  2. make cz-alpha      # Create alpha release"
	@echo "  3. Test the release"
	@echo "  4. make cz-beta       # Promote to beta"
	@echo "  5. Test more"
	@echo "  6. make cz-release    # Promote to stable"
	@echo "  7. git push && git push --tags"
	@echo ""
	@echo "See docs/COMMITIZEN_GUIDE.md for detailed guide"
	@echo "=========================================="

.PHONY: all game clean rebuild run showcase showcase_sprites enemy_showcase generate_sprites sprites debug release help directories audio_gui audio_cli run_audio_gui run_audio_cli player_showcase projectiles spaceships player powerup_showcase build_powerup_showcase manual manual-full clean-manual clean-manual-all populate_highscores run_populate_highscores force_populate_highscores cz-install cz-commit cz-bump cz-bump-major cz-bump-minor cz-bump-patch cz-alpha cz-beta cz-rc cz-release cz-changelog cz-version cz-check cz-help cz-install cz-commit cz-bump cz-bump-major cz-bump-minor cz-bump-patch cz-alpha cz-beta cz-rc cz-release cz-changelog cz-version cz-check