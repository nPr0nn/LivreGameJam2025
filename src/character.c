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

void character_read_input(Character *ch, bool is_paused) {
  ch->acc = (Vector2){0}; // reset each frame

  float move_acc = 600.0f; // px/s² — much stronger now

  if (IsKeyDown(KEY_LEFT))
    ch->acc.x -= move_acc;
  if (IsKeyDown(KEY_RIGHT))
    ch->acc.x += move_acc;

  // Jump
  if (IsKeyPressed(KEY_SPACE) && ch->pos.y == 0 && !is_paused) {
    ch->vel.y += -300.0f;
  } else if (IsKeyPressed(KEY_SPACE) && is_paused && ch->pos.y == 0) {
    ch->vel.y += -200.0f;
}

}

void character_update(Character *ch, float dt, bool is_paused) {
    float gravity = 800.0f; // px/s²
    float friction = 6.0f; // per second — not per frame!

    if(!is_paused) {
        // ✅ Gravity
        ch->acc.y += gravity;
        
        // ✅ Apply horizontal damping (friction)
        ch->vel.x -= ch->vel.x * friction * dt;
        
        // ✅ Integrate position
        ch->pos.x += ch->vel.x * dt;
        ch->pos.y += ch->vel.y * dt;
    }
    
    // ✅ Integrate velocity
    ch->vel.x += ch->acc.x * dt;
    ch->vel.y += ch->acc.y * dt;
        
    // ✅ Ground collision
    if (ch->pos.y > 0) {
        ch->pos.y = 0;
        ch->vel.y = 0;
    }
        
}

void character_draw(const Character *ch) {
  DrawCircleV(ch->pos, ch->radius, ch->color);
}
