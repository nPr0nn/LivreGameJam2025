#ifndef ENEMY_H
#define ENEMY_H

#include "../vendor/raylib/raylib.h"

typedef struct Enemy {
    Vector2 position;
    Vector2 radius;
    float speed;
    int direction; // -1 for left, 1 for right
} Enemy;

void enemy_init(Enemy *enemy, Vector2 startPosition, float speed);
void enemy_update(Enemy *enemy, float dt);
void enemy_draw(const Enemy *enemy);

#endif // ENEMY_H