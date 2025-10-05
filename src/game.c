#include <iso646.h>
#define SLC_IMPL

#include "character.h"
#include "collision_system.h"
#include "enemy.h"
#include "game.h"
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
  g->pos = (Vector2){0, 0};
  g->is_paused = false; // Initialize paused state to false

  // --- Entities Init ---
  character_init(&g->player, (Vector2){0, 0}, 8, BLUE);
  enemy_init(&g->enemy, (Vector2){30, 0}, 30.0f);

  i32 num_object = 100;
  g->collision_rects =
      mem_arena_alloc(g->g_arena, num_object * sizeof(Rectangle));
  g->collision_rects[0] = (Rectangle){20, 0, 16, 16};
  g->collision_rects[1] = (Rectangle){40, 0, 16, 16};
  g->collision_rects[2] = (Rectangle){60, 0, 20, 20};
  g->collision_rects[3] = (Rectangle){-20, 0, 20, 20};
  g->collision_rects[4] = (Rectangle){80, 0, 20, 20};
  g->collision_rects[5] = (Rectangle){140, -30, 20, 20};
  g->collision_rects[6] = (Rectangle){160, -40, 20, 20};

  g->collision_rects_count = 7;
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
  character_draw(&g->player);
  enemy_draw(&g->enemy);

  // Draw Collisions Rects
  for (int i = 0; i < g->collision_rects_count; i++) {
    Rectangle centered_rect = {
        g->collision_rects[i].x - ((g->collision_rects[i].width) / 2.0f),
        g->collision_rects[i].y - ((g->collision_rects[i].height) / 2.0f),
        g->collision_rects[i].width, g->collision_rects[i].height};
    DrawRectangleLinesEx(centered_rect, 20, BLUE);
  }

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

  float dt = GetFrameTime();

  // Toggle pause state when P is pressed
  if (IsKeyPressed(KEY_P)) {
    g->is_paused = !g->is_paused;
  }

  Vector2 screen_mouse_pos = GetMousePosition();
  g->world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, g->camera);
  g->camera.target = g->player.pos;
  character_read_input(&g->player, g->is_paused);
  character_pre_update(&g->player, dt, g->is_paused);

  // Skip game updates if paused
  if (g->is_paused) {
    return;
  }

  // --- Collision Resolution Loop ---

  // We might collide multiple times, so we may need to iterate.
  // For a simple game, once is often enough, but a loop is more robust.
  for (int i = 0; i < 4; i++) {
    // 1. Find the EARLIEST collision for this frame's movement
    Vector2 origin_vel = (Vector2){g->player.vel.x * dt, g->player.vel.y * dt};
    Ray2D movement_ray = {g->player.pos, origin_vel};

    // Assume no collision
    CollisionInfo nearest_collision = {.t_hit = 1.0f};
    bool did_collide = false;

    for (int j = 0; j < g->collision_rects_count; j++) {
      CollisionInfo current_collision;
      if (check_collision_entity_bbox(&movement_ray, &g->player.bbox,
                                      &g->collision_rects[j],
                                      &current_collision)) {
        if (current_collision.t_hit < nearest_collision.t_hit) {
          nearest_collision = current_collision;
          did_collide = true;
        }
      }
    }

    // 2. If a collision was found, resolve it
    if (did_collide) {
      if (nearest_collision.contact_normal.y < 0) {
        g->player.is_grounded = true;
      }
      // STEP A: CORRECT POSITION
      // Move the player to the exact point of contact. This prevents overlap.
      // We add a tiny amount in the direction of velocity to avoid getting
      // stuck.
      g->player.pos.x +=
          g->player.vel.x * dt * (nearest_collision.t_hit - 0.001f);
      g->player.pos.y +=
          g->player.vel.y * dt * (nearest_collision.t_hit - 0.001f);

      // STEP B: CORRECT VELOCITY (for sliding)
      float dot_product =
          (g->player.vel.x * nearest_collision.contact_normal.x +
           g->player.vel.y * nearest_collision.contact_normal.y);

      g->player.vel.x -= dot_product * nearest_collision.contact_normal.x;
      g->player.vel.y -= dot_product * nearest_collision.contact_normal.y;
    }
  }

  character_update(&g->player, GetFrameTime(), g->is_paused);

  enemy_update(&g->enemy, GetFrameTime());
}

void game_loop(void *ctx) {
  game_update(ctx);
  game_draw(ctx);
}

void game_exit(void *ctx) { CloseWindow(); }
