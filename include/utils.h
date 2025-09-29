#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <stdio.h>

// Logging functions
void InitLogger(Game* game);
void CloseLogger(Game* game);
void LogCollision(Game* game, const char* type, int id, Rectangle obj1, Rectangle obj2);
void LogPlayerMovement(Game* game, Vector2 oldPos, Vector2 newPos);
void LogEvent(Game* game, const char* format, ...);

#endif // UTILS_H
