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
- [x] Allow configurable screen size
  - [x] Add resolution options in settings (8 presets from 800x600 to 2560x1440)
  - [x] Maintain aspect ratio when resizing
  - [x] Settings persisted in database
- [x] Implement fullscreen mode toggle (F11 or Alt+Enter)
- [x] Add windowed fullscreen option (3 modes: Windowed, Borderless, Exclusive)
- [x] Implement proper scaling for different resolutions
- [x] VSync toggle option

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
- [x] High score system with leaderboard
  - [x] SQLite database for persistent storage
  - [x] Top 10 scores per difficulty level
  - [x] High scores menu screen
  - [x] Automatic schema migration for existing databases
  - [ ] Player name input dialog
- [x] User settings persistence
  - [x] Audio settings (sound & music volume)
  - [x] Video settings (resolution, fullscreen mode, vsync)
  - [x] All settings saved to SQLite database
  - [x] Automatic migration for legacy databases
- [x] Statistics tracking (enemies killed, accuracy, time played)
- [ ] Achievements/unlockables
- [x] Pause menu with options

## ⚙️ Technical Improvements
- [x] Implement entity pooling for better performance
- [x] Add database for persistent storage (SQLite)
- [ ] Improve collision detection with spatial partitioning
- [x] Add delta time for frame-independent movement
- [x] Implement proper game states (Menu, Playing, Paused, GameOver)
- [x] Add debug mode with additional information display

## 🎨 UI/UX Improvements
- [x] Main menu screen
  - [x] Start game
  - [x] Settings (with submenus)
  - [x] High scores
  - [x] Credits
  - [x] Exit (ESC in main menu)
- [x] Settings menu
  - [x] Sound options submenu (with database persistence)
  - [x] Video options submenu (resolution, fullscreen, vsync with database persistence)
  - [ ] Game options submenu (placeholder for future options)
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
