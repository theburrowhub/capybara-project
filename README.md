# Shoot'em Up Prototype

A simple side-scrolling shoot'em up game prototype built with Raylib.

## Prerequisites

- Raylib library installed
  - macOS: `brew install raylib`
  - Linux: `sudo apt-get install libraylib-dev` (Debian/Ubuntu)
  - Windows: Download from [raylib.com](https://www.raylib.com)

## Build

```bash
make
```

## Run

```bash
make run
# or
./shootemup
```

## Controls

- **Movement**: WASD or Arrow keys
- **Shoot**: Space bar or Left mouse button
- **Show Hitboxes** (debug): H key
- **Restart** (after game over): R key

## Game Features

- Side-scrolling background with stars
- Player spaceship with health system
- **Weapon Overheating System:**
  - Weapon heats up with continuous fire
  - Visual heat bar with color indicators (Green → Yellow → Orange → Red → Purple)
  - Automatic overheat at 100% heat
  - 3-second cooldown period when overheated
  - Passive cooling when not firing
- **Enhanced Enemy System:**
  - Circular enemies with gradient effects
  - Random movement patterns (zigzag, dive, smooth)
  - Unpredictable behavior with individual speeds
  - Size variation for each enemy
- **Procedural obstacles generation:**
  - Orange obstacles: Destructible (2 hits to destroy)
  - Gray obstacles: Indestructible
  - Instant game over on collision with any obstacle
- **Progressive difficulty:**
  - Speed increases every 20 seconds
  - Enemies spawn faster at higher levels
  - Obstacles spawn more frequently
  - Visual speed level indicator with countdown warning
- Score tracking with enemy/obstacle counter
- Debug mode with collision logging (collision_log.txt)
- Death cause display on game over
- Collision detection with visual debugging (H key)

## Clean Build

```bash
make clean
```
