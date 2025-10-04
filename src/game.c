#include "game.h"
#include "game_context.h"
#include "utils.h"
#include "character.h"

void game_init(void *ctx) {
  GameContext *g = (GameContext *)ctx;

#ifndef PLATFORM_WEB
  i32 current_monitor_id = GetCurrentMonitor();
  i32 monitor_width = GetMonitorWidth(current_monitor_id);
  i32 monitor_height = GetMonitorHeight(current_monitor_id);
  f32 scale = 0.5;
  InitWindow((i32)(scale * monitor_width), (i32)(scale * monitor_height),
             "Livre GameJam");
#else
  InitWindow(1080, 720, "Livre GameJam");
#endif

  i32 camera_initial_pos_x = (scale*monitor_width)/2;
  i32 camera_initial_pos_y = (scale*monitor_height)/1.25;

  g->camera = (Camera2D){{camera_initial_pos_x, camera_initial_pos_y}, {0, 0}, 0.0, 1.0};
  g->pos = (Vector2){0, 0};
  g->is_paused = false; // Initialize paused state to false
  character_init(&g->player, (Vector2){0, 0}, 15, BLUE);
}

void game_draw(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  BeginDrawing();
  ClearBackground(BLACK);

  BeginMode2D(g->camera);
  DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
  DrawInfiniteGrid(g->camera, 25, LIGHTGRAY);
  DrawCircleV(g->world_mouse_pos, 10, RED);
  character_draw(&g->player);
  EndMode2D();

  // Draw pause message if paused
  if (g->is_paused) {
    DrawText("Game Paused", 400, 300, 30, RED);
    DrawText("Press P to Resume", 380, 340, 20, LIGHTGRAY);
  }

  EndDrawing();
}

void game_update(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  // Toggle pause state when P is pressed
  if (IsKeyPressed(KEY_P)) {
    g->is_paused = !g->is_paused;
  }

  // Skip game updates if paused
  if (g->is_paused) {
    character_update(&g->player, g->is_paused);
    return;
  }

  // g->pos.x = g->player.pos.x;
  // g->pos.y = g->player.pos.y;

  Vector2 screen_mouse_pos = GetMousePosition();
  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->pos;
  character_update(&g->player, g->is_paused);
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
