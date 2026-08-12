# JSON-Based Wave Orchestration Refactor

## Summary

The wave orchestration system has been successfully refactored from hardcoded C arrays to a **JSON-based data-driven architecture**. This allows for easy creation and modification of levels without recompiling the game.

## What Changed

### Files Added

1. **cJSON Library**
   - `include/cJSON.h` - JSON parser header
   - `src/utils/cJSON.c` - JSON parser implementation

2. **JSON Loader Module**
   - `include/json_loader.h` - JSON loading interface
   - `src/utils/json_loader.c` - JSON parsing implementation

3. **Level System Integration**
   - `src/gameplay/level_system_json.c` - Wave plan loader from JSON

4. **JSON Configuration Files**
   - `assets/levels/meta.json` - Meta orchestration file (level order)
   - `assets/levels/level1.json` - Level 1 configuration (136 waves)
   - `assets/levels/level2.json` - Level 2 configuration (127 waves)

### Files Modified

1. **Build System**
   - `Makefile` - Added new source files, removed old wave files
   - `CMakeLists.txt` - Updated for JSON-based system

2. **Level System**
   - `include/level_system.h` - Added `jsonFilePath` field, updated function signatures
   - `src/gameplay/level_system.c` - Loads levels from meta.json dynamically

3. **Wave System**
   - `include/wave_system.h` - Documentation updates
   - `src/gameplay/wave_system.c` - Uses dynamic allocation, calls JSON loader

4. **Documentation**
   - `docs/LEVEL_SYSTEM.md` - Complete rewrite for JSON-based system
   - `docs/ARCHITECTURE.md` - Updated module descriptions
   - `README.md` - Added JSON features highlight

### Files Removed

- `src/gameplay/level1_waves.c` - Replaced by `assets/levels/level1.json`
- `src/gameplay/level2_waves.c` - Replaced by `assets/levels/level2.json`

## Benefits

### 1. Data-Driven Design
- Levels are now data files, not code
- No recompilation needed to modify or add levels
- Game designers can work independently of developers

### 2. Easy Level Creation
Creating a new level is now a 3-step process:
1. Create `levelX.json` with wave definitions
2. Add entry to `meta.json`
3. Add audio file

### 3. Dynamic Memory Management
- Spawn events are allocated based on JSON array size
- Properly freed when no longer needed
- No more hardcoded array limits

### 4. Maintainability
- Wave data is structured and readable
- Easy to validate and debug
- Version control friendly (JSON diffs are clear)

## JSON Structure

### Meta Orchestration (`assets/levels/meta.json`)

```json
{
  "version": "1.0",
  "levels": [
    {"id": 1, "file": "level1.json"},
    {"id": 2, "file": "level2.json"}
  ]
}
```

### Level Configuration (`assets/levels/levelX.json`)

```json
{
  "levelNumber": 1,
  "name": "Initiation",
  "description": "Welcome to the front lines...",
  "audioPath": "assets/audio/level1.mp3",
  "duration": 553.82,
  "targetScore": 5000,
  "waves": [
    {
      "time": 3.0,
      "type": "ENEMY_GRUNT",
      "x": 1250,
      "y": 300,
      "count": 3,
      "interval": 120,
      "pattern": "straight"
    }
  ]
}
```

## Implementation Details

### Enemy Type Mapping

Enemy types are represented as strings in JSON and mapped to C enums:

- `"ENEMY_GRUNT"` → `ENEMY_GRUNT`
- `"ENEMY_BOSS"` → `ENEMY_BOSS`
- etc.

The mapping is handled by `ParseEnemyType()` in `json_loader.c`.

### Memory Management

- **Levels**: Dynamically allocated in `InitLevelManager()`, freed in `CleanupLevelManager()`
- **Spawn Events**: Dynamically allocated per level, freed in `CleanupWaveSystem()`
- **Strings**: All strings in `LevelConfig` are dynamically allocated and properly freed

### Error Handling

The system includes comprehensive error handling:
- File existence checks
- JSON parsing validation
- Graceful degradation on errors
- Detailed error logging

## Migration Statistics

- **Level 1**: 1,485 lines of C code → 1,233 lines of JSON (136 events)
- **Level 2**: 739 lines of C code → 441 lines of JSON (127 events)
- **Total Code Removed**: 2,224 lines of hardcoded wave definitions
- **Total Code Added**: 456 lines (JSON loader + integration)
- **Net Result**: -1,768 lines of code, +1,674 lines of data

## Build Verification

✅ Build completed successfully
✅ No compilation errors
✅ All demos build correctly
✅ JSON files validated and loadable

## Future Enhancements

Possible future improvements:
1. JSON schema validation
2. Level editor tool
3. Hot-reload of level files during development
4. Wave pattern presets
5. Visual wave timeline editor

## Testing

To test the JSON loading:
1. Run the game: `./build/shootemup`
2. Start a new game from the menu
3. Check console output for JSON loading messages
4. Verify level 1 and level 2 play correctly

## Conclusion

The refactoring was completed successfully. The game now uses a modern, data-driven architecture for level design while maintaining full backward compatibility with existing gameplay mechanics.

