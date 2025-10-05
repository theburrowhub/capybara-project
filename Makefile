# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include $(shell pkg-config --cflags raylib)
LIBS = $(shell pkg-config --libs raylib) -lm

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
              $(SRC_DIR)/systems/static_waves.c \
              $(SRC_DIR)/systems/projectile_types.c \
              $(SRC_DIR)/systems/explosion.c \
              $(SRC_DIR)/systems/combat_system.c \
              $(SRC_DIR)/systems/projectile_manager.c
              $(SRC_DIR)/systems/menu.c

UTIL_SRCS = $(SRC_DIR)/utils/logger.c

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
                $(SRC_DIR)/systems/static_waves.c \
                $(SRC_DIR)/systems/combat_system.c \
                $(SRC_DIR)/systems/projectile_manager.c \
                $(SRC_DIR)/systems/collision.c \
                $(SRC_DIR)/systems/explosion.c \
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
                       $(SRC_DIR)/entities/enemy_types.c

# Player sprite generator source files
PLAYER_GEN_SRCS = $(SRC_DIR)/tools/generate_player_sprite.c

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
AUDIO_GUI_OBJS = $(AUDIO_GUI_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
AUDIO_CLI_OBJS = $(AUDIO_CLI_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

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
AUDIO_GUI_TARGET = $(BIN_DIR)/audio_analysis_gui
AUDIO_CLI_TARGET = $(BIN_DIR)/audio_analysis_cli

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS += -lGL -lpthread -ldl -lrt -lX11
endif
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Default target
all: directories $(TARGET) $(SHOWCASE_TARGET)

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

# Link the audio analysis GUI executable
$(AUDIO_GUI_TARGET): $(AUDIO_GUI_OBJS) $(AUDIO_ANALYSIS_OBJS)
	$(CC) $(AUDIO_GUI_OBJS) $(AUDIO_ANALYSIS_OBJS) -o $@ $(LIBS) -lm

# Link the audio analysis CLI executable
$(AUDIO_CLI_TARGET): $(AUDIO_CLI_OBJS) $(AUDIO_ANALYSIS_OBJS)
	$(CC) $(AUDIO_CLI_OBJS) $(AUDIO_ANALYSIS_OBJS) -o $@ $(LIBS) -lm

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
run: $(TARGET)
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

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build with optimizations
release: CFLAGS += -O3
release: all

# Show help
help:
	@echo "Available targets:"
	@echo "  all              - Build both game and showcase (default)"
	@echo "  enemy_showcase   - Build only the enemy showcase"
	@echo "  generate_sprites - Generate enemy sprite images"
	@echo "  showcase_sprites - Run sprite-based enemy showcase"
	@echo "  sprites          - Generate sprites and run sprite showcase"
	@echo "  player           - Generate player sprite and run player showcase"
	@echo "  player_showcase  - Run the player ship showcase"
	@echo "  audio_gui        - Build the audio analysis GUI"
	@echo "  audio_cli        - Build the audio analysis CLI"
	@echo "  run_audio_gui    - Build and run the audio analysis GUI"
	@echo "  run_audio_cli    - Build and run the audio analysis CLI"
	@echo "  clean            - Remove build artifacts"
	@echo "  rebuild          - Clean and rebuild"
	@echo "  run              - Build and run the game"
	@echo "  showcase         - Build and run the enemy showcase"
	@echo "  debug            - Build with debug symbols"
	@echo "  release          - Build with optimizations"
	@echo "  help             - Show this help message"

.PHONY: all clean rebuild run showcase showcase_sprites enemy_showcase generate_sprites sprites debug release help directories audio_gui audio_cli run_audio_gui run_audio_cli