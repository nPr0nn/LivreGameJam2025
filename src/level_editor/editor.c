
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
  g->panel_height = 100;
  g->selected_option = -1;

  for (i32 i = 0; i < MAX_EDITOR_OPTIONS; i++) {
    g->options[i] = (Rectangle){
      20 + i * 60, 
      GetScreenHeight() - g->panel_height + 20, 
      50, 50
    };
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

  DrawInfiniteGrid(g->camera, 25, LIGHTGRAY);
  DrawCircleV(g->world_mouse_pos, 10, RED);

  EndMode2D();

  // Draw editor panel
  DrawRectangle(0, GetScreenHeight() - g->panel_height,
                GetScreenWidth(), g->panel_height, LIGHTGRAY);

  // Draw editor options
  for (i32 i = 0; i < MAX_EDITOR_OPTIONS; i++) {
    Color option_color = (i == g->selected_option) ? RED : DARKGRAY;
    DrawRectangleRec(g->options[i], option_color);
    DrawText(TextFormat("%d", i + 1), 
    g->options[i].x + 15, g->options[i].y + 15, 20, WHITE);
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

  // Other stuff
  Vector2 screen_mouse_pos = GetMousePosition();

  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->pos;

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    // Check if clicking on any editor option
    for (i32 i = 0; i < MAX_EDITOR_OPTIONS; i++) {
      if (CheckCollisionPointRec(screen_mouse_pos, g->options[i])) {
        g->selected_option = i;
        break;
      }
    }
  }
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
