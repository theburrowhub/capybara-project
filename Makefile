# Makefile for Raylib Shoot'em Up

# Compiler
CC = clang

# Target executable
TARGET = shootemup

# Source files
SOURCES = main.c

# Compiler flags
CFLAGS = -Wall -std=c99 -O2

# Raylib paths (for macOS with Homebrew)
RAYLIB_PATH = /opt/homebrew/Cellar/raylib
RAYLIB_INCLUDE = -I/opt/homebrew/include
RAYLIB_LIB = -L/opt/homebrew/lib

# Libraries
LIBS = -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

# Build rules
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(RAYLIB_INCLUDE) $(RAYLIB_LIB) $(LIBS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

# Alternative build command if Raylib is installed differently
alternative:
	$(CC) $(SOURCES) -lraylib -lm -lpthread -ldl -o $(TARGET)

.PHONY: all run clean alternative
