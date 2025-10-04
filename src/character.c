#include "character.h"
#include "game.h"
#include "utils.h"
#include <math.h>

void character_init(Character *ch, Vector2 start_pos, float radius,
                    Color color) {
  ch->pos = start_pos;
  ch->vel = (Vector2){0};
  ch->acc = (Vector2){0};
  ch->radius = radius;
  ch->color = color;
}

void character_read_input(Character *ch) {
  ch->acc = (Vector2){0}; // reset each frame

  float move_acc = 600.0f; // px/s² — much stronger now

  if (IsKeyDown(KEY_LEFT))
    ch->acc.x -= move_acc;
  if (IsKeyDown(KEY_RIGHT))
    ch->acc.x += move_acc;

  // Jump
  if (IsKeyPressed(KEY_SPACE) && ch->pos.y == 0) {
    ch->vel.y = -300.0f;
  }
}

void character_update(Character *ch, float dt) {
  // ✅ Gravity
  float gravity = 800.0f; // px/s²
  ch->acc.y += gravity;

  // ✅ Integrate velocity
  ch->vel.x += ch->acc.x * dt;
  ch->vel.y += ch->acc.y * dt;

  // ✅ Apply horizontal damping (friction)
  float friction = 6.0f; // per second — not per frame!
  ch->vel.x -= ch->vel.x * friction * dt;

  // ✅ Integrate position
  ch->pos.x += ch->vel.x * dt;
  ch->pos.y += ch->vel.y * dt;

  // ✅ Ground collision
  if (ch->pos.y > 0) {
    ch->pos.y = 0;
    ch->vel.y = 0;
  }

  // ✅ Dead zone to prevent jitter
  if (fabsf(ch->vel.x) < 5.0f)
    ch->vel.x = 0;
}

void character_draw(const Character *ch) {
  DrawCircleV(ch->pos, ch->radius, ch->color);
}
