
#include "../vendor/raylib/raylib.h"
#include "../vendor/slc.h"

#include <math.h>

void DrawMyGrid(Camera2D cam, f32 spacing, Color color) {
  f32 window_width = GetScreenWidth();
  f32 window_height = GetScreenHeight();

  Vector2 top_left = GetScreenToWorld2D((Vector2){0, 0}, cam);
  Vector2 bottom_right =
      GetScreenToWorld2D((Vector2){window_width, window_height}, cam);

  i32 min_x = floor(top_left.x / spacing) - 1;
  i32 max_x = ceil(bottom_right.x / spacing) - 1;
  i32 min_y = floor(top_left.y / spacing) - 1;
  i32 max_y = ceil(bottom_right.y / spacing) - 1;

  for (i32 x = min_x; x <= max_x; x++) {
    f32 x_pos = (f32)x * spacing;
    DrawLineEx((Vector2){x_pos, top_left.y}, (Vector2){x_pos, bottom_right.y},
               1.5, color);
  }

  for (i32 y = min_y; y <= max_y; y++) {
    f32 y_pos = (f32)y * spacing;
    DrawLineEx((Vector2){top_left.x, y_pos}, (Vector2){bottom_right.x, y_pos},
               1.5, color);
  }
}

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");

  Camera2D cam = {{0, 0}, {0, 0}, 0.0, 1.0};

  Vector2 pos = {0, 0};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(cam);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    if (IsKeyDown(KEY_LEFT))
      pos.x -= 1;
    if (IsKeyDown(KEY_RIGHT))
      pos.x += 1;
    if (IsKeyDown(KEY_UP))
      pos.y -= 1;
    if (IsKeyDown(KEY_DOWN))
      pos.y += 1;

    DrawMyGrid(cam, 25, LIGHTGRAY);
    Vector2 screen_mouse_pos = GetMousePosition();
    Vector2 world_mouse_pos = GetScreenToWorld2D(screen_mouse_pos, cam);
    DrawCircleV(world_mouse_pos, 10, RED);

    cam.target = pos;

    EndMode2D();

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
