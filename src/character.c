#include "character.h"
#include "entity.h"
#include "particle_system.h"
#include "shader_manager.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

typedef enum {
  CHAR_STATE_IDLE_RUN = 0,
  CHAR_STATE_JUMP = 1,
} CharacterState;

#define JUMP_APEX_FRAME 1
#define DOUBLE_TAP_WINDOW 0.25f
#define RUN_SPEED_MULTIPLIER 1.8f
#define RUN_SPRITE_TILT 10.0f

void character_init(Character *ch, Vector2 start_pos, float radius,
                    Color color) {
  ch->en.owner = (void *)ch;
  ch->en.pos = start_pos;
  ch->en.vel = (Vector2){0};
  ch->en.acc = (Vector2){0};

  Image sprite_sheet_image = LoadImage("images/voaqueiro.png");
  ch->sprite_sheet = LoadTextureFromImage(sprite_sheet_image);
  UnloadImage(sprite_sheet_image);

  ch->total_run_animation_time = 8;
  ch->current_run_animation_time = ch->total_run_animation_time;
  ch->run_animation_decay_rate = 1;

  ch->total_jump_animation_time = 10;
  ch->current_jump_animation_time = ch->total_jump_animation_time;
  ch->jump_animation_decay_rate = 1;

  ch->num_frames = 3;
  ch->num_states = 2;
  ch->current_frame = 0;
  ch->current_state = CHAR_STATE_IDLE_RUN;
  ch->frame_height = (float)sprite_sheet_image.height / ch->num_states;
  ch->frame_width = (float)sprite_sheet_image.width / ch->num_frames;

  ch->is_look_right = true;

  f32 bbox_width = 12;
  f32 bbox_height = 16;
  ch->en.bbox =
      (Rectangle){ch->en.pos.x, ch->en.pos.y, bbox_width, bbox_height};

  ch->is_grounded = true;
  ch->coyote_timer = 0.0f;
  ch->jump_buffer_timer = 0.0f;

  ch->gravity = 400.0f;
  ch->friction = 8.0f;
  ch->jump_velocity = -180.0f;
  ch->jump_velocity_modifier = 1.0f;

  ch->is_running = false;
  ch->sprite_rotation = 0.0f;
  ch->last_left_press_time = 0.0;
  ch->last_right_press_time = 0.0;
}

void character_read_input(Character *ch, bool is_paused) {
  ch->en.acc = (Vector2){0};

  if (IsKeyPressed(KEY_LEFT)) {
    if (GetTime() - ch->last_left_press_time < DOUBLE_TAP_WINDOW) {
      ch->is_running = true;
    }
    ch->last_left_press_time = GetTime();
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    if (GetTime() - ch->last_right_press_time < DOUBLE_TAP_WINDOW) {
      ch->is_running = true;
    }
    ch->last_right_press_time = GetTime();
  }

  float move_acc = 800.0f;
  if (ch->is_running) {
    move_acc *= RUN_SPEED_MULTIPLIER;
  }

  if (IsKeyDown(KEY_LEFT)) {
    // MOMENTUM: If paused, directly add to velocity. Otherwise, use
    // acceleration.
    if (is_paused) {
      ch->en.vel.x -= move_acc * 0.02f;
    } else {
      ch->en.acc.x -= move_acc;
    }
  } else if (IsKeyDown(KEY_RIGHT)) {
    // MOMENTUM: If paused, directly add to velocity. Otherwise, use
    // acceleration.
    if (is_paused) {
      ch->en.vel.x += move_acc * 0.02f;
    } else {
      ch->en.acc.x += move_acc;
    }
  } else {
    ch->is_running = false;
  }

  if ((IsKeyDown(KEY_LEFT) && ch->en.vel.x > 0) ||
      (IsKeyDown(KEY_RIGHT) && ch->en.vel.x < 0)) {
    ch->is_running = false;
  }

  if (IsKeyPressed(KEY_SPACE)) {
    ch->jump_buffer_timer = 0.2f; // JUMP_BUFFER_SECONDS

    // MOMENTUM: Charge the jump if paused, otherwise reset the modifier.
    if (is_paused) {
      ch->jump_velocity_modifier += 0.25f;
      if (ch->jump_velocity_modifier > 4.0f) { // Cap the multiplier
        ch->jump_velocity_modifier = 4.0f;
      }
    } else {
      ch->jump_velocity_modifier = 1.0f;
    }
  }
}

void character_pre_update(Character *ch, ParticleSystem *particle_system,
                          float dt, bool is_paused) {
  if (!is_paused) {
    ch->coyote_timer -= dt;
    ch->jump_buffer_timer -= dt;
  }

  if (ch->is_grounded) {
    ch->coyote_timer = 0.1f; // COYOTE_TIME_SECONDS
  }

  if (!is_paused) {
    if (ch->jump_buffer_timer > 0.0f && ch->coyote_timer > 0.0f) {
      // MOMENTUM: Apply the charged jump modifier to the jump velocity.
      ch->en.vel.y = ch->jump_velocity * ch->jump_velocity_modifier;

      // MOMENTUM: Reset the modifier after the jump is used.
      ch->jump_velocity_modifier = 1.0f;

      ch->jump_buffer_timer = 0.0f;
      ch->coyote_timer = 0.0f;
      ch->is_grounded = false;
      ch->is_running = false;

      ch->current_state = CHAR_STATE_JUMP;
      ch->current_frame = 0;
      ch->current_jump_animation_time = ch->total_jump_animation_time;

      Vector2 feet_pos = {ch->en.pos.x, ch->en.pos.y + ch->en.bbox.height / 2};
      ParticleDefinition def = (ParticleDefinition){
          .pos = feet_pos,
          .vel = (Vector2){get_random_float(-40.0f, 40.0f),
                           get_random_float(-30.0f, -80.0f)},
          .color = (Color){240, 221, 205, 255},
          .radius = get_random_float(1.0f, 2.5f),
          .lifetime = get_random_float(0.4f, 0.8f),
      };
      particle_system_emit(particle_system, def, PARTICLE_MODE_FADE, 15);
    }

    if (IsKeyReleased(KEY_SPACE) && ch->en.vel.y < 0) {
      ch->en.vel.y *= 0.5f;
    }

    ch->en.acc.y += ch->gravity;
    ch->en.vel.x -= ch->en.vel.x * ch->friction * dt;

    ch->en.vel.x += ch->en.acc.x * dt;
    ch->en.vel.y += ch->en.acc.y * dt;
  }
}

void character_update(Character *ch, ParticleSystem *particle_system, float dt,
                      bool is_paused) {
  // MOMENTUM: Only apply physics-based position updates if the game is not
  // paused
  if (!is_paused) {
    ch->en.pos.x += ch->en.vel.x * dt;
    ch->en.pos.y += ch->en.vel.y * dt;
  }

  switch (ch->current_state) {
  case CHAR_STATE_IDLE_RUN: {
    if (fabsf(ch->en.vel.x) > 0.1f) {
      ch->current_run_animation_time -= ch->run_animation_decay_rate;
      if (ch->is_running) {
        ch->current_run_animation_time -= ch->run_animation_decay_rate;
      }

      if (ch->current_run_animation_time <= 0) {
        ch->current_frame = (ch->current_frame + 1) % ch->num_frames;
        ch->current_run_animation_time = ch->total_run_animation_time;
      }
    } else {
      ch->current_frame = 0;
    }

    if (ch->en.vel.x > 0.1f)
      ch->is_look_right = true;
    if (ch->en.vel.x < -0.1f)
      ch->is_look_right = false;

  } break;

  case CHAR_STATE_JUMP: {
    if (ch->current_frame < JUMP_APEX_FRAME) {
      ch->current_jump_animation_time -= ch->jump_animation_decay_rate;
      if (ch->current_jump_animation_time <= 0) {
        ch->current_frame++;
        ch->current_jump_animation_time = ch->total_jump_animation_time;
      }
    } else {
      ch->current_frame = JUMP_APEX_FRAME;
    }

    if (ch->is_grounded) {
      ch->current_state = CHAR_STATE_IDLE_RUN;
      ch->current_frame = 0;
    }
  } break;
  }

  if (ch->is_running && ch->is_grounded && fabs(ch->en.vel.x) > 0) {
    ch->sprite_rotation =
        ch->is_look_right ? -RUN_SPRITE_TILT : RUN_SPRITE_TILT;
    Vector2 feet_pos = {ch->en.pos.x, ch->en.pos.y + ch->en.bbox.height / 2};
    ParticleDefinition def = (ParticleDefinition){
        .pos = feet_pos,
        .vel = (Vector2){get_random_float(-40.0f, 40.0f),
                         get_random_float(-30.0f, -80.0f)},
        .color = (Color){240, 221, 205, 255},
        .radius = get_random_float(1.0f, 2.5f),
        .lifetime = get_random_float(0.4f, 0.8f),
    };
    particle_system_emit(particle_system, def, PARTICLE_MODE_FADE,
                         2); // Reduced from 50 to 2 for a less dense effect
  } else {
    ch->sprite_rotation = 0.0f;
  }

  ch->en.bbox.x = ch->en.pos.x;
  ch->en.bbox.y = ch->en.pos.y;

  if (ch->en.pos.y > 0) {
    ch->en.pos.y = 0;
    ch->en.vel.y = 0;
    ch->is_grounded = true;
  }
}

void character_draw(const Character *ch, ShaderManager *sm) {
  float flip = ch->is_look_right ? 1.0f : -1.0f;
  Rectangle source_rec = {(float)ch->current_frame * ch->frame_width,
                          (float)ch->current_state * ch->frame_height,
                          flip * ch->frame_width, ch->frame_height};

  Rectangle dest_rec = {ch->en.pos.x, ch->en.pos.y, ch->frame_width,
                        ch->frame_height};
  Vector2 origin = {ch->frame_width / 2.0f, ch->frame_height / 2.0f};

  BeginShaderMode(sm->glitch_shader);
  DrawTexturePro(ch->sprite_sheet, source_rec, dest_rec, origin,
                 ch->sprite_rotation, WHITE);
  EndShaderMode();
}

void character_on_collision(void *entity_owner,
                            const CollisionInfo *collision_info, float dt) {
  Character *player = (Character *)entity_owner;

  if (collision_info->contact_normal.y < 0) {
    player->is_grounded = true;
  }

  player->en.pos.x += player->en.vel.x * dt * (collision_info->t_hit - 0.001f);
  player->en.pos.y += player->en.vel.y * dt * (collision_info->t_hit - 0.001f);

  float dot = (player->en.vel.x * collision_info->contact_normal.x +
               player->en.vel.y * collision_info->contact_normal.y);
  player->en.vel.x -= dot * collision_info->contact_normal.x;
  player->en.vel.y -= dot * collision_info->contact_normal.y;
}
