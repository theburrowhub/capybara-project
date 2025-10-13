# High Score Presets - Legendary Game Developers

## Overview

The game includes a tool to populate the high scores database with preset scores based on 40 legendary game developers. This provides an impressive and motivating leaderboard for players to compete against.

## Developer Legends by Difficulty

### 🏆 INSANE Difficulty (TOP 1-10)
The cream of the crop - pioneers who defined entire genres and shaped the industry:

| Rank | Developer | Username | Score | Notable Work |
|------|-----------|----------|-------|--------------|
| 1 | Shigeru Miyamoto | shigmoto | 100,000 | Creator of Mario and Zelda |
| 2 | Hideo Kojima | kojimaster | 96,500 | Metal Gear Solid, Death Stranding |
| 3 | John Carmack | carmatron | 94,000 | Doom, Quake (3D Revolution) |
| 4 | John Romero | romeroid | 92,500 | id Software, FPS Pioneer |
| 5 | Sid Meier | simeierX | 91,000 | Civilization |
| 6 | Gabe Newell | gabenator | 90,000 | Valve, Steam |
| 7 | Yu Suzuki | yusonic | 87,800 | Shenmue, Virtua Fighter |
| 8 | Will Wright | simwright | 85,300 | SimCity, The Sims |
| 9 | Todd Howard | howardcore | 82,900 | Skyrim, Fallout |
| 10 | Hironobu Sakaguchi | hirofinal | 80,400 | Final Fantasy |

### 🔥 HARD Difficulty (TOP 11-20)
Masters of their craft - innovators and genre leaders:

| Rank | Developer | Username | Score | Notable Work |
|------|-----------|----------|-------|--------------|
| 11 | Masahiro Sakurai | sakurush | 78,700 | Kirby, Super Smash Bros |
| 12 | Ken Levine | levinecore | 77,900 | BioShock |
| 13 | Yuji Naka | sonaknaka | 76,800 | Sonic the Hedgehog |
| 14 | Kazunori Yamauchi | granYamauchi | 75,600 | Gran Turismo |
| 15 | Markus Persson | notchcode | 74,800 | Minecraft |
| 16 | Roberta Williams | robertadream | 73,900 | King's Quest |
| 17 | Jade Raymond | jadevision | 72,600 | Assassin's Creed |
| 18 | Amy Hennig | hennigstory | 71,800 | Uncharted |
| 19 | Toru Iwatani | iwapac | 70,900 | Pac-Man |
| 20 | Satoru Iwata | iwataheart | 70,000 | Nintendo President |

### 💥 NORMAL Difficulty (TOP 21-30)
Established professionals who left their mark:

| Rank | Developer | Username | Score | Notable Work |
|------|-----------|----------|-------|--------------|
| 21 | Ed Boon | boonfatal | 69,500 | Mortal Kombat |
| 22 | Cory Barlog | barloggamer | 68,700 | God of War |
| 23 | Cliff Bleszinski | cliffyB | 68,000 | Gears of War |
| 24 | Peter Molyneux | molygod | 67,300 | Fable, Black & White |
| 25 | Koji Kondo | kondobeat | 66,800 | Mario/Zelda Music |
| 26 | Shinji Mikami | mikahorror | 66,000 | Resident Evil |
| 27 | Carmine Russo | russoAI | 65,400 | AI Innovation |
| 28 | Eiji Aonuma | aonulink | 65,000 | Zelda Series |
| 29 | Hideki Kamiya | kamiRage | 64,700 | Devil May Cry |
| 30 | Fumito Ueda | uedadreamer | 63,800 | ICO, Shadow of the Colossus |

### 🎯 EASY Difficulty (TOP 31-40)
Rising stars and industry influencers:

| Rank | Developer | Username | Score | Notable Work |
|------|-----------|----------|-------|--------------|
| 31 | Yoko Taro | masktaro | 63,000 | NieR |
| 32 | Tetsuya Nomura | nomustyle | 62,500 | Kingdom Hearts |
| 33 | Jeff Kaplan | kapover | 61,900 | Overwatch |
| 34 | Hidetaka Miyazaki | miyaborn | 61,500 | Dark Souls, Elden Ring |
| 35 | Cory Ledesma | ledesmactrl | 60,800 | WWE Games |
| 36 | Ralph Baer | baertech | 60,000 | First Home Console |
| 37 | Gunpei Yokoi | yokoitech | 59,500 | Game Boy |
| 38 | Phil Spencer | spencore | 58,700 | Xbox Game Pass |
| 39 | Reggie Fils-Aimé | regginator | 58,000 | Nintendo of America |
| 40 | Herman Hulst | hulstcore | 57,500 | PlayStation Studios |

## Automatic Population

**The high scores are populated automatically!**

When you run the game for the first time (or if the database doesn't exist), the system automatically:
1. Creates the database at the appropriate location
2. Creates all necessary tables
3. **Populates the high_scores table with the 40 legendary developers**

No manual action needed - just run the game and the high scores will be there! 🎮

## Manual Reset Tool (Optional)

If you want to reset the high scores back to the original presets, you can use the optional tool:

```bash
# Force repopulate (clear existing scores and restore presets)
make force_populate_highscores
```

### Commands

1. **Normal mode** - Checks if scores exist, only populates if empty
   ```bash
   ./bin/populate_highscores
   ```

2. **Force mode** - Clears existing scores and repopulates with presets
   ```bash
   ./bin/populate_highscores --force
   ```

### Building Only

```bash
make populate_highscores
```

## Technical Details

- **Total Entries**: 40 (10 per difficulty)
- **Database**: SQLite3 at `~/Library/Application Support/CapybaraProject/game.db` (macOS)
- **Auto-cleanup**: Maintains only top 10 per difficulty
- **Timestamps**: Each entry gets a unique timestamp

## Integration

The presets integrate seamlessly with the game's high score system:
- **Automatic population** on first run - no setup required
- Scores appear immediately in the "High Scores" menu
- **Navigate between difficulties** using LEFT/RIGHT arrow keys in the High Scores menu
- Each difficulty displays its own top 10 legendary developers
- Color-coded difficulty selector: 🟢 EASY, 🔵 NORMAL, 🟠 HARD, 🔴 INSANE
- Players can compete against these legendary developers from day one
- New player scores automatically slot into the correct position
- If a player beats a preset, that developer's score is pushed down or removed
- The system only populates once - existing scores are never overwritten automatically

## Motivation

These presets serve multiple purposes:
1. **Inspiration**: Players compete against gaming legends
2. **Challenge**: Meaningful goals for each difficulty level
3. **Education**: Introduction to gaming history and influential developers
4. **Immersion**: Creates a living leaderboard from day one