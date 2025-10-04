#ifndef CONTEXT_H
#define CONTEXT_H

#include "../../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../../vendor/slc.h"

#define MAX_EDITOR_OPTIONS 10

#include <math.h>

typedef struct GameContext {
  // Memmory Management
  slc_MemArena *g_arena; // per game allocation
  slc_MemArena *f_arena; // per frame allocation

  // Game
  Camera2D camera;
  Vector2 pos;
  Vector2 world_mouse_pos;

  // Editor
  int panel_height;
  Rectangle options[MAX_EDITOR_OPTIONS];
  int selected_option; 

} GameContext;

#endif
