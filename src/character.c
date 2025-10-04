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
  ch->acc = (Vector2){0};

  float move_acc = 800.0f;       // px/s² —
  float jump_velocity = -180.0f; // px/s —

  if (IsKeyDown(KEY_LEFT))
    ch->acc.x -= move_acc;
  if (IsKeyDown(KEY_RIGHT))
    ch->acc.x += move_acc;

  if (IsKeyPressed(KEY_SPACE) && ch->pos.y == 0)
    ch->vel.y = jump_velocity;
}

void character_update(Character *ch, float dt) {
  float gravity = 400.0f; // px/s² — feels like normal gravity at this scale
  float friction = 8.0f;  // per second — enough to stop quickly on release
  float velocity_deadzone = 5.0f; // px/s — avoids tiny jitter

  // Apply gravity
  ch->acc.y += gravity;

  // Integrate velocity
  ch->vel.x += ch->acc.x * dt;
  ch->vel.y += ch->acc.y * dt;

  // Apply friction
  ch->vel.x -= ch->vel.x * friction * dt;

  // Integrate position
  ch->pos.x += ch->vel.x * dt;
  ch->pos.y += ch->vel.y * dt;

  // Ground collision
  if (ch->pos.y > 0) {
    ch->pos.y = 0;
    ch->vel.y = 0;
  }

  // Deadzone for jitter
  if (fabsf(ch->vel.x) < velocity_deadzone)
    ch->vel.x = 0;
}

void character_draw(const Character *ch) {
  DrawCircleV(ch->pos, ch->radius, ch->color);
}
