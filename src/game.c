#include "game.h"
#include "game_context.h"
#include "utils.h"
#include "character.h"
#include "menu.h"

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

  g->camera = (Camera2D){{(scale*monitor_width)/2, (scale*monitor_height)/2}, {0, 0}, 0.0, 1.0};
  g->pos = (Vector2){0, 0};
  character_init(&g->player, (Vector2){0, 0}, 15, BLUE);
  menu_init(&g->menu, (Vector2){0.0,0.0});
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
  menu_draw(&g->menu);
  EndDrawing();
}

void game_update(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  g->pos.x = g->player.pos.x;
  g->pos.y = g->player.pos.y;

  // Other stuf
  Vector2 screen_mouse_pos = GetMousePosition();

  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->pos;
  character_update(&g->player);
  menu_update(&g->menu, g);
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
