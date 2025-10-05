#ifndef CONTEXT_H
#define CONTEXT_H

#include "../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

#define NUM_TILES 10
#define TILE_SIZE 16

#define MAP_W 100
#define MAP_H 100

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
  int collisions[MAP_W][MAP_H]; // 0 = none, 1 = solid, 2 = death, 3 = trigger
  int collisions_id[MAP_W][MAP_H]; // integer id for triggers (only meaningful when collisions==3)
  int map[MAP_W][MAP_H]; // -1 if empty, otherwise index of tile
  
  Texture2D tiles[NUM_TILES]; // tile textures
  slc_String *paths; // list of tile image paths
  char tile_names[NUM_TILES][256]; // basename of the tile path (e.g. voaqueiro.png)
  Vector2 player_spawn; // world position where player should spawn
  bool player_spawn_set;

} GameContext;

#endif
