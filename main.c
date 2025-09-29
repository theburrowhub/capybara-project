#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

// Constants
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 600
#define MAX_BULLETS 50
#define MAX_ENEMIES 20
#define MAX_OBSTACLES 15
#define PLAYER_SPEED 5.0f
#define BULLET_SPEED 12.0f
#define BASE_ENEMY_SPEED 3.0f
#define BASE_SCROLL_SPEED 2.0f
#define BASE_OBSTACLE_SPEED 2.0f
#define SPEED_INCREMENT 0.5f
#define SPEED_UPDATE_INTERVAL 20  // Seconds (reduced from 60)

// Structs
typedef struct Player {
    Vector2 position;
    Rectangle bounds;
    Color color;
    int health;
    float weaponHeat;     // Current weapon heat (0-100)
    float maxHeat;        // Maximum heat before overheat
    bool overheated;      // Is weapon overheated?
    float cooldownTime;   // Time remaining for cooldown
} Player;

typedef struct Bullet {
    Vector2 position;
    Rectangle bounds;
    bool active;
    float speed;
} Bullet;

typedef struct Enemy {
    Vector2 position;
    Rectangle bounds;
    Color color;
    bool active;
    float speed;
    int health;
    int id;  // Unique ID
    float radius;         // Circle radius
    float moveTimer;      // For random movement
    float targetY;        // Target Y position for movement
    float speedX;         // Individual X speed
    float speedY;         // Individual Y speed
} Enemy;

typedef struct Obstacle {
    Vector2 position;
    Rectangle bounds;
    Color color;
    bool active;
    bool destructible;  // Can be destroyed by bullets
    int health;
    float width;
    float height;
    int id;  // Unique ID
} Obstacle;

typedef struct Game {
    Player player;
    Bullet bullets[MAX_BULLETS];
    Enemy enemies[MAX_ENEMIES];
    Obstacle obstacles[MAX_OBSTACLES];
    int score;
    int enemySpawnTimer;
    int obstacleSpawnTimer;
    float backgroundX;
    bool gameOver;
    float gameTime;  // Time in seconds
    float scrollSpeed;
    float enemySpeed;
    float obstacleSpeed;
    int speedLevel;  // Current speed level (increases every minute)
    // Collision logging
    char deathCause[256];
    FILE* logFile;
    int nextEnemyId;
    int nextObstacleId;
} Game;

// Function prototypes
void InitGame(Game *game);
void UpdatePlayer(Game *game);
void UpdateBullets(Game *game);
void UpdateEnemies(Game *game);
void UpdateObstacles(Game *game);
void CheckCollisions(Game *game);
void DrawGame(Game *game);
void ShootBullet(Bullet bullets[], Vector2 position);
void SpawnEnemy(Game *game);
void SpawnObstacle(Game *game);
void LogCollision(Game *game, const char* type, int id, Rectangle obj1, Rectangle obj2);
int CountActiveEnemies(Game *game);
int CountActiveObstacles(Game *game);

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shoot'em Up - Prototype");
    SetTargetFPS(60);
    
    Game game;
    InitGame(&game);
    
    while (!WindowShouldClose()) {
        if (!game.gameOver) {
            // Update game time
            float deltaTime = GetFrameTime();
            game.gameTime += deltaTime;
            
            // Update speed every minute
            int newSpeedLevel = (int)(game.gameTime / SPEED_UPDATE_INTERVAL) + 1;
            if (newSpeedLevel != game.speedLevel) {
                game.speedLevel = newSpeedLevel;
                game.scrollSpeed = BASE_SCROLL_SPEED + (SPEED_INCREMENT * (game.speedLevel - 1));
                game.enemySpeed = BASE_ENEMY_SPEED + (SPEED_INCREMENT * (game.speedLevel - 1));
                game.obstacleSpeed = BASE_OBSTACLE_SPEED + (SPEED_INCREMENT * (game.speedLevel - 1));
                
                // Update existing enemies speed
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (game.enemies[i].active) {
                        game.enemies[i].speed = game.enemySpeed;
                    }
                }
            }
            
            // Update
            UpdatePlayer(&game);
            UpdateBullets(&game);
            UpdateEnemies(&game);
            UpdateObstacles(&game);
            CheckCollisions(&game);
            
            // Background scroll with dynamic speed
            game.backgroundX -= game.scrollSpeed;
            if (game.backgroundX <= -SCREEN_WIDTH) {
                game.backgroundX = 0;
            }
            
            // Game over check
            if (game.player.health <= 0) {
                game.gameOver = true;
            }
        } else {
            // Restart game
            if (IsKeyPressed(KEY_R)) {
                InitGame(&game);
            }
        }
        
        // Draw
        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawGame(&game);
        EndDrawing();
    }
    
    CloseWindow();
    
    // Close log file
    if (game.logFile) {
        fprintf(game.logFile, "\n=== Game Session Ended ===\n");
        fclose(game.logFile);
    }
    
    return 0;
}

void InitGame(Game *game) {
    // Open log file
    game->logFile = fopen("collision_log.txt", "w");
    if (game->logFile) {
        time_t now = time(NULL);
        fprintf(game->logFile, "=== Collision Log Started: %s", ctime(&now));
        fprintf(game->logFile, "Format: [Time] Event - Details\n\n");
    }
    
    // Initialize player
    game->player.position = (Vector2){100, SCREEN_HEIGHT/2};
    game->player.bounds = (Rectangle){game->player.position.x - 20, game->player.position.y - 15, 40, 30};
    game->player.color = GREEN;
    game->player.health = 3;
    game->player.weaponHeat = 0.0f;
    game->player.maxHeat = 100.0f;
    game->player.overheated = false;
    game->player.cooldownTime = 0.0f;
    
    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
        game->bullets[i].speed = BULLET_SPEED;
    }
    
    // Initialize enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].active = false;
        game->enemies[i].speed = BASE_ENEMY_SPEED;
    }
    
    // Initialize obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        game->obstacles[i].active = false;
    }
    
    game->score = 0;
    game->enemySpawnTimer = 0;
    game->obstacleSpawnTimer = 0;
    game->backgroundX = 0;
    game->gameOver = false;
    game->gameTime = 0.0f;
    game->scrollSpeed = BASE_SCROLL_SPEED;
    game->enemySpeed = BASE_ENEMY_SPEED;
    game->obstacleSpeed = BASE_OBSTACLE_SPEED;
    game->speedLevel = 1;
    game->nextEnemyId = 1;
    game->nextObstacleId = 1;
    strcpy(game->deathCause, "Alive");
}

void UpdatePlayer(Game *game) {
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
            fprintf(game->logFile, "[%.2f] Player Position: (%.0f,%.0f) Bounds: (%.0f,%.0f,%.0f,%.0f)\n",
                    game->gameTime, player->position.x, player->position.y,
                    player->bounds.x, player->bounds.y, player->bounds.width, player->bounds.height);
            fflush(game->logFile);
        }
    }
}

void ShootBullet(Bullet bullets[], Vector2 position) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            bullets[i].position = position;
            bullets[i].position.x += 25;
            bullets[i].bounds = (Rectangle){bullets[i].position.x - 5, bullets[i].position.y - 2, 10, 4};
            bullets[i].active = true;
            break;
        }
    }
}

void UpdateBullets(Game *game) {
    Bullet *bullets = game->bullets;
    Player *player = &game->player;
    float deltaTime = GetFrameTime();
    
    // Update weapon heat system
    if (player->overheated) {
        player->cooldownTime -= deltaTime;
        if (player->cooldownTime <= 0) {
            player->overheated = false;
            player->weaponHeat = 0;
            player->cooldownTime = 0;
        }
    } else {
        // Cool down weapon when not shooting
        if (player->weaponHeat > 0 && !IsKeyDown(KEY_SPACE) && !IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            player->weaponHeat -= 30.0f * deltaTime;  // Cool down rate
            if (player->weaponHeat < 0) player->weaponHeat = 0;
        }
    }
    
    // Shoot only if not overheated
    static float shootTimer = 0;
    shootTimer -= deltaTime;
    
    if ((IsKeyDown(KEY_SPACE) || IsMouseButtonDown(MOUSE_LEFT_BUTTON)) && 
        !player->overheated && shootTimer <= 0) {
        ShootBullet(bullets, player->position);
        shootTimer = 0.12f;  // Fire rate
        
        // Increase heat
        player->weaponHeat += 8.0f;  // Heat per shot
        
        // Check for overheat
        if (player->weaponHeat >= player->maxHeat) {
            player->weaponHeat = player->maxHeat;
            player->overheated = true;
            player->cooldownTime = 3.0f;  // 3 seconds cooldown
            
            if (game->logFile) {
                fprintf(game->logFile, "[%.2f] Weapon overheated! 3 second cooldown initiated\n", game->gameTime);
                fflush(game->logFile);
            }
        }
    }
    
    // Update bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].position.x += bullets[i].speed;
            bullets[i].bounds.x = bullets[i].position.x - 5;
            
            // Deactivate off-screen bullets
            if (bullets[i].position.x > SCREEN_WIDTH + 50) {
                bullets[i].active = false;
            }
        }
    }
}

void SpawnEnemy(Game *game) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) {
            game->enemies[i].position.x = SCREEN_WIDTH + 50;
            game->enemies[i].position.y = GetRandomValue(50, SCREEN_HEIGHT - 50);
            game->enemies[i].radius = GetRandomValue(15, 25);  // Random size
            game->enemies[i].bounds = (Rectangle){
                game->enemies[i].position.x - game->enemies[i].radius,
                game->enemies[i].position.y - game->enemies[i].radius,
                game->enemies[i].radius * 2,
                game->enemies[i].radius * 2
            };
            game->enemies[i].color = RED;
            game->enemies[i].active = true;
            game->enemies[i].health = 1;
            game->enemies[i].id = game->nextEnemyId++;
            game->enemies[i].speed = game->enemySpeed;
            game->enemies[i].moveTimer = 0;
            game->enemies[i].targetY = game->enemies[i].position.y;
            game->enemies[i].speedX = game->enemySpeed + GetRandomValue(-10, 10) * 0.1f;
            game->enemies[i].speedY = 0;
            
            if (game->logFile) {
                fprintf(game->logFile, "[%.2f] Enemy spawned - ID:%d Pos:(%.0f,%.0f) Radius:%.0f\n", 
                        game->gameTime, game->enemies[i].id, 
                        game->enemies[i].position.x, game->enemies[i].position.y,
                        game->enemies[i].radius);
                fflush(game->logFile);
            }
            break;
        }
    }
}

void UpdateEnemies(Game *game) {
    // Spawn timer - reduce spawn interval as speed increases
    game->enemySpawnTimer++;
    int spawnInterval = 60 - (game->speedLevel - 1) * 5;  // Faster spawning at higher speeds
    if (spawnInterval < 20) spawnInterval = 20;  // Minimum spawn interval
    
    if (game->enemySpawnTimer >= spawnInterval) {
        SpawnEnemy(game);
        game->enemySpawnTimer = 0;
    }
    
    // Update enemies with random movement
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            // Update position
            game->enemies[i].position.x -= game->enemies[i].speedX;
            
            // Random movement pattern
            game->enemies[i].moveTimer += GetFrameTime();
            
            // Change direction randomly every 0.5-2 seconds
            if (game->enemies[i].moveTimer > GetRandomValue(5, 20) * 0.1f) {
                game->enemies[i].moveTimer = 0;
                game->enemies[i].targetY = GetRandomValue(50, SCREEN_HEIGHT - 50);
                
                // Occasionally do zigzag or spiral
                int pattern = GetRandomValue(0, 3);
                if (pattern == 0) {
                    // Zigzag
                    game->enemies[i].speedY = GetRandomValue(-3, 3);
                } else if (pattern == 1) {
                    // Dive towards player
                    float diffY = game->player.position.y - game->enemies[i].position.y;
                    game->enemies[i].speedY = (diffY > 0 ? 2 : -2);
                } else {
                    // Smooth movement to target
                    game->enemies[i].speedY = 0;
                }
            }
            
            // Apply vertical movement
            float diffY = game->enemies[i].targetY - game->enemies[i].position.y;
            if (fabs(diffY) > 5) {
                game->enemies[i].position.y += (diffY > 0 ? 1 : -1) * 2 + game->enemies[i].speedY;
            }
            
            // Add some wobble
            game->enemies[i].position.y += sinf(game->gameTime * 5 + i) * 1.5f;
            
            // Keep in bounds
            if (game->enemies[i].position.y < 30) game->enemies[i].position.y = 30;
            if (game->enemies[i].position.y > SCREEN_HEIGHT - 30) game->enemies[i].position.y = SCREEN_HEIGHT - 30;
            
            // Update bounds
            game->enemies[i].bounds.x = game->enemies[i].position.x - game->enemies[i].radius;
            game->enemies[i].bounds.y = game->enemies[i].position.y - game->enemies[i].radius;
            
            // Deactivate off-screen enemies
            if (game->enemies[i].position.x < -50) {
                game->enemies[i].active = false;
            }
        }
    }
}

void SpawnObstacle(Game *game) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!game->obstacles[i].active) {
            game->obstacles[i].position.x = SCREEN_WIDTH + 50;
            // Procedural generation - random position and size
            game->obstacles[i].position.y = GetRandomValue(80, SCREEN_HEIGHT - 80);
            game->obstacles[i].width = GetRandomValue(30, 70);
            game->obstacles[i].height = GetRandomValue(30, 80);
            
            // 60% chance of being destructible
            game->obstacles[i].destructible = (GetRandomValue(1, 10) <= 6);
            
            if (game->obstacles[i].destructible) {
                game->obstacles[i].color = ORANGE;  // Destructible obstacles are orange
                game->obstacles[i].health = 2;      // Takes 2 hits to destroy
            } else {
                game->obstacles[i].color = GRAY;    // Indestructible obstacles are gray
                game->obstacles[i].health = 999;    // Cannot be destroyed
            }
            
            game->obstacles[i].bounds = (Rectangle){
                game->obstacles[i].position.x - game->obstacles[i].width/2,
                game->obstacles[i].position.y - game->obstacles[i].height/2,
                game->obstacles[i].width,
                game->obstacles[i].height
            };
            game->obstacles[i].active = true;
            game->obstacles[i].id = game->nextObstacleId++;
            
            if (game->logFile) {
                fprintf(game->logFile, "[%.2f] Obstacle spawned - ID:%d Type:%s Pos:(%.0f,%.0f) Size:(%.0fx%.0f)\n", 
                        game->gameTime, game->obstacles[i].id,
                        game->obstacles[i].destructible ? "Destructible" : "Indestructible",
                        game->obstacles[i].position.x, game->obstacles[i].position.y,
                        game->obstacles[i].width, game->obstacles[i].height);
                fflush(game->logFile);
            }
            break;
        }
    }
}

void UpdateObstacles(Game *game) {
    // Spawn timer - reduce spawn interval as speed increases
    game->obstacleSpawnTimer++;
    int spawnInterval = 90 - (game->speedLevel - 1) * 10;  // Faster spawning at higher speeds
    if (spawnInterval < 30) spawnInterval = 30;  // Minimum spawn interval
    
    if (game->obstacleSpawnTimer >= spawnInterval) {
        SpawnObstacle(game);
        game->obstacleSpawnTimer = 0;
    }
    
    // Update obstacles with dynamic speed
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (game->obstacles[i].active) {
            game->obstacles[i].position.x -= game->obstacleSpeed;
            // Update BOTH x and y bounds to ensure they're properly aligned
            game->obstacles[i].bounds.x = game->obstacles[i].position.x - game->obstacles[i].width/2;
            game->obstacles[i].bounds.y = game->obstacles[i].position.y - game->obstacles[i].height/2;
            
            // Deactivate off-screen obstacles
            if (game->obstacles[i].position.x < -100) {
                game->obstacles[i].active = false;
            }
        }
    }
}

void CheckCollisions(Game *game) {
    // Check bullet-enemy collisions
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (game->bullets[b].active) {
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (game->enemies[e].active) {
                    if (CheckCollisionRecs(game->bullets[b].bounds, game->enemies[e].bounds)) {
                        game->bullets[b].active = false;
                        game->enemies[e].health--;
                        
                        if (game->enemies[e].health <= 0) {
                            game->enemies[e].active = false;
                            game->score += 100;
                        }
                    }
                }
            }
        }
    }
    
    // Check bullet-obstacle collisions
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (game->bullets[b].active) {
            for (int o = 0; o < MAX_OBSTACLES; o++) {
                if (game->obstacles[o].active) {
                    if (CheckCollisionRecs(game->bullets[b].bounds, game->obstacles[o].bounds)) {
                        game->bullets[b].active = false;
                        
                        // Only damage destructible obstacles
                        if (game->obstacles[o].destructible) {
                            game->obstacles[o].health--;
                            
                            // Change color when damaged
                            if (game->obstacles[o].health == 1) {
                                game->obstacles[o].color = Fade(ORANGE, 0.5f);
                            }
                            
                            if (game->obstacles[o].health <= 0) {
                                game->obstacles[o].active = false;
                                game->score += 50;  // Bonus points for destroying obstacles
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Check player-enemy collisions
    for (int e = 0; e < MAX_ENEMIES; e++) {
        if (game->enemies[e].active) {
            if (CheckCollisionRecs(game->player.bounds, game->enemies[e].bounds)) {
                LogCollision(game, "Enemy", game->enemies[e].id, game->player.bounds, game->enemies[e].bounds);
                
                snprintf(game->deathCause, sizeof(game->deathCause), 
                        "Collision with Enemy #%d at position (%.0f, %.0f)", 
                        game->enemies[e].id, game->enemies[e].position.x, game->enemies[e].position.y);
                
                game->enemies[e].active = false;
                game->player.health--;
            }
        }
    }
    
    // Check player-obstacle collisions (instant game over)
    for (int o = 0; o < MAX_OBSTACLES; o++) {
        if (game->obstacles[o].active) {
            // Add a small margin to avoid false positives
            Rectangle playerHitbox = {
                game->player.bounds.x + 3,
                game->player.bounds.y + 3,
                game->player.bounds.width - 6,
                game->player.bounds.height - 6
            };
            Rectangle obstacleHitbox = {
                game->obstacles[o].bounds.x + 3,
                game->obstacles[o].bounds.y + 3,
                game->obstacles[o].bounds.width - 6,
                game->obstacles[o].bounds.height - 6
            };
            
            // Log before checking collision for debugging
            float distX = fabs(game->player.position.x - game->obstacles[o].position.x);
            float distY = fabs(game->player.position.y - game->obstacles[o].position.y);
            
            if (distX < 200 && distY < 200 && game->logFile) {
                fprintf(game->logFile, "[%.2f] NEAR MISS - Obstacle #%d Distance: X=%.1f Y=%.1f\n",
                        game->gameTime, game->obstacles[o].id, distX, distY);
                fflush(game->logFile);
            }
            
            if (CheckCollisionRecs(playerHitbox, obstacleHitbox)) {
                LogCollision(game, "Obstacle", game->obstacles[o].id, playerHitbox, obstacleHitbox);
                
                snprintf(game->deathCause, sizeof(game->deathCause), 
                        "Collision with %s Obstacle #%d at position (%.0f, %.0f) - Size: %.0fx%.0f",
                        game->obstacles[o].destructible ? "Destructible" : "Indestructible",
                        game->obstacles[o].id, 
                        game->obstacles[o].position.x, game->obstacles[o].position.y,
                        game->obstacles[o].width, game->obstacles[o].height);
                
                game->player.health = 0;  // Instant death
                game->gameOver = true;
            }
        }
    }
}

void DrawGame(Game *game) {
    // Draw scrolling background
    for (int i = 0; i < 3; i++) {
        float x = game->backgroundX + (i * SCREEN_WIDTH);
        // Stars
        for (int j = 0; j < 20; j++) {
            int starX = (int)(x + (j * 60) + ((j * 17) % 40));
            int starY = (j * 47) % SCREEN_HEIGHT;
            if (starX >= 0 && starX <= SCREEN_WIDTH) {
                DrawCircle(starX, starY, 2, WHITE);
            }
        }
    }
    
    // Draw player (spaceship shape)
    Vector2 v1 = {game->player.position.x - 20, game->player.position.y};
    Vector2 v2 = {game->player.position.x + 20, game->player.position.y - 15};
    Vector2 v3 = {game->player.position.x + 20, game->player.position.y + 15};
    DrawTriangle(v1, v3, v2, game->player.color);
    
    // Debug: Show hitbox when pressing H
    if (IsKeyDown(KEY_H)) {
        DrawRectangleLines(
            game->player.bounds.x, game->player.bounds.y,
            game->player.bounds.width, game->player.bounds.height,
            LIME
        );
    }
    
    // Draw bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            DrawRectangle(game->bullets[i].position.x - 5, game->bullets[i].position.y - 2, 10, 4, YELLOW);
        }
    }
    
    // Draw enemies (circles with gradient)
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            // Draw enemy as circle with gradient effect
            DrawCircle(game->enemies[i].position.x, game->enemies[i].position.y, 
                      game->enemies[i].radius, game->enemies[i].color);
            DrawCircle(game->enemies[i].position.x, game->enemies[i].position.y, 
                      game->enemies[i].radius * 0.7f, Fade(MAROON, 0.7f));
            DrawCircle(game->enemies[i].position.x - game->enemies[i].radius * 0.3f, 
                      game->enemies[i].position.y - game->enemies[i].radius * 0.3f, 
                      game->enemies[i].radius * 0.3f, Fade(WHITE, 0.3f));
            
            // Draw enemy ID
            DrawText(TextFormat("%d", game->enemies[i].id), 
                    game->enemies[i].position.x - 8, game->enemies[i].position.y - 8, 
                    15, WHITE);
            
            // Debug: Show hitbox when pressing H
            if (IsKeyDown(KEY_H)) {
                DrawRectangleLines(
                    game->enemies[i].bounds.x, game->enemies[i].bounds.y,
                    game->enemies[i].bounds.width, game->enemies[i].bounds.height,
                    PINK
                );
            }
        }
    }
    
    // Draw obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (game->obstacles[i].active) {
            // Draw main obstacle body
            DrawRectangle(
                game->obstacles[i].position.x - game->obstacles[i].width/2,
                game->obstacles[i].position.y - game->obstacles[i].height/2,
                game->obstacles[i].width,
                game->obstacles[i].height,
                game->obstacles[i].color
            );
            
            // Draw obstacle ID
            DrawText(TextFormat("#%d", game->obstacles[i].id), 
                    game->obstacles[i].position.x - 15, game->obstacles[i].position.y - 5,
                    18, BLACK);
            
            // Draw border to differentiate types
            if (game->obstacles[i].destructible) {
                // Destructible obstacles have dashed lines pattern
                for (int j = 0; j < 4; j++) {
                    DrawRectangleLines(
                        game->obstacles[i].position.x - game->obstacles[i].width/2 + j*2,
                        game->obstacles[i].position.y - game->obstacles[i].height/2 + j*2,
                        game->obstacles[i].width - j*4,
                        game->obstacles[i].height - j*4,
                        Fade(YELLOW, 0.5f)
                    );
                }
            } else {
                // Indestructible obstacles have solid thick border
                DrawRectangleLines(
                    game->obstacles[i].position.x - game->obstacles[i].width/2 - 2,
                    game->obstacles[i].position.y - game->obstacles[i].height/2 - 2,
                    game->obstacles[i].width + 4,
                    game->obstacles[i].height + 4,
                    DARKGRAY
                );
                DrawRectangleLines(
                    game->obstacles[i].position.x - game->obstacles[i].width/2,
                    game->obstacles[i].position.y - game->obstacles[i].height/2,
                    game->obstacles[i].width,
                    game->obstacles[i].height,
                    BLACK
                );
            }
            
            // Debug: Show hitbox when pressing H
            if (IsKeyDown(KEY_H)) {
                DrawRectangleLines(
                    game->obstacles[i].bounds.x, game->obstacles[i].bounds.y,
                    game->obstacles[i].bounds.width, game->obstacles[i].bounds.height,
                    MAGENTA
                );
            }
        }
    }
    
    // Draw UI
    DrawText(TextFormat("Score: %d", game->score), 10, 10, 30, WHITE);
    DrawText(TextFormat("Health: %d", game->player.health), 10, 50, 30, WHITE);
    
    // Draw weapon heat bar
    DrawText("Weapon Heat:", 10, 85, 20, WHITE);
    float heatBarWidth = 150;
    float heatFillWidth = (game->player.weaponHeat / game->player.maxHeat) * heatBarWidth;
    
    // Background bar
    DrawRectangle(120, 85, heatBarWidth, 20, DARKGRAY);
    
    // Heat fill bar - color changes based on heat level
    Color heatColor;
    if (game->player.overheated) {
        heatColor = PURPLE;  // Overheated
        DrawText("OVERHEATED!", 280, 85, 20, RED);
        DrawText(TextFormat("Cooldown: %.1fs", game->player.cooldownTime), 280, 105, 18, ORANGE);
    } else if (game->player.weaponHeat > 75) {
        heatColor = RED;     // Critical
    } else if (game->player.weaponHeat > 50) {
        heatColor = ORANGE;  // Warning
    } else if (game->player.weaponHeat > 25) {
        heatColor = YELLOW;  // Warm
    } else {
        heatColor = GREEN;   // Cool
    }
    
    DrawRectangle(120, 85, heatFillWidth, 20, heatColor);
    DrawRectangleLines(120, 85, heatBarWidth, 20, WHITE);
    
    // Draw speed level and time
    DrawText(TextFormat("Speed Level: %d", game->speedLevel), 10, 115, 25, YELLOW);
    int minutes = (int)(game->gameTime / 60);
    int seconds = (int)game->gameTime % 60;
    DrawText(TextFormat("Time: %02d:%02d", minutes, seconds), 10, 145, 25, WHITE);
    
    // Show speed increase warning
    int timeUntilNextSpeed = SPEED_UPDATE_INTERVAL - ((int)game->gameTime % SPEED_UPDATE_INTERVAL);
    if (timeUntilNextSpeed <= 5 && timeUntilNextSpeed > 0) {
        DrawText(TextFormat("SPEED INCREASE IN %d!", timeUntilNextSpeed), 
                SCREEN_WIDTH/2 - 150, 50, 30, RED);
    }
    
    // Draw controls
    DrawText("Controls: WASD/Arrows - Move | SPACE/Click - Shoot | H - Show Hitboxes", 10, SCREEN_HEIGHT - 30, 20, WHITE);
    
    // Draw debug info
    if (!game->gameOver) {
        DrawText(TextFormat("Enemies: %d | Obstacles: %d", 
                CountActiveEnemies(game), CountActiveObstacles(game)), 
                SCREEN_WIDTH - 250, 10, 20, GREEN);
    }
    
    if (game->gameOver) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
        DrawText("GAME OVER", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 50, RED);
        
        // Show death cause with better formatting
        int textWidth = MeasureText(game->deathCause, 20);
        DrawText("Cause of Death:", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 30, 22, ORANGE);
        DrawText(game->deathCause, SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2, 20, YELLOW);
        
        DrawText(TextFormat("Final Score: %d", game->score), SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 40, 30, WHITE);
        DrawText("Press R to Restart", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 80, 25, WHITE);
        DrawText("Check collision_log.txt for details", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 110, 18, GRAY);
    }
}

// Helper functions
int CountActiveEnemies(Game *game) {
    int count = 0;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) count++;
    }
    return count;
}

int CountActiveObstacles(Game *game) {
    int count = 0;
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (game->obstacles[i].active) count++;
    }
    return count;
}

void LogCollision(Game *game, const char* type, int id, Rectangle obj1, Rectangle obj2) {
    if (game->logFile) {
        fprintf(game->logFile, "\n=== COLLISION DETECTED ===\n");
        fprintf(game->logFile, "[%.2f] %s #%d\n", game->gameTime, type, id);
        fprintf(game->logFile, "Player Bounds: X=%.1f Y=%.1f W=%.1f H=%.1f\n", 
                obj1.x, obj1.y, obj1.width, obj1.height);
        fprintf(game->logFile, "%s Bounds: X=%.1f Y=%.1f W=%.1f H=%.1f\n", type,
                obj2.x, obj2.y, obj2.width, obj2.height);
        fprintf(game->logFile, "Player Position: (%.1f, %.1f)\n", 
                game->player.position.x, game->player.position.y);
        fprintf(game->logFile, "=========================\n\n");
        fflush(game->logFile);
    }
}
