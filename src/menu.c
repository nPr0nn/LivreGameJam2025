#include "../vendor/raylib/raylib.h"
#include "menu.h"
#include "game_context.h"
#include "game.h"

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

Slider slider_init(int x, int y, int width, int height, int r, int g, int b, int a, enum B_Type type, float percentage)
{
    Rectangle rec = (Rectangle){x,y,width,height};
    Color color = (Color){r,g,b,a};
    return (Slider){rec, color, type, percentage};
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

void slider_draw(Slider *self)
{

    DrawRectangleRounded((Rectangle){self->rec.x,self->rec.y, self->rec.width, 3}, 1, 1, self->color);
    DrawRectangleRounded((Rectangle){self->rec.x+1,self->rec.y+1, self->rec.width-2, 1}, 1, 1, BLACK);
    DrawCircle(self->rec.x+self->percentage*self->rec.width,self->rec.y+1, self->rec.height/4, self->color);

}

void au_lib_init(Menu *self)
{
    self->au_lib = (Audios_library){0};
    self->au_lib.background_music = LoadMusicStream("sounds/musica1.mp3");
    self->au_lib.bolha = LoadSound("sounds/bolha.wav");



}

void menu_init(Menu *self, Vector2 pos, Vector2 screen_dim, Vector2 window_dim, Vector2 scaled_screen_dim)
{
    self->buttons[0] = button_init(screen_dim.x/2-20,10,16,16, "", 255,255,255,255, 10, MUSIC, "images/sound.png");
    self->buttons[1] = button_init(screen_dim.x/2-20,30,16,16, "", 255,255,255,255, 10, SOUND_EFFECTS, "images/musicnote.png");
    self->n_buttons = 2;
    
    self->sliders[0] = slider_init(screen_dim.x/2-56,16,32,16, 255,255,255,255, SOUND_EFFECTS, 0.5f);
    self->sliders[1] = slider_init(screen_dim.x/2-56,36,32,16, 255,255,255,255, SOUND_EFFECTS, 0.5f);
    self->n_sliders = 2;
    self->screen_dim = screen_dim;
    self->window_dim = window_dim;
    self->scaled_screen_dim = scaled_screen_dim;
    au_lib_init(self);
    PlayMusicStream(self->au_lib.background_music);
}

int detect_click_button(Button *self, Vector2 screen_dim, Vector2 window_dim, Vector2 scaled_screen_dim)
{
    return (CheckCollisionPointRec(pos_to_texture(GetMousePosition(), screen_dim, window_dim, scaled_screen_dim), self->rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}
int detect_click_slider(Slider *self, Vector2 screen_dim, Vector2 window_dim, Vector2 scaled_screen_dim)
{
    bool result = (CheckCollisionPointRec(pos_to_texture(GetMousePosition(), screen_dim, window_dim, scaled_screen_dim), self->rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
    return result;
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
    for(int i = 0; i<self->n_buttons;i++)
    {
        slider_draw(&self->sliders[i]);
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
void action_slider(Slider *self, Menu *menu)
{

    Vector2 mousePoint = pos_to_texture(GetMousePosition(), menu->screen_dim, menu->window_dim, menu->scaled_screen_dim);
    self->percentage = (mousePoint.x - self->rec.x) / self->rec.width;
    if(self->percentage < 0) self->percentage = 0;
    if(self->percentage > 1) self->percentage = 1;

    switch (self->button_type)
    {
    case MUSIC:
        
        // float volume = 1.0f;
        // if(self->pressed){volume=0.f;}
        // SetMusicVolume(menu->background_music, volume);
        break;
    case SOUND_EFFECTS:
        
        
        break;
    default:
        break;
    }


}

void menu_update(Menu *self, GameContext *g)
{
    for(int i = 0; i<self->n_buttons;i++)
    {
        if(detect_click_button(&self->buttons[i], self->screen_dim, self->window_dim, self->scaled_screen_dim))
        {
            PlaySound(self->au_lib.bolha);
            self->buttons[i].pressed = !self->buttons[i].pressed;
            printf("%d\n", self->buttons[i].button_type);
            action_button(&self->buttons[i], self);
        }
    }
    for(int i = 0; i<self->n_sliders;i++)
    {
        if(detect_click_slider(&self->sliders[i], self->screen_dim, self->window_dim, self->scaled_screen_dim))
        {
            

            printf("%d\n", self->buttons[i].button_type);
            action_slider(&self->sliders[i], self);
        }
    }


}

