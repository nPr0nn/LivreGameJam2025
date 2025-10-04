#include "../vendor/raylib/raylib.h"
#include "menu.h"

void menu_draw(const Menu *self)
{
Rectangle rec = (Rectangle){10, 10, 10, 100};
Color color = (Color){255,255,255,255};
DrawRectangleRoundedLinesEx(rec, 1, 1, 1, color);

}