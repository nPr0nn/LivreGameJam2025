
#include "editor.h"
#include "game_context.h"
#include "utils.h"

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
    }
  }
  
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
  DrawTexturePro(tex, src, dest, (Vector2){0, 0}, 0.0f, WHITE);

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
      // single-tile erase
      g->map[tile_x][tile_y] = -1;
    } else {
      // rectangle erase uses same two-click flow but with pending_action = -1
      if (g->click_count == 0) {
        g->click_points[0] = (Vector2){(float)tile_x, (float)tile_y};
        g->click_count = 1;
        g->pending_action = -1;
      } else {
        // second click: erase rectangle
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
              g->map[x][y] = -1;
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
      // single-tile placement
      g->map[tile_x][tile_y] = g->selected_tile;
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
              g->map[x][y] = g->selected_tile;
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
