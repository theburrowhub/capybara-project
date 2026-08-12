#include "explosion.h"
#include "constants.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Helper function to get random float between min and max
static float RandomFloat(float min, float max) {
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

// Helper function to get random direction vector
static Vector2 RandomDirection(void) {
    float angle = RandomFloat(0, 2 * PI);
    return (Vector2){cosf(angle), sinf(angle)};
}

void InitExplosionSystem(ExplosionSystem* system) {
    memset(system, 0, sizeof(ExplosionSystem));
    
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        system->explosions[i].active = false;
    }
    
    system->activeCount = 0;
    system->screenShakeIntensity = 0.0f;
    system->screenShakeDuration = 0.0f;
    system->screenShakeOffset = (Vector2){0, 0};
}

void UpdateExplosionSystem(ExplosionSystem* system, float deltaTime) {
    // Update screen shake
    if (system->screenShakeDuration > 0) {
        system->screenShakeDuration -= deltaTime;
        if (system->screenShakeDuration <= 0) {
            system->screenShakeIntensity = 0;
            system->screenShakeOffset = (Vector2){0, 0};
        } else {
            // Random shake offset
            system->screenShakeOffset.x = RandomFloat(-system->screenShakeIntensity, system->screenShakeIntensity);
            system->screenShakeOffset.y = RandomFloat(-system->screenShakeIntensity, system->screenShakeIntensity);
            
            // Reduce intensity over time
            system->screenShakeIntensity *= 0.95f;
        }
    }
    
    // Update each explosion
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        Explosion* exp = &system->explosions[i];
        if (!exp->active) continue;
        
        // Update explosion life
        exp->life -= deltaTime * 2.0f; // Fade rate
        if (exp->life <= 0) {
            exp->active = false;
            system->activeCount--;
            continue;
        }
        
        // Expand explosion radius
        if (exp->radius < exp->maxRadius) {
            exp->radius += exp->expandSpeed * deltaTime;
            if (exp->radius > exp->maxRadius) {
                exp->radius = exp->maxRadius;
            }
        }
        
        // Update intensity (fade out)
        exp->intensity = exp->life;
        
        // Update shockwave
        if (exp->hasShockwave) {
            exp->shockwaveRadius += exp->shockwaveSpeed * deltaTime;
            exp->shockwaveAlpha = exp->life * 0.5f;
        }
        
        // Update particles
        for (int j = 0; j < exp->particleCount; j++) {
            Particle* p = &exp->particles[j];
            if (!p->active) continue;
            
            // Update particle physics
            p->position.x += p->velocity.x * deltaTime;
            p->position.y += p->velocity.y * deltaTime;
            
            // Apply gravity for debris-like particles
            if (exp->type == EXPLOSION_PLAYER) {
                p->velocity.y += 200.0f * deltaTime; // Gravity
            }
            
            // Apply drag
            p->velocity.x *= 0.98f;
            p->velocity.y *= 0.98f;
            
            // Fade out
            p->life -= p->fadeRate * deltaTime;
            if (p->life <= 0) {
                p->active = false;
            }
            
            // Shrink particle
            p->size *= 0.98f;
        }
        
        // Update debris pieces
        for (int j = 0; j < exp->debrisCount; j++) {
            Debris* d = &exp->debris[j];
            if (!d->active) continue;
            
            // Update debris physics
            d->position.x += d->velocity.x * deltaTime;
            d->position.y += d->velocity.y * deltaTime;
            d->rotation += d->rotationSpeed * deltaTime;
            
            // Apply gravity
            d->velocity.y += 300.0f * deltaTime;
            
            // Apply drag
            d->velocity.x *= 0.99f;
            d->velocity.y *= 0.99f;
            
            // Fade out
            d->life -= deltaTime * 0.8f;
            if (d->life <= 0) {
                d->active = false;
            }
        }
    }
    
    // Cleanup inactive explosions periodically
    static float cleanupTimer = 0;
    cleanupTimer += deltaTime;
    if (cleanupTimer > 1.0f) {
        CleanupInactiveExplosions(system);
        cleanupTimer = 0;
    }
}

void DrawExplosions(const ExplosionSystem* system) {
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        const Explosion* exp = &system->explosions[i];
        if (!exp->active) continue;
        
        // Draw main explosion circle (multiple layers for better effect)
        float alpha = exp->intensity;
        
        // Outer glow
        DrawCircle(exp->position.x, exp->position.y, 
                  exp->radius * 1.5f, 
                  Fade(exp->outerColor, alpha * 0.2f));
        
        // Middle layer
        DrawCircle(exp->position.x, exp->position.y, 
                  exp->radius * 1.2f, 
                  Fade(exp->outerColor, alpha * 0.4f));
        
        // Inner core
        DrawCircle(exp->position.x, exp->position.y, 
                  exp->radius, 
                  Fade(exp->innerColor, alpha * 0.6f));
        
        // Bright center
        DrawCircle(exp->position.x, exp->position.y, 
                  exp->radius * 0.5f, 
                  Fade(WHITE, alpha * 0.8f));
        
        // Draw shockwave ring
        if (exp->hasShockwave && exp->shockwaveAlpha > 0) {
            DrawCircleLines(exp->position.x, exp->position.y, 
                          exp->shockwaveRadius, 
                          Fade(WHITE, exp->shockwaveAlpha));
            DrawCircleLines(exp->position.x, exp->position.y, 
                          exp->shockwaveRadius * 0.9f, 
                          Fade(exp->outerColor, exp->shockwaveAlpha * 0.7f));
        }
        
        // Draw particles
        for (int j = 0; j < exp->particleCount; j++) {
            const Particle* p = &exp->particles[j];
            if (!p->active) continue;
            
            Color particleColor = Fade(p->color, p->life);
            
            // Draw particle with glow
            DrawCircle(p->position.x, p->position.y, 
                      p->size * 1.5f, 
                      Fade(particleColor, p->life * 0.3f));
            DrawCircle(p->position.x, p->position.y, 
                      p->size, 
                      particleColor);
            
            // Add spark trails for some particles
            if (exp->type == EXPLOSION_PLASMA || exp->type == EXPLOSION_PLAYER) {
                float trailLength = 10.0f * p->life;
                Vector2 trailEnd = {
                    p->position.x - p->velocity.x * 0.05f,
                    p->position.y - p->velocity.y * 0.05f
                };
                DrawLineEx(p->position, trailEnd, p->size * 0.5f, 
                          Fade(particleColor, p->life * 0.5f));
            }
        }
        
        // Draw debris pieces
        for (int j = 0; j < exp->debrisCount; j++) {
            const Debris* d = &exp->debris[j];
            if (!d->active) continue;
            
            // Draw rotating debris piece as a triangle
            Vector2 v1, v2, v3;
            float angle1 = d->rotation;
            float angle2 = d->rotation + 2.094f; // 120 degrees
            float angle3 = d->rotation + 4.189f; // 240 degrees
            
            v1.x = d->position.x + cosf(angle1) * d->size;
            v1.y = d->position.y + sinf(angle1) * d->size;
            v2.x = d->position.x + cosf(angle2) * d->size;
            v2.y = d->position.y + sinf(angle2) * d->size;
            v3.x = d->position.x + cosf(angle3) * d->size;
            v3.y = d->position.y + sinf(angle3) * d->size;
            
            DrawTriangle(v1, v2, v3, Fade(d->color, d->life));
            DrawTriangleLines(v1, v2, v3, Fade(WHITE, d->life * 0.5f));
        }
    }
}

void CreateExplosion(ExplosionSystem* system, Vector2 position, ExplosionType type) {
    // Find an inactive explosion slot
    Explosion* exp = NULL;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!system->explosions[i].active) {
            exp = &system->explosions[i];
            break;
        }
    }
    
    if (!exp) return; // No available slot
    
    // Initialize explosion
    memset(exp, 0, sizeof(Explosion));
    exp->position = position;
    exp->type = type;
    exp->active = true;
    exp->life = 1.0f;
    system->activeCount++;
    
    // Set properties based on type
    switch (type) {
        case EXPLOSION_SMALL:
            exp->maxRadius = 30.0f;
            exp->expandSpeed = 200.0f;
            exp->innerColor = YELLOW;
            exp->outerColor = ORANGE;
            exp->particleCount = 10;
            exp->hasShockwave = false;
            break;
            
        case EXPLOSION_MEDIUM:
            exp->maxRadius = 50.0f;
            exp->expandSpeed = 250.0f;
            exp->innerColor = WHITE;
            exp->outerColor = ORANGE;
            exp->particleCount = 20;
            exp->hasShockwave = true;
            exp->shockwaveSpeed = 300.0f;
            TriggerScreenShake(system, 3.0f, 0.2f);
            break;
            
        case EXPLOSION_LARGE:
            exp->maxRadius = 80.0f;
            exp->expandSpeed = 300.0f;
            exp->innerColor = WHITE;
            exp->outerColor = RED;
            exp->particleCount = 30;
            exp->hasShockwave = true;
            exp->shockwaveSpeed = 400.0f;
            TriggerScreenShake(system, 8.0f, 0.4f);
            break;
            
        case EXPLOSION_PLAYER:
            exp->maxRadius = 100.0f;
            exp->expandSpeed = 350.0f;
            exp->innerColor = SKYBLUE;
            exp->outerColor = BLUE;
            exp->particleCount = 30;
            exp->debrisCount = 15;
            exp->hasShockwave = true;
            exp->shockwaveSpeed = 500.0f;
            TriggerScreenShake(system, 15.0f, 0.6f);
            break;
            
        case EXPLOSION_SHOCKWAVE:
            exp->maxRadius = 150.0f;
            exp->expandSpeed = 500.0f;
            exp->innerColor = Fade(SKYBLUE, 0.5f);
            exp->outerColor = BLUE;
            exp->particleCount = 0;
            exp->hasShockwave = true;
            exp->shockwaveSpeed = 600.0f;
            break;
            
        case EXPLOSION_PLASMA:
            exp->maxRadius = 40.0f;
            exp->expandSpeed = 180.0f;
            exp->innerColor = PURPLE;
            exp->outerColor = VIOLET;
            exp->particleCount = 25;
            exp->hasShockwave = false;
            break;
    }
    
    exp->radius = 5.0f; // Start small
    exp->intensity = 1.0f;
    
    // Create particles
    for (int i = 0; i < exp->particleCount; i++) {
        Particle* p = &exp->particles[i];
        p->active = true;
        p->position = position;
        
        // Random velocity in all directions
        Vector2 dir = RandomDirection();
        float speed = RandomFloat(100, 400);
        p->velocity.x = dir.x * speed;
        p->velocity.y = dir.y * speed;
        
        // Random size and color variation
        p->size = RandomFloat(2, 8);
        p->life = 1.0f;
        p->fadeRate = RandomFloat(1.0f, 3.0f);
        
        // Color based on explosion type
        if (type == EXPLOSION_PLAYER) {
            p->color = (rand() % 2) ? SKYBLUE : WHITE;
        } else if (type == EXPLOSION_PLASMA) {
            p->color = (rand() % 2) ? PURPLE : VIOLET;
        } else {
            p->color = (rand() % 3) ? YELLOW : (rand() % 2) ? ORANGE : RED;
        }
    }
    
    // Create debris for player explosion
    if (type == EXPLOSION_PLAYER && exp->debrisCount > 0) {
        for (int i = 0; i < exp->debrisCount; i++) {
            Debris* d = &exp->debris[i];
            d->active = true;
            d->position = position;
            
            // Random velocity (mostly upward and outward)
            Vector2 dir = RandomDirection();
            float speed = RandomFloat(150, 350);
            d->velocity.x = dir.x * speed;
            d->velocity.y = dir.y * speed - 100; // Bias upward
            
            d->rotation = RandomFloat(0, 2 * PI);
            d->rotationSpeed = RandomFloat(-10, 10);
            d->size = RandomFloat(5, 15);
            d->life = 1.0f;
            d->color = (rand() % 2) ? DARKBLUE : DARKGRAY;
        }
    }
}

void CreateEnemyExplosion(ExplosionSystem* system, Vector2 position, Color enemyColor, float size) {
    // Boss-class enemies (120+) get special multi-explosion sequence!
    if (size > 100) {
        CreateBossExplosion(system, position, enemyColor);
        return;
    }
    
    // Determine explosion type based on enemy size for spectacular scaling
    // Enemy sizes: Swarm(20) Speeder(24) Grunt(32) Zigzag(40) Ghost(44) 
    //              Shield(48) Elite(52) Bomber(60) Tank(80)
    ExplosionType type = EXPLOSION_SMALL;
    float sizeMultiplier = 1.0f;
    int extraParticles = 0;
    
    if (size > 70) {
        // Tank-class enemies (80+) - Large explosion
        type = EXPLOSION_LARGE;
        sizeMultiplier = 1.0f;
        extraParticles = 10;
        TriggerScreenShake(system, 8.0f, 0.4f);  // Medium shake
    } else if (size > 50) {
        // Elite/Bomber class (52-60) - Upper medium explosion
        type = EXPLOSION_MEDIUM;
        sizeMultiplier = 1.2f;  // 20% bigger
        extraParticles = 5;
        TriggerScreenShake(system, 4.0f, 0.25f);  // Light shake
    } else if (size > 35) {
        // Shield/Zigzag class (40-48) - Standard medium explosion
        type = EXPLOSION_MEDIUM;
        sizeMultiplier = 1.0f;
        extraParticles = 0;
        TriggerScreenShake(system, 2.0f, 0.15f);  // Very light shake
    } else {
        // Small enemies (20-32) - Small explosion
        type = EXPLOSION_SMALL;
        sizeMultiplier = 1.0f;
        extraParticles = 0;
        // No screen shake for small enemies
    }
    
    // Find an inactive explosion slot
    Explosion* exp = NULL;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!system->explosions[i].active) {
            exp = &system->explosions[i];
            break;
        }
    }
    
    if (!exp) return;
    
    // Create base explosion
    CreateExplosion(system, position, type);
    
    // Apply size multiplier for extra spectacle
    exp->maxRadius *= sizeMultiplier;
    exp->expandSpeed *= sizeMultiplier;
    
    // Customize colors based on enemy color
    exp->outerColor = enemyColor;
    exp->innerColor = WHITE;
    
    // Add extra particles for larger enemies
    int totalParticles = exp->particleCount + extraParticles;
    if (totalParticles > MAX_PARTICLES_PER_EXPLOSION) {
        totalParticles = MAX_PARTICLES_PER_EXPLOSION;
    }
    
    // Create extra particles
    for (int i = exp->particleCount; i < totalParticles; i++) {
        Vector2 dir = RandomDirection();
        float speed = RandomFloat(50, 150);
        
        exp->particles[i].position = exp->position;
        exp->particles[i].velocity = (Vector2){dir.x * speed, dir.y * speed};
        exp->particles[i].color = enemyColor;
        exp->particles[i].size = RandomFloat(2, 6);
        exp->particles[i].life = 1.0f;
        exp->particles[i].fadeRate = RandomFloat(0.8f, 1.2f);
        exp->particles[i].active = true;
    }
    exp->particleCount = totalParticles;
}

void CreatePlayerExplosion(ExplosionSystem* system, Vector2 position) {
    CreateExplosion(system, position, EXPLOSION_PLAYER);
    
    // Add extra dramatic effects for player death
    // Create secondary explosions
    for (int i = 0; i < 3; i++) {
        Vector2 offset = {
            RandomFloat(-20, 20),
            RandomFloat(-20, 20)
        };
        Vector2 secondaryPos = {
            position.x + offset.x,
            position.y + offset.y
        };
        
        // Delay secondary explosions slightly (would need timer system for true delay)
        CreateExplosion(system, secondaryPos, EXPLOSION_MEDIUM);
    }
}

void CreateShockwaveExplosion(ExplosionSystem* system, Vector2 position, float force) {
    CreateExplosion(system, position, EXPLOSION_SHOCKWAVE);
    
    // Find the explosion we just created and customize it
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (system->explosions[i].active && 
            system->explosions[i].position.x == position.x &&
            system->explosions[i].position.y == position.y) {
            system->explosions[i].shockwaveSpeed = 300.0f * force;
            system->explosions[i].maxRadius = 100.0f * force;
            break;
        }
    }
}

void CreateBossExplosion(ExplosionSystem* system, Vector2 position, Color bossColor) {
    // Create spectacular multi-explosion sequence for boss death
    // This creates 1 main explosion + 8 secondary explosions in a circle pattern
    
    // Main explosion - very large with extended lifetime
    Explosion* mainExp = NULL;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (!system->explosions[i].active) {
            mainExp = &system->explosions[i];
            break;
        }
    }
    
    if (!mainExp) return;
    
    // Initialize main boss explosion
    memset(mainExp, 0, sizeof(Explosion));
    mainExp->position = position;
    mainExp->type = EXPLOSION_LARGE;
    mainExp->active = true;
    mainExp->life = 3.0f;  // 3× longer lifetime - lasts on screen!
    mainExp->radius = 15.0f;
    mainExp->maxRadius = 180.0f;  // Massive radius
    mainExp->expandSpeed = 350.0f;
    mainExp->innerColor = WHITE;
    mainExp->outerColor = bossColor;
    mainExp->intensity = 2.0f;  // Extra bright
    mainExp->hasShockwave = true;
    mainExp->shockwaveSpeed = 600.0f;
    system->activeCount++;
    
    // Create 50 particles for main explosion
    mainExp->particleCount = MAX_PARTICLES_PER_EXPLOSION;
    for (int i = 0; i < mainExp->particleCount; i++) {
        Vector2 dir = RandomDirection();
        float speed = RandomFloat(120, 300);
        
        mainExp->particles[i].position = position;
        mainExp->particles[i].velocity = (Vector2){dir.x * speed, dir.y * speed};
        mainExp->particles[i].color = (i % 2 == 0) ? WHITE : bossColor;
        mainExp->particles[i].size = RandomFloat(4, 10);  // Larger particles
        mainExp->particles[i].life = RandomFloat(2.0f, 3.5f);  // Very long life
        mainExp->particles[i].fadeRate = 0.4f;  // Very slow fade
        mainExp->particles[i].active = true;
    }
    
    // Create 8 secondary explosions in a circle around boss (chain reaction!)
    for (int sec = 0; sec < 8; sec++) {
        Explosion* secExp = NULL;
        for (int i = 0; i < MAX_EXPLOSIONS; i++) {
            if (!system->explosions[i].active) {
                secExp = &system->explosions[i];
                break;
            }
        }
        
        if (!secExp) break;
        
        // Position in circle pattern around boss
        float angle = (sec * 45.0f) * DEG2RAD;  // 8 explosions, 45° apart
        float distance = 60.0f + RandomFloat(-15, 15);
        Vector2 secPos = {
            position.x + cosf(angle) * distance,
            position.y + sinf(angle) * distance
        };
        
        // Initialize secondary explosion
        memset(secExp, 0, sizeof(Explosion));
        secExp->position = secPos;
        secExp->type = (sec % 2 == 0) ? EXPLOSION_LARGE : EXPLOSION_MEDIUM;
        secExp->active = true;
        secExp->life = 2.0f;  // Long lifetime
        secExp->radius = 8.0f;
        secExp->maxRadius = (sec % 2 == 0) ? 90.0f : 65.0f;
        secExp->expandSpeed = 320.0f;
        secExp->innerColor = (sec % 3 == 0) ? bossColor : WHITE;
        secExp->outerColor = bossColor;
        secExp->intensity = 1.2f;
        secExp->hasShockwave = true;
        secExp->shockwaveSpeed = 400.0f;
        system->activeCount++;
        
        // Add particles to secondary explosions
        secExp->particleCount = 20;
        for (int i = 0; i < secExp->particleCount; i++) {
            Vector2 dir = RandomDirection();
            float speed = RandomFloat(80, 180);
            
            secExp->particles[i].position = secPos;
            secExp->particles[i].velocity = (Vector2){dir.x * speed, dir.y * speed};
            secExp->particles[i].color = (i % 2 == 0) ? bossColor : Fade(WHITE, 0.9f);
            secExp->particles[i].size = RandomFloat(3, 7);
            secExp->particles[i].life = RandomFloat(1.5f, 2.5f);
            secExp->particles[i].fadeRate = 0.6f;
            secExp->particles[i].active = true;
        }
    }
    
    // Trigger massive screen shake
    TriggerScreenShake(system, 25.0f, 1.2f);  // Very intense and long!
}

Vector2 GetScreenShakeOffset(const ExplosionSystem* system) {
    return system->screenShakeOffset;
}

void TriggerScreenShake(ExplosionSystem* system, float intensity, float duration) {
    // Only apply if new shake is stronger
    if (intensity > system->screenShakeIntensity) {
        system->screenShakeIntensity = intensity;
        system->screenShakeDuration = duration;
    }
}

void CleanupInactiveExplosions(ExplosionSystem* system) {
    // This function is called periodically to ensure the explosion array stays clean
    // Currently, explosions are already cleaned up when they become inactive
    // This is here for future expansion if needed
    system->activeCount = 0;
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
        if (system->explosions[i].active) {
            system->activeCount++;
        }
    }
}
