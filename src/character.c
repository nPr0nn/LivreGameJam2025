#include "game.h"
#include "utils.h"
#include "character.h"

void character_init(Character *ch, Vector2 start_pos, float radius, Color color) {
    ch->pos = start_pos;
    ch->radius = radius;
    ch->color = color;
    ch->velocity_y = 0.0f;
}

void character_update(Character *ch) {
    // Horizontal movement
    if (IsKeyDown(KEY_LEFT))  ch->pos.x -= 5;
    if (IsKeyDown(KEY_RIGHT)) ch->pos.x += 5;

    // Gravity
    float gravity = 0.5f;
    ch->velocity_y -= gravity;
    ch->pos.y -= ch->velocity_y;

    // Ground collision (y=0 is ground)
    if (ch->pos.y > 0) {
        ch->pos.y = 0;
        ch->velocity_y = 0;
    }

    // Jump (optional)
    if (IsKeyPressed(KEY_SPACE) && ch->pos.y == 0) {
        ch->velocity_y = 10.0f;
    }
}

void character_draw(const Character *ch) {
    DrawCircleV(ch->pos, ch->radius, ch->color);
}