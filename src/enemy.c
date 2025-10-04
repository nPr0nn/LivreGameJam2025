#include "enemy.h"
#include "game.h"
#include "utils.h"
#include <math.h>

void enemy_init(Enemy *enemy, Vector2 startPosition, float speed) {
    enemy->position = startPosition;
    enemy->radius = (Vector2){10, 10}; // Enemy size is 10x10
    enemy->speed = speed;
    enemy->direction = 1; // Start moving to the right
}

void enemy_update(Enemy *enemy, float dt) {
    // Update position based on direction and speed
    enemy->position.x += enemy->direction * enemy->speed * dt;

    // Reverse direction if the enemy reaches the screen edges
    if (enemy->position.x <= 0 || enemy->position.x >= 100) {
        enemy->direction *= -1;
    }

    // Ensure the enemy stays on the ground level
    enemy->position.y = -5;
}

void enemy_draw(const Enemy *enemy) {
    DrawRectangleV(enemy->position, enemy->radius, RED); // Draw a red square for the enemy
}