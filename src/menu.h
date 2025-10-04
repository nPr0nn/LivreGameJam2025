#ifndef MENU_H
#define MENU_H


#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"

typedef struct GameContext GameContext; 

enum B_Type {
  MUSIC,
  VOLUME_MUSIC,
  SOUND_EFFECTS,
  VOLUME_SOUND_EFFECTS,
  BRIGHT,
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
} Button;

typedef struct {
    Music background_music;
    Sound bolha;
} Audios_library;

typedef struct {
    Vector2 pos;
    int n_buttons;
    Button buttons[100];
    Vector2 screen_dim;
    Vector2 window_dim;
    Audios_library au_lib;

} Menu;


void menu_init(Menu *self, Vector2 pos, Vector2 screen_dim, Vector2 window_dim);
void menu_update(Menu *self, GameContext *g);
void menu_draw(Menu *self);

#endif // MENU_H