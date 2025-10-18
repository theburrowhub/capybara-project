## v0.3.0 (2025-10-18)

### Refactor

- **MAJOR**: Reorganize src/ directory structure with domain-based organization
  - Created 8 specialized directories: input/, gameplay/, rendering/, physics/, effects/, ui/
  - Moved 16 files from mixed src/systems/ to appropriate domain directories
  - Eliminated src/systems/ directory
- **Input System**: Consolidate input handling and eliminate duplication
  - Removed duplicate input detection from menu.c (~100 lines)
  - Removed keyboard fallbacks from player_ship.c and game.c
  - Unified all input through InputManager (single source of truth)
- **Demo Utilities**: Create shared demo_common module
  - Added 9 reusable functions for demo initialization
  - Eliminated ~200 lines of duplicate initialization code across 5 demos
- **Documentation**: Complete update of architecture and API docs
  - Updated ARCHITECTURE.md with new directory structure
  - Reorganized API.md by domain with new sections
  - Added demo utilities documentation

### Impact

- Eliminated ~350+ lines of duplicate code
- Improved code organization and maintainability
- Zero breaking changes to public APIs
- All 13 build targets compile successfully

## v0.2.0a7 (2025-10-14)

### Fix

- improve dependency installation logic in GitHub Actions

## v0.2.0a6 (2025-10-14)

### Fix

- correct package naming logic and enhance error reporting in release workflow

## v0.2.0a5 (2025-10-14)

## v0.2.0a4 (2025-10-13)

## v0.2.0a3 (2025-10-13)

## v0.2.0a2 (2025-10-13)

### Refactor

- update release process in GitHub Actions

## v0.2.0a1 (2025-10-13)

### Fix

- update GitHub Actions workflow to include write permissions for contents

## v0.2.0a0 (2025-10-13)

### Feat

- add build scripts for static linking on Windows and Linux/macOS
- add db for store selected options and high scores (#15)
- add video and sound options to menu system (#13)
- add main menu (#6)

### Fix

-  enemy sprite drawing logic for ENEMY_SWARM case
