#include "enemy_types.h"
#include "constants.h"
#include "projectile_types.h"
#include <math.h>
#include <string.h>

// Global enemy type definitions
EnemyTypeDefinition enemyTypes[ENEMY_TYPE_COUNT];

void InitEnemyTypes(void) {
    // GRUNT - Basic enemy, small and weak but fast
    enemyTypes[ENEMY_GRUNT] = (EnemyTypeDefinition){
        .type = ENEMY_GRUNT,
        .name = "Grunt",
        .size = 0.8f,
        .health = 1,
        .speed = 1.2f,
        .power = 10,
        .primaryColor = (Color){200, 50, 50, 255},    // Dark Red
        .secondaryColor = (Color){150, 30, 30, 255},  // Darker Red
        .glowColor = (Color){255, 100, 100, 128},     // Light Red glow
        .resistance = 0.0f,
        .symbol = "▲"
    };
    
    // TANK - Large, slow, very high health (20 hits - heavy assault platform)
    enemyTypes[ENEMY_TANK] = (EnemyTypeDefinition){
        .type = ENEMY_TANK,
        .name = "Tank",
        .size = 2.0f,
        .health = 20,  // Increased from 7 to 20 for sustained combat
        .speed = 0.5f,
        .power = 30,
        .primaryColor = (Color){80, 80, 100, 255},    // Dark Gray-Blue
        .secondaryColor = (Color){60, 60, 80, 255},   // Darker Gray
        .glowColor = (Color){120, 120, 150, 128},     // Light Gray glow
        .resistance = 0.2f,  // Reduced from 0.5 to ensure damage is taken
        .symbol = "■"
    };
    
    // SPEEDER - Very fast, low health
    enemyTypes[ENEMY_SPEEDER] = (EnemyTypeDefinition){
        .type = ENEMY_SPEEDER,
        .name = "Speeder",
        .size = 0.6f,
        .health = 1,
        .speed = 2.5f,
        .power = 15,
        .primaryColor = (Color){255, 200, 0, 255},    // Yellow
        .secondaryColor = (Color){255, 150, 0, 255},  // Orange
        .glowColor = (Color){255, 255, 100, 128},     // Light Yellow glow
        .resistance = 0.0f,
        .symbol = "»"
    };
    
    // ZIGZAG - Medium stats, erratic movement
    enemyTypes[ENEMY_ZIGZAG] = (EnemyTypeDefinition){
        .type = ENEMY_ZIGZAG,
        .name = "Zigzag",
        .size = 1.0f,
        .health = 2,
        .speed = 1.0f,
        .power = 20,
        .primaryColor = (Color){0, 200, 100, 255},    // Green
        .secondaryColor = (Color){0, 150, 75, 255},   // Dark Green
        .glowColor = (Color){100, 255, 150, 128},     // Light Green glow
        .resistance = 0.1f,
        .symbol = "◊"
    };
    
    // BOMBER - Large, dangerous (6 hits per flight plan)
    enemyTypes[ENEMY_BOMBER] = (EnemyTypeDefinition){
        .type = ENEMY_BOMBER,
        .name = "Bomber",
        .size = 1.5f,
        .health = 6,
        .speed = 0.8f,
        .power = 40,
        .primaryColor = (Color){150, 50, 200, 255},   // Purple
        .secondaryColor = (Color){100, 30, 150, 255}, // Dark Purple
        .glowColor = (Color){200, 100, 255, 128},     // Light Purple glow
        .resistance = 0.1f,  // Reduced to ensure proper damage
        .symbol = "◉"
    };
    
    // SHIELD - Has protective shield (7 hits total per flight plan)
    enemyTypes[ENEMY_SHIELD] = (EnemyTypeDefinition){
        .type = ENEMY_SHIELD,
        .name = "Shield",
        .size = 1.2f,
        .health = 7,  // 3 for shield + 4 for enemy = 7 total
        .speed = 0.9f,
        .power = 25,
        .primaryColor = (Color){0, 150, 200, 255},    // Cyan
        .secondaryColor = (Color){0, 100, 150, 255},  // Dark Cyan
        .glowColor = (Color){100, 200, 255, 128},     // Light Cyan glow
        .resistance = 0.3f,  // Reduced from 0.7 to ensure damage
        .symbol = "◎"
    };
    
    // SWARM - Small, appears in groups
    enemyTypes[ENEMY_SWARM] = (EnemyTypeDefinition){
        .type = ENEMY_SWARM,
        .name = "Swarm",
        .size = 0.5f,
        .health = 1,
        .speed = 1.5f,
        .power = 8,
        .primaryColor = (Color){255, 100, 0, 255},    // Orange
        .secondaryColor = (Color){200, 80, 0, 255},   // Dark Orange
        .glowColor = (Color){255, 150, 50, 128},      // Light Orange glow
        .resistance = 0.0f,
        .symbol = "•"
    };
    
    // ELITE - Strong all-around (5 hits - balanced challenge)
    enemyTypes[ENEMY_ELITE] = (EnemyTypeDefinition){
        .type = ENEMY_ELITE,
        .name = "Elite",
        .size = 1.3f,
        .health = 5,
        .speed = 1.3f,
        .power = 35,
        .primaryColor = (Color){200, 200, 0, 255},    // Gold
        .secondaryColor = (Color){150, 150, 0, 255},  // Dark Gold
        .glowColor = (Color){255, 255, 200, 128},     // Light Gold glow
        .resistance = 0.2f,  // Reduced to ensure proper damage
        .symbol = "★"
    };
    
    // GHOST - Phases in and out
    enemyTypes[ENEMY_GHOST] = (EnemyTypeDefinition){
        .type = ENEMY_GHOST,
        .name = "Ghost",
        .size = 1.1f,
        .health = 2,
        .speed = 1.1f,
        .power = 22,
        .primaryColor = (Color){150, 150, 255, 180},  // Semi-transparent Blue
        .secondaryColor = (Color){100, 100, 200, 180},// Dark Blue
        .glowColor = (Color){200, 200, 255, 100},     // Light Blue glow
        .resistance = 0.4f,
        .symbol = "◈"
    };
    
    // BOSS - Very large and powerful (no shield, just lots of health)
    enemyTypes[ENEMY_BOSS] = (EnemyTypeDefinition){
        .type = ENEMY_BOSS,
        .name = "Boss",
        .size = 3.0f,
        .health = 150,  // Balanced health pool, no shield mechanic
        .speed = 0.6f,
        .power = 50,
        .primaryColor = (Color){150, 0, 0, 255},      // Dark Red
        .secondaryColor = (Color){100, 0, 0, 255},    // Very Dark Red
        .glowColor = (Color){255, 50, 50, 128},       // Red glow
        .resistance = 0.3f,  // 30% damage reduction
        .symbol = "✦"
    };
}

const EnemyTypeDefinition* GetEnemyTypeDefinition(EnemyType type) {
    if (type < 0 || type >= ENEMY_TYPE_COUNT) {
        return &enemyTypes[ENEMY_GRUNT];  // Default to grunt
    }
    return &enemyTypes[type];
}

void InitializeEnemyFromType(EnemyEx* enemy, EnemyType type, float x, float y) {
    const EnemyTypeDefinition* def = GetEnemyTypeDefinition(type);
    
    // Basic properties
    enemy->position.x = x;
    enemy->position.y = y;
    enemy->active = true;
    enemy->type = type;
    
    // Type-specific properties
    enemy->radius = 20.0f * def->size;  // Base radius of 20
    enemy->health = def->health;
    enemy->maxHealth = def->health;
    enemy->speed = BASE_ENEMY_SPEED * def->speed;
    enemy->speedX = enemy->speed;
    enemy->speedY = 0;
    enemy->power = def->power;
    enemy->resistance = def->resistance;
    
    // Colors
    enemy->color = def->primaryColor;
    enemy->secondaryColor = def->secondaryColor;
    enemy->glowColor = def->glowColor;
    enemy->symbol = def->symbol;
    
    // Bounds
    enemy->bounds = (Rectangle){
        enemy->position.x - enemy->radius,
        enemy->position.y - enemy->radius,
        enemy->radius * 2,
        enemy->radius * 2
    };
    
    // Animation and timers
    enemy->animationTimer = 0;
    enemy->specialTimer = 0;
    enemy->moveTimer = 0;
    enemy->targetY = y;
    
    // Special properties
    enemy->isVisible = true;
    enemy->shieldAngle = 0;
    enemy->swarmGroup = 0;
    
    // Boss-specific initialization (no shield, just high health)
    if (type == ENEMY_BOSS) {
        // Boss has no shield, just lots of health
        enemy->shieldAngle = 0;
        enemy->specialTimer = 0;
    }
    
    // Firing properties
    enemy->fireTimer = 0;
    enemy->nextFireTime = 1.0f / GetEnemyWeaponConfig(type)->fireRate;
    enemy->can_fire = true;  // Default to true, will be overridden by wave system if needed
    
    // Hit tracking
    enemy->hitsTaken = 0;
}

void DrawEnemyEx(const EnemyEx* enemy) {
    if (!enemy->active) return;
    
    // Handle ghost visibility
    float alpha = 1.0f;
    if (enemy->type == ENEMY_GHOST) {
        if (!enemy->isVisible) {
            alpha = 0.3f;  // Semi-transparent when phased
        }
    }
    
    // Main body
    Color mainColor = enemy->color;
    mainColor.a = (unsigned char)(mainColor.a * alpha);
    DrawCircle(enemy->position.x, enemy->position.y, enemy->radius, mainColor);
    
    // Inner circle for depth
    Color innerColor = enemy->secondaryColor;
    innerColor.a = (unsigned char)(innerColor.a * alpha);
    DrawCircle(enemy->position.x, enemy->position.y, enemy->radius * 0.7f, innerColor);
    
    // Special effects based on type
    switch (enemy->type) {
        case ENEMY_TANK:
            // Draw armor plating
            DrawRectangleLines(
                enemy->position.x - enemy->radius * 0.8f,
                enemy->position.y - enemy->radius * 0.8f,
                enemy->radius * 1.6f,
                enemy->radius * 1.6f,
                Fade(DARKGRAY, alpha)
            );
            break;
            
        case ENEMY_SHIELD:
            // Draw rotating shield
            for (int i = 0; i < 6; i++) {
                float angle = enemy->shieldAngle + (i * 60);
                float x = enemy->position.x + cosf(angle * DEG2RAD) * (enemy->radius + 10);
                float y = enemy->position.y + sinf(angle * DEG2RAD) * (enemy->radius + 10);
                DrawCircle(x, y, 5, Fade(enemy->glowColor, alpha * 0.7f));
            }
            break;
            
        case ENEMY_SPEEDER:
            // Draw speed lines
            for (int i = 0; i < 3; i++) {
                DrawLine(
                    enemy->position.x + enemy->radius + i * 5,
                    enemy->position.y - 5 + i * 5,
                    enemy->position.x + enemy->radius + 20 + i * 5,
                    enemy->position.y - 5 + i * 5,
                    Fade(enemy->glowColor, alpha * (0.7f - i * 0.2f))
                );
            }
            break;
            
        case ENEMY_BOMBER:
            // Draw danger indicator
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                          enemy->radius + 5 + sinf(enemy->animationTimer * 5) * 3, 
                          Fade(RED, alpha * 0.5f));
            break;
            
        case ENEMY_ELITE:
            // Draw crown/star effect
            for (int i = 0; i < 5; i++) {
                float angle = (72 * i - 90) * DEG2RAD;
                float x = enemy->position.x + cosf(angle) * (enemy->radius * 0.5f);
                float y = enemy->position.y + sinf(angle) * (enemy->radius * 0.5f);
                DrawCircle(x, y, 3, Fade(GOLD, alpha));
            }
            break;
            
        case ENEMY_BOSS:
            // Draw multiple rings
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                          enemy->radius + 10, Fade(RED, alpha * 0.7f));
            DrawCircleLines(enemy->position.x, enemy->position.y, 
                          enemy->radius + 20, Fade(MAROON, alpha * 0.5f));
            
            // Draw shield if active (shieldAngle stores shield health)
            if (enemy->shieldAngle > 0) {
                float shieldAlpha = (enemy->shieldAngle / (float)BOSS_SHIELD_HEALTH) * 0.5f;
                Color shieldColor = (Color){100, 200, 255, (unsigned char)(255 * shieldAlpha)};
                DrawCircle(enemy->position.x, enemy->position.y, 
                          enemy->radius + 25, shieldColor);
                DrawCircleLines(enemy->position.x, enemy->position.y, 
                              enemy->radius + 25, SKYBLUE);
                
                // Draw shield health indicator
                DrawText(TextFormat("Shield: %d", (int)enemy->shieldAngle), 
                        enemy->position.x - 30, enemy->position.y - enemy->radius - 40, 
                        10, SKYBLUE);
            }
            break;
            
        case ENEMY_SWARM:
            // Draw connection lines to nearby swarm members (would need access to other enemies)
            DrawCircle(enemy->position.x, enemy->position.y, 
                      enemy->radius * 0.3f, Fade(WHITE, alpha * 0.8f));
            break;
            
        case ENEMY_GHOST:
            // Draw phasing effect
            if (!enemy->isVisible) {
                DrawCircleLines(enemy->position.x, enemy->position.y, 
                              enemy->radius + 5, Fade(enemy->glowColor, 0.5f));
            }
            break;
            
        case ENEMY_ZIGZAG:
            // Draw zigzag pattern indicator
            DrawTriangle(
                (Vector2){enemy->position.x - enemy->radius * 0.5f, enemy->position.y},
                (Vector2){enemy->position.x, enemy->position.y - enemy->radius * 0.5f},
                (Vector2){enemy->position.x + enemy->radius * 0.5f, enemy->position.y},
                Fade(enemy->glowColor, alpha * 0.6f)
            );
            break;
            
        default:
            break;
    }
    
    // Glow effect
    DrawCircle(enemy->position.x - enemy->radius * 0.3f, 
              enemy->position.y - enemy->radius * 0.3f,
              enemy->radius * 0.3f, 
              Fade(enemy->glowColor, alpha * 0.5f));
    
    // Draw symbol identifier
    if (enemy->symbol) {
        int fontSize = (int)(enemy->radius * 0.8f);
        int textWidth = MeasureText(enemy->symbol, fontSize);
        DrawText(enemy->symbol, 
                enemy->position.x - textWidth/2, 
                enemy->position.y - fontSize/2,
                fontSize, 
                Fade(WHITE, alpha * 0.9f));
    }
    
    // Health bar (if damaged)
    if (enemy->health < enemy->maxHealth && enemy->maxHealth > 1) {
        float barWidth = enemy->radius * 2;
        float barHeight = 4;
        float healthPercent = (float)enemy->health / (float)enemy->maxHealth;
        
        DrawRectangle(enemy->position.x - enemy->radius, 
                     enemy->position.y - enemy->radius - 10,
                     barWidth, barHeight, Fade(RED, alpha * 0.3f));
        DrawRectangle(enemy->position.x - enemy->radius, 
                     enemy->position.y - enemy->radius - 10,
                     barWidth * healthPercent, barHeight, Fade(GREEN, alpha * 0.8f));
    }
}

const char* GetEnemyTypeName(EnemyType type) {
    const EnemyTypeDefinition* def = GetEnemyTypeDefinition(type);
    return def->name;
}

Color GetEnemyTypeColor(EnemyType type) {
    const EnemyTypeDefinition* def = GetEnemyTypeDefinition(type);
    return def->primaryColor;
}
