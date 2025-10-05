# Energy Mode System

## Overview

The player ship now features a dual-mode energy system that allows switching between **OFFENSIVE MODE** and **DEFENSIVE MODE**. Each mode provides different benefits when the energy bar is full, and has a special ability that can be activated.

## Energy Modes

### Offensive Mode
- **Default mode** at game start
- **HUD Color**: Orange/Gold
- **Passive Damage Bonus** (only when energy is FULL):
  - Weapon deals **2x damage** to all enemies
  - Bonus only active when energy bar shows "FULL"
- **Special Ability** (hold **E** key - works with any energy):
  - **Devastating Attack**: Fires continuous spread of bullets
  - Rapid fire with wide spread pattern (-60° to +60°)
  - Drains energy at 20 per second while held
  - Lasts as long as you have energy and hold E
  - Creates a spectacular sustained bullet storm

### Defensive Mode
- **HUD Color**: Cyan/Light Blue  
- **Passive Shield Bonus** (only when energy is FULL):
  - Shield regenerates **2x faster**
  - Bonus only active when energy bar shows "FULL"
- **Special Ability** (hold **E** key - works with any energy):
  - **Enhanced Shield**: Activates much larger, stronger shield
  - Shield absorbs **50% of all damage** while active
  - Shield visual is enlarged (60 units vs normal 40 units)
  - Brighter cyan color with pulsing effect
  - Drains energy at 20 per second while held
  - Lasts as long as you have energy and hold E

## Controls

| Key | Action |
|-----|--------|
| **Q** | Switch between Offensive and Defensive modes |
| **E** | Activate special ability (when energy is full) |
| **SPACE** | Fire weapon |
| **WASD/Arrows** | Move ship |
| **P** | Pause game |

## HUD Display

The enhanced HUD now shows:

1. **HULL**: Ship health bar (red)
2. **SHIELD**: Shield strength bar (cyan, brighter when enhanced shield is active)
3. **ENERGY**: Energy level bar (color matches current mode - orange for offensive, cyan for defensive)
   - Shows "FULL" indicator when at maximum
4. **MODE**: Current mode display
   - Shows "OFFENSIVE" or "DEFENSIVE" in a colored box
   - Color matches the mode (orange or cyan)
5. **Special Ability Status**:
   - In Defensive mode while holding CTRL: Shows "CHARGING..." with progress bar
   - When active: Shows "DEVASTATE!" (offensive) or "ENHANCED!" (defensive) with timer

## Energy Mechanics

- Energy **regenerates slowly** at 2 per second
- After energy reaches 0, there's a **5 second delay** before regeneration starts
- Energy can be gained faster by collecting **Energy Powerups** (purple orbs)
- Special abilities work with **any amount** of energy
- Hold **E** to activate special - drains at 20 energy per second
- The longer you hold E, the longer the special lasts (until energy runs out)
- Strategic: Use short bursts or drain completely depending on situation

## Strategic Gameplay

### When to Use Offensive Mode
- When facing waves of weaker enemies
- To quickly clear the screen during intense combat
- When you want to maximize damage output
- Use devastating attack to clear entire enemy formations

### When to Use Defensive Mode
- When taking heavy damage
- During boss fights or against tough enemies
- When your shield is low and you need fast regeneration
- Use enhanced shield when you need to survive a dangerous phase

### Energy Management
- Plan your special ability usage carefully since energy doesn't regenerate naturally
- Switch modes strategically based on the current combat situation
- Collect energy powerups when you see them
- Save special abilities for critical moments

## Visual Feedback

### Offensive Mode
- Energy bar glows **orange**
- Mode display shows **"OFFENSIVE"** in orange box
- When devastating attack is active, you see massive bullet spread

### Defensive Mode  
- Energy bar glows **cyan/light blue**
- Mode display shows **"DEFENSIVE"** in cyan box
- Shield becomes much larger and brighter when enhanced shield is active
- Shield has additional outer rings and pulsing glow effect

## Implementation Details

### Code Changes
- Added `EnergyMode` enum to `player_ship.h`
- Added mode tracking fields to `PlayerShip` struct
- Modified `HandlePlayerInput()` to handle mode switching and special abilities
- Updated `UpdatePlayerShip()` to apply mode-specific effects
- Modified `DamagePlayerShip()` to apply enhanced shield damage reduction
- Updated `DrawShieldEffect()` for enhanced shield visualization
- Modified weapon system in `weapon.c` to handle devastating attack
- Updated collision system to apply offensive mode damage multiplier
- Enhanced HUD display in `DrawShipHUD()` to show mode and ability status

### Balance Notes
- 2x damage in offensive mode (vs 1x normal)
- 2x shield regen in defensive mode (vs 1x normal)
- Enhanced shield reduces damage by 50%
- Devastating attack fires 20 bullets in spread pattern
- Enhanced shield requires 5 second charge time
- Both special abilities deplete all energy

## Future Enhancements

Potential additions to the energy system:
- Additional energy modes (e.g., Stealth, Overdrive)
- Upgradeable special abilities
- Energy regeneration as an upgrade option
- Mode-specific visual effects for the ship itself
- Combo bonuses for switching modes strategically
