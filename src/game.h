#ifndef GAME_H
#define GAME_H
#include "../vendor/raylib/raylib.h"

void game_init(void *ctx);
void game_update(void *ctx);
void game_draw(void *ctx);
void game_loop(void *ctx);
void game_exit(void *ctx);
Vector2 pos_to_texture(Vector2 pos, Vector2 screen_dim, Vector2 window_dim, Vector2 scaled_screen_dim);

#endif
