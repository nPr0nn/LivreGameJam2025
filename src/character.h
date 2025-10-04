#ifndef CHARACTER_H
#define CHARACTER_H

typedef struct {
    Vector2 pos;
    float radius;
    Color color;
} Character;

void character_init(Character *ch, Vector2 start_pos, float radius, Color color);
void character_update(Character *ch);
void character_draw(const Character *ch);

#endif // CHARACTER_H