#include "player.h"
#include "constants.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

void InitPlayer(Player* player) {
    player->position = (Vector2){100, SCREEN_HEIGHT/2};
    player->bounds = (Rectangle){
        player->position.x - 20, 
        player->position.y - 15, 
        40, 
        30
    };
    player->color = GREEN;
    player->health = 3;
    player->weaponHeat = 0.0f;
    player->maxHeat = 100.0f;
    player->overheated = false;
    player->cooldownTime = 0.0f;
}

void UpdatePlayer(Game* game) {
    Player *player = &game->player;
    Vector2 oldPos = player->position;
    
    // Movement
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        player->position.y -= PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        player->position.y += PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        player->position.x -= PLAYER_SPEED;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        player->position.x += PLAYER_SPEED;
    }
    
    // Keep player on screen
    if (player->position.x < 30) player->position.x = 30;
    if (player->position.x > SCREEN_WIDTH/2) player->position.x = SCREEN_WIDTH/2;
    if (player->position.y < 20) player->position.y = 20;
    if (player->position.y > SCREEN_HEIGHT - 20) player->position.y = SCREEN_HEIGHT - 20;
    
    // Update bounds - IMPORTANT: These must be correctly aligned with the visual representation
    player->bounds.x = player->position.x - 20;
    player->bounds.y = player->position.y - 15;
    player->bounds.width = 40;
    player->bounds.height = 30;
    
    // Log significant movement
    if (game->logFile && (fabs(oldPos.x - player->position.x) > 0 || fabs(oldPos.y - player->position.y) > 0)) {
        static int moveCounter = 0;
        moveCounter++;
        if (moveCounter % 60 == 0) { // Log every second of movement
            LogPlayerMovement(game, oldPos, player->position);
        }
    }
}

void DrawPlayer(const Player* player, bool showHitbox) {
    // Draw player (spaceship shape)
    Vector2 v1 = {player->position.x - 20, player->position.y};
    Vector2 v2 = {player->position.x + 20, player->position.y - 15};
    Vector2 v3 = {player->position.x + 20, player->position.y + 15};
    DrawTriangle(v1, v3, v2, player->color);
    
    // Debug: Show hitbox when requested
    if (showHitbox) {
        DrawRectangleLines(
            player->bounds.x, player->bounds.y,
            player->bounds.width, player->bounds.height,
            LIME
        );
    }
}
