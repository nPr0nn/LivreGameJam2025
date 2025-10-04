
#include "editor.h"
#include "game_context.h"
#include "utils.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#endif

#define MAX_EDITOR_OPTIONS 10

void game_init(void *ctx) {
  GameContext *g = (GameContext *)ctx;

#ifndef PLATFORM_WEB
  i32 current_monitor_id = GetCurrentMonitor();
  i32 monitor_width = GetMonitorWidth(current_monitor_id);
  i32 monitor_height = GetMonitorHeight(current_monitor_id);
  f32 scale = 0.5;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow((i32)(scale * monitor_width), (i32)(scale * monitor_height),
             "Livre GameJam");
#endif

  // Editor defaults
  g->selected_tile = 1;
  g->click_count = 0;
  g->edit_mode = 0; // start in single-tile mode
  g->pending_action = 0;
  g->save_flash_counter = 0;

  // Load tiles
  for (int i = 0; i < NUM_TILES; i++) {
    char path[64];
    snprintf(path, sizeof(path), "assets/tiles/tile_%d.png", i);
    g->tiles[i] = LoadTexture(path);
  }
  
  // Initialize map to empty
  for (int x = 0; x < MAP_W; x++) {
    for (int y = 0; y < MAP_H; y++) {
      g->map[x][y] = -1; // -1 indicates empty tile
      g->collision_type[x][y] = 0; // none
      g->collision_id[x][y] = 0;
    }
  }
  g->active_layer = 0; // start editing tiles layer
  g->collision_visible = 0;
  g->current_collision_type = 1; // default: solid
  
  g->camera = (Camera2D){{0, 0}, {0, 0}, 0.0, 1.0};
  g->pos = (Vector2){0, 0};
}

void game_draw(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  BeginDrawing();
  ClearBackground(BLACK);

  BeginMode2D(g->camera);
  DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

  // Draw grid using tile size so tiles align with grid
  DrawInfiniteGrid(g->camera, TILE_SIZE, LIGHTGRAY);

  // Snap mouse world position to TILE_SIZE grid and compute center of that tile
  Vector2 world_mouse = g->world_mouse_pos;
  int tile_x = (int)floor(world_mouse.x / TILE_SIZE);
  int tile_y = (int)floor(world_mouse.y / TILE_SIZE);
  float center_x = (float)tile_x * TILE_SIZE + (TILE_SIZE / 2.0f);
  float center_y = (float)tile_y * TILE_SIZE + (TILE_SIZE / 2.0f);

  // Draw tile selection at the center of the snapped tile and scale it to TILE_SIZE

  // Draw existing map tiles
  for (int x = 0; x < MAP_W; x++) {
    for (int y = 0; y < MAP_H; y++) {
      int t = g->map[x][y];
      if (t >= 0 && t < NUM_TILES) {
        Texture2D tx = g->tiles[t];
        Rectangle s = (Rectangle){0.0f, 0.0f, (float)tx.width, (float)tx.height};
        Rectangle d = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE};
        DrawTexturePro(tx, s, d, (Vector2){0, 0}, 0.0f, WHITE);
      }
    }
  }

  // Draw collision overlay if visible
  if (g->collision_visible) {
    for (int x = 0; x < MAP_W; x++) {
      for (int y = 0; y < MAP_H; y++) {
        int ct = g->collision_type[x][y];
        if (ct != 0) {
          Rectangle r = (Rectangle){x * TILE_SIZE, y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE};
          if (ct == 1) { // solid
            DrawRectangleRec(r, (Color){255, 0, 0, 80});
            DrawRectangleLinesEx(r, 1, RED);
          } else if (ct == 2) { // death
            DrawRectangleRec(r, (Color){128, 0, 128, 80});
            DrawRectangleLinesEx(r, 1, PURPLE);
          } else if (ct == 3) { // trigger
            DrawRectangleRec(r, (Color){0, 128, 255, 80});
            DrawRectangleLinesEx(r, 1, BLUE);
            // draw trigger id
            char idbuf[8];
            snprintf(idbuf, sizeof(idbuf), "%d", g->collision_id[x][y]);
            DrawText(idbuf, x * TILE_SIZE + 4, y * TILE_SIZE + 4, 10, WHITE);
          }
        }
      }
    }
  }

  // If in rectangle mode and we have one click, draw preview rectangle between first click and current mouse
  if (g->edit_mode == 1 && g->click_count == 1) {
    Vector2 p = g->click_points[0];
    int x0 = (int)p.x;
    int y0 = (int)p.y;
    int x1 = tile_x;
    int y1 = tile_y;
    int rx = (x0 < x1) ? x0 : x1;
    int ry = (y0 < y1) ? y0 : y1;
    int rw = abs(x1 - x0) + 1;
    int rh = abs(y1 - y0) + 1;
    Rectangle preview = (Rectangle){rx * TILE_SIZE, ry * TILE_SIZE, rw * TILE_SIZE, rh * TILE_SIZE};
    Color line_col = GREEN;
    Color fill_col = (Color){0, 255, 0, 50};
    if (g->pending_action == -1) {
      // erase preview
      line_col = RED;
      fill_col = (Color){255, 0, 0, 50};
    }
    DrawRectangleLinesEx(preview, 2, line_col);
    // draw translucent fill
    DrawRectangleRec(preview, fill_col);
  }

  // Draw selected tile preview on top of map / preview
  Texture2D tex = g->tiles[g->selected_tile];
  Rectangle src = (Rectangle){0.0f, 0.0f, (float)tex.width, (float)tex.height};
  Rectangle dest = (Rectangle){center_x - (TILE_SIZE / 2.0f),
                               center_y - (TILE_SIZE / 2.0f),
                               (float)TILE_SIZE,
                               (float)TILE_SIZE};
  // Draw semi-transparent preview for selected tile
  // Draw semi-transparent preview for selected tile or collision cell
  if (g->active_layer == 0) {
    Color preview_tint = (Color){255, 255, 255, 180};
    DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, preview_tint);
  } else {
    // collision preview
    Rectangle pr = (Rectangle){center_x - (TILE_SIZE/2.0f), center_y - (TILE_SIZE/2.0f), (float)TILE_SIZE, (float)TILE_SIZE};
    if (g->current_collision_type == 1) DrawRectangleRec(pr, (Color){255, 0, 0, 120});
    else if (g->current_collision_type == 2) DrawRectangleRec(pr, (Color){128, 0, 128, 120});
    else if (g->current_collision_type == 3) DrawRectangleRec(pr, (Color){0, 128, 255, 120});
    DrawRectangleLinesEx(pr, 1, BLACK);
  }

  EndMode2D();

  // HUD: show current edit mode
  char mode_buf[64];
  if (g->edit_mode == 0) {
    snprintf(mode_buf, sizeof(mode_buf), "Mode: Single (press M to toggle)");
  } else {
    if (g->click_count == 1)
      snprintf(mode_buf, sizeof(mode_buf), "Mode: Rectangle (click 2nd point)");
    else
      snprintf(mode_buf, sizeof(mode_buf), "Mode: Rectangle (press M to toggle)");
  }
  DrawText(mode_buf, 10, 10, 20, WHITE);

  if (g->save_flash_counter > 0) {
    DrawText("Map saved to assets/maps/map.json", 10, 36, 18, YELLOW);
  }

  // Active layer HUD
  char layer_buf[64];
  if (g->active_layer == 0) snprintf(layer_buf, sizeof(layer_buf), "Layer: Tiles (TAB to switch)");
  else snprintf(layer_buf, sizeof(layer_buf), "Layer: Collision (TAB to switch)");
  DrawText(layer_buf, 10, 60, 18, WHITE);

  char vis_buf[64];
  snprintf(vis_buf, sizeof(vis_buf), "Collision overlay: %s (press L)", g->collision_visible ? "Visible" : "Hidden");
  DrawText(vis_buf, 10, 80, 16, LIGHTGRAY);

  if (g->active_layer == 1) {
    char ctype_buf[64];
    const char *ctname = "";
    if (g->current_collision_type == 1) ctname = "Solid";
    else if (g->current_collision_type == 2) ctname = "Death";
    else if (g->current_collision_type == 3) ctname = "Trigger";
    snprintf(ctype_buf, sizeof(ctype_buf), "Collision type: %s (Z=Solid, C=Death, X=Trigger, tile ID -> trigger ID)", ctname);
    DrawText(ctype_buf, 10, 100, 14, LIGHTGRAY);
  }

  EndDrawing();
}

void game_update(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  // Inputs
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    g->pos.x -= 1;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    g->pos.x += 1;
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    g->pos.y -= 1;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    g->pos.y += 1;

  // Toggle fullscreen
  if (IsKeyPressed(KEY_F11)) {
    ToggleFullscreen();
  }

  // Editor
  for (int key = KEY_ZERO; key <= KEY_NINE; key++) {
    if (IsKeyPressed(key)) {
      g->selected_tile = key - KEY_ZERO;
    }
  }

  // Handle mouse clicks
  // Right mouse button: erase behavior
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    int tile_x = (int)floor(g->world_mouse_pos.x / TILE_SIZE);
    int tile_y = (int)floor(g->world_mouse_pos.y / TILE_SIZE);
    // Clamp
    if (tile_x < 0) tile_x = 0;
    if (tile_y < 0) tile_y = 0;
    if (tile_x >= MAP_W) tile_x = MAP_W - 1;
    if (tile_y >= MAP_H) tile_y = MAP_H - 1;

    if (g->edit_mode == 0) {
      // single-tile erase or collision toggle
      if (g->active_layer == 0) {
        g->map[tile_x][tile_y] = -1;
      } else {
        g->collision_type[tile_x][tile_y] = 0;
        g->collision_id[tile_x][tile_y] = 0;
      }
    } else {
      // rectangle erase uses same two-click flow but with pending_action = -1
      if (g->click_count == 0) {
        g->click_points[0] = (Vector2){(float)tile_x, (float)tile_y};
        g->click_count = 1;
        g->pending_action = -1;
      } else {
        // second click: erase rectangle or clear collision rectangle
        int x0 = (int)g->click_points[0].x;
        int y0 = (int)g->click_points[0].y;
        int x1 = tile_x;
        int y1 = tile_y;
        int sx = (x0 < x1) ? x0 : x1;
        int ex = (x0 < x1) ? x1 : x0;
        int sy = (y0 < y1) ? y0 : y1;
        int ey = (y0 < y1) ? y1 : y0;
        for (int x = sx; x <= ex; x++) {
          for (int y = sy; y <= ey; y++) {
            if (x >= 0 && y >= 0 && x < MAP_W && y < MAP_H) {
              if (g->active_layer == 0) g->map[x][y] = -1;
              else { g->collision_type[x][y] = 0; g->collision_id[x][y] = 0; }
            }
          }
        }
        g->click_count = 0;
        g->pending_action = 0;
      }
    }
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    int tile_x = (int)floor(g->world_mouse_pos.x / TILE_SIZE);
    int tile_y = (int)floor(g->world_mouse_pos.y / TILE_SIZE);
    // Clamp to map bounds
    if (tile_x < 0) tile_x = 0;
    if (tile_y < 0) tile_y = 0;
    if (tile_x >= MAP_W) tile_x = MAP_W - 1;
    if (tile_y >= MAP_H) tile_y = MAP_H - 1;

    if (g->edit_mode == 0) {
      // single-tile placement or collision set
      if (g->active_layer == 0) g->map[tile_x][tile_y] = g->selected_tile;
      else {
        g->collision_type[tile_x][tile_y] = g->current_collision_type;
        if (g->current_collision_type == 3) g->collision_id[tile_x][tile_y] = g->selected_tile; // use selected tile as trigger id
      }
    } else {
      // rectangle mode: store clicks and on second click fill rectangle
      if (g->click_count == 0) {
        g->click_points[0] = (Vector2){(float)tile_x, (float)tile_y};
        g->click_count = 1;
        g->pending_action = 1; // place
      } else {
        g->click_points[1] = (Vector2){(float)tile_x, (float)tile_y};
        // fill rectangle between points inclusive
        int x0 = (int)g->click_points[0].x;
        int y0 = (int)g->click_points[0].y;
        int x1 = tile_x;
        int y1 = tile_y;
        int sx = (x0 < x1) ? x0 : x1;
        int ex = (x0 < x1) ? x1 : x0;
        int sy = (y0 < y1) ? y0 : y1;
        int ey = (y0 < y1) ? y1 : y0;
        for (int x = sx; x <= ex; x++) {
          for (int y = sy; y <= ey; y++) {
            if (x >= 0 && y >= 0 && x < MAP_W && y < MAP_H) {
              if (g->active_layer == 0) g->map[x][y] = g->selected_tile;
              else { g->collision_type[x][y] = g->current_collision_type; if (g->current_collision_type == 3) g->collision_id[x][y] = g->selected_tile; }
            }
          }
        }
        g->click_count = 0;
        g->pending_action = 0;
      }
    }
  }

  // Toggle edit mode
  if (IsKeyPressed(KEY_M)) {
    g->edit_mode = (g->edit_mode == 0) ? 1 : 0;
    // reset click count when switching modes
    g->click_count = 0;
    g->pending_action = 0;
  }

  // Switch active layer
  if (IsKeyPressed(KEY_TAB)) {
    g->active_layer = (g->active_layer == 0) ? 1 : 0;
    g->click_count = 0;
    g->pending_action = 0;
  }

  // Toggle collision visibility
  if (IsKeyPressed(KEY_L)) {
    g->collision_visible = (g->collision_visible == 0) ? 1 : 0;
  }

  // Change current collision type
  if (IsKeyPressed(KEY_Z)) g->current_collision_type = 1; // solid
  if (IsKeyPressed(KEY_X)) g->current_collision_type = 3; // trigger
  if (IsKeyPressed(KEY_C)) g->current_collision_type = 2; // death

  // Save map to JSON
  if (IsKeyPressed(KEY_F5)) {
    // ensure directory exists
    const char *dir = "assets/maps";
    // try to create directory (POSIX)
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
      // create directory with 0755
      #ifdef _WIN32
        _mkdir(dir);
      #else
        mkdir(dir, 0755);
      #endif
    }

    const char *path = "assets/maps/map.json";
    FILE *f = fopen(path, "w");
    if (f) {
      // Write header
      fprintf(f, "{\"map_w\": %d, \"map_h\": %d,\n", MAP_W, MAP_H);

      // Tiles: write as list of rectangles {tile,x,y,w,h}
      fprintf(f, "\"tiles\": [\n");
      int visited[MAP_W][MAP_H];
      memset(visited, 0, sizeof(visited));
      int first = 1;
      for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
          if (visited[x][y]) continue;
          int v = g->map[x][y];
          if (v < 0) continue;
          // expand width
          int w = 1;
          while (x + w < MAP_W) {
            int ok = 1;
            if (g->map[x + w][y] != v) ok = 0;
            if (!ok) break;
            w++;
          }
          // expand height while all rows match
          int h = 1;
          while (y + h < MAP_H) {
            int ok = 1;
            for (int xi = x; xi < x + w; xi++) {
              if (g->map[xi][y + h] != v) { ok = 0; break; }
            }
            if (!ok) break;
            h++;
          }
          // mark visited
          for (int yy = y; yy < y + h; yy++) for (int xx = x; xx < x + w; xx++) visited[xx][yy] = 1;
          if (!first) fprintf(f, ",\n");
          fprintf(f, "  {\"tile\": %d, \"x\": %d, \"y\": %d, \"w\": %d, \"h\": %d}", v, x, y, w, h);
          first = 0;
        }
      }
      fprintf(f, "\n],\n");

      // Collisions: write list of rectangles {type,id,x,y,w,h}
      fprintf(f, "\"collisions\": [\n");
      memset(visited, 0, sizeof(visited));
      first = 1;
      for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
          if (visited[x][y]) continue;
          int ct = g->collision_type[x][y];
          int cid = g->collision_id[x][y];
          if (ct == 0) continue;
          // expand width where both type and id match
          int w = 1;
          while (x + w < MAP_W) {
            int ok = 1;
            if (g->collision_type[x + w][y] != ct) ok = 0;
            if (g->collision_id[x + w][y] != cid) ok = 0;
            if (!ok) break;
            w++;
          }
          int h = 1;
          while (y + h < MAP_H) {
            int ok = 1;
            for (int xi = x; xi < x + w; xi++) {
              if (g->collision_type[xi][y + h] != ct) { ok = 0; break; }
              if (g->collision_id[xi][y + h] != cid) { ok = 0; break; }
            }
            if (!ok) break;
            h++;
          }
          for (int yy = y; yy < y + h; yy++) for (int xx = x; xx < x + w; xx++) visited[xx][yy] = 1;
          if (!first) fprintf(f, ",\n");
          fprintf(f, "  {\"type\": %d, \"id\": %d, \"x\": %d, \"y\": %d, \"w\": %d, \"h\": %d}", ct, cid, x, y, w, h);
          first = 0;
        }
      }
      fprintf(f, "\n]\n}\n");
      fclose(f);
      g->save_flash_counter = 120; // show message for ~2 seconds at 60fps
    }
  }

  if (g->save_flash_counter > 0) g->save_flash_counter--;

  // Other stuff
  Vector2 screen_mouse_pos = GetMousePosition();

  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->pos;
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
