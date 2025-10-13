# High Scores Menu UI

## Overview

The High Scores menu provides an interactive interface to view the top 10 players for each difficulty level, complete with legendary game developer presets.

## Features

### 🎨 Visual Design

**Difficulty Selector**
- Color-coded for easy identification:
  - 🟢 **EASY**: Green (#64FF64)
  - 🔵 **NORMAL**: Blue (#64C8FF)
  - 🟠 **HARD**: Orange (#FFA500)
  - 🔴 **INSANE**: Red (#FF3232)

**Top 3 Medals**
- 🥇 **1st Place**: Gold glow effect with animated pulsing
- 🥈 **2nd Place**: Silver glow effect
- 🥉 **3rd Place**: Bronze glow effect

**Layout**
- Title at top center
- Difficulty selector with animated arrows (< DIFFICULTY >)
- Table with columns: RANK | PLAYER | SCORE
- Navigation hints at bottom

### 🕹️ Controls

| Action | Keys |
|--------|------|
| Switch Difficulty | LEFT/RIGHT or A/D |
| Return to Menu | ESC, BACKSPACE, ENTER, or SPACE |

The difficulty selector wraps around:
- EASY ← → NORMAL ← → HARD ← → INSANE ← → EASY

### 📊 Display Information

**Table Structure:**
```
RANK    PLAYER          SCORE
  1.    shigmoto       100000
  2.    kojimaster      96500
  3.    carmatron       94000
  ...
```

**Per Difficulty:**
- Shows top 10 scores for the selected difficulty
- Empty state message if no scores exist
- Error message if database query fails

### 🎮 User Experience

**First Time Players:**
1. Open "High Scores" from main menu
2. See legendary developers with impressive scores
3. Get motivated to compete against gaming legends
4. Switch difficulties to see different developer rosters

**Returning Players:**
- See their own scores mixed with developer presets
- Compare performance across difficulty levels
- Track progress and improvement

### ⚡ Dynamic Features

**Animations:**
- Difficulty selector arrows pulse in/out
- Top 3 entries have subtle glow animation
- Smooth color transitions

**Responsive:**
- Adapts to different screen resolutions
- Maintains readability on all supported resolutions (800x600 to 2560x1440)
- Centered layout on all screen sizes

### 🏆 Integration with Gameplay

When a player finishes a game:
1. Game checks if their score qualifies as a high score
2. If yes, automatically saves to database (currently as "Player")
3. Score appears in the appropriate difficulty level
4. Lowest score is removed if more than 10 entries exist
5. Player can immediately see their ranking

### 🔮 Future Enhancements

Planned improvements:
- [ ] Player name input dialog for personalized scores
- [ ] Difficulty selector in main game menu
- [ ] Achievement indicators (beat specific developers)
- [ ] Filter by date range
- [ ] Export leaderboard to file
- [ ] Online leaderboard integration

## Technical Implementation

**Data Source:** SQLite database query
```c
DB_GetHighScores(menu->selectedDifficulty, entries, 10, &count);
```

**State Management:**
- `menu->selectedDifficulty` stores current selection (0-3)
- Initializes to NORMAL (1) on menu load
- Persists during menu navigation
- Resets to NORMAL when returning to main menu

**Rendering:**
- Uses responsive design with `GetScreenWidth()` and `GetScreenHeight()`
- Dynamic text centering and positioning
- Color interpolation for glow effects
- Sin wave animation for arrows

## See Also

- [High Scores Presets](HIGH_SCORES_PRESETS.md) - Legendary developer data
- [Database Schema](DATABASE_SCHEMA.md) - Database structure
- [Gameplay Guide](GAMEPLAY.md) - Complete controls reference

