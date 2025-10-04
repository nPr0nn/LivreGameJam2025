#include "character.h"
#include "game.h"
#include "utils.h"
#include "character.h"
#include "../vendor/raylib/raylib.h"
#include <math.h>

void character_init(Character *ch, Vector2 start_pos, float radius,
                    Color color) {
  ch->pos = start_pos;
  ch->vel = (Vector2){0};
  ch->acc = (Vector2){0};
  ch->radius = radius;
  ch->idle_color = color;
  ch->render_color = color;
  ch->colision_color = GREEN;

  Image sprite_sheet_image = LoadImage("images/voaqueiro.png");
  ch->sprite_sheet = LoadTextureFromImage(sprite_sheet_image);
  UnloadImage(sprite_sheet_image);

  ch->num_frames = 3;
  ch->current_frame = 0;
  ch->frame_height = sprite_sheet_image.height / 1;
  ch->frame_width = sprite_sheet_image.width / ch->num_frames;
}

void character_read_input(Character *ch, bool is_paused) {
  ch->acc = (Vector2){0}; // reset each frame

  float move_acc = 800.0f;       // px/s² —
  float jump_velocity = -180.0f; // px/s —

  if (IsKeyDown(KEY_LEFT))
    ch->acc.x -= move_acc;
  if (IsKeyDown(KEY_RIGHT))
    ch->acc.x += move_acc;

  // Jump
  if (IsKeyPressed(KEY_SPACE) && ch->pos.y == 0 && !is_paused) {
    ch->vel.y += jump_velocity;
  } else if (IsKeyPressed(KEY_SPACE) && is_paused && ch->pos.y == 0) {
    ch->vel.y += jump_velocity / 4;
  }
}

void character_update(Character *ch, float dt, bool is_paused) {
  float gravity = 400.0f; // px/s² — feels like normal gravity at this scale
  float friction = 8.0f;  // per second — enough to stop quickly on release
  float velocity_deadzone = 5.0f; // px/s — avoids tiny jitter

  if (!is_paused) {
    // ✅ Gravity
    ch->acc.y += gravity;

    // ✅ Apply horizontal damping (friction)
    ch->vel.x -= ch->vel.x * friction * dt;

    // ✅ Integrate position
    ch->pos.x += ch->vel.x * dt;
    ch->pos.y += ch->vel.y * dt;

    // Deadzone for jitter
    if (fabsf(ch->vel.x) < velocity_deadzone)
      ch->vel.x = 0;
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
  Rectangle source_rec = {ch->current_frame, 0, ch->frame_width,
                          ch->frame_height};
  DrawTextureRec(ch->sprite_sheet, source_rec, ch->pos, WHITE);
}

void character_check_collision(Character *ch, Vector2 *enemy_position,
                               Vector2 *enemy_size) {
  Rectangle enemyRect = {enemy_position->x, enemy_position->y, enemy_size->x,
                         enemy_size->y}; // Enemy size is 10x10
  Rectangle playerRect = {ch->pos.x - ch->radius, ch->pos.y - ch->radius,
                          ch->radius * 2, ch->radius * 2};

  if (CheckCollisionRecs(enemyRect, playerRect)) {
    ch->render_color = ch->colision_color; // Change color to green on collision
  } else {
    ch->render_color =
        ch->idle_color; // Revert to original color if no collision
  }
}
