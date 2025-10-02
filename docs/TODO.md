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
- [x] Create/add simple sprites
  - [ ] Capybara in a rocket as player character
  - [x] Different enemy types with unique sprites
  - [x] Animated sprite sheets for movement/actions
  - [x] Explosion animations
  - [x] Projectile sprites (lasers, missiles)
- [x] Add particle effects for:
  - [x] Engine thrust
  - [x] Weapon impacts
  - [x] Explosions
  - [x] Shield effects
- [ ] Implement parallax background scrolling
- [ ] Add space-themed background with stars/planets

## 🎯 Gameplay Features
- [x] Power-ups system
  - [x] Weapon upgrades (spread shot, laser, missiles)
  - [x] Shield/health pickups
  - [x] Speed boost
  - [ ] Score multipliers
- [x] Boss battles at level intervals
- [x] Wave-based enemy spawning with patterns
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
- [x] Statistics tracking (enemies killed, accuracy, time played)
- [ ] Achievements/unlockables
- [x] Pause menu with options

## ⚙️ Technical Improvements
- [x] Implement entity pooling for better performance
- [ ] Add configuration file (JSON/INI) for game settings
- [ ] Improve collision detection with spatial partitioning
- [x] Add delta time for frame-independent movement
- [x] Implement proper game states (Menu, Playing, Paused, GameOver)
- [x] Add debug mode with additional information display

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
  - [x] Wave indicator
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
