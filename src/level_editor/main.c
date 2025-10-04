
#include "editor.h"
#define SLC_IMPL
#include "game_context.h"

typedef void (*void_func_ptr)(void *);
#ifdef PLATFORM_WEB
#include <emscripten.h>
void set_application_loop(void *ctx, void_func_ptr game_loop) {
  emscripten_set_main_loop_arg(game_loop, ctx, -1, 1);
}
#else
void set_application_loop(void *ctx, void_func_ptr game_loop) {
  while (!WindowShouldClose()) {
    game_loop(ctx);
  }
}
#endif

i32 main(void) {

  GameContext game;
  MemArena global_arena = {0};
  MemArena frame_arena = {0};
  game.g_arena = &global_arena;
  game.f_arena = &frame_arena;

  game_init(&game);
  set_application_loop(&game, game_loop);
  game_exit(&game);

  mem_arena_free(&global_arena);
  mem_arena_free(&frame_arena);

  return 0;
}
