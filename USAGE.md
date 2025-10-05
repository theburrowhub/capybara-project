# Usage Guide

## Essential Commands

### Build and Run

```bash
make              # Build everything
make run          # Build and run the game
make showcase     # Build and run enemy showcase
```

### Audio Tools

```bash
make run_audio_gui    # Build and run audio analysis GUI
make run_audio_cli    # Build and run audio analysis CLI
```

### Advanced Debugging

For advanced debugging options (invulnerability, phase selection, weapon overheating, debug logging):

```bash
./run_debug_game.sh --help        # Show all options
./run_debug_game.sh -l            # List all phases
./run_debug_game.sh -i            # Invulnerable mode
./run_debug_game.sh -p 3          # Start at specific phase
./run_debug_game.sh -i -o -p 11   # Combined options
```

### Build Management

```bash
make clean        # Remove build artifacts
make rebuild      # Clean and rebuild
make debug        # Build with debug symbols
make release      # Build with optimizations
```

### Help

```bash
make help         # Show all available Makefile targets
```
