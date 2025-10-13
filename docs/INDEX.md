# Documentation Index

Complete documentation for Capybara Shoot'em Up.

## 🚀 Quick Links

| Need | Read This |
|------|-----------|
| **Create a release** | [RELEASES.md](../RELEASES.md) - Quick reference |
| **CI/CD complete guide** | [CI_CD_COMPLETE_GUIDE.md](CI_CD_COMPLETE_GUIDE.md) - Everything about releases and automation |
| **Build locally** | [BUILD.md](BUILD.md) or [BUILDING.md](../BUILDING.md) |
| **Learn gameplay** | [GAMEPLAY.md](GAMEPLAY.md) |

## 📖 Documentation Structure

### Getting Started
- **[README.md](../README.md)** - Project overview and quick start
- **[RELEASES.md](../RELEASES.md)** - Quick guide to creating releases
- **[BUILDING.md](../BUILDING.md)** - Building the game locally

### CI/CD & Version Management
- **[CI_CD_COMPLETE_GUIDE.md](CI_CD_COMPLETE_GUIDE.md)** - Complete CI/CD documentation
  - Quick start
  - Creating releases (alpha, beta, stable)
  - Commitizen version management
  - Workflows explained
  - Building locally
  - Troubleshooting
  - Reference

### Game Development
- **[BUILD.md](BUILD.md)** - Detailed build instructions
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Project structure and design
- **[API.md](API.md)** - Module interfaces and functions
- **[DEBUG_FEATURES.md](DEBUG_FEATURES.md)** - Testing tools and debug modes
- **[TODO.md](TODO.md)** - Planned features

### Game Content
- **[GAMEPLAY.md](GAMEPLAY.md)** - Controls, mechanics, and strategies
- **[LEVEL_SYSTEM.md](LEVEL_SYSTEM.md)** - Multiple levels system
- **[ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md)** - All 10 enemy types
- **[WEAPONS_AND_DAMAGE.md](WEAPONS_AND_DAMAGE.md)** - Weapon systems and damage
- **[POWERUPS.md](POWERUPS.md)** - 4 powerup types
- **[PLAYER_SHIP.md](PLAYER_SHIP.md)** - Player ship mechanics
- **[ENERGY_MODE_SYSTEM.md](ENERGY_MODE_SYSTEM.md)** - Energy modes

### Specific Features
- **[LEVEL1_PHASES.md](LEVEL1_PHASES.md)** - Level 1 wave progression
- **[LEVEL2_PHASES.md](LEVEL2_PHASES.md)** - Level 2 wave progression
- **[INTERLEVEL_TRANSITION.md](INTERLEVEL_TRANSITION.md)** - Between levels
- **[HIGH_SCORES_UI.md](HIGH_SCORES_UI.md)** - High score system
- **[HIGH_SCORES_PRESETS.md](HIGH_SCORES_PRESETS.md)** - Preset high scores
- **[NAME_INPUT_DIALOG.md](NAME_INPUT_DIALOG.md)** - Name entry
- **[AUDIO_ANALYZER.md](AUDIO_ANALYZER.md)** - Audio analysis tools
- **[ENEMY_TEST_ARENA.md](ENEMY_TEST_ARENA.md)** - Enemy testing

### User Documentation
- **[USER_MANUAL.pdf](USER_MANUAL.pdf)** - Complete user manual (PDF)
- **[USER_MANUAL_README.md](USER_MANUAL_README.md)** - About the manual
- **[USER_MANUAL.tex](USER_MANUAL.tex)** - LaTeX source (full)
- **[USER_MANUAL_SIMPLE.tex](USER_MANUAL_SIMPLE.tex)** - LaTeX source (simple)

### Database
- **[DATABASE_SCHEMA.md](DATABASE_SCHEMA.md)** - Database structure
- **[RESUMEN_DB_HIGHSCORES.md](../RESUMEN_DB_HIGHSCORES.md)** - High scores DB summary

## 🎯 Common Tasks

### I want to create a release
1. **Quick**: [RELEASES.md](../RELEASES.md)
2. **Detailed**: [CI_CD_COMPLETE_GUIDE.md](CI_CD_COMPLETE_GUIDE.md) → "Creating Releases"

### I want to build locally
1. **Quick**: `make` or `./build_static.sh`
2. **Guide**: [BUILD.md](BUILD.md) or [BUILDING.md](../BUILDING.md)

### I want to understand the code
1. **Architecture**: [ARCHITECTURE.md](ARCHITECTURE.md)
2. **API**: [API.md](API.md)

### I want to add a feature
1. **Architecture**: [ARCHITECTURE.md](ARCHITECTURE.md)
2. **TODO**: [TODO.md](TODO.md)
3. **Debug**: [DEBUG_FEATURES.md](DEBUG_FEATURES.md)

### I want to understand gameplay
1. **Gameplay**: [GAMEPLAY.md](GAMEPLAY.md)
2. **Enemies**: [ENEMIES_AND_SPRITES.md](ENEMIES_AND_SPRITES.md)
3. **Weapons**: [WEAPONS_AND_DAMAGE.md](WEAPONS_AND_DAMAGE.md)

## 📁 Documentation Organization

```
docs/
├── INDEX.md (this file)
│
├── CI/CD & Releases
│   └── CI_CD_COMPLETE_GUIDE.md
│
├── Building & Development
│   ├── BUILD.md
│   ├── ARCHITECTURE.md
│   ├── API.md
│   └── DEBUG_FEATURES.md
│
├── Game Content
│   ├── GAMEPLAY.md
│   ├── LEVEL_SYSTEM.md
│   ├── ENEMIES_AND_SPRITES.md
│   ├── WEAPONS_AND_DAMAGE.md
│   ├── POWERUPS.md
│   └── ...
│
└── User Manual
    ├── USER_MANUAL.pdf
    └── USER_MANUAL_*.tex
```

## 🆕 Consolidated Documentation

All CI/CD documentation has been consolidated into [CI_CD_COMPLETE_GUIDE.md](CI_CD_COMPLETE_GUIDE.md):
- CI/CD setup and configuration
- Commitizen usage and workflows
- Version management
- Release processes
- Workflow triggers
- Troubleshooting

**Result**: One comprehensive guide instead of multiple separate files.

## 📝 Contributing to Documentation

When updating documentation:
1. Keep related info in the same file
2. Update INDEX.md if adding new docs
3. Link between related documents
4. Remove outdated information instead of duplicating

---

**Most Popular Documents:**
1. [CI_CD_COMPLETE_GUIDE.md](CI_CD_COMPLETE_GUIDE.md) - For releases
2. [GAMEPLAY.md](GAMEPLAY.md) - For players
3. [BUILD.md](BUILD.md) - For developers
4. [ARCHITECTURE.md](ARCHITECTURE.md) - For contributors
