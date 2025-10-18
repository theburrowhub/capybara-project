#include "wave_system.h"
#include "enemy_types.h"
#include "constants.h"
#include <stdio.h>

// Static wave definitions for LEVEL 2 - SYNCHRONIZED WITH BASS LOG (level2.log)
//
// LEVEL 2 DESIGN PHILOSOPHY: HIGH INTENSITY FROM START
// - NO warm-up period - combat starts IMMEDIATELY at 0.40s
// - Bass starts at LOW then jumps to MEDIUM at 0.50s → INSTANT ACTION
// - Rapid intensity changes following bass transitions
// - Main combat phase (0.50-370s): Continuous high intensity with fluctuations
// - Calm recovery (370-480s): Sparse enemies, breathing room
// - Final climax (480-593s): Intensity returns with BOSS at 555s
//
// BASS EVENT MAPPING:
// - 0.40s: Bass START (LOW) → Immediate combat begins!
// - 0.50s: Bass→MEDIUM → High intensity starts
// - 0.50-86.70s: MEDIUM bass → Heavy enemies constantly
// - 86.70-367s: Continuous MEDIUM with rapid fluctuations → Dynamic intensity
// - 367-479s: Sparse LOW bursts → Calm recovery period
// - 479-593s: MEDIUM returns → Final climax with BOSS
//
SpawnEvent* CreateLevel2Waveplan(int* eventCount) {
    static SpawnEvent events[600];
    int index = 0;
    
    // ===================================================================
    // PHASE 0: INSTANT ACTION (0.4s) - NO WARM-UP!
    // Bass starts at 0.40s - combat begins IMMEDIATELY
    // ===================================================================
    
    // 0.4s: Bass START (LOW) - First wave spawns instantly!
    events[index++] = (SpawnEvent){
        .time = 0.4f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 3,
        .interval = 120,
        .pattern = "rush"
    };
    
    // ===================================================================
    // PHASE 1: HIGH INTENSITY START (0.5-90s)
    // Bass jumps to MEDIUM at 0.50s → Immediate high intensity
    // ===================================================================
    
    // 0.5s: Bass→MEDIUM - Heavy enemies start!
    events[index++] = (SpawnEvent){
        .time = 0.5f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 1,
        .interval = 0,
        .pattern = "elite_retreat"
    };
    
    events[index++] = (SpawnEvent){
        .time = 1.0f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 2,
        .interval = 150,
        .pattern = "tank_assault"
    };
    
    // 3s: Mixed assault
    events[index++] = (SpawnEvent){
        .time = 3.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 240,
        .count = 4,
        .interval = 110,
        .pattern = "zigzag"
    };
    
    // 6s: Speeder rush
    events[index++] = (SpawnEvent){
        .time = 6.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 5,
        .interval = 100,
        .pattern = "rush"
    };
    
    // 10s: Shield wall
    events[index++] = (SpawnEvent){
        .time = 10.0f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 2,
        .interval = 170,
        .pattern = "horizontal"
    };
    
    // 14s: Bomber wave
    events[index++] = (SpawnEvent){
        .time = 14.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 260,
        .count = 2,
        .interval = 180,
        .pattern = "slow_advance"
    };
    
    // 18s: Swarm assault
    events[index++] = (SpawnEvent){
        .time = 18.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 7,
        .interval = 70,
        .pattern = "erratic"
    };
    
    // 22s: Elite + support
    events[index++] = (SpawnEvent){
        .time = 22.0f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 180,
        .pattern = "elite_retreat"
    };
    
    // 26s: Tank wave
    events[index++] = (SpawnEvent){
        .time = 26.0f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 270,
        .count = 2,
        .interval = 160,
        .pattern = "tank_assault"
    };
    
    // 30s: Zigzag chaos
    events[index++] = (SpawnEvent){
        .time = 30.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 115,
        .pattern = "zigzag"
    };
    
    // 35s: Speeder rush
    events[index++] = (SpawnEvent){
        .time = 35.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 210,
        .count = 4,
        .interval = 110,
        .pattern = "rush"
    };
    
    // 40s: Shield + Bomber combo
    events[index++] = (SpawnEvent){
        .time = 40.0f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 260,
        .count = 2,
        .interval = 170,
        .pattern = "horizontal"
    };
    
    events[index++] = (SpawnEvent){
        .time = 43.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 180,
        .pattern = "slow_advance"
    };
    
    // 48s: Swarm wave
    events[index++] = (SpawnEvent){
        .time = 48.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 290,
        .count = 8,
        .interval = 65,
        .pattern = "erratic"
    };
    
    // 53s: Elite assault
    events[index++] = (SpawnEvent){
        .time = 53.0f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 1,
        .interval = 0,
        .pattern = "elite_retreat"
    };
    
    // 58s: Tank assault
    events[index++] = (SpawnEvent){
        .time = 58.0f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 270,
        .count = 2,
        .interval = 155,
        .pattern = "tank_assault"
    };
    
    // 63s: Zigzag wave
    events[index++] = (SpawnEvent){
        .time = 63.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 240,
        .count = 3,
        .interval = 125,
        .pattern = "zigzag"
    };
    
    // 68s: Speeder rush
    events[index++] = (SpawnEvent){
        .time = 68.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 5,
        .interval = 100,
        .pattern = "rush"
    };
    
    // 73s: Shield wall
    events[index++] = (SpawnEvent){
        .time = 73.0f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 3,
        .interval = 140,
        .pattern = "horizontal"
    };
    
    // 78s: Bomber + Swarm combo
    events[index++] = (SpawnEvent){
        .time = 78.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 260,
        .count = 2,
        .interval = 180,
        .pattern = "slow_advance"
    };
    
    events[index++] = (SpawnEvent){
        .time = 81.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 320,
        .count = 7,
        .interval = 70,
        .pattern = "erratic"
    };
    
    // 86s: Elite wave
    events[index++] = (SpawnEvent){
        .time = 86.0f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 170,
        .pattern = "elite_retreat"
    };
    
    // ===================================================================
    // PHASE 2: CONTINUOUS INTENSE COMBAT (90-370s)
    // Bass stays mostly MEDIUM with rapid fluctuations
    // Waves every 4-6 seconds - non-stop action!
    // Using pattern rotation for varied but predictable intensity
    // ===================================================================
    
    // 90s wave pattern (continuing intensity)
    // Pattern: Heavy → Medium → Light, repeat with variations
    float times_90_370[] = {
        95, 100, 104, 109, 114, 118, 123, 128, 132, 137,
        142, 146, 151, 156, 160, 165, 170, 174, 179, 184,
        188, 193, 198, 202, 207, 212, 216, 221, 226, 230,
        235, 240, 244, 249, 254, 258, 263, 268, 272, 277,
        282, 286, 291, 296, 300, 305, 310, 314, 319, 324,
        328, 333, 338, 342, 347, 352, 356, 361, 366
    };
    
    EnemyType pattern1[] = {ENEMY_TANK, ENEMY_ZIGZAG, ENEMY_SWARM};
    EnemyType pattern2[] = {ENEMY_ELITE, ENEMY_SPEEDER, ENEMY_SWARM};
    EnemyType pattern3[] = {ENEMY_BOMBER, ENEMY_GHOST, ENEMY_SWARM};
    EnemyType pattern4[] = {ENEMY_SHIELD, ENEMY_ZIGZAG, ENEMY_SWARM};
    
    int patternIndex = 0;
    for (int i = 0; i < 59 && index < 580; i++) {
        EnemyType* currentPattern;
        switch (patternIndex % 4) {
            case 0: currentPattern = pattern1; break;
            case 1: currentPattern = pattern2; break;
            case 2: currentPattern = pattern3; break;
            default: currentPattern = pattern4; break;
        }
        
        int subIndex = i % 3;  // Cycle through each pattern
        EnemyType enemyType = currentPattern[subIndex];
        
        // Determine count and pattern based on enemy type
        int count = 2;
        int interval = 150;
        const char* pattern = "straight";
        
        if (enemyType == ENEMY_TANK) {
            count = 2; interval = 155; pattern = "tank_assault";
        } else if (enemyType == ENEMY_ELITE) {
            count = 1; interval = 0; pattern = "elite_retreat";
        } else if (enemyType == ENEMY_BOMBER) {
            count = 2; interval = 175; pattern = "slow_advance";
        } else if (enemyType == ENEMY_SHIELD) {
            count = 2; interval = 165; pattern = "horizontal";
        } else if (enemyType == ENEMY_SPEEDER) {
            count = 4; interval = 105; pattern = "rush";
        } else if (enemyType == ENEMY_ZIGZAG) {
            count = 3; interval = 120; pattern = "zigzag";
        } else if (enemyType == ENEMY_GHOST) {
            count = 3; interval = 130; pattern = "phasing";
        } else if (enemyType == ENEMY_SWARM) {
            count = 7; interval = 70; pattern = "erratic";
        }
        
        events[index++] = (SpawnEvent){
            .time = times_90_370[i],
            .type = enemyType,
            .x = SCREEN_WIDTH + 50,
            .y = 250 + ((i * 37) % 100) - 50,  // Vary Y position deterministically
            .count = count,
            .interval = interval,
            .pattern = pattern
        };
        
        if (subIndex == 2) patternIndex++;  // Move to next pattern after completing cycle
    }
    
    // ===================================================================
    // PHASE 3: CALM RECOVERY (370-480s)
    // Bass sparse LOW bursts - INCREASING cadence for anticipation
    // Enemies spawn slowly at first, then FASTER and FASTER
    // STOPS at 475s → 5 second silence → INTENSE PHASE at 480s
    // Player notices: "They're spawning faster... FASTER... IT'S RAMPING UP!"
    // ===================================================================
    
    // INCREASING cadence pattern:
    // Start (373-420s): Every 8-10 seconds (moderate, recovery with action)
    // Middle (420-450s): Every 5-7 seconds (building up)
    // Fast (450-472s): Every 3-4 seconds (rapid building!)
    // FINAL 3 waves: VERY CLOSE (1-2s apart) - multiple waves on screen!
    // Stop: 477s (3 second gap before 480s intense phase)
    float calmTimes[] = {
        373,   // +3s from 370 (phase start)
        382,   // +9s - Moderate start
        391,   // +9s - Still moderate
        400,   // +9s - Moderate
        409,   // +9s - Moderate
        417,   // +8s - Getting faster
        425,   // +8s - Faster
        432,   // +7s - Speeding up
        439,   // +7s - Faster now
        445,   // +6s - Rapid
        450,   // +5s - Very rapid
        455,   // +5s - Very rapid
        459,   // +4s - Really fast!
        463,   // +4s - Really fast!
        466,   // +3s - Intense!
        469,   // +3s - Intense!
        472,   // +3s - Building!
        474,   // +2s - CLUSTER START!
        476,   // +2s - CLUSTER! Wave 2 overlaps!
        477    // +1s - CLUSTER! Wave 3 overlaps! (3 waves nearly simultaneous!)
    };
    
    EnemyType calmEnemies[] = {
        ENEMY_GRUNT, ENEMY_SWARM, ENEMY_GRUNT, ENEMY_SWARM,    // Moderate start (373-409s)
        ENEMY_GRUNT, ENEMY_SWARM, ENEMY_GRUNT, ENEMY_SWARM,    // Faster (417-439s)
        ENEMY_GRUNT, ENEMY_SWARM, ENEMY_SWARM, ENEMY_GRUNT,    // Rapid (445-459s)
        ENEMY_SWARM, ENEMY_GRUNT, ENEMY_SWARM, ENEMY_GRUNT,    // Very rapid (463-469s)
        ENEMY_SWARM, ENEMY_SPEEDER, ENEMY_ZIGZAG, ENEMY_GRUNT  // Building + Cluster! (472-477s)
    };
    
    for (int i = 0; i < 20 && index < 595; i++) {
        EnemyType enemyType = calmEnemies[i];
        int count = 3;
        int interval = 120;
        const char* pattern = "straight";
        
        // Determine count and pattern based on enemy type
        if (enemyType == ENEMY_GRUNT) {
            count = 3; interval = 120; pattern = "straight";
        } else if (enemyType == ENEMY_SWARM) {
            count = 4; interval = 90; pattern = "erratic";
        } else if (enemyType == ENEMY_SPEEDER) {
            count = 3; interval = 130; pattern = "rush";
        } else if (enemyType == ENEMY_ZIGZAG) {
            count = 3; interval = 120; pattern = "zigzag";
        }
        
        events[index++] = (SpawnEvent){
            .time = calmTimes[i],
            .type = enemyType,
            .x = SCREEN_WIDTH + 50,
            .y = 250 + ((i * 37) % 120) - 60,  // Vary Y deterministically
            .count = count,
            .interval = interval,
            .pattern = pattern
        };
    }
    
    // NOTE: Last 3 waves (474s, 476s, 477s) spawn VERY CLOSE together!
    // Player will fight 2-3 waves simultaneously - "They're all coming at once!"
    // 477s: LAST spawn in calm phase
    // 480s: Intense phase begins (3 second gap after cluster)
    // Player will notice: "They're spawning faster! MULTIPLE AT ONCE! HERE IT COMES!"
    
    // ===================================================================
    // PHASE 4: FINAL CLIMAX - EXTREME INTENSITY (480-490s)
    // Bass returns to MEDIUM - MAXIMUM DANGER!
    // Waves every 2-3 seconds - almost NO rest time
    // Many enemies on screen simultaneously - overwhelming pressure
    // Short but INTENSE burst before BOSS at 490s
    // ===================================================================
    
    // 480s: Intensity EXPLODES back!
    events[index++] = (SpawnEvent){
        .time = 480.0f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 2,
        .interval = 170,
        .pattern = "elite_retreat"
    };
    
    // 482s: Tanks
    events[index++] = (SpawnEvent){
        .time = 482.5f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 270,
        .count = 2,
        .interval = 160,
        .pattern = "tank_assault"
    };
    
    // 485s: Swarms
    events[index++] = (SpawnEvent){
        .time = 485.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 7,
        .interval = 70,
        .pattern = "erratic"
    };
    
    // 487s: Speeders
    events[index++] = (SpawnEvent){
        .time = 487.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 210,
        .count = 5,
        .interval = 100,
        .pattern = "rush"
    };
    
    // Continue extreme intensity until boss spawn at 530s!
    
    // 492s: Elite wave
    events[index++] = (SpawnEvent){
        .time = 492.5f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 1,
        .interval = 0,
        .pattern = "elite_retreat"
    };
    
    // 495s: Speeders
    events[index++] = (SpawnEvent){
        .time = 495.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 205,
        .count = 5,
        .interval = 105,
        .pattern = "rush"
    };
    
    // 497s: Swarms
    events[index++] = (SpawnEvent){
        .time = 497.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 7,
        .interval = 70,
        .pattern = "erratic"
    };
    
    // 500s: Shields
    events[index++] = (SpawnEvent){
        .time = 500.0f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 275,
        .count = 2,
        .interval = 170,
        .pattern = "horizontal"
    };
    
    // 502s: Zigzags
    events[index++] = (SpawnEvent){
        .time = 502.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 245,
        .count = 3,
        .interval = 125,
        .pattern = "zigzag"
    };
    
    // 505s: Bombers
    events[index++] = (SpawnEvent){
        .time = 505.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 265,
        .count = 2,
        .interval = 175,
        .pattern = "slow_advance"
    };
    
    // 507s: Speeders
    events[index++] = (SpawnEvent){
        .time = 507.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 210,
        .count = 5,
        .interval = 105,
        .pattern = "rush"
    };
    
    // 510s: Swarms
    events[index++] = (SpawnEvent){
        .time = 510.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 295,
        .count = 7,
        .interval = 70,
        .pattern = "erratic"
    };
    
    // 512s: Tanks
    events[index++] = (SpawnEvent){
        .time = 512.5f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 270,
        .count = 2,
        .interval = 160,
        .pattern = "tank_assault"
    };
    
    // 515s: Elite
    events[index++] = (SpawnEvent){
        .time = 515.0f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 285,
        .count = 1,
        .interval = 0,
        .pattern = "elite_retreat"
    };
    
    // 517s: Zigzags
    events[index++] = (SpawnEvent){
        .time = 517.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 3,
        .interval = 120,
        .pattern = "zigzag"
    };
    
    // 520s: Speeders + Swarms
    events[index++] = (SpawnEvent){
        .time = 520.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 215,
        .count = 5,
        .interval = 105,
        .pattern = "rush"
    };
    
    events[index++] = (SpawnEvent){
        .time = 522.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 7,
        .interval = 70,
        .pattern = "erratic"
    };
    
    // 525s: Shields + Bombers
    events[index++] = (SpawnEvent){
        .time = 525.0f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 275,
        .count = 2,
        .interval = 170,
        .pattern = "horizontal"
    };
    
    events[index++] = (SpawnEvent){
        .time = 527.5f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 265,
        .count = 2,
        .interval = 175,
        .pattern = "slow_advance"
    };
    
    // ===================================================================
    // PHASE 5: BOSS BATTLE (530-600s)
    // Boss spawns at 530s, escapes at 600s (70 seconds to kill!)
    // Minimal support - focus on boss!
    // ===================================================================
    
    // 530s: BOSS SPAWNS!
    events[index++] = (SpawnEvent){
        .time = 530.0f,
        .type = ENEMY_BOSS,
        .x = SCREEN_WIDTH + 100,
        .y = 300,
        .count = 1,
        .interval = 0,
        .pattern = "boss"
    };
    
    // 545s: Light boss support - just grunts
    events[index++] = (SpawnEvent){
        .time = 545.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 2,
        .interval = 130,
        .pattern = "minion"
    };
    
    // 560s: Minimal support - small swarms
    events[index++] = (SpawnEvent){
        .time = 560.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 310,
        .count = 3,
        .interval = 100,
        .pattern = "erratic"
    };
    
    // 580s: Light support - grunts
    events[index++] = (SpawnEvent){
        .time = 580.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 270,
        .count = 2,
        .interval = 130,
        .pattern = "minion"
    };
    
    // No other waves during boss battle (530-600s) except minimal support
    // Player focuses on boss!
    
    // ===================================================================
    // PHASE 6: POST-BOSS / VICTORY LAP (600-612s)
    // After boss escape/death, quick victory lap
    // ===================================================================
    
    // 602s: Light swarms
    events[index++] = (SpawnEvent){
        .time = 602.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "erratic"
    };
    
    // 607s: Final grunts
    events[index++] = (SpawnEvent){
        .time = 607.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 3,
        .interval = 120,
        .pattern = "straight"
    };
    
    *eventCount = index;
    printf("[LEVEL 2 WAVES] Created %d spawn events - HIGH INTENSITY from start!\n", index);
    return events;
}