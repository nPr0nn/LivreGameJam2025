#include "game.h"
#include "character.h"
#include "enemy.h"
#include "game_context.h"
#include "utils.h"
#include <stdio.h>
#include "map_loader.h"

void game_init(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  // const char *folder_path = "images";
  // size_t count = 0;
  // slc_String *paths = slc_list_files(folder_path, &count, g->g_arena);
  // for (i32 i = 0; i < count; i++) {
  //   printf("path: %s \n", paths[i].data);
  // }

  // --- Retro target resolution ---
  const i32 target_width = 160;
  const i32 target_height = 144;
  const i32 scale_factor = 5; // e.g. 1x, 2x, 3x, 4x...

  // --- Scaled resolution ---
  const i32 scaled_width = target_width * scale_factor;
  const i32 scaled_height = target_height * scale_factor;

  i32 monitor_width = 1080;
  i32 monitor_height = 540;

  if (monitor_width < scaled_width)
    monitor_width = scaled_width;
  if (monitor_height < scaled_height)
    monitor_height = scaled_height;

  InitWindow(monitor_width, monitor_height, "Livre GameJam");

  // --- Create the low-res render texture ---
  g->screen = LoadRenderTexture(target_width, target_height);
  SetTextureFilter(g->screen.texture,
                   TEXTURE_FILTER_POINT); // pixel-perfect scaling
  SetTargetFPS(60);
  
  // --- Load map ---
  load_map(g, "assets/maps/map.json");

  // --- Camera setup (in retro coordinate space) ---
  g->camera =
      (Camera2D){.target = (Vector2){0, 0},
                 .offset = (Vector2){target_width / 2.0f, target_height / 2.0f},
                 .rotation = 0.0f,
                 .zoom = 1.0f};
  g->pos = (Vector2){0, 0};
  g->is_paused = false; // Initialize paused state to false
  // Initialize player at recorded spawn (from map) if available
  Vector2 player_start = (Vector2){0, 0};
  if (g->player_spawn_set) player_start = g->player_spawn;
  character_init(&g->player, player_start, 8, BLUE);
  enemy_init(&g->enemy, (Vector2){30, 0}, 30.0f); // Initialize enemy
  
  // Debug print map
  for (int y = 0; y < 50; y++) {
    for (int x = 0; x < 50; x++) {
      printf("%2d ", g->map[x][y]);
    }
    printf("\n");
  }
  
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
  DrawInfiniteGrid(g->camera, TILE_SIZE, LIGHTGRAY);

  // Draw map tiles (Passar para map_loader.c depois)
  for (int x = 0; x < MAP_W; x++) {
    for (int y = 0; y < MAP_H; y++) {
      int tile_idx = g->map[x][y];
      if (tile_idx >= 0 && tile_idx < NUM_TILES) {
        Texture2D tex = g->tiles[tile_idx];
        // Skip drawing if tile is "voaqueiro.png"
        if (tex.id != 0 && strcmp(g->tile_names[tile_idx], "voaqueiro.png") != 0) {
          DrawTexturePro(tex,
                         (Rectangle){0, 0, (float)tex.width, (float)tex.height},
                         (Rectangle){(float)(x * TILE_SIZE), (float)(y * TILE_SIZE),
                                     (float)TILE_SIZE, (float)TILE_SIZE},
                         (Vector2){0, 0}, 0.0f, WHITE);
        }
      }
    }
  }
  character_draw(&g->player);
  enemy_draw(&g->enemy);

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

  character_read_input(&g->player, g->is_paused);
  character_update(&g->player, GetFrameTime(), g->is_paused);

  // Toggle pause state when P is pressed
  if (IsKeyPressed(KEY_P)) {
    g->is_paused = !g->is_paused;
  }

  // Skip game updates if paused
  if (g->is_paused) {
    character_read_input(&g->player, g->is_paused);
    character_update(&g->player, GetFrameTime(), g->is_paused);
    return;
  }

  Vector2 screen_mouse_pos = GetMousePosition();
  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->player.pos;
  character_read_input(&g->player, g->is_paused);
  character_update(&g->player, GetFrameTime(), g->is_paused);
  enemy_update(&g->enemy, GetFrameTime());
  character_check_collision(&g->player, &g->enemy.position, &g->enemy.radius);
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
