# Gameplay Guide

## Controls

### In-Game Controls (All Inputs Work Simultaneously!)

| Action | Keyboard | Gamepad |
|--------|----------|---------|
| **Movement** | WASD or Arrow Keys | Left Stick or D-Pad (always active) |
| **Fire** | Space, **J key** | **R2 / Right Trigger** (always active) |
| **Special Ability** | E, **K key** | **R1 / Right Bumper** (always active) |
| **Switch Weapon Mode** | R, **U key** | L1 / Left Bumper (always active) |
| **Switch Energy Mode** | Q, **I key** | L2 / Left Trigger (always active) |
| **Pause & Exit Menu** | ESC | Gamepad START Button |
| **Debug Mode** | H | - | - |
| **Restart** | R (when game over) | - | - |
| **Quick Fullscreen** | F11 or ALT+ENTER | - | - |

**Note**: 
- ✅ **Keyboard and Gamepad work simultaneously** - switch between them anytime
- ✅ **Gamepad buttons marked "always active"** work even without configuration
- ✅ **Configurable controls** via Options > Controls menu (up to 4 bindings per action)
- ✅ **JKUI keys** form a diamond pattern (like gamepad face buttons) for easy right-hand control!
  - **J** = Fire (bottom, like A button)
  - **K** = Special (right, like B button)
  - **U** = Weapon Mode (left, like X button)
  - **I** = Energy Mode (top, like Y button)
- ✅ Gamepad left stick and D-Pad always work for movement

### Menu Navigation
- **Navigate Options**: UP/DOWN, W/S, or Gamepad D-Pad/Left Stick Up/Down
- **Select**: ENTER, SPACE, or Gamepad A Button
- **Back**: ESC, BACKSPACE, or Gamepad B Button
- **Adjust Settings**: LEFT/RIGHT, D, or Gamepad D-Pad/Left Stick Left/Right
- **Change Difficulty (High Scores)**: LEFT/RIGHT or Gamepad D-Pad/Left Stick to view different difficulty levels
- **Exit Game**: Select **EXIT** from main menu (keyboard or gamepad)

### Name Input (High Score Dialog)
When you achieve a high score, a dialog appears to enter your name:
- **Type Name**: Any alphanumeric characters (max 31)
- **Delete**: BACKSPACE
- **Submit**: ENTER (requires at least 1 character)
- **Use Default**: ESC (saves as "Player")

### Controller Support
- **Full simultaneous support**: Keyboard, Mouse, and Gamepad all work at the same time
- **Gamepad**: Xbox/PlayStation controllers with automatic detection
- **Configurable controls** via in-game menu
- Each action can be mapped to 2 different inputs (e.g., Space + Left Mouse for firing)
- **Hardcoded gamepad support**: Critical gamepad buttons (A, B, X, Y, START) always work for their default actions, even without configuration
- **Movement**: Gamepad left analog stick and D-Pad always work alongside keyboard controls

### Configuring Controls
1. From the main menu, select **Options** (keyboard or gamepad)
2. Select **Controls** from the options menu
3. Navigate with **UP/DOWN** or **D-Pad** to select an action
4. Use **LEFT/RIGHT** or **D-Pad** to choose between Primary or Secondary binding
5. Press **ENTER** or **A Button** to rebind the selected input
6. Press the desired key, mouse button, or gamepad button
7. Press **ESC** or **B Button** to cancel rebinding
8. Select **SAVE CONFIGURATION** to persist your changes
9. Select **RESET TO DEFAULTS** to restore default controls

**Gamepad Status**: The controls menu shows if a gamepad is connected (green text in top-right corner)

**Configuration File**: Controls are saved to `.input_config.conf` in the game directory.

## Game Mechanics

### Player Ship
- **Health**: 10 hit points
- **Movement**: Limited to left half of screen
- **Weapon**: Rapid-fire projectiles with heat management

### Pause & Exit Menu
- Press **ESC** key or **START button** during gameplay to open the pause/exit dialog
- When the dialog appears:
  - The game automatically pauses
  - All game updates are frozen (enemies, projectiles, timers)
  - Music pauses
  - A dialog appears with two options:
    - **RESUME GAME** - Continue playing (default)
    - **EXIT TO MENU** - Return to main menu (game will restart from beginning when you start again)
- Navigate with UP/DOWN, D-Pad, or Left Stick
- Select with ENTER or A Button
- Cancel with ESC, START, or B Button (resumes the game)
- Prevents accidental exits!

### Weapon Heat System
The weapon system prevents continuous spam shooting:
- Heat builds up with each shot (8% per shot)
- Weapon cools down when not firing (30% per second)
- **Overheat** at 100% heat triggers 3-second cooldown
- Visual indicator shows heat level with color coding:
  - 🟢 Green: 0-25% (Cool)
  - 🟡 Yellow: 25-50% (Warm)
  - 🟠 Orange: 50-75% (Warning)
  - 🔴 Red: 75-100% (Critical)
  - 🟣 Purple: Overheated

### Enemies
- 10 unique enemy types with different behaviors
- Each type has unique stats (health, speed, damage, resistance)
- Spawn from the right side of screen
- **Points**: Varies by enemy type (8-50 damage on collision)
- **Full Details**: See [Enemies and Sprites Documentation](ENEMIES_AND_SPRITES.md)

### Obstacles
Two types of obstacles spawn randomly:

#### Destructible (Orange)
- Can be destroyed with 2 hits
- Changes color when damaged
- Worth 50 points when destroyed

#### Indestructible (Gray)
- Cannot be destroyed
- Must be avoided
- Instant death on collision

### Difficulty Progression
The game increases in difficulty over time:
- Speed increases every 20 seconds
- Affects:
  - Enemy movement speed
  - Obstacle movement speed
  - Background scroll speed
  - Spawn rates
- Visual warning 5 seconds before speed increase

## Scoring System

- **Enemy Kills**: Points vary by enemy type
  - Swarm: 80 points
  - Grunt/Speeder: 100-150 points
  - Tank/Elite/Boss: 300-500 points
- **Obstacle Destroyed**: 50 points
- **Survival Bonus**: Points accumulate over time
- **Combo Multiplier**: Chain kills for bonus points

## Game Over Conditions

The game ends when:
- Player health reaches 0 (from enemy collisions)
- Player collides with any obstacle (instant death)

## Tips & Strategies

1. **Positioning**: Stay mobile but avoid the right side
2. **Priority Targets**: Clear destructible obstacles for points
3. **Pattern Recognition**: Learn enemy movement patterns
4. **Risk vs Reward**: Destroying obstacles gives points but risks collision
5. **Collect Powerups**: They drift away quickly, prioritize shield and weapon upgrades

## Debug Features

Press **H** to enable hitbox visualization:
- Green outline: Player hitbox
- Pink outline: Enemy hitboxes
- Magenta outline: Obstacle hitboxes

Collision events are logged to `collision_log.txt` for analysis.

## Advanced Features

### Available Enemy Types
The game includes 10 different enemy types:
- **Grunt, Tank, Speeder, Zigzag, Bomber**
- **Shield, Swarm, Elite, Ghost, Boss**
- Each with unique behaviors and weapon configurations
- See [Enemies and Sprites](ENEMIES_AND_SPRITES.md) for details

### Projectile System
4 projectile types with different properties:
- **Laser**: Fast, straight shots
- **Plasma**: Medium speed with homing
- **Missile**: Slow but high damage with strong homing
- **Energy Orb**: Area damage with piercing
- See [Weapons and Damage](WEAPONS_AND_DAMAGE.md) for details

## Planned Features

See [TODO List](TODO.md) for upcoming features including:
- Power-ups and upgrades
- Boss battle phases
- Controller support
- Capybara-themed sprites
- Local co-op mode
