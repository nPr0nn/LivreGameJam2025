#include "game.h"
#include "character.h"
#include "game_context.h"
#include "utils.h"

void game_init(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  // --- Retro target resolution ---
  const i32 target_width = 160;
  const i32 target_height = 144;
  const i32 scale_factor = 5; // e.g. 1x, 2x, 3x, 4x...

  // --- Scaled resolution ---
  const i32 scaled_width = target_width * scale_factor;
  const i32 scaled_height = target_height * scale_factor;

#ifndef PLATFORM_WEB
  i32 current_monitor_id = GetCurrentMonitor();
  f32 monitor_scale = 0.8f; // a bit larger by default
  i32 monitor_width =
      (i32)(monitor_scale * GetMonitorWidth(current_monitor_id));
  i32 monitor_height =
      (i32)(monitor_scale * GetMonitorHeight(current_monitor_id));

  // Make sure the window is at least as large as the scaled canvas
  if (monitor_width < scaled_width)
    monitor_width = scaled_width;
  if (monitor_height < scaled_height)
    monitor_height = scaled_height;

  InitWindow(monitor_width, monitor_height, "Livre GameJam");
#else
  i32 monitor_width = 960;
  i32 monitor_height = 540;
  InitWindow(monitor_width, monitor_height, "Livre GameJam");
#endif

  // --- Create the low-res render texture ---
  g->screen = LoadRenderTexture(target_width, target_height);
  SetTextureFilter(g->screen.texture,
                   TEXTURE_FILTER_POINT); // pixel-perfect scaling
  SetTargetFPS(60);

  // --- Camera setup (in retro coordinate space) ---
  g->camera =
      (Camera2D){.target = (Vector2){0, 0},
                 .offset = (Vector2){target_width / 2.0f, target_height / 2.0f},
                 .rotation = 0.0f,
                 .zoom = 1.0f};
  g->pos = (Vector2){0, 0};
  g->is_paused = false; // Initialize paused state to false
  character_init(&g->player, (Vector2){0, 0}, 8, BLUE);

  g->dt = 0;
}

void game_draw(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  const int target_width = g->screen.texture.width;
  const int target_height = g->screen.texture.height;

  const int window_width = GetScreenWidth();
  const int window_height = GetScreenHeight();

  // --- Scale based on window height only ---
  int scale_factor = window_height / target_height; // integer scale
  if (scale_factor < 1)
    scale_factor = 1; // fallback

  const int scaled_width = target_width * scale_factor;
  const int scaled_height = target_height * scale_factor;

  // Center horizontally -> vertical bars only
  const int offset_x = (window_width - scaled_width) / 2;
  const int offset_y = 0; // fill full height, no horizontal bars

  // --- Render to low-res texture ---
  BeginTextureMode(g->screen);
  ClearBackground((Color){30, 30, 50, 255});
  BeginMode2D(g->camera);
  DrawInfiniteGrid(g->camera, 25, LIGHTGRAY);
  DrawCircleV(g->world_mouse_pos, 10, RED);
  character_draw(&g->player);

  EndMode2D();
  DrawText("Congrats! You created your first window!", 10, 10, 10, LIGHTGRAY);
  EndTextureMode();

  // --- Draw to screen with vertical bars ---
  BeginDrawing();
  ClearBackground(BLACK); // fills the bars

  DrawTexturePro(g->screen.texture,
                 (Rectangle){0, 0, (float)target_width, -(float)target_height},
                 (Rectangle){offset_x, offset_y, (float)scaled_width,
                             (float)scaled_height},
                 (Vector2){0, 0}, 0.0f, WHITE);

    // Draw pause message if paused
    if (g->is_paused) {
      DrawText("Game Paused", 400, 300, 30, RED);
      DrawText("Press P to Resume", 380, 340, 20, LIGHTGRAY);
    }
  EndDrawing();
}

void game_update(void *ctx) {
  GameContext *g = (GameContext *)ctx;
  g->dt = GetFrameTime();

  // Toggle pause state when P is pressed
  if (IsKeyPressed(KEY_P)) {
    g->is_paused = !g->is_paused;
  }

  // Skip game updates if paused
  if (g->is_paused) {
    character_update(&g->player, g->dt,  g->is_paused);
    return;
  }

  // g->pos.x = g->player.pos.x;
  // g->pos.y = g->player.pos.y;

  Vector2 screen_mouse_pos = GetMousePosition();
  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->pos;
  character_read_input(&g->player);
  character_update(&g->player, g->dt, g->is_paused);
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
