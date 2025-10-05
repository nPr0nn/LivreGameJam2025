#include "character.h"
#include "utils.h" // Assuming f32, etc. are defined here
#include <math.h>
#include <stdio.h>

void character_init(Character *ch, Vector2 start_pos, float radius,
                    Color color) {
  ch->pos = start_pos;
  ch->vel = (Vector2){0};
  ch->acc = (Vector2){0};

  Image sprite_sheet_image = LoadImage("images/voaqueiro.png");
  ch->sprite_sheet = LoadTextureFromImage(sprite_sheet_image);
  UnloadImage(sprite_sheet_image);

  ch->animate_time = 10;
  ch->animate_decay_rate = 1;
  ch->current_animate_time = ch->animate_time;

  ch->num_frames = 3;
  ch->current_frame = 0;
  ch->frame_height = (float)sprite_sheet_image.height;
  ch->frame_width = (float)sprite_sheet_image.width / ch->num_frames;

  ch->is_look_right = true;

  f32 bbox_width = 16;
  f32 bbox_height = 16;
  ch->bbox = (Rectangle){ch->pos.x, ch->pos.y, bbox_width, bbox_height};

  // Initialize new state variables
  ch->is_grounded = true;
  ch->coyote_timer = 0.0f;
  ch->jump_buffer_timer = 0.0f;

  // Physics constants
  ch->gravity = 400.0f;
  ch->friction = 8.0f;
  ch->jump_velocity = -180.0f;
  ch->jump_velocity_modifier = 1.0f;
}

void character_read_input(Character *ch, bool is_paused) {
  // Reset acceleration each frame
  ch->acc = (Vector2){0};

  // Horizontal movement: accumulate momentum if paused
  float move_acc = 800.0f;
  if (IsKeyDown(KEY_LEFT)) {
    if (is_paused) {
      ch->vel.x -= move_acc * 0.02f; // accumulate momentum slowly while paused
    } else {
      ch->acc.x -= move_acc;
    }
  }
  if (IsKeyDown(KEY_RIGHT)) {
    if (is_paused) {
      ch->vel.x += move_acc * 0.02f;
    } else {
      ch->acc.x += move_acc;
    }
  }

  // Jump buffering — even while paused
  if (IsKeyPressed(KEY_SPACE)) {
    ch->jump_buffer_timer = JUMP_BUFFER_SECONDS;
    if (is_paused) {
      // Build up jump power slowly while paused
      ch->jump_velocity_modifier += 0.25f;
      if (ch->jump_velocity_modifier > 5.0f) // cap multiplier
        ch->jump_velocity_modifier = 5.0f;
    } else {
      ch->jump_velocity_modifier = 1.0f;
    }
  }
}

void character_pre_update(Character *ch, float dt, bool is_paused) {
  // ONLY tick down timers when the game is not paused.
  if (!is_paused) {
    ch->coyote_timer -= dt;
    ch->jump_buffer_timer -= dt;
  }

  // Reset coyote time if grounded (This is your original, working order)
  if (ch->is_grounded) {
    ch->coyote_timer = COYOTE_TIME_SECONDS;
  }

  // Only do physics integration when not paused
  if (!is_paused) {
    // Jump logic
    if (ch->jump_buffer_timer > 0.0f && ch->coyote_timer > 0.0f) {
      ch->vel.y = ch->jump_velocity * ch->jump_velocity_modifier;
      ch->jump_buffer_timer = 0.0f;
      ch->coyote_timer = 0.0f;
      ch->is_grounded = false;
      ch->jump_velocity_modifier = 1.0f; // reset after jump
    }

    // Variable jump height
    if (IsKeyReleased(KEY_SPACE) && ch->vel.y < 0) {
      ch->vel.y *= 0.5f;
    }

    // Gravity + friction
    ch->acc.y += ch->gravity;
    ch->vel.x -= ch->vel.x * ch->friction * dt;

    // Integrate velocity
    ch->vel.x += ch->acc.x * dt;
    ch->vel.y += ch->acc.y * dt;
  }
}

// This function should be called AFTER the main collision/position update loop
void character_update(Character *ch, float dt, bool is_paused) {

  ch->pos.x += ch->vel.x * dt;
  ch->pos.y += ch->vel.y * dt;

  // Update sprite animation based on velocity
  if (fabsf(ch->vel.x) > 0.1f) { // Use a small threshold
    ch->current_animate_time -= ch->animate_decay_rate;
    if (ch->current_animate_time <= 0) {
      ch->current_frame = (ch->current_frame + 1) % ch->num_frames;
      ch->current_animate_time = ch->animate_time;
    }

    if (ch->vel.x > 0)
      ch->is_look_right = true;
    if (ch->vel.x < 0)
      ch->is_look_right = false;

  } else {
    ch->current_frame = 0; // Idle frame
    ch->current_animate_time = ch->animate_time;
  }

  // Update Bounding Box position to follow the character
  ch->bbox.x = ch->pos.x;
  ch->bbox.y = ch->pos.y;

  if (ch->pos.y > 0) {
    ch->pos.y = 0;
    ch->vel.y = 0;
    ch->is_grounded = true;
  }
}

void character_draw(const Character *ch) {
  float flip = ch->is_look_right ? 1.0f : -1.0f;

  // Draw Player Sprite
  Rectangle source_rec = {ch->current_frame * ch->frame_width, 0,
                          flip * ch->frame_width, ch->frame_height};

  Vector2 draw_pos = {ch->pos.x - (ch->frame_width / 2.0f),
                      ch->pos.y - (ch->frame_height / 2.0f)};

  DrawTextureRec(ch->sprite_sheet, source_rec, draw_pos, WHITE);

  // Draw debug bounding box
  DrawRectangleLines(ch->bbox.x - (ch->bbox.width / 2.0f),
                     ch->bbox.y - (ch->bbox.height / 2.0f), ch->bbox.width,
                     ch->bbox.height, RED);
}
