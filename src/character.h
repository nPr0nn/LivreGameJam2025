#ifndef CHARACTER_H
#define CHARACTER_H

#include "../vendor/raylib/raylib.h"
#include "collision_system.h"
#include "entity.h"
#include "particle_system.h"
#include "shader_manager.h"

#define COYOTE_TIME_SECONDS 0.01f
#define JUMP_BUFFER_SECONDS 0.05f

typedef struct Character {
  // Entity
  Entity en;
  f32 ground_height;

  // Sprite and animation
  Texture2D sprite_sheet;

  int num_states;
  int num_frames;
  int current_frame;
  int current_state;

  int total_run_animation_time;
  int current_run_animation_time;
  int run_animation_decay_rate;

  int total_jump_animation_time;
  int current_jump_animation_time;
  int jump_animation_decay_rate;

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

  // JUICE: Running state
  bool is_running;
  float sprite_rotation;
  double last_left_press_time;
  double last_right_press_time;

} Character;

void character_init(Character *ch, Vector2 start_pos, Color color);
void character_pre_update(Character *ch, ParticleSystem *ps, float dt,
                          bool is_paused);
void character_update(Character *ch, ParticleSystem *ps, float dt,
                      bool is_paused);
void character_read_input(Character *ch, bool is_paused);
void character_draw(const Character *ch, ShaderManager *sm);

void character_on_collision(void *entity, const CollisionInfo *collision_info,
                            float dt);

#endif // CHARACTER_H
