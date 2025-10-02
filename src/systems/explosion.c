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
    // Determine explosion type based on size
    ExplosionType type = EXPLOSION_SMALL;
    if (size > 60) {
        type = EXPLOSION_LARGE;
    } else if (size > 30) {
        type = EXPLOSION_MEDIUM;
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
    
    // Customize colors based on enemy color
    exp->outerColor = enemyColor;
    exp->innerColor = WHITE;
    
    // Add some enemy-specific particles
    for (int i = 0; i < exp->particleCount; i++) {
        if (rand() % 2) {
            exp->particles[i].color = enemyColor;
        }
    }
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
