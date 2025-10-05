#ifndef SHADER_MAN
#define SHADER_MAN

#include "../vendor/raylib/raylib.h"

#define RIPPLE_LIFETIME 1.5f

typedef struct ShaderManager {
  Shader glitch_shader;
  int glitch_shader_time_loc;

  Shader spotlight_shader;
  int spotlight_shader_radius_loc;
  int spotlight_shader_falloff_loc;
  int spotlight_shader_center_loc;
  float spotlight_radius;
  float spotlight_falloff;
  Vector2 spotlight_center;

  Shader ripple_shader;
  int ripple_shader_center_loc;
  int ripple_shader_time_loc;
  bool is_ripple_active;
  float ripple_start_time;
  Vector2 ripple_origin_pos;

} ShaderManager;

static inline void shader_manager_init(ShaderManager *sm) {

#ifdef PLATFORM_WEB
  sm->glitch_shader = LoadShader("", "images/shaders/glitch_web.fs");
  sm->spotlight_shader = LoadShader("", "images/shaders/spotlight_web.fs");
  sm->ripple_shader = LoadShader("", "images/shaders/ripple_web.fs");
#else
  sm->spotlight_shader = LoadShader("", "images/shaders/spotlight.fs");
  sm->glitch_shader = LoadShader("", "images/shaders/glitch.fs");
  sm->ripple_shader = LoadShader("", "images/shaders/ripple.fs");
#endif

  sm->glitch_shader_time_loc = GetShaderLocation(sm->glitch_shader, "time");
  sm->spotlight_shader_center_loc =
      GetShaderLocation(sm->spotlight_shader, "lightCenter");
  sm->spotlight_shader_radius_loc =
      GetShaderLocation(sm->spotlight_shader, "lightRadius");
  sm->spotlight_shader_falloff_loc =
      GetShaderLocation(sm->spotlight_shader, "lightFalloff");

  // Get the memory locations (addresses) of the shader's uniform variables
  sm->ripple_shader_center_loc =
      GetShaderLocation(sm->ripple_shader, "rippleCenter");
  sm->ripple_shader_time_loc = GetShaderLocation(sm->ripple_shader, "time");

  // Initialize the ripple state to be off by default
  sm->is_ripple_active = false;
  sm->ripple_start_time = 0.0f;
}

static inline void shader_manager_update(ShaderManager *sm) {
  // Glitch Shader
  float time = GetTime();
  SetShaderValue(sm->glitch_shader, sm->glitch_shader_time_loc, &time,
                 SHADER_UNIFORM_FLOAT);

  // Spotlight Shader
  sm->spotlight_radius = 0.05f;
  sm->spotlight_falloff = 0.05f;
  SetShaderValue(sm->spotlight_shader, sm->spotlight_shader_center_loc,
                 &sm->spotlight_center, SHADER_UNIFORM_VEC2);
  SetShaderValue(sm->spotlight_shader, sm->spotlight_shader_radius_loc,
                 &sm->spotlight_radius, SHADER_UNIFORM_FLOAT);
  SetShaderValue(sm->spotlight_shader, sm->spotlight_shader_falloff_loc,
                 &sm->spotlight_falloff, SHADER_UNIFORM_FLOAT);

  // Ripple Shader
  if (sm->is_ripple_active) {
    float elapsed_time = GetTime() - sm->ripple_start_time;

    SetShaderValue(sm->ripple_shader, sm->ripple_shader_time_loc, &elapsed_time,
                   SHADER_UNIFORM_FLOAT);
    SetShaderValue(sm->ripple_shader, sm->ripple_shader_center_loc,
                   &sm->ripple_origin_pos, SHADER_UNIFORM_VEC2);

    if (elapsed_time >= RIPPLE_LIFETIME) {
      sm->is_ripple_active = false;
    }
  }
}

static inline void trigger_ripple(ShaderManager *sm, Vector2 trigger_pos) {
  // Only start a new ripple if one isn't already active
  if (!sm->is_ripple_active) {
    sm->is_ripple_active = true;
    sm->ripple_start_time = GetTime();
    sm->ripple_origin_pos = trigger_pos;
  }
}

static inline void shader_manager_unload(ShaderManager *sm) {
  UnloadShader(sm->ripple_shader);
  UnloadShader(sm->spotlight_shader);
  UnloadShader(sm->glitch_shader);
}

#endif
