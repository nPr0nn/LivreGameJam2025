#ifndef CONTEXT_H
#define CONTEXT_H

#include "../../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../../vendor/slc.h"

#define NUM_TILES 128
#define TILE_SIZE 32

#define MAP_W 100
#define MAP_H 100

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
  int selected_tile;
  Vector2 click_points[3];
  int click_count;
  int edit_mode; // 0 = single-tile place, 1 = rectangle place (two clicks)
  int pending_action; // 0 = none, 1 = place, -1 = erase (for rectangle mode)
  int save_flash_counter; // frames to show "saved" message
  slc_String *paths; // list of tile image paths
  int paths_count; // number of entries in paths
  int tile_page; // current page (0..9)
  int tiles_per_page; // usually 10 (digits 0..9)
  // Collision layer: support types and IDs
  // collision_type: 0 = none, 1 = solid, 2 = death, 3 = trigger
  int collision_type[MAP_W][MAP_H];
  // collision_id: integer id for triggers (only meaningful when collision_type==3)
  int collision_id[MAP_W][MAP_H];
  int active_layer; // 0 = tiles, 1 = collision
  int collision_visible; // 0 = hidden, 1 = visible
  int current_collision_type; // 1=solid,2=death,3=trigger (used when editing collision layer)

  // Map
  int map[MAP_W][MAP_H]; // -1 if empty, otherwise index of tile

  Texture2D tiles[NUM_TILES];
} GameContext;

#endif
