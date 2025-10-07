#include "menu.h"
#include "game.h"
#include "game_context.h"

Button button_init(int x, int y, int width, int height, char *text, int r,
                   int g, int b, int a, int text_size, enum B_Type type,
                   char *img_path) {
  Rectangle rec = (Rectangle){x, y, width, height};
  Color color = (Color){r, g, b, a};

  // imagem
  Image sprite_sheet_image = LoadImage(img_path);
  Texture sprite_sheet = LoadTextureFromImage(sprite_sheet_image);
  UnloadImage(sprite_sheet_image);

  // passa tudo pro botao
  Button but = (Button){rec,  text,         color, text_size,
                        type, sprite_sheet, false, (Color){255, 255, 255, 255}};
  return but;
}

Slider slider_init(int x, int y, int width, int height, int r, int g, int b,
                   int a, enum B_Type type, float percentage) {
  Rectangle rec = (Rectangle){x, y, width, height};
  Color color = (Color){r, g, b, a};
  return (Slider){rec, color, type, percentage};
}

void button_draw(Button *self) {
  DrawRectangleRoundedLinesEx(self->rec, 0.1, 0, 1,
                              (Color){255, 255, 255, 255});
  DrawRectangleRounded(self->rec, 0.1, 1, self->color);
  DrawText(self->text,
           self->rec.x + self->rec.width / 2 -
               MeasureText(self->text, self->text_size) / 2,
           self->rec.y + self->rec.height / 2 - self->text_size / 2,
           self->text_size, LIGHTGRAY);
  Color color = self->bright;
  if (self->pressed)
    color = GRAY;
  DrawTextureRec(self->sprite_sheet, (Rectangle){0, 0, 16, 16},
                 (Vector2){self->rec.x, self->rec.y}, color);
}

void slider_draw(Slider *self) {

  DrawRectangleRounded(
      (Rectangle){self->rec.x, self->rec.y, self->rec.width, 3}, 1, 1,
      self->color);
  DrawRectangleRounded(
      (Rectangle){self->rec.x + 1, self->rec.y + 1, self->rec.width - 2, 1}, 1,
      1, BLACK);
  DrawCircle(self->rec.x + self->percentage * self->rec.width, self->rec.y + 1,
             self->rec.height / 4, self->color);
}

void au_lib_init(Menu *self) {
  self->au_lib = (Audios_library){0};
  self->au_lib.background_music = LoadMusicStream("sounds/musica1.mp3");
  self->au_lib.start_music = LoadMusicStream("sounds/musica_start.mp3");
  self->au_lib.bolha = LoadSound("sounds/bolha.wav");
}

void menu_init(Menu *self, Vector2 pos, Vector2 screen_dim, Vector2 window_dim,
               Vector2 scaled_screen_dim, GameContext *game) {
  self->game = game;
  self->moving_slider = -1;
  self->buttons[0] = button_init(screen_dim.x - 35, 10, 16, 16, "", 255, 255,
                                 255, 255, 10, MUSIC, "images/musicnote.png");
  self->buttons[1] =
      button_init(screen_dim.x - 35, 30, 16, 16, "", 255, 255, 255, 255, 10,
                  SOUND_EFFECTS, "images/sound.png");
  self->buttons[2] = button_init(screen_dim.x - 35, 50, 16, 16, "", 255, 255,
                                 255, 255, 10, BRIGHT, "images/lampada.png");
  self->buttons[3] = button_init(screen_dim.x - 35, 70, 16, 16, "", 255, 255,
                                 255, 255, 10, EXIT, "images/x.png");
  self->n_buttons = 4;

  self->sliders[0] =
      slider_init(15, 16, 100, 16, 255, 255, 255, 255, VOLUME_MUSIC, 0.5f);
  self->sliders[1] = slider_init(15, 36, 100, 16, 255, 255, 255, 255,
                                 VOLUME_SOUND_EFFECTS, 0.5f);
  self->sliders[2] =
      slider_init(15, 56, 100, 16, 255, 255, 255, 255, BRIGHT_LEVEL, 0.5f);
  self->n_sliders = 3;
  self->screen_dim = screen_dim;
  self->window_dim = window_dim;
  self->scaled_screen_dim = scaled_screen_dim;
  self->gamma = 1.0f;
  au_lib_init(self);
  PlayMusicStream(self->au_lib.start_music);

  Image start_image = LoadImage("images/telainicio.png");
  self->start_texture = LoadTextureFromImage(start_image);
  UnloadImage(start_image);
  Image lose_image = LoadImage("images/gameover.png");
  self->lose_texture = LoadTextureFromImage(lose_image);
  UnloadImage(lose_image);
}

int detect_click_button(Button *self, Vector2 screen_dim, Vector2 window_dim,
                        Vector2 scaled_screen_dim) {
  return (CheckCollisionPointRec(pos_to_texture(GetMousePosition(), screen_dim,
                                                window_dim, scaled_screen_dim),
                                 self->rec) &&
          IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}
int detect_click_slider(Slider *self, Vector2 screen_dim, Vector2 window_dim,
                        Vector2 scaled_screen_dim) {
  bool result =
      (CheckCollisionPointRec(pos_to_texture(GetMousePosition(), screen_dim,
                                             window_dim, scaled_screen_dim),
                              self->rec) &&
       IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
  return result;
}
int hover_button(Button *self, Vector2 screen_dim, Vector2 window_dim,
                 Vector2 scaled_screen_dim) {
  return (CheckCollisionPointRec(pos_to_texture(GetMousePosition(), screen_dim,
                                                window_dim, scaled_screen_dim),
                                 self->rec));
}

void menu_draw(Menu *self) {
  switch (self->game->stage) {
  case PAUSED:
    DrawRectangle(
        0, 0, self->screen_dim.x, self->screen_dim.y,
        (Color){0, 0, 0, 100}); // deixa o fundo mais escuro quando pausado
    Rectangle rec =
        (Rectangle){5, 5, self->screen_dim.x - 10, self->screen_dim.y - 10};
    Color color = (Color){255, 255, 255, 255};
    DrawRectangleRoundedLinesEx(rec, 0.05, 0, 2, color);
    DrawRectangleRounded(rec, 0.05, 1, (Color){100, 100, 100, 50});

    for (int i = 0; i < self->n_buttons; i++) {
      button_draw(&self->buttons[i]);
    }
    for (int i = 0; i < self->n_buttons; i++) {
      slider_draw(&self->sliders[i]);
    }

    // Exit
    DrawText("Exit game", self->buttons[3].rec.x - 57,
             self->buttons[3].rec.y + 3, 10, RED);

    // Draw pause message if paused
    DrawText("Game Paused", self->screen_dim.x / 2 - 30,
             self->screen_dim.y - 30, 10, RED);
    DrawText("Press P to Resume", self->screen_dim.x / 2 - 30 - 15,
             self->screen_dim.y - 20, 10, LIGHTGRAY);
    break;

  case START:
    DrawTextureRec(self->start_texture,
                   (Rectangle){0, 0, self->screen_dim.x, self->screen_dim.y},
                   (Vector2){0.f, 0.f}, (Color){255, 255, 255, 255});
    DrawText("Voaqueiro", 10, 10, 29, RED);
    Vector2 posStart =
        (Vector2){self->screen_dim.x / 2 - 10, self->screen_dim.y - 18};
    Vector2 tamanho = (Vector2){32, 12};
    DrawText("Start", posStart.x + 2, posStart.y + 2, 10, LIGHTGRAY);

    Color cor_botao_start = (Color){100, 100, 100, 200};
    if (CheckCollisionPointRec(
            pos_to_texture(GetMousePosition(), self->screen_dim,
                           self->window_dim, self->scaled_screen_dim),
            (Rectangle){posStart.x, posStart.y, tamanho.x, tamanho.y})) {
      cor_botao_start = (Color){150, 150, 150, 200};
    }
    DrawRectangleRounded(
        (Rectangle){posStart.x, posStart.y, tamanho.x, tamanho.y}, 0.1, 1,
        cor_botao_start);
    DrawRectangleRoundedLinesEx(
        (Rectangle){posStart.x, posStart.y, tamanho.x, tamanho.y}, 0.1, 0, 1,
        (Color){255, 255, 255, 255});
    break;

  case LOSE:

    DrawTextureRec(self->lose_texture,
                   (Rectangle){0, 0, self->screen_dim.x, self->screen_dim.y},
                   (Vector2){0.f, 0.f}, (Color){255, 255, 255, 255});
    DrawText("Você perdeu :( ", 10, 10, 18, RED);
    DrawText("Aperte C \n para recomecar", 20, 70, 10, LIGHTGRAY);
    break;
  case WIN:

    DrawTextureRec(self->start_texture,
                   (Rectangle){0, 0, self->screen_dim.x, self->screen_dim.y},
                   (Vector2){0.f, 0.f}, (Color){255, 255, 255, 255});
    DrawText("Você ganhou! :) ", 10, 10, 18, GREEN);
    DrawText("Aperte C \n para recomecar", 20, 70, 10, LIGHTGRAY);
    break;
  default:
    break;
  }
}

void define_all_sounds_volume(Menu *menu, float volume) {
  SetSoundVolume(menu->au_lib.bolha, volume);
}

void action_button(Button *self, Menu *menu) {
  switch (self->button_type) {
  case MUSIC:
    if (self->pressed) {
      PauseMusicStream(menu->au_lib.background_music);
      PauseMusicStream(menu->au_lib.start_music);
    } else {
      if (menu->game->stage == START)
        ResumeMusicStream(menu->au_lib.start_music);
      if (menu->game->stage == RUNNING || menu->game->stage == PAUSED) {
        ResumeMusicStream(menu->au_lib.background_music);
      }
    }

    // float volume = 1.0f;
    // if(self->pressed){volume=0.f;}
    // SetMusicVolume(menu->background_music, volume);
    break;
  case SOUND_EFFECTS:
    if (self->pressed) {
      define_all_sounds_volume(menu, 0.0f);
    } else {
      define_all_sounds_volume(menu, 1.0f);
    }

    break;
  case BRIGHT:
    if (self->pressed) {
      menu->gamma = 0.5f;
    } else {
      menu->gamma = menu->sliders[2].percentage;
    }

    break;
  case EXIT:
    menu->game->is_running = false;

    break;
  default:
    break;
  }
}

void action_slider(Slider *self, Menu *menu) {
  Vector2 mousePoint =
      pos_to_texture(GetMousePosition(), menu->screen_dim, menu->window_dim,
                     menu->scaled_screen_dim);
  self->percentage = (mousePoint.x - self->rec.x) / self->rec.width;
  if (self->percentage < 0)
    self->percentage = 0;
  if (self->percentage > 1)
    self->percentage = 1;

  switch (self->button_type) {
  case VOLUME_MUSIC:

    SetMusicVolume(menu->au_lib.background_music, self->percentage);
    break;
  case VOLUME_SOUND_EFFECTS:
    if (!menu->buttons[1].pressed) {
      define_all_sounds_volume(menu, self->percentage);
    }

    break;
  case BRIGHT_LEVEL:
    if (!menu->buttons[2].pressed) {
      menu->gamma = self->percentage;
    }
    break;
  default:
    break;
  }
}

void menu_update(Menu *self, GameContext *g) {
  switch (self->game->stage) {
  case PAUSED:
    for (int i = 0; i < self->n_buttons; i++) {
      if (hover_button(&self->buttons[i], self->screen_dim, self->window_dim,
                       self->scaled_screen_dim)) {
        self->buttons[i].bright = (Color){230, 230, 230, 255};
      } else {
        self->buttons[i].bright = (Color){255, 255, 255, 255};
      }
      if (detect_click_button(&self->buttons[i], self->screen_dim,
                              self->window_dim, self->scaled_screen_dim)) {
        PlaySound(self->au_lib.bolha);
        self->buttons[i].pressed = !self->buttons[i].pressed;
        printf("%d\n", self->buttons[i].button_type);
        action_button(&self->buttons[i], self);
      }
    }
    for (int i = 0; i < self->n_sliders; i++) {
      if (detect_click_slider(&self->sliders[i], self->screen_dim,
                              self->window_dim, self->scaled_screen_dim)) {
        self->moving_slider = i;
        PlaySound(self->au_lib.bolha);
      }

      if (self->moving_slider == i) {
        action_slider(&self->sliders[i], self);
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
          self->moving_slider = -1;
        }
      }
    }
    break;

  case START:
    if ((GetKeyPressed() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
         IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))) {
      self->game->stage = RUNNING; // RUNNING
      StopMusicStream(self->au_lib.start_music);
      PlayMusicStream(self->au_lib.background_music);
    }
    break;
  case LOSE:
    if (IsKeyPressed(KEY_C)) {
      self->game->stage = RESETING;
    }
    break;

  case WIN:
    if (IsKeyPressed(KEY_C)) {
      self->game->stage = START;
      self->game->progression = 1;
    }
    break;

  default:
    break;
  }
}
