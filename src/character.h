#ifndef CHARACTER_H
#define CHARACTER_H

#include "../vendor/raylib/raylib.h"

typedef struct {
  Vector2 pos;
  Vector2 vel;
  Vector2 acc;
  float radius;
  Color color;
} Character;

void character_init(Character *ch, Vector2 start_pos, float radius,
                    Color color);
void character_update(Character *ch, float dt, bool is_paused);
void character_read_input(Character *ch, bool is_paused);

void character_draw(const Character *ch);

#endif // CHARACTER_H
