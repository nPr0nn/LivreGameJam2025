#include "../vendor/raylib/raylib.h"
#include "menu.h"
#include "game_context.h"

Button button_init(int x, int y, int width, int height, char * text, int r, int g, int b, int a, int text_size, enum B_Type type)
{
    Rectangle rec = (Rectangle){x,y,width,height};
    Color color = (Color){r,g,b,a};
    Button but = (Button){rec, text, color, text_size, type};
    return but;
}

void button_draw(Button *self)
{
DrawRectangleRoundedLinesEx(self->rec, 0.1, 0, 10, (Color){255,255,255,255});
DrawRectangleRounded(self->rec, 0.1, 10, self->color);
DrawText(self->text, self->rec.x + self->rec.width/2 - MeasureText(self->text, 20)/2, self->rec.y + self->rec.height/2-self->text_size/2, self->text_size, LIGHTGRAY);

}

void menu_init(Menu *self, Vector2 pos)
{
    self->buttons[0] = button_init(GetScreenWidth()/2-50,50,100,50, "Olá mundo!", 255,255,255,255, 20, DUMMY);
    self->buttons[1] = button_init(GetScreenWidth()/2-50,150,100,50, "Música!", 255,255,255,255, 20, MUSIC);
    self->n_buttons = 2;
}

int detect_click(Button *self)
{
    Vector2 mousePoint = GetMousePosition();
    // printf("pontes: %lf %lf\n", mousePoint.x, mousePoint.y);
    
    return (CheckCollisionPointRec(mousePoint, self->rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
}



void menu_draw(Menu *self)
{
Rectangle rec = (Rectangle){10, 10, GetScreenWidth()-20,GetScreenHeight()-20};
Color color = (Color){255,255,255,255};
DrawRectangleRoundedLinesEx(rec, 0.05, 0, 10, color);
DrawRectangleRounded(rec, 0.05, 10, (Color){100,100,100,230});

    for(int i = 0; i<self->n_buttons;i++)
    {
        button_draw(&self->buttons[i]);
    }


}


void menu_update(Menu *self, GameContext *g)
{
    for(int i = 0; i<self->n_buttons;i++)
    {
        if(detect_click(&self->buttons[i]))
        {
            
            printf("clique\n");
        }
    }


}

