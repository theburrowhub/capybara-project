#include "utils.h"
#include <time.h>
#include <stdarg.h>
#include <string.h>

void InitLogger(Game* game) {
    game->logFile = fopen("collision_log.txt", "w");
    if (game->logFile) {
        time_t now = time(NULL);
        fprintf((FILE*)game->logFile, "=== Collision Log Started: %s", ctime(&now));
        fprintf((FILE*)game->logFile, "Format: [Time] Event - Details\n\n");
    }
}

void CloseLogger(Game* game) {
    if (game->logFile) {
        fprintf((FILE*)game->logFile, "\n=== Game Session Ended ===\n");
        fclose((FILE*)game->logFile);
        game->logFile = NULL;
    }
}

void LogCollision(Game* game, const char* type, int id, Rectangle obj1, Rectangle obj2) {
    if (game->logFile) {
        FILE* file = (FILE*)game->logFile;
        fprintf(file, "\n=== COLLISION DETECTED ===\n");
        fprintf(file, "[%.2f] %s #%d\n", game->gameTime, type, id);
        fprintf(file, "Player Bounds: X=%.1f Y=%.1f W=%.1f H=%.1f\n", 
                obj1.x, obj1.y, obj1.width, obj1.height);
        fprintf(file, "%s Bounds: X=%.1f Y=%.1f W=%.1f H=%.1f\n", type,
                obj2.x, obj2.y, obj2.width, obj2.height);
        fprintf(file, "Player Position: (%.1f, %.1f)\n", 
                game->player.position.x, game->player.position.y);
        fprintf(file, "=========================\n\n");
        fflush(file);
    }
}

void LogPlayerMovement(Game* game, Vector2 oldPos, Vector2 newPos) {
    (void)oldPos; // Suppress unused parameter warning
    if (game->logFile) {
        FILE* file = (FILE*)game->logFile;
        fprintf(file, "[%.2f] Player Position: (%.0f,%.0f) Bounds: (%.0f,%.0f,%.0f,%.0f)\n",
                game->gameTime, newPos.x, newPos.y,
                game->player.bounds.x, game->player.bounds.y, 
                game->player.bounds.width, game->player.bounds.height);
        fflush(file);
    }
}

void LogEvent(Game* game, const char* format, ...) {
    if (game->logFile) {
        FILE* file = (FILE*)game->logFile;
        va_list args;
        va_start(args, format);
        vfprintf(file, format, args);
        va_end(args);
        fprintf(file, "\n");
        fflush(file);
    }
}
