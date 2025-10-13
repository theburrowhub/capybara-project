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
2. If yes, **shows name input dialog** over the game screen
3. Player enters their name (up to 31 characters)
4. Press ENTER to save or ESC to use default "Player" name
5. Score is saved to database with the entered name
6. Score appears in the appropriate difficulty level
7. Lowest score is removed if more than 10 entries exist
8. Player returns to main menu and can view their ranking

### 💬 Name Input Dialog

**Visual Design:**
- Semi-transparent overlay over game screen
- Centered dialog box with border effects
- Shows: "NEW HIGH SCORE!" in yellow
- Displays achieved score and difficulty (color-coded)
- Text input field with blinking cursor
- Character counter (31 max)

**Controls:**
- Type any alphanumeric characters and symbols
- BACKSPACE to delete characters
- ENTER to submit (requires at least 1 character)
- ESC to cancel and use default "Player" name

**Features:**
- Prevents submission of empty names
- Visual feedback: ENTER button shows green when name is valid
- Cursor blinks smoothly for better UX
- Input field adapts to different screen sizes

### 🔮 Future Enhancements

Planned improvements:
- [ ] Difficulty selector in main game menu (currently fixed to NORMAL)
- [ ] Achievement indicators (beat specific developers)
- [ ] Filter by date range
- [ ] Export leaderboard to file
- [ ] Online leaderboard integration
- [ ] Name validation (profanity filter)
- [ ] Save last used name as default suggestion

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

