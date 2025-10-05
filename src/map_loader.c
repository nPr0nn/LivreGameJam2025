#include "game_context.h"
#include "map_loader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void load_map(GameContext *ctx, const char *file_path) {
  // Initialize map and collisions to empty
  for (int x = 0; x < MAP_W; x++) {
    for (int y = 0; y < MAP_H; y++) {
      ctx->map[x][y] = -1;
      ctx->collisions[x][y] = 0;
      ctx->collisions_id[x][y] = 0;
    }
  }

  FILE *f = fopen(file_path, "rb");
  if (!f) return; // file not found -> nothing to load

  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = (char *)malloc((size_t)sz + 1);
  if (!buf) { fclose(f); return; }
  fread(buf, 1, (size_t)sz, f);
  buf[sz] = '\0';
  fclose(f);

  // Track loaded tile textures by path (up to NUM_TILES)
  char seen_paths[NUM_TILES][256];
  int seen_count = 0;
  for (int i = 0; i < NUM_TILES; i++) seen_paths[i][0] = '\0';

  // --- Parse "tiles" array ---
  char *tiles_key = strstr(buf, "\"tiles\"");
  if (tiles_key) {
    char *arr = strchr(tiles_key, '[');
    if (arr) {
      char *p = arr + 1;
      while (1) {
        char *obj = strchr(p, '{');
        if (!obj) break;
        char *obj_end = strchr(obj, '}');
        if (!obj_end) break;

        // extract tile path string
        char tile_path[256] = {0};
        char *k = strstr(obj, "\"tile\"");
        if (k && k < obj_end) {
          char *q = strchr(k, ':');
          if (q && q < obj_end) {
            char *quote = strchr(q, '"');
            if (quote && quote < obj_end) {
              quote++;
              char *quote_end = strchr(quote, '"');
              if (quote_end && quote_end <= obj_end) {
                size_t len = (size_t)(quote_end - quote);
                if (len >= sizeof(tile_path)) len = sizeof(tile_path) - 1;
                memcpy(tile_path, quote, len);
                tile_path[len] = '\0';
              }
            }
          }
        }

        int x = 0, y = 0, w = 1, h = 1;
        char *xk = strstr(obj, "\"x\""); if (xk && xk < obj_end) sscanf(xk, "\"x\"%*[^0-9-]%d", &x);
        char *yk = strstr(obj, "\"y\""); if (yk && yk < obj_end) sscanf(yk, "\"y\"%*[^0-9-]%d", &y);
        char *wk = strstr(obj, "\"w\""); if (wk && wk < obj_end) sscanf(wk, "\"w\"%*[^0-9-]%d", &w);
        char *hk = strstr(obj, "\"h\""); if (hk && hk < obj_end) sscanf(hk, "\"h\"%*[^0-9-]%d", &h);

        if (tile_path[0] != '\0') {
          int idx = -1;
          for (int i = 0; i < seen_count; i++) {
            if (strcmp(seen_paths[i], tile_path) == 0) { idx = i; break; }
          }
          if (idx == -1 && seen_count < NUM_TILES) {
            idx = seen_count;
            strncpy(seen_paths[seen_count], tile_path, sizeof(seen_paths[0]) - 1);
            seen_paths[seen_count][sizeof(seen_paths[0]) - 1] = '\0';
            // attempt to load texture; if fails, texture will be empty but index reserved
            ctx->tiles[idx] = LoadTexture(seen_paths[seen_count]);
            // store basename into ctx->tile_names if available
            const char *p = strrchr(seen_paths[seen_count], '/');
            if (p) p++; else p = seen_paths[seen_count];
            strncpy(ctx->tile_names[idx], p, sizeof(ctx->tile_names[0]) - 1);
            ctx->tile_names[idx][sizeof(ctx->tile_names[0]) - 1] = '\0';
            // if this tile is the player spawn marker, record spawn and reserve the slot
            if (strcmp(ctx->tile_names[idx], "voaqueiro.png") == 0) {
              // store spawn at the first occurrence (tile's x,y converted to world px)
              ctx->player_spawn = (Vector2){ (float)(x * TILE_SIZE), (float)(y * TILE_SIZE) };
              ctx->player_spawn_set = true;
              // do not paint this tile into the map; leave the cell empty
            }
            seen_count++;
          }

          if (idx != -1) {
            // If this tile is the voaqueiro marker, avoid painting it to the map so it won't be drawn.
            bool is_spawn_marker = (strcmp(ctx->tile_names[idx], "voaqueiro.png") == 0);
            if (!is_spawn_marker) {
              for (int ix = x; ix < x + w; ix++) {
                for (int iy = y; iy < y + h; iy++) {
                  if (ix >= 0 && iy >= 0 && ix < MAP_W && iy < MAP_H) ctx->map[ix][iy] = idx;
                }
              }
            }
          }
        }

        p = obj_end + 1;
        // stop when we reach end of tiles array
        char *next_arr_end = strchr(p, ']');
        if (!next_arr_end) continue;
        // if next ']' comes before next '{', break
        char *next_obj = strchr(p, '{');
        if (!next_obj || next_arr_end < next_obj) break;
      }
    }
  }

  // --- Parse "collisions" array ---
  char *coll_key = strstr(buf, "\"collisions\"");
  if (coll_key) {
    char *arr = strchr(coll_key, '[');
    if (arr) {
      char *p = arr + 1;
      while (1) {
        char *obj = strchr(p, '{');
        if (!obj) break;
        char *obj_end = strchr(obj, '}');
        if (!obj_end) break;

        char type_buf[32] = {0};
        char *tk = strstr(obj, "\"type\"");
        if (tk && tk < obj_end) {
          char *q = strchr(tk, ':');
          if (q && q < obj_end) {
            char *quote = strchr(q, '"');
            if (quote && quote < obj_end) {
              quote++;
              char *quote_end = strchr(quote, '"');
              if (quote_end && quote_end <= obj_end) {
                size_t len = (size_t)(quote_end - quote);
                if (len >= sizeof(type_buf)) len = sizeof(type_buf) - 1;
                memcpy(type_buf, quote, len);
                type_buf[len] = '\0';
              }
            }
          }
        }

        int id = 0;
        char *idk = strstr(obj, "\"id\""); if (idk && idk < obj_end) sscanf(idk, "\"id\"%*[^0-9-]%d", &id);
        int x = 0, y = 0, w = 1, h = 1;
        char *xk2 = strstr(obj, "\"x\""); if (xk2 && xk2 < obj_end) sscanf(xk2, "\"x\"%*[^0-9-]%d", &x);
        char *yk2 = strstr(obj, "\"y\""); if (yk2 && yk2 < obj_end) sscanf(yk2, "\"y\"%*[^0-9-]%d", &y);
        char *wk2 = strstr(obj, "\"w\""); if (wk2 && wk2 < obj_end) sscanf(wk2, "\"w\"%*[^0-9-]%d", &w);
        char *hk2 = strstr(obj, "\"h\""); if (hk2 && hk2 < obj_end) sscanf(hk2, "\"h\"%*[^0-9-]%d", &h);

        int type = 0;
        if (strcmp(type_buf, "solid") == 0) type = 1;
        else if (strcmp(type_buf, "death") == 0) type = 2;
        else if (strcmp(type_buf, "trigger") == 0) type = 3;

        for (int ix = x; ix < x + w; ix++) {
          for (int iy = y; iy < y + h; iy++) {
            if (ix >= 0 && iy >= 0 && ix < MAP_W && iy < MAP_H) {
              ctx->collisions[ix][iy] = type;
              ctx->collisions_id[ix][iy] = id;
            }
          }
        }

        p = obj_end + 1;
        char *next_arr_end = strchr(p, ']');
        if (!next_arr_end) continue;
        char *next_obj = strchr(p, '{');
        if (!next_obj || next_arr_end < next_obj) break;
      }
    }
  }

  free(buf);
}