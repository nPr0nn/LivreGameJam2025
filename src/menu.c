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
    Button but = (Button){rec, text, color, text_size, type, sprite_sheet};
    return but;
}

void button_draw(Button *self)
{
DrawRectangleRoundedLinesEx(self->rec, 0.1, 0, 1, (Color){255,255,255,255});
DrawRectangleRounded(self->rec, 0.1, 1, self->color);
DrawText(self->text, self->rec.x + self->rec.width/2 - MeasureText(self->text, self->text_size)/2, self->rec.y + self->rec.height/2-self->text_size/2, self->text_size, LIGHTGRAY);
DrawTextureRec(self->sprite_sheet, source_rec, ch->pos, WHITE);
}

void menu_init(Menu *self, Vector2 pos, Vector2 screen_dim, Vector2 window_dim)
{
    // self->buttons[0] = button_init(screen_dim.x/2-20,10,40,15, "Olá mundo!", 255,255,255,255, 10, DUMMY, NULL);
    self->buttons[1] = button_init(screen_dim.x/2-20,30,40,15, "Música!", 255,255,255,255, 10, MUSIC, "music");
    self->n_buttons = 2;
    self->screen_dim = screen_dim;
    self->window_dim = window_dim;
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
Rectangle rec = (Rectangle){0, 0, 160,144};
Color color = (Color){255,255,255,255};
DrawRectangleRoundedLinesEx(rec, 0.05, 0, 1, color);
DrawRectangleRounded(rec, 0.05, 1, (Color){100,100,100,230});

    for(int i = 0; i<self->n_buttons;i++)
    {
        button_draw(&self->buttons[i]);
    }


}


void menu_update(Menu *self, GameContext *g)
{
    for(int i = 0; i<self->n_buttons;i++)
    {
        if(detect_click(&self->buttons[i], self->screen_dim, self->window_dim))
        {

            printf("%d\n", self->buttons[i].button_type);
        }
    }


}

