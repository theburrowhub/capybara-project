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

ENTITY_SRCS = $(SRC_DIR)/entities/player.c \
              $(SRC_DIR)/entities/enemy.c \
              $(SRC_DIR)/entities/obstacle.c

SYSTEM_SRCS = $(SRC_DIR)/systems/weapon.c \
              $(SRC_DIR)/systems/collision.c \
              $(SRC_DIR)/systems/renderer.c

UTIL_SRCS = $(SRC_DIR)/utils/logger.c

# All source files
SRCS = $(CORE_SRCS) $(ENTITY_SRCS) $(SYSTEM_SRCS) $(UTIL_SRCS)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executable name
TARGET = $(BIN_DIR)/shootemup

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS += -lGL -lpthread -ldl -lrt -lX11
endif
ifeq ($(UNAME_S),Darwin)
    LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Default target
all: directories $(TARGET)

# Create build and bin directories
directories:
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/entities
	@mkdir -p $(BUILD_DIR)/systems
	@mkdir -p $(BUILD_DIR)/utils
	@mkdir -p $(BIN_DIR)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
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

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Release build with optimizations
release: CFLAGS += -O3
release: all

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the game (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and rebuild"
	@echo "  run      - Build and run the game"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build with optimizations"
	@echo "  help     - Show this help message"

.PHONY: all clean rebuild run debug release help directories