#ifndef CONTEXT_H
#define CONTEXT_H

#include "../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

#include "character.h"
#include "menu.h"
#include "enemy.h"
#include <math.h>

enum Game_stage
{
    START,
    RUNNING,
    PAUSED,
    WIN,
    LOSE,
};

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
  Menu menu;
  Enemy enemy;
  f64 dt;
  bool is_running;
  enum Game_stage stage;
} GameContext;

#endif
