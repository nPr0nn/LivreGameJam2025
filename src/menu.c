#include "../vendor/raylib/raylib.h"
#include "menu.h"
#include "game_context.h"

Button button_init(int x, int y, int width, int height, char * text, int r, int g, int b, int a, int text_size, enum B_Type type, char *img_path)
{
    Rectangle rec = (Rectangle){x,y,width,height};
    Color color = (Color){r,g,b,a};

    //imagem
    Image sprite_sheet_image = LoadImage(img_path);
    Texture sprite_sheet = LoadTextureFromImage(sprite_sheet_image);
    UnloadImage(sprite_sheet_image);

    //passa tudo pro botao
    Button but = (Button){rec, text, color, text_size, type, sprite_sheet, false};
    return but;
}

void button_draw(Button *self)
{
DrawRectangleRoundedLinesEx(self->rec, 0.1, 0, 1, (Color){255,255,255,255});
DrawRectangleRounded(self->rec, 0.1, 1, self->color);
DrawText(self->text, self->rec.x + self->rec.width/2 - MeasureText(self->text, self->text_size)/2, self->rec.y + self->rec.height/2-self->text_size/2, self->text_size, LIGHTGRAY);
Color color = WHITE;
if(self->pressed)
    color = GRAY;
DrawTextureRec(self->sprite_sheet, (Rectangle){0,0,16,16}, (Vector2){self->rec.x, self->rec.y}, color);
}

void au_lib_init(Menu *self)
{
    self->au_lib = (Audios_library){0};
    self->au_lib.background_music = LoadMusicStream("sounds/musica1.mp3");
    self->au_lib.bolha = LoadSound("sounds/bolha.wav");



}

void menu_init(Menu *self, Vector2 pos, Vector2 screen_dim, Vector2 window_dim)
{
    self->buttons[0] = button_init(screen_dim.x/2-20,10,16,16, "", 255,255,255,255, 10, MUSIC, "images/sound.png");
    self->buttons[1] = button_init(screen_dim.x/2-20,30,16,16, "", 255,255,255,255, 10, SOUND_EFFECTS, "images/musicnote.png");
    self->n_buttons = 2;
    self->screen_dim = screen_dim;
    self->window_dim = window_dim;
    au_lib_init(self);
    PlayMusicStream(self->au_lib.background_music);
}

int detect_click(Button *self, Vector2 screen_dim, Vector2 window_dim)
{
    Vector2 mousePoint = GetMousePosition();
    mousePoint.x = (mousePoint.x/window_dim.x) * screen_dim.x;
    mousePoint.y = (mousePoint.y/window_dim.y) * screen_dim.y;
    return (CheckCollisionPointRec(mousePoint, self->rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}



void menu_draw(Menu *self)
{
Rectangle rec = (Rectangle){5, 5, self->screen_dim.x-10,self->screen_dim.y-10};
Color color = (Color){255,255,255,255};
DrawRectangleRoundedLinesEx(rec, 0.05, 0, 2, color);
DrawRectangleRounded(rec, 0.05, 1, (Color){100,100,100,230});

    for(int i = 0; i<self->n_buttons;i++)
    {
        button_draw(&self->buttons[i]);
    }

// Draw pause message if paused
DrawText("Game Paused", self->screen_dim.x/2-30, self->screen_dim.y-30, 10, RED);
DrawText("Press P to Resume", self->screen_dim.x/2-30-15, self->screen_dim.y-20, 10, LIGHTGRAY);


}

void define_all_sounds_volume(Menu *menu, float volume)
{
    SetSoundVolume(menu->au_lib.bolha, volume);

}

void action_button(Button *self, Menu *menu)
{
    switch (self->button_type)
    {
    case MUSIC:
        if(self->pressed){PauseMusicStream(menu->au_lib.background_music);}
        else{ResumeMusicStream(menu->au_lib.background_music);}
        
        // float volume = 1.0f;
        // if(self->pressed){volume=0.f;}
        // SetMusicVolume(menu->background_music, volume);
        break;
    case SOUND_EFFECTS:
        if(self->pressed){define_all_sounds_volume(menu,0.0f);}
        else{define_all_sounds_volume(menu, 1.0f);}
        
        break;
    default:
        break;
    }


}

void menu_update(Menu *self, GameContext *g)
{
    for(int i = 0; i<self->n_buttons;i++)
    {
        if(detect_click(&self->buttons[i], self->screen_dim, self->window_dim))
        {
            PlaySound(self->au_lib.bolha);
            self->buttons[i].pressed = !self->buttons[i].pressed;
            printf("%d\n", self->buttons[i].button_type);
            action_button(&self->buttons[i], self);
        }
    }


}

