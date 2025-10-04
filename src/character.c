#include "game.h"
#include "utils.h"
#include "character.h"

void character_init(Character *ch, Vector2 start_pos, float radius, Color color) {
    ch->pos = start_pos;
    ch->radius = radius;
    ch->color = color;
}

void character_update(Character *ch) {
    if (IsKeyDown(KEY_LEFT))  ch->pos.x -= 5;
    if (IsKeyDown(KEY_RIGHT)) ch->pos.x += 5;
    if (IsKeyDown(KEY_UP))    ch->pos.y -= 5;
    if (IsKeyDown(KEY_DOWN))  ch->pos.y += 5;
}

void character_draw(const Character *ch) {
    DrawCircleV(ch->pos, ch->radius, ch->color);
}