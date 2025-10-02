
#include "../vendor/raylib/raylib.h"
#include "../vendor/slc.h"

int main(void) {
  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);

    DrawCircle(GetMouseX(), GetMouseY(), 10, RED);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
