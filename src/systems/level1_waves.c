#include "wave_system.h"
#include "enemy_types.h"
#include "constants.h"

// Static wave definitions - SYNCHRONIZED WITH BASS LOG (level1.log)
//
// BASS EVENT MAPPING STRATEGY:
// - Bass START (LOW): Spawn light enemies (Grunts, Swarms)
// - Bass CHANGE to MEDIUM: Spawn medium enemies (Speeders, Zigzags) or intensify
// - Bass CHANGE to HIGH: Heavy enemies (Tanks, Elites, Bombers, Shields)
// - Long bass sections (>20s): Multiple escalating waves
// - Short bass bursts (<1s): Quick single-type spawns
// - No bass periods: Calm recovery phases
//
// MIXED WAVE DESIGN:
// - Combine complementary enemy types for dynamic gameplay
// - Example: Tanks (slow/tough) + Speeders (fast/weak) = varied threats
// - Use spacing intervals to prevent collision
//
SpawnEvent* CreateLevel1Waveplan(int* eventCount) {
    static SpawnEvent events[400];
    int index = 0;
    
    // ===================================================================
    // PHASE 0: WARM-UP (0-55s) - Tutorial, no bass, no firing
    // Multiple formations to introduce gameplay mechanics
    // ===================================================================
    
    // Wave 1 (3s): Vertical line formation - classic introduction
    events[index++] = (SpawnEvent){
        .time = 3.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 120,
        .pattern = "straight"
    };
    
    // Wave 2 (6s): Horizontal line formation - enemies spread horizontally
    events[index++] = (SpawnEvent){
        .time = 6.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 6.3f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 150,
        .y = 200,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 6.6f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 250,
        .y = 200,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 3 (10s): Diamond/Rhombus formation
    events[index++] = (SpawnEvent){
        .time = 10.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,  // Center
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 10.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 150,
        .y = 220,  // Top
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 10.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 150,
        .y = 380,  // Bottom
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 10.4f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 250,
        .y = 300,  // Back center
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 4 (15s): Swarm - small enemies in vertical line
    events[index++] = (SpawnEvent){
        .time = 15.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 5,
        .interval = 90,
        .pattern = "straight"
    };
    
    // Wave 5 (19s): Square formation
    events[index++] = (SpawnEvent){
        .time = 19.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 220,  // Top-left
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 19.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 380,  // Bottom-left
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 19.4f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 200,
        .y = 220,  // Top-right
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 19.6f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 200,
        .y = 380,  // Bottom-right
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 6 (24s): V-formation (ascending diagonal)
    events[index++] = (SpawnEvent){
        .time = 24.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 360,  // Bottom
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 24.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 120,
        .y = 300,  // Middle
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 24.4f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 190,
        .y = 240,  // Top
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 7 (28s): Two vertical lines
    events[index++] = (SpawnEvent){
        .time = 28.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 3,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 28.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 240,
        .count = 3,
        .interval = 100,
        .pattern = "straight"
    };
    
    // Wave 8 (33s): Inverted V-formation (descending diagonal)
    events[index++] = (SpawnEvent){
        .time = 33.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 220,  // Top
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 33.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 120,
        .y = 280,  // Middle
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 33.4f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 190,
        .y = 340,  // Bottom
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 9 (37s): Cross formation
    events[index++] = (SpawnEvent){
        .time = 37.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,  // Center
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 37.2f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 220,  // Top
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 37.2f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 380,  // Bottom
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 37.4f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 150,
        .y = 300,  // Right
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 10 (42s): Wide horizontal spread
    events[index++] = (SpawnEvent){
        .time = 42.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 180,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 42.3f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 120,
        .y = 260,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 42.6f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 190,
        .y = 340,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 42.9f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 260,
        .y = 420,
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 11 (47s): Arrow formation pointing at player
    events[index++] = (SpawnEvent){
        .time = 47.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,  // Arrow tip
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 47.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 120,
        .y = 260,  // Upper wing
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 47.2f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 120,
        .y = 340,  // Lower wing
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 47.4f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 190,
        .y = 220,  // Upper tail
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 47.4f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 190,
        .y = 380,  // Lower tail
        .count = 1,
        .interval = 0,
        .pattern = "straight"
    };
    
    // Wave 12 (52s): Final warm-up - big vertical column
    events[index++] = (SpawnEvent){
        .time = 52.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 85,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 1: [55.85-83.02] - Duration: 27.17s (LOW→MEDIUM)
    // First combat experience with MEDIUM bass energy
    // ===================================================================
    
    // [55.85] Bass START - Light introduction
    events[index++] = (SpawnEvent){
        .time = 56.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    // [55.95] Bass MEDIUM - Increase intensity with swarms
    events[index++] = (SpawnEvent){
        .time = 58.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 400,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    // Mid-section: Mixed wave
    events[index++] = (SpawnEvent){
        .time = 65.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 65.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 350,
        .count = 2,
        .interval = 100,
        .pattern = "zigzag"
    };
    
    // Continue pressure
    events[index++] = (SpawnEvent){
        .time = 73.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    
    // [82.87] Bass drops to LOW - Wind down with light enemies
    events[index++] = (SpawnEvent){
        .time = 80.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 120,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 2: [83.27-106.53] - Duration: 23.26s (LOW→MEDIUM)
    // First major assault with MEDIUM bass
    // ===================================================================
    
    // [83.38] Bass MEDIUM - Start with mixed assault
    events[index++] = (SpawnEvent){
        .time = 84.0f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 1,
        .interval = 0,
        .pattern = "tank_assault"
    };
    events[index++] = (SpawnEvent){
        .time = 84.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 350,
        .count = 3,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // Escalate with more enemies
    events[index++] = (SpawnEvent){
        .time = 90.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 90.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 400,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    // Continue pressure
    events[index++] = (SpawnEvent){
        .time = 96.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // Final push before bass change
    events[index++] = (SpawnEvent){
        .time = 102.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 3,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 102.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 400,
        .count = 4,
        .interval = 85,
        .pattern = "straight"
    };
    
    // ===================================================================
    // SHORT BASS BURSTS: [106.63-110.12] - Multiple short bursts
    // Quick swarm spawns for each burst
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 106.7f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 3: [110.68-135.27] - Duration: 24.59s (LOW→MEDIUM)
    // Sustained combat with variety
    // ===================================================================
    
    // [110.78] Bass MEDIUM - Mixed assault
    events[index++] = (SpawnEvent){
        .time = 111.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 105,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 111.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 350,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // Continuous pressure with waves
    events[index++] = (SpawnEvent){
        .time = 118.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 124.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    events[index++] = (SpawnEvent){
        .time = 124.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 400,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    // Final wave before bass drop
    events[index++] = (SpawnEvent){
        .time = 131.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // ===================================================================
    // BASS SEGMENT 4: [138.14-163.55] - Duration: 25.41s (LOW→MEDIUM)
    // Escalating intensity
    // ===================================================================
    
    // [138.24] Bass MEDIUM - Strong mixed assault
    events[index++] = (SpawnEvent){
        .time = 138.5f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 1,
        .interval = 0,
        .pattern = "tank_assault"
    };
    events[index++] = (SpawnEvent){
        .time = 139.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 350,
        .count = 4,
        .interval = 90,
        .pattern = "zigzag"
    };
    
    // Maintain pressure
    events[index++] = (SpawnEvent){
        .time = 145.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 145.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // Continue assault
    events[index++] = (SpawnEvent){
        .time = 152.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 152.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 380,
        .count = 3,
        .interval = 100,
        .pattern = "straight"
    };
    
    // Final push
    events[index++] = (SpawnEvent){
        .time = 159.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    // ===================================================================
    // SHORT BASS BURSTS: [164.57-165.65] - Quick bursts
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 165.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 4,
        .interval = 85,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 5: [165.65-190.53] - Duration: 24.99s (MEDIUM)
    // High-intensity combat with MEDIUM bass
    // ===================================================================
    
    // [165.65] Bass MEDIUM - Immediate assault
    events[index++] = (SpawnEvent){
        .time = 166.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 110,
        .pattern = "erratic"
    };
    events[index++] = (SpawnEvent){
        .time = 166.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 370,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // Escalate with heavier enemies
    events[index++] = (SpawnEvent){
        .time = 172.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 1,
        .interval = 0,
        .pattern = "horizontal"
    };
    events[index++] = (SpawnEvent){
        .time = 172.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    // Continue pressure
    events[index++] = (SpawnEvent){
        .time = 178.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 178.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 400,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // Final assault
    events[index++] = (SpawnEvent){
        .time = 185.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // ===================================================================
    // CALM PERIOD: [190.69-222.12] - ~31s NO BASS
    // Recovery phase with minimal enemies
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 200.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 212.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 6: [222.12-247.97] - Duration: 25.85s (LOW→MEDIUM)
    // RAPID BASS CHANGES - Very intense section with constant variety
    // This is the most dynamic bass section in the entire track!
    // ===================================================================
    
    // [222.22] Bass MEDIUM - Start strong
    events[index++] = (SpawnEvent){
        .time = 222.5f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 1,
        .interval = 0,
        .pattern = "elite_retreat"
    };
    events[index++] = (SpawnEvent){
        .time = 223.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // Continuous waves matching rapid bass changes
    events[index++] = (SpawnEvent){
        .time = 228.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 228.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 370,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    events[index++] = (SpawnEvent){
        .time = 233.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 238.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 220,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    events[index++] = (SpawnEvent){
        .time = 238.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 390,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 243.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 4,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // ===================================================================
    // BASS SEGMENT 7: [249.56-271.31] - Duration: ~21.75s (LOW→MEDIUM)
    // Continued intensity with rapid changes
    // ===================================================================
    
    events[index++] = (SpawnEvent){
        .time = 250.0f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 1,
        .interval = 0,
        .pattern = "tank_assault"
    };
    events[index++] = (SpawnEvent){
        .time = 250.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 360,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    events[index++] = (SpawnEvent){
        .time = 256.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 256.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 262.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 4,
        .interval = 110,
        .pattern = "erratic"
    };
    
    events[index++] = (SpawnEvent){
        .time = 267.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 240,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    events[index++] = (SpawnEvent){
        .time = 267.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 380,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    // ===================================================================
    // CALM PERIOD: [271.42-276.69] - ~5s recovery
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 274.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 120,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 8: [276.69-305.45] - Duration: 28.76s (LOW→MEDIUM)
    // Long sustained combat with variety
    // ===================================================================
    
    // [277.05] Bass MEDIUM - Strong mixed assault
    events[index++] = (SpawnEvent){
        .time = 277.5f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 150,
        .pattern = "formation"
    };
    events[index++] = (SpawnEvent){
        .time = 278.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // Escalate pressure
    events[index++] = (SpawnEvent){
        .time = 283.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 1,
        .interval = 0,
        .pattern = "horizontal"
    };
    events[index++] = (SpawnEvent){
        .time = 283.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 289.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 289.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 400,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    events[index++] = (SpawnEvent){
        .time = 295.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 220,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    events[index++] = (SpawnEvent){
        .time = 300.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 300.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 370,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    // ===================================================================
    // SHORT BASS BURSTS: [305.70-310.46] - Multiple short bursts
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 306.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    // ===================================================================
    // CALM PERIOD: [310.46-346.02] - ~36s NO BASS
    // Long recovery - prepare for second half
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 320.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 332.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 4,
        .interval = 90,
        .pattern = "straight"
    };
    
    // ===================================================================
    // SHORT BASS BURST: [346.02-346.07] - Single burst
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 346.1f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 100,
        .pattern = "straight"
    };
    
    // ===================================================================
    // CALM PERIOD: [346.07-372.99] - ~27s recovery before finale
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 355.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 365.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 9: [372.99-417.28] - Duration: 44.29s (LOW→MEDIUM)
    // LONGEST BASS SECTION - Build to boss with escalating intensity
    // ===================================================================
    
    // [373.09] Bass MEDIUM - Start the epic build-up
    events[index++] = (SpawnEvent){
        .time = 373.5f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 2,
        .interval = 150,
        .pattern = "elite_retreat"
    };
    events[index++] = (SpawnEvent){
        .time = 374.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 350,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // Escalate with heavy enemies
    events[index++] = (SpawnEvent){
        .time = 380.0f,
        .type = ENEMY_TANK,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 2,
        .interval = 150,
        .pattern = "tank_assault"
    };
    events[index++] = (SpawnEvent){
        .time = 380.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 370,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 387.0f,
        .type = ENEMY_SHIELD,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 150,
        .pattern = "formation"
    };
    events[index++] = (SpawnEvent){
        .time = 387.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 110,
        .pattern = "erratic"
    };
    
    events[index++] = (SpawnEvent){
        .time = 394.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 150,
        .pattern = "horizontal"
    };
    events[index++] = (SpawnEvent){
        .time = 394.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 6,
        .interval = 75,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 401.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 5,
        .interval = 90,
        .pattern = "zigzag"
    };
    events[index++] = (SpawnEvent){
        .time = 401.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 370,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 408.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 4,
        .interval = 110,
        .pattern = "erratic"
    };
    
    // [414.25] Bass drops to LOW - Final pre-boss wave
    events[index++] = (SpawnEvent){
        .time = 414.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    // ===================================================================
    // SHORT BASS BURSTS: [417.33-418.81] - Quick bursts before boss
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 417.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    // ===================================================================
    // CALM PERIOD: [418.81-427.47] - ~9s calm before boss
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 423.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 10: [427.93-451.38] - Duration: 23.55s (MEDIUM)
    // BOSS BATTLE SYNCHRONIZED WITH BASS MEDIUM!
    // ===================================================================
    
    // [427.93] Bass MEDIUM - BOSS SPAWNS
    events[index++] = (SpawnEvent){
        .time = 428.0f,
        .type = ENEMY_BOSS,
        .x = SCREEN_WIDTH + 100,
        .y = PLAY_ZONE_HEIGHT / 2,
        .count = 1,
        .interval = 0,
        .pattern = "boss"
    };
    
    // Boss support waves during intense bass
    events[index++] = (SpawnEvent){
        .time = 433.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 3,
        .interval = 110,
        .pattern = "minion"
    };
    
    events[index++] = (SpawnEvent){
        .time = 440.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 350,
        .count = 4,
        .interval = 85,
        .pattern = "minion"
    };
    
    events[index++] = (SpawnEvent){
        .time = 447.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 100,
        .pattern = "minion"
    };
    
    // ===================================================================
    // SHORT BASS BURSTS: [453.96-455.31] - Post-boss bursts
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 454.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    // ===================================================================
    // BASS SEGMENT 11: [455.31-479.53] - Duration: 24.71s (MEDIUM→LOW)
    // Post-boss recovery with moderate intensity
    // ===================================================================
    
    events[index++] = (SpawnEvent){
        .time = 458.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    events[index++] = (SpawnEvent){
        .time = 458.5f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 370,
        .count = 3,
        .interval = 110,
        .pattern = "erratic"
    };
    
    events[index++] = (SpawnEvent){
        .time = 465.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 472.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 260,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // ===================================================================
    // CALM PERIOD: [479.53-482.69] - ~3s brief recovery
    // ===================================================================
    
    // ===================================================================
    // BASS SEGMENT 12: [482.80-510.36] - Duration: 27.67s (MEDIUM→LOW)
    // FINAL PUSH - Last major combat sequence
    // ===================================================================
    
    // [482.80] Bass MEDIUM - Final elite encounter
    events[index++] = (SpawnEvent){
        .time = 483.0f,
        .type = ENEMY_ELITE,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 150,
        .pattern = "elite_retreat"
    };
    events[index++] = (SpawnEvent){
        .time = 483.5f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 200,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    events[index++] = (SpawnEvent){
        .time = 490.0f,
        .type = ENEMY_BOMBER,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 2,
        .interval = 150,
        .pattern = "horizontal"
    };
    events[index++] = (SpawnEvent){
        .time = 490.5f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 250,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 497.0f,
        .type = ENEMY_ZIGZAG,
        .x = SCREEN_WIDTH + 50,
        .y = 280,
        .count = 4,
        .interval = 110,
        .pattern = "erratic"
    };
    events[index++] = (SpawnEvent){
        .time = 497.5f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 380,
        .count = 5,
        .interval = 80,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 504.0f,
        .type = ENEMY_SPEEDER,
        .x = SCREEN_WIDTH + 50,
        .y = 240,
        .count = 4,
        .interval = 95,
        .pattern = "zigzag"
    };
    
    // [510.15] Bass drops to LOW - Final wave
    events[index++] = (SpawnEvent){
        .time = 509.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 4,
        .interval = 100,
        .pattern = "straight"
    };
    
    // ===================================================================
    // ENDING: Victory lap (510-550s) - Easy finish
    // ===================================================================
    events[index++] = (SpawnEvent){
        .time = 520.0f,
        .type = ENEMY_SWARM,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 90,
        .pattern = "straight"
    };
    
    events[index++] = (SpawnEvent){
        .time = 535.0f,
        .type = ENEMY_GRUNT,
        .x = SCREEN_WIDTH + 50,
        .y = 300,
        .count = 3,
        .interval = 110,
        .pattern = "straight"
    };
    
    *eventCount = index;
    return events;
}
