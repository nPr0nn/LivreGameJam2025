#ifndef CONTEXT_H
#define CONTEXT_H

#include "../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

#include "character.h"
#include "collision_system.h"
#include "enemy.h"
#include "particle_system.h"
#include "shader_manager.h"
#include <math.h>

typedef struct GameContext {
  // Memmory Management
  slc_MemArena *g_arena; // per game allocation
  slc_MemArena *f_arena; // per frame allocation

  // Shader Manager
  ShaderManager shader_manager;

  // Collisions Block
  Rectangle *collision_rects;
  i32 collision_rects_count;

  // Particle System
  ParticleSystem *particle_system;

  // Map
  Font western_font;
  Texture2D background;

  // Game
  RenderTexture screen;
  Camera2D camera;
  Vector2 pos;
  Vector2 world_mouse_pos;
  Character player;
  Enemy enemy;
  f64 dt;
  bool is_paused;
} GameContext;

#endif
