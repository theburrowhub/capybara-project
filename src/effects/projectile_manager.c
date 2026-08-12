#include "projectile_manager.h"

void ProjectileManager_UpdateAll(ProjectileManager* mgr, float deltaTime) {
    for (int i = 0; i < mgr->maxProjectiles; i++) {
        if (mgr->projectiles[i].active) {
            // Update projectile physics and behavior
            UpdateProjectile(&mgr->projectiles[i], deltaTime);
            
            // Cull projectiles that go off screen
            Projectile* proj = &mgr->projectiles[i];
            if (proj->position.x < mgr->minX || proj->position.x > mgr->maxX ||
                proj->position.y < mgr->minY || proj->position.y > mgr->maxY) {
                proj->active = false;
            }
        }
    }
}

void ProjectileManager_DrawAll(const ProjectileManager* mgr) {
    for (int i = 0; i < mgr->maxProjectiles; i++) {
        if (mgr->projectiles[i].active) {
            DrawProjectile(&mgr->projectiles[i]);
        }
    }
}

int ProjectileManager_CountActive(const ProjectileManager* mgr) {
    int count = 0;
    for (int i = 0; i < mgr->maxProjectiles; i++) {
        if (mgr->projectiles[i].active) {
            count++;
        }
    }
    return count;
}

void ProjectileManager_InitAll(ProjectileManager* mgr) {
    for (int i = 0; i < mgr->maxProjectiles; i++) {
        mgr->projectiles[i].active = false;
    }
}
