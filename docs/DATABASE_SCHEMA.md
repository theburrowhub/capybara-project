# Database Schema Documentation

## Overview

The game uses SQLite3 for persistent storage of user settings and high scores. The database is located at:

- **macOS**: `~/Library/Application Support/CapybaraProject/game.db`
- **Linux**: `~/.config/capybara-project/game.db`

## Tables

### settings

Stores user preferences and configuration options.

| Column | Type | Default | Description |
|--------|------|---------|-------------|
| `id` | INTEGER | PRIMARY KEY | Always 1 (single row table) |
| `sound_volume` | REAL | 1.0 | Sound effects volume (0.0 - 1.0) |
| `music_volume` | REAL | 0.5 | Background music volume (0.0 - 1.0) |
| `fullscreen` | INTEGER | 0 | Legacy fullscreen flag (kept for backwards compatibility) |
| `resolution_width` | INTEGER | 1200 | Window width in pixels |
| `resolution_height` | INTEGER | 600 | Window height in pixels |
| `fullscreen_mode` | INTEGER | 0 | Fullscreen mode: 0=Off, 1=Borderless, 2=Exclusive |
| `vsync` | INTEGER | 1 | VSync enabled flag (0=Off, 1=On) |

**Available Resolutions:**
- 800x600
- 1024x768
- 1200x600 (default)
- 1280x720
- 1366x768
- 1600x900
- 1920x1080
- 2560x1440

### high_scores

Stores the top 10 scores per difficulty level.

| Column | Type | Default | Description |
|--------|------|---------|-------------|
| `id` | INTEGER | PRIMARY KEY AUTOINCREMENT | Unique score ID |
| `player_name` | TEXT | - | Player's name (max 64 chars) |
| `score` | INTEGER | - | Final score achieved |
| `difficulty` | INTEGER | - | Difficulty level (0=Easy, 1=Normal, 2=Hard, 3=Insane) |
| `timestamp` | INTEGER | - | Unix timestamp when score was achieved |

**Index:** `idx_difficulty_score` on `(difficulty, score DESC)` for fast queries.

## Schema Migration

The database automatically migrates from older versions when initialized. The migration adds missing columns with default values:

```sql
ALTER TABLE settings ADD COLUMN resolution_width INTEGER DEFAULT 1200;
ALTER TABLE settings ADD COLUMN resolution_height INTEGER DEFAULT 600;
ALTER TABLE settings ADD COLUMN fullscreen_mode INTEGER DEFAULT 0;
ALTER TABLE settings ADD COLUMN vsync INTEGER DEFAULT 1;
```

This ensures backwards compatibility with databases created before video options were added.

## API Functions

### Initialization
```c
bool DB_Init(void);
void DB_Cleanup(void);
const char* DB_GetDatabasePath(void);
```

### Settings Management
```c
bool DB_LoadSettings(UserSettings* settings);
bool DB_SaveSettings(const UserSettings* settings);
```

### High Scores
```c
bool DB_AddHighScore(const char* playerName, int score, DifficultyLevel difficulty);
bool DB_GetHighScores(DifficultyLevel difficulty, HighScoreEntry* entries, int maxEntries, int* outCount);
bool DB_IsHighScore(int score, DifficultyLevel difficulty);
```

## UserSettings Structure

```c
typedef struct {
    // Audio settings
    float soundVolume;
    float musicVolume;
    
    // Video settings
    int resolutionWidth;
    int resolutionHeight;
    int fullscreenMode;  // 0=Off, 1=Windowed, 2=Exclusive
    bool vsync;
    
    // Legacy field (kept for backwards compatibility)
    bool fullscreen;  // Maps to fullscreenMode != 0
} UserSettings;
```

## High Score Management

- Only the **top 10 scores** per difficulty level are kept
- When a new score is added, if it exceeds 10 entries for that difficulty, the lowest score is automatically removed
- Scores are indexed and sorted by difficulty and score (descending) for optimal query performance
- **Automatic population**: The database is automatically populated with 40 legendary developer presets on first run
- The system only populates once - if scores already exist, they are preserved

## Notes

- Settings are automatically saved when changed in the options menu
- The database is created automatically on first run
- All database operations include error handling with descriptive messages
- The legacy `fullscreen` field is maintained for backwards compatibility with older code

