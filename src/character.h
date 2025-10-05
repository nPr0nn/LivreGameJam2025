#ifndef CHARACTER_H
#define CHARACTER_H

#include "../vendor/raylib/raylib.h"

#define COYOTE_TIME_SECONDS 0.01f
#define JUMP_BUFFER_SECONDS 0.05f

typedef struct Character {
  // Entity
  Vector2 pos;
  Vector2 vel;
  Vector2 acc;
  Rectangle bbox;

  // Sprite and animation
  Texture2D sprite_sheet;
  int animate_time;
  int current_animate_time;
  int animate_decay_rate;
  int num_frames;
  int current_frame;
  float frame_height;
  float frame_width;
  bool is_look_right;

  // Physics
  float gravity;
  float friction;
  float jump_velocity;
  float jump_velocity_modifier;

  // Polished movement state
  bool is_grounded;
  float coyote_timer;
  float jump_buffer_timer;

} Character;

void character_init(Character *ch, Vector2 start_pos, float radius,
                    Color color);
void character_pre_update(Character *ch, float dt, bool is_paused);
void character_update(Character *ch, float dt, bool is_paused);
void character_read_input(Character *ch, bool is_paused);
void character_draw(const Character *ch);

void character_on_collision_detection();
void character_on_collision_resolution();

#endif // CHARACTER_H
