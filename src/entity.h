#ifndef ENTITY_H
#define ENTITY_H

#include "../vendor/raylib/raylib.h"

enum EntityType { PLAYER, STATIC_BOX, BASIC_ENEMY, COW };

typedef struct Entity {
  Vector2 pos;
  Vector2 vel;
  Vector2 acc;
  Rectangle bbox;

  void *owner;

  int id;
  enum EntityType type;
} Entity;

#endif
