# Combat System Refactoring - Testing Plan

## ✅ Refactoring Complete!

**What was done:**
- Created `src/systems/combat_system.c` with generic combat functions
- Refactored `src/core/game.c` to use the combat system (~60 lines eliminated)
- Refactored `src/demo/enemy_showcase.c` to use the combat system (~60 lines eliminated)
- Updated Makefile to include combat_system.c
- **Total code duplication eliminated: ~120 lines**

---

## 🧪 Testing Instructions

### Phase 1: Test Main Game (All Enemy Types)

**Goal:** Verify that enemies fire correctly in the main game

```bash
./bin/shootemup
```

**Test Checklist:**

#### 1. Basic Enemies (First Wave - 0-60s)
- [ ] **Grunt**: Fires single laser shots (1/s)
  - Should fire straight at player
  - Yellow laser projectiles
  
- [ ] **Swarm**: Fires rapid weak lasers (2/s)
  - Small orange ships
  - Fast fire rate

#### 2. Tank Enemies (Tank Assault - 35-55s, 300-320s)
- [ ] **Tank**: Fires 3-missile barrage (0.6/s)
  - Advances, pauses, fires heavily
  - 3 missiles with 20° spread
  - Mix of missiles and energy orbs
  - **Critical test:** Ensure they fire while paused

#### 3. Speed Enemies (Speeder phases)
- [ ] **Speeder**: Fires rapid 3-shot bursts (3/s)
  - Very fast movement
  - 3 lasers with 15° spread
  - High fire rate

#### 4. Special Enemies (Mid-game)
- [ ] **Zigzag**: Fires 2 plasma shots (1.5/s)
  - Erratic movement
  - 30° spread
  - Mix of plasma and laser
  
- [ ] **Bomber**: Fires heavy energy orbs (0.8/s)
  - Large projectiles
  - Slow fire rate but high damage
  
- [ ] **Shield**: Fires in ALL DIRECTIONS (1.2/s)
  - **Critical test:** 6 shots in 360° pattern
  - Rotating shield visible
  - Plasma projectiles

#### 5. Elite Enemies (Late game - 120s+)
- [ ] **Elite**: Fires 4-shot spread (2/s)
  - 45° spread angle
  - Mix of plasma and missiles
  - Advance-pause-retreat pattern
  
- [ ] **Ghost**: Fires 2 plasma shots (1/s)
  - **Critical test:** Only fires when visible
  - 20° spread
  - Should NOT fire when phased out

#### 6. Boss Fight (427-451s)
- [ ] **Boss**: Multiple weapon patterns
  - Various projectile types
  - Heavy barrage
  - Hovers in last third of screen

### Phase 2: Test Enemy Showcase (Individual Testing)

**Goal:** Verify showcase enemies fire correctly

```bash
./bin/enemy_showcase
```

**Test Checklist:**

For **EACH enemy type** in the showcase:
1. Click the enemy in the grid
2. Enter the test arena
3. Observe firing behavior

#### Test Each Enemy:
- [ ] **Grunt** - Single straight shots
- [ ] **Tank** - 3-missile spread (watch during pause)
- [ ] **Speeder** - Rapid 3-shot bursts
- [ ] **Zigzag** - 2-shot spread while zigzagging
- [ ] **Bomber** - Heavy single shots
- [ ] **Shield** - **360° circular pattern** (most important!)
- [ ] **Swarm** - Rapid single shots
- [ ] **Elite** - 4-shot spread pattern
- [ ] **Ghost** - Fires only when visible
- [ ] **Boss** - Complex multi-pattern

### Phase 3: Behavioral Verification

**Goal:** Ensure behavior is identical in both game and showcase

#### Test: Shield Enemy (Most Complex)
1. **In Main Game:** Watch shield enemy fire 360° pattern
2. **In Showcase:** Click shield, watch it fire 360° pattern
3. **Verify:** Both should fire 6 projectiles in a circle

#### Test: Ghost Enemy (Special Condition)
1. **In Main Game:** Watch ghost phase in/out, only fires when visible
2. **In Showcase:** Watch ghost phase in/out, only fires when visible
3. **Verify:** Firing behavior matches

#### Test: Tank Enemy (Movement + Firing)
1. **In Main Game:** Watch tank advance, pause, fire barrage, retreat
2. **In Showcase:** Watch tank advance, pause, fire barrage, retreat
3. **Verify:** Fires 3 missiles while paused in both

---

## 🐛 What to Report if Something is Broken

### If Enemy Doesn't Fire:
**Symptoms:**
- Enemy appears but doesn't shoot
- No projectiles visible

**Report:**
```
Enemy type: [e.g., Tank, Shield, etc.]
Where: [Main game / Showcase]
Expected: Should fire [X] shots at [Y] rate
Actual: No projectiles appear
```

### If Projectile Pattern is Wrong:
**Symptoms:**
- Wrong number of projectiles
- Wrong spread angle
- Wrong projectile type

**Report:**
```
Enemy type: [e.g., Shield]
Where: [Main game / Showcase]
Expected: 6 shots in 360° circle
Actual: [describe what you see]
```

### If Fire Rate is Wrong:
**Symptoms:**
- Fires too fast or too slow
- Continuous fire instead of intervals

**Report:**
```
Enemy type: [e.g., Speeder]
Where: [Main game / Showcase]
Expected: 3 shots/second
Actual: [describe the rate]
```

### If Behavior Differs Between Game and Showcase:
**Symptoms:**
- Enemy fires in game but not in showcase (or vice versa)
- Different patterns

**Report:**
```
Enemy type: [e.g., Ghost]
Main game behavior: [describe]
Showcase behavior: [describe]
Difference: [explain]
```

---

## ✅ Success Criteria

The refactoring is successful if:

1. **All enemies fire** in both game and showcase
2. **Fire patterns match** the enemy weapon configurations
3. **Fire rates match** the configured rates
4. **Special behaviors work** (Ghost phasing, Shield 360°, etc.)
5. **No crashes** in either game or showcase
6. **Behavior is identical** between game and showcase

---

## 🔧 Quick Fix Commands

If you need to rebuild:
```bash
make clean
make all
```

If only showcase changed:
```bash
make enemy_showcase
```

If only main game changed:
```bash
make
```

---

## 📊 Testing Progress

Track your progress:

**Main Game:**
- [ ] Grunt
- [ ] Tank
- [ ] Speeder
- [ ] Zigzag
- [ ] Bomber
- [ ] Shield
- [ ] Swarm
- [ ] Elite
- [ ] Ghost
- [ ] Boss

**Showcase:**
- [ ] Grunt
- [ ] Tank
- [ ] Speeder
- [ ] Zigzag
- [ ] Bomber
- [ ] Shield
- [ ] Swarm
- [ ] Elite
- [ ] Ghost
- [ ] Boss

**Both Match:**
- [ ] Grunt
- [ ] Tank
- [ ] Speeder
- [ ] Zigzag
- [ ] Bomber
- [ ] Shield
- [ ] Swarm
- [ ] Elite
- [ ] Ghost
- [ ] Boss

---

## 🎯 Priority Tests

If you have limited time, test these **critical** cases first:

1. **Shield** - Most complex (360° pattern)
2. **Tank** - Fires during pause behavior
3. **Ghost** - Conditional firing (only when visible)
4. **Boss** - Multiple projectile types
5. **One basic enemy** (Grunt or Swarm) - Verify basics work

---

## 📝 Notes

- Player is invincible in showcase (projectiles pass through)
- Main game has full collision detection
- Both use the same `Combat_FireEnemyProjectile()` function now
- Fire rates are defined in `projectile_types.c` (EnemyWeaponConfig)

Happy testing! 🚀
