#ifndef MENU_H
#define MENU_H



typedef struct {
    Vector2 pos;
} Menu;

void menu_init(Menu *self, Vector2 pos);
void menu_update(Menu *self);
void menu_draw(const Menu *self);

#endif // MENU_H