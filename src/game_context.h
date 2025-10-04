#ifndef CONTEXT_H
#define CONTEXT_H

#include "../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

#include "character.h"
#include "enemy.h"
#include <math.h>

typedef struct GameContext {
  // Memmory Management
  slc_MemArena *g_arena; // per game allocation
  slc_MemArena *f_arena; // per frame allocation

  // Game
  RenderTexture screen;
  Camera2D camera;
  Vector2 pos;
  Vector2 world_mouse_pos;
  Character player;
  Enemy enemy;
  f64 dt;
  bool is_paused;

  // Map and Tiles
  
} GameContext;

#endif
