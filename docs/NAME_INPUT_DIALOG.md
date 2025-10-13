# Name Input Dialog System

## Overview

The name input dialog appears automatically when a player achieves a score that qualifies for the top 10 in the current difficulty level. It provides a polished interface for personalizing high score entries.

## When It Appears

The dialog is triggered when:
1. **Game ends** (either Game Over or Victory)
2. Player presses **ESC** to return to menu
3. The score **qualifies** as a high score for the current difficulty
4. The check uses `DB_IsHighScore(score, difficulty)` to determine eligibility

## Visual Design

### Layout
```
┌──────────────────────────────────┐
│      NEW HIGH SCORE!             │
│                                  │
│      Score: 75000                │
│      Difficulty: HARD            │
│                                  │
│      Enter your name:            │
│   ┌──────────────────────┐      │
│   │ PlayerName_          │      │
│   └──────────────────────┘      │
│                     12/31        │
│                                  │
│   Press ENTER to save            │
│   Press ESC to use default name  │
└──────────────────────────────────┘
```

### Colors and Effects

**Dialog Box:**
- Semi-transparent black overlay (alpha: 180)
- Dark blue box with double border
- Outer border: Light blue (#64, #96, #FF)
- Inner border: Medium blue (#3C, #5A, #B4)
- Drop shadow for depth

**Title:**
- "NEW HIGH SCORE!" in YELLOW (#FFFF00)
- Size: 36px

**Score:**
- White text, 24px
- Format: "Score: XXXXX"

**Difficulty:**
- Color-coded by level:
  - 🟢 EASY: Green (#64FF64)
  - 🔵 NORMAL: Blue (#64C8FF)
  - 🟠 HARD: Orange (#FFA500)
  - 🔴 INSANE: Red (#FF3232)
- Size: 20px

**Input Field:**
- Dark background (#0A0A14)
- White border
- White text (24px)
- Blinking cursor (underscore)

**Instructions:**
- ENTER instruction: Green when name is valid, gray when empty
- ESC instruction: Light gray
- Size: 16px

## Controls

### Text Input
- **Allowed Characters**: ASCII 32-125 (alphanumeric + symbols)
- **Maximum Length**: 31 characters
- **Real-time Feedback**: Character counter updates as you type

### Actions

| Key | Action |
|-----|--------|
| Type | Add character to name (if under 31 chars) |
| BACKSPACE | Delete last character |
| ENTER | Submit name (requires ≥1 character) |
| ESC | Cancel and use "Player" as default name |

### Behavior

**On ENTER:**
1. Validates that name is not empty
2. Calls `FinishNameInput(menu)` which:
   - Saves score to database with entered name
   - Prints confirmation to console
   - Sets `nameInputActive = false`
3. Transitions to main menu
4. Highlights "High Scores" option
5. Game cleanup occurs

**On ESC:**
1. Sets name to "Player"
2. Calls `FinishNameInput(menu)` with default name
3. Transitions to main menu
4. Game cleanup occurs

## Technical Implementation

### Data Structure

Added to `Menu` struct:
```c
char playerName[32];      // Input buffer
int nameLength;           // Current length
int pendingScore;         // Score to save
int pendingDifficulty;    // Difficulty level
bool nameInputActive;     // Dialog active flag
float nameInputBlink;     // Cursor animation timer
```

### Key Functions

#### `StartNameInput(Menu* menu, int score, int difficulty)`
Initializes the name input dialog:
- Sets menu state to `MENU_NAME_INPUT`
- Stores pending score and difficulty
- Clears input buffer
- Resets blink animation

#### `DrawNameInput(Menu* menu)`
Renders the dialog:
- Semi-transparent overlay
- Centered dialog box
- Score and difficulty display
- Input field with cursor
- Character counter
- Instructions

#### `FinishNameInput(Menu* menu)`
Completes the input process:
- Validates name length > 0
- Saves to database via `DB_AddHighScore()`
- Prints confirmation
- Cleans up state

#### `IsNameInputActive(const Menu* menu)`
Returns whether dialog is currently active

### Integration Points

**In `main.c`:**
```c
// When game ends and player presses ESC
if (game.gameOver && IsKeyPressed(KEY_ESCAPE)) {
    if (DB_IsHighScore(game.score, currentDifficulty)) {
        StartNameInput(&menu, game.score, currentDifficulty);
        awaitingNameInput = true;
    } else {
        // No high score, go to menu
    }
}

// Check if name input finished
if (awaitingNameInput && !IsNameInputActive(&menu)) {
    // Cleanup game and return to menu
}
```

**In `menu.c` UpdateMenu():**
```c
case MENU_NAME_INPUT:
    // Capture keyboard input
    int key = GetCharPressed();
    // Handle backspace
    // Submit on ENTER
    // Cancel on ESC
```

## Animation Details

### Cursor Blink
- **Speed**: 2.0 Hz (updates with `deltaTime * 2.0f`)
- **Effect**: Sine wave interpolation for smooth fade
- **Range**: 0-255 alpha
- **Formula**: `(sin(time) + 1.0) / 2.0`

### Arrow Animations (in high scores view)
- **Speed**: 4.0 Hz
- **Offset**: ±3 pixels
- **Creates**: Pulsing arrow effect for difficulty selector

## Error Handling

**Empty Name Prevention:**
- ENTER button is visually disabled (gray) when name is empty
- ENTER key press is ignored if `nameLength == 0`
- User must type at least 1 character to submit

**Length Limit:**
- Characters beyond 31 are silently ignored
- Character counter shows limit clearly
- Cursor stops blinking at max length

**Database Failure:**
- If `DB_AddHighScore()` fails, error is logged to console
- Player is still returned to menu (graceful degradation)
- High score is not saved but game continues normally

## User Experience Flow

### Successful High Score
1. Player dies/wins → Game Over screen
2. Press ESC to exit → System checks if high score
3. **Dialog appears** over game screen
4. Player types name → See live feedback
5. Press ENTER → Score saved
6. Return to main menu → See "High Scores" highlighted
7. Navigate to High Scores → See new entry in table

### Non-High Score
1. Player dies/wins → Game Over screen
2. Press ESC to exit → System checks if high score
3. **No dialog** → Direct to main menu
4. No database write occurs

## Accessibility

- **Visual Clarity**: Large fonts, high contrast
- **Clear Instructions**: Multiple prompts guide user
- **Forgiving**: ESC provides easy exit
- **Feedback**: Character counter, cursor blink, color changes
- **Responsive**: Works at all resolutions

## See Also

- [High Scores Presets](HIGH_SCORES_PRESETS.md) - Legendary developer data
- [High Scores UI](HIGH_SCORES_UI.md) - Main high scores interface
- [Database Schema](DATABASE_SCHEMA.md) - Database structure
- [Gameplay Guide](GAMEPLAY.md) - Complete controls reference

