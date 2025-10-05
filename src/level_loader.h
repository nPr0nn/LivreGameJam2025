#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include "../vendor/json.h"
#include "../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

typedef struct t_Tile {
  const char *tile;
  Texture2D sprite;
  i32 x, y, w, h;
} t_Tile;

typedef struct t_Collision {
  const char *type;
  i32 id, x, y, w, h;
} t_Collision;

typedef struct LevelData {
  i32 map_w;
  i32 map_h;
  t_Tile *tiles;
  usize tile_count;
  t_Collision *collisions;
  usize collision_count;
} LevelData;

static inline void level_init(LevelData *level_data) {
  for (int i = 0; i < level_data->tile_count; i++) {
    Image tile_image = LoadImage(level_data->tiles[i].tile);
    level_data->tiles[i].sprite = LoadTextureFromImage(tile_image);
    UnloadImage(tile_image);
  }
}

static inline void level_draw(LevelData *level_data) {
  for (int i = 0; i < level_data->tile_count; i++) {

    i32 start_x = level_data->tiles[i].x;
    i32 start_y = level_data->tiles[i].y;

    i32 end_x = start_x + level_data->tiles[i].w;
    i32 end_y = start_y + level_data->tiles[i].h;

    for (int y = start_y; y < end_y; y++) {
      for (int x = start_x; x < end_x; x++) {
        DrawTexture(level_data->tiles[i].sprite, x * 16, y * 16, WHITE);
      }
    }
  }
}

static inline Vector2 level_get_player_position(LevelData *level_data) {
  for (int i = 0; i < level_data->tile_count; i++) {
    t_Tile tile = level_data->tiles[i];
    if (strcmp(tile.tile, "images/voaqueiro.png") == 0) {
      return (Vector2){(f32)tile.x * 16, (f32)tile.y * 16};
    }
  }

  printf("DEU MERDA AQUI \n");
  return (Vector2){-9999, -9999};
}

static inline LevelData *load_level_data(const char *json_path,
                                         slc_MemArena *arena_ptr) {
  FILE *f = fopen(json_path, "rb");
  if (!f) {
    fprintf(stderr, "Failed to open %s\n", json_path);
    return NULL;
  }

  // --- Read entire file into arena ---
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *buffer = (char *)slc_mem_arena_alloc(arena_ptr, size + 1);
  if (!buffer) {
    fprintf(stderr, "Arena alloc failed for buffer\n");
    fclose(f);
    return NULL;
  }

  fread(buffer, 1, size, f);
  buffer[size] = '\0';
  fclose(f);

  struct json_value_s *root = json_parse(buffer, size);
  if (!root) {
    fprintf(stderr, "Failed to parse JSON\n");
    return NULL;
  }

  struct json_object_s *obj = json_value_as_object(root);
  if (!obj) {
    fprintf(stderr, "Root is not a JSON object\n");
    return NULL;
  }

  // --- Allocate LevelData ---
  LevelData *level =
      (LevelData *)slc_mem_arena_calloc(arena_ptr, sizeof(LevelData));

  for (struct json_object_element_s *el = obj->start; el; el = el->next) {
    const char *key = el->name->string;

    // map_w / map_h
    if (strcmp(key, "map_w") == 0)
      level->map_w =
          (i32)((struct json_number_s *)el->value->payload)->number_size;
    else if (strcmp(key, "map_h") == 0)
      level->map_h =
          (i32)((struct json_number_s *)el->value->payload)->number_size;

    // tiles array
    else if (strcmp(key, "tiles") == 0) {
      struct json_array_s *arr = json_value_as_array(el->value);
      level->tile_count = arr->length;
      level->tiles = (t_Tile *)slc_mem_arena_calloc(
          arena_ptr, sizeof(t_Tile) * level->tile_count);

      struct json_array_element_s *elem = arr->start;
      for (usize i = 0; i < level->tile_count && elem; i++, elem = elem->next) {
        struct json_object_s *tile_obj = json_value_as_object(elem->value);
        for (struct json_object_element_s *prop = tile_obj->start; prop;
             prop = prop->next) {
          const char *pkey = prop->name->string;
          if (strcmp(pkey, "tile") == 0)
            level->tiles[i].tile =
                ((struct json_string_s *)prop->value->payload)->string;
          else if (strcmp(pkey, "x") == 0)
            level->tiles[i].x =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "y") == 0)
            level->tiles[i].y =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "w") == 0)
            level->tiles[i].w =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "h") == 0)
            level->tiles[i].h =
                atoi(((struct json_number_s *)prop->value->payload)->number);
        }
      }
    }

    // collisions array
    else if (strcmp(key, "collisions") == 0) {
      struct json_array_s *arr = json_value_as_array(el->value);
      level->collision_count = arr->length;
      level->collisions = (t_Collision *)slc_mem_arena_calloc(
          arena_ptr, sizeof(t_Collision) * level->collision_count);

      struct json_array_element_s *elem = arr->start;
      for (usize i = 0; i < level->collision_count && elem;
           i++, elem = elem->next) {
        struct json_object_s *col_obj = json_value_as_object(elem->value);
        for (struct json_object_element_s *prop = col_obj->start; prop;
             prop = prop->next) {
          const char *pkey = prop->name->string;
          if (strcmp(pkey, "type") == 0)
            level->collisions[i].type =
                ((struct json_string_s *)prop->value->payload)->string;
          else if (strcmp(pkey, "id") == 0)
            level->collisions[i].id =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "x") == 0)
            level->collisions[i].x =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "y") == 0)
            level->collisions[i].y =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "w") == 0)
            level->collisions[i].w =
                atoi(((struct json_number_s *)prop->value->payload)->number);
          else if (strcmp(pkey, "h") == 0)
            level->collisions[i].h =
                atoi(((struct json_number_s *)prop->value->payload)->number);
        }
      }
    }
  }

  return level;
}

#endif // LEVEL_LOADER_H
