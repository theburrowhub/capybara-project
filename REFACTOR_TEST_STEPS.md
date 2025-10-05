# Refactoring Test Steps - Combat & Projectile Systems

## ✅ Refactoring Complete!

**Modules Created:**
1. `projectile_manager.c` - Generic projectile lifecycle management
2. Enhanced `collision.c` - Generic bullet-enemy collision system

**Code Eliminated:**
- ~50 lines from game.c and enemy_showcase.c (total ~100 lines)
- Now using shared modules for projectiles and collisions

---

## 🧪 TESTING STEPS

### **Phase 1: Quick Smoke Test (5 minutes)**

#### Test 1.1: Main Game Launch
```bash
./bin/shootemup
```

**Expected:**
- ✅ Game starts without crash
- ✅ Player can move and shoot
- ✅ Background scrolls

**If it crashes:** Report error message immediately

---

#### Test 1.2: Basic Enemy Engagement (55-90 seconds into game)
```bash
./bin/shootemup
```

**Wait until ~55 seconds (enemies start appearing)**

**Test:**
- [ ] Enemies appear from right side
- [ ] Shoot enemies with SPACE
- [ ] Enemies take damage and die
- [ ] Score increases when enemies destroyed

**Expected:** Enemies can be destroyed normally

**If broken:** Report which part fails (spawning, damage, destruction, score)

---

#### Test 1.3: Enemy Showcase Launch
```bash
./bin/enemy_showcase
```

**Test:**
- [ ] Grid screen appears with all 10 enemies
- [ ] Click any enemy (try Grunt first)
- [ ] Arena mode loads
- [ ] Player appears on left
- [ ] Enemy spawns from right

**Expected:** Showcase loads and transitions to arena

**If broken:** Report at which step it fails

---

### **Phase 2: Enemy Firing Test (10 minutes)**

#### Test 2.1: Shield Enemy (360° Pattern) - CRITICAL TEST
```bash
./bin/enemy_showcase
```

**Steps:**
1. Click **Shield** enemy in grid
2. Wait for shield to spawn and approach
3. **Watch for projectiles**

**Expected:**
- ✅ Shield fires **6 projectiles** in a **circle** (360°)
- ✅ Projectiles spread in all directions
- ✅ Fires at ~1.2 shots/second
- ✅ Projectiles are **plasma** (cyan/purple color)

**If broken:** 
```
Report:
- Number of projectiles: [count]
- Pattern: [describe what you see]
- Fire rate: [too fast/slow/none]
```

---

#### Test 2.2: Tank Enemy (Burst Pattern) - CRITICAL TEST
```bash
./bin/enemy_showcase
```

**Steps:**
1. Click **Tank** enemy in grid
2. Wait for tank to spawn
3. Tank will: Advance → Pause → **Fire** → Retreat
4. **Watch for missiles when tank pauses**

**Expected:**
- ✅ Tank fires **3 missiles** in a spread pattern
- ✅ Missiles have slight angle (20° spread)
- ✅ Fires at ~0.6 shots/second
- ✅ Mix of missiles and energy orbs

**If broken:**
```
Report:
- Does tank pause: [yes/no]
- Does tank fire: [yes/no]
- Number of missiles: [count]
- Pattern: [describe]
```

---

#### Test 2.3: Ghost Enemy (Conditional Firing) - CRITICAL TEST
```bash
./bin/enemy_showcase
```

**Steps:**
1. Click **Ghost** enemy in grid
2. Watch ghost carefully
3. Ghost will phase in and out (transparent/solid)
4. **Ghost should ONLY fire when solid (visible)**

**Expected:**
- ✅ Ghost fires **2 projectiles** when visible
- ✅ Ghost does **NOT fire** when phased out (transparent)
- ✅ 20° spread pattern
- ✅ Plasma projectiles

**If broken:**
```
Report:
- Fires when phased: [yes/no]
- Fires when visible: [yes/no]
- Pattern: [describe]
```

---

### **Phase 3: All Enemy Types (15 minutes)**

Test each enemy in the showcase individually:

#### Test 3.1: Grunt
```bash
./bin/enemy_showcase → Click Grunt
```
**Expected:** Single laser straight at player (1/s)

---

#### Test 3.2: Speeder
```bash
./bin/enemy_showcase → Click Speeder
```
**Expected:** 3 rapid laser bursts (3/s), 15° spread, very fast movement

---

#### Test 3.3: Zigzag
```bash
./bin/enemy_showcase → Click Zigzag
```
**Expected:** 2 plasma shots (1.5/s), 30° spread, zigzag movement

---

#### Test 3.4: Bomber
```bash
./bin/enemy_showcase → Click Bomber
```
**Expected:** Single heavy energy orb (0.8/s), slow methodical approach

---

#### Test 3.5: Swarm
```bash
./bin/enemy_showcase → Click Swarm
```
**Expected:** Rapid single lasers (2/s), erratic movement

---

#### Test 3.6: Elite
```bash
./bin/enemy_showcase → Click Elite
```
**Expected:** 4-shot spread pattern (2/s), 45° spread, advance-pause-retreat

---

#### Test 3.7: Boss
```bash
./bin/enemy_showcase → Click Boss
```
**Expected:** Multiple projectile types, complex patterns, hovers in last third

---

### **Phase 4: Collision System Test (5 minutes)**

#### Test 4.1: Damage Calculation
```bash
./bin/enemy_showcase → Click Tank
```

**Test:**
1. Count how many hits to destroy tank
2. Tank has 20 HP and 20% resistance
3. Player bullets do 1 damage × 0.8 (after resistance) = 1 damage minimum
4. Should take **20 shots** to destroy

**Expected:** Tank destroyed after ~20 hits

**If broken:** Report actual number of hits needed

---

#### Test 4.2: Ghost Invulnerability
```bash
./bin/enemy_showcase → Click Ghost
```

**Test:**
1. Shoot ghost when it's phased out (transparent)
2. Bullets should **pass through** (no collision)
3. Shoot ghost when visible (solid)
4. Bullets should **hit** normally

**Expected:** Ghosts only take damage when visible

---

#### Test 4.3: Boss Shield
```bash
./bin/shootemup
```

**Play until boss fight (~427 seconds / 7+ minutes)**

**Test:**
1. Boss appears with shield
2. Shoot boss
3. Shield should break after many hits
4. Then boss takes health damage

**Expected:** Shield breaks, then health goes down

**If broken:** Report shield behavior

---

### **Phase 5: Projectile Manager Test (3 minutes)**

#### Test 5.1: Projectile Cleanup
```bash
./bin/enemy_showcase → Click Speeder
```

**Test:**
1. Let speeder fire many times
2. Let projectiles fly off screen
3. Watch for memory issues or performance problems
4. Should handle gracefully

**Expected:** Projectiles disappear off-screen, no performance issues

---

#### Test 5.2: Maximum Projectiles
```bash
./bin/enemy_showcase → Click Shield
```

**Test:**
1. Shield fires 360° pattern (6 projectiles)
2. Let it fire multiple times
3. Should handle up to MAX_PROJECTILES (200)
4. Watch for projectile limit handling

**Expected:** No crashes, projectiles managed properly

---

## 📝 REPORTING TEMPLATE

If you find issues, use this template:

```
TEST PHASE: [Phase X.X name]
LOCATION: [Main game / Showcase]
ENEMY TYPE: [which enemy]

EXPECTED BEHAVIOR:
[what should happen]

ACTUAL BEHAVIOR:
[what actually happened]

STEPS TO REPRODUCE:
1. [step 1]
2. [step 2]
...

ADDITIONAL INFO:
[any other details]
```

---

## ✅ SUCCESS CRITERIA

The refactoring is successful if:

### Main Game:
- [ ] All enemies spawn correctly
- [ ] All enemies fire projectiles
- [ ] Projectiles have correct patterns (spread, burst, etc.)
- [ ] Collisions work (bullets hit enemies)
- [ ] Damage calculation works (resistance applied)
- [ ] Score increases correctly
- [ ] Explosions appear when enemies destroyed
- [ ] Boss shield mechanic works

### Showcase:
- [ ] All 10 enemies can be selected
- [ ] All enemies fire projectiles in arena
- [ ] Projectile patterns match main game
- [ ] Bullets destroy enemies
- [ ] Enemies killed counter increases
- [ ] No explosions (by design)
- [ ] No crashes or memory issues
- [ ] ESC returns to grid

### Both:
- [ ] **Identical firing behavior** between game and showcase
- [ ] Same projectile speeds and patterns
- [ ] Same damage calculations
- [ ] Same special cases (Ghost phasing, Boss shield)

---

## 🎯 PRIORITY TESTS

If you're short on time, test these **critical** cases:

1. **Shield enemy** (360° pattern) - Most complex
2. **Tank enemy** (burst + pause) - Tests timing
3. **Ghost enemy** (conditional firing) - Tests special logic
4. **Any basic enemy** (Grunt/Swarm) - Sanity check

If these 4 work, the refactoring is almost certainly successful!

---

## 🚀 Quick Commands

**Main Game:**
```bash
./bin/shootemup
```

**Showcase:**
```bash
./bin/enemy_showcase
```

**Rebuild if needed:**
```bash
make clean && make all && make enemy_showcase
```

---

Ready to test! Let me know what you find! 🎮
