#include "shader_manager.h"
#define SLC_IMPL

#include "character.h"
#include "collision_system.h"
#include "enemy.h"
#include "game.h"
#include "game_context.h"
#include "level_loader.h"
#include "utils.h"

Vector2 get_world_pos_in_texture(GameContext *g, Vector2 world_pos) {
  Vector2 screen_pos = GetWorldToScreen2D(world_pos, g->camera);
  screen_pos.y = g->screen.texture.height - screen_pos.y;
  return screen_pos;
}

void game_init(void *ctx) {
  GameContext *g = (GameContext *)ctx;

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

  // --- Camera setup (in retro coordinate space) ---
  g->camera =
      (Camera2D){.target = (Vector2){0, 0},
                 .offset = (Vector2){target_width / 2.0f, target_height / 2.0f},
                 .rotation = 0.0f,
                 .zoom = 1.0f};
  g->is_paused = false; // Initialize paused state to false

  // --- Shader Manager ---
  shader_manager_init(&g->shader_manager);

  // --- Particle System ---
  g->particle_system = particle_system_create(g->g_arena, 1000);

  // --- Map Init ---
  // g->western_font = LoadFont("images/PublicPixel.ttf");
  Image background_image = LoadImage("images/background.png");
  g->background = LoadTextureFromImage(background_image);
  UnloadImage(background_image);
  g->level_data = load_level_data("images/levels/map.json", g->g_arena);
  level_init(g->level_data);

  // --- Entities Init ---
  g->anchor = level_get_player_position(g->level_data);
  character_init(&g->player, g->anchor, BLUE);
}

void game_draw(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  const int target_width = g->screen.texture.width;
  const int target_height = g->screen.texture.height;

  const int window_width = GetScreenWidth();
  const int window_height = GetScreenHeight();

  int scale_factor = window_height / target_height;
  if (scale_factor < 1)
    scale_factor = 1;

  const int scaled_width = target_width * scale_factor;
  const int scaled_height = target_height * scale_factor;

  const int offset_x = (window_width - scaled_width) / 2;
  const int offset_y = 0;

  // --- Render to low-res texture ---
  BeginTextureMode(g->screen);
  ClearBackground((Color){237, 165, 63, 255});
  BeginMode2D(g->camera);

  // scrolling.
  float bg_scale = 0.4f;
  float parallax_factor =
      0.5f; // How much slower the background scrolls (0.5 = 50% speed)

  // The source rectangle scrolls its X position.
  // fmodf makes the value wrap around when it exceeds the texture's width,
  // creating the infinite looping effect.
  Rectangle source_rec = {
      fmodf(g->camera.target.x * parallax_factor, g->background.width), 0.0f,
      (float)g->background.width, (float)g->background.height};

  // The destination rectangle should cover the entire visible screen area where
  // the background is meant to be seen. We draw it a bit wider than the
  // target width to prevent any visible seams at the edges during movement.
  Rectangle dest_rec = {
      g->camera.target.x -
          (target_width / 2.0f), // Align with the left edge of the camera
      g->anchor.y - 250,         // Your original Y position
      (float)target_width,       // Match the camera's width
      (float)g->background.height * bg_scale};

  DrawTextureTiled(g->background, source_rec, dest_rec, (Vector2){0, 0}, 0.0f,
                   bg_scale, WHITE);

  // --- End of new background drawing logic ---

  // Draw THE WORLD
  level_draw(g->level_data, g->player.en.pos);

  // for (int i = 0; i < g->level_data->collision_count; i++) {
  //   DrawRectangleLinesEx(g->collision_rects[i], 20, BLUE);
  // }

  character_draw(&g->player, &g->shader_manager);
  particle_system_draw(g->particle_system);
  enemy_draw(&g->enemy);

  EndMode2D();
  DrawText("O Voaqueiro: o Inicio", 5.0, 5.0, 5.0, WHITE);

  EndTextureMode();
  // --- Draw final texture to screen (logic remains the same) ---
  BeginDrawing();
  DrawFPS(10, 10);
  ClearBackground(BLACK);

  if (g->shader_manager.is_ripple_active) {
    BeginShaderMode(g->shader_manager.ripple_shader);
  } else {
    if (!g->is_paused) {
      BeginShaderMode(g->shader_manager.glitch_shader);
    }
  }

  DrawTexturePro(g->screen.texture,
                 (Rectangle){0, 0, (float)target_width, -(float)target_height},
                 (Rectangle){offset_x, offset_y, (float)scaled_width,
                             (float)scaled_height},
                 (Vector2){0, 0}, 0.0f, WHITE);

  // Check if a shader was active before trying to end the mode
  if (g->shader_manager.is_ripple_active || !g->is_paused) {
    EndShaderMode();
  }

  if (g->is_paused) {
    DrawText("Game Paused", 400, 300, 30, RED);
    DrawText("Press P to Resume", 380, 340, 20, LIGHTGRAY);
  }
  EndDrawing();
}

void game_update(void *ctx) {
  GameContext *g = (GameContext *)ctx;
  bool to_trigger = false;

  float dt = GetFrameTime();
  Vector2 player_texture_pos = get_world_pos_in_texture(g, g->player.en.pos);
  g->shader_manager.spotlight_center.x =
      player_texture_pos.x / g->screen.texture.width;
  g->shader_manager.spotlight_center.y =
      player_texture_pos.y / g->screen.texture.height;
  shader_manager_update(&g->shader_manager);

  // Toggle pause state when P is pressed
  if (IsKeyPressed(KEY_P)) {
    // First, flip the pause state
    g->is_paused = !g->is_paused;

    if (!g->is_paused) {
      const float HORIZONTAL_MOMENTUM_THRESHOLD = 150.0f;

      // Check if the player has enough momentum to trigger the ripple
      if (fabs(g->player.en.vel.x) >= HORIZONTAL_MOMENTUM_THRESHOLD ||
          g->player.jump_velocity_modifier >= 1.5) {
        trigger_ripple(&g->shader_manager, g->shader_manager.spotlight_center);
      }
    }
  }

  Vector2 screen_mouse_pos = GetMousePosition();
  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->player.en.pos;
  character_read_input(&g->player, g->is_paused);
  character_pre_update(&g->player, g->particle_system, dt, g->is_paused);

  // Skip game updates if paused
  if (g->is_paused) {
    return;
  }

  // --- Collision Resolution Loop ---
  run_collisions_on_entity(&g->player.en, g->level_data->collisions,
                           g->level_data->collision_count, dt,
                           character_on_collision);
  character_update(&g->player, g->particle_system, dt, g->is_paused);
  particle_system_update(g->particle_system, dt);
  enemy_update(&g->enemy, dt);
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) {
  GameContext *g = (GameContext *)ctx;

  CloseWindow();
  shader_manager_unload(&g->shader_manager);
}
