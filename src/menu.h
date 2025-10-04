#ifndef MENU_H
#define MENU_H


#define SLC_NO_LIB_PREFIX
#include "../vendor/slc.h"



typedef struct {
    Vector2 pos;
    
} Menu;

typedef struct {
    Rectangle rec;
    String text;// = string_from_cstr("kasjaksj", arena_ptr);

    // text.size

} Menu;

void menu_init(Menu *self, Vector2 pos);
void menu_update(Menu *self);
void menu_draw(const Menu *self);

#endif // MENU_H