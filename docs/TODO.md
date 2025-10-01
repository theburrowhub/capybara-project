# TODO List - Capybara Shoot'em Up

## 🎮 Input & Controls
- [ ] Add Xbox controller support
  - [ ] Map movement to left analog stick
  - [ ] Map shooting to right trigger or A button
  - [ ] Add vibration feedback for collisions/shooting
- [ ] Add PlayStation controller support
- [ ] Implement configurable key bindings
- [ ] Add mouse aiming option

## 🖥️ Display & Graphics
- [ ] Allow configurable screen size
  - [ ] Add resolution options in settings
  - [ ] Maintain aspect ratio when resizing
- [ ] Implement fullscreen mode toggle (F11 or Alt+Enter)
- [ ] Add windowed fullscreen option
- [ ] Implement proper scaling for different resolutions

## 🎨 Sprites & Visual Assets
- [ ] Create/add simple sprites
  - [ ] Capybara in a rocket as player character
  - [ ] Different enemy types with unique sprites
  - [ ] Animated sprite sheets for movement/actions
  - [ ] Explosion animations
  - [ ] Projectile sprites (lasers, missiles)
- [ ] Add particle effects for:
  - [ ] Engine thrust
  - [ ] Weapon impacts
  - [ ] Explosions
  - [ ] Shield effects
- [ ] Implement parallax background scrolling
- [ ] Add space-themed background with stars/planets

## 🎯 Gameplay Features
- [ ] Power-ups system
  - [ ] Weapon upgrades (spread shot, laser, missiles)
  - [ ] Shield/health pickups
  - [ ] Speed boost
  - [ ] Score multipliers
- [ ] Boss battles at level intervals
- [ ] Wave-based enemy spawning with patterns
- [ ] Combo system for chaining enemy kills
- [ ] Different difficulty modes (Easy, Normal, Hard, Insane)

## 🔊 Audio
- [ ] Add sound effects
  - [ ] Shooting sounds
  - [ ] Explosion sounds
  - [ ] Hit/damage sounds
  - [ ] Power-up collection sounds
  - [ ] Warning sounds for low health/overheat
- [ ] Background music
  - [ ] Dynamic music that intensifies with action
  - [ ] Boss battle themes
- [ ] Volume controls for SFX and music

## 💾 Game Systems
- [ ] Save/Load game state
- [ ] High score system with leaderboard
- [ ] Statistics tracking (enemies killed, accuracy, time played)
- [ ] Achievements/unlockables
- [ ] Pause menu with options

## ⚙️ Technical Improvements
- [ ] Implement entity pooling for better performance
- [ ] Add configuration file (JSON/INI) for game settings
- [ ] Improve collision detection with spatial partitioning
- [ ] Add delta time for frame-independent movement
- [ ] Implement proper game states (Menu, Playing, Paused, GameOver)
- [ ] Add debug mode with additional information display

## 🎨 UI/UX Improvements
- [ ] Main menu screen
  - [ ] Start game
  - [ ] Settings
  - [ ] High scores
  - [ ] Credits
  - [ ] Exit
- [ ] Settings menu
  - [ ] Graphics options
  - [ ] Audio options
  - [ ] Control configuration
- [ ] HUD improvements
  - [ ] Mini-map or radar
  - [ ] Boss health bar
  - [ ] Combo counter
  - [ ] Wave indicator
- [ ] Smooth transitions between screens

## 🚀 Future Enhancements
- [ ] Multiplayer support (local co-op)
- [ ] Level editor
- [ ] Mod support
- [ ] Steam integration (if publishing)
- [ ] Mobile port considerations

## 🐛 Known Issues to Fix
- [ ] Optimize rendering for better FPS on lower-end systems
- [ ] Review and improve memory management
- [ ] Add proper error handling for file I/O operations

---
*Priority items are marked with 🔴 (high), 🟡 (medium), or 🟢 (low)*

Feel free to add more items or mark completed ones with [x]!
