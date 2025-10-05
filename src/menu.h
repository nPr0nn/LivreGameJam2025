#ifndef MENU_H
#define MENU_H

#include "../vendor/raylib/raylib.h"

#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

typedef struct GameContext GameContext;

enum B_Type {
  MUSIC,
  VOLUME_MUSIC,
  SOUND_EFFECTS,
  VOLUME_SOUND_EFFECTS,
  BRIGHT,
  BRIGHT_LEVEL,
  EXIT,
  DUMMY,
};

typedef struct {
  Rectangle rec;
  char *text;
  Color color;
  int text_size;
  enum B_Type button_type;
  Texture sprite_sheet;
  bool pressed;
  Color bright;
} Button;

typedef struct {
  Rectangle rec;
  Color color;
  enum B_Type button_type;
  float percentage;
} Slider;

typedef struct {
  Music background_music;
  Music start_music;
  Sound bolha;
} Audios_library;

typedef struct {
    Vector2 pos;
    int n_buttons;
    Button buttons[10];
    int n_sliders;
    Slider sliders[10];
    Vector2 screen_dim;
    Vector2 window_dim;
    Vector2 scaled_screen_dim;
    Audios_library au_lib;
    float gamma;
    int moving_slider;
    GameContext *game;
    Texture start_texture;
    Texture lose_texture;

} Menu;

void menu_init(Menu *self, Vector2 pos, Vector2 screen_dim, Vector2 window_dim,
               Vector2 scaled_screen_dim, GameContext *game);
void menu_update(Menu *self, GameContext *g);
void menu_draw(Menu *self);

#endif // MENU_H
