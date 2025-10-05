#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../vendor/raylib/raylib.h"
#include "entity.h"
#include "level_loader.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct CollisionPair {
  float t_hit;
  int index;
} CollisionPair;

typedef struct Ray2D {
  Vector2 origin;
  Vector2 direction;
} Ray2D;

typedef struct CollisionInfo {
  Vector2 contact_point;
  Vector2 contact_normal;
  const char *type;
  i32 id;
  float t_hit;
} CollisionInfo;

static inline int collision_pair_comp(const void *a, const void *b) {
  float val1 = ((const CollisionPair *)a)->t_hit;
  float val2 = ((const CollisionPair *)b)->t_hit;
  return (val1 > val2) - (val1 < val2);
}

static inline void print_rect(Rectangle *rect) {
  printf("x: %f ", rect->x);
  printf("y: %f ", rect->y);
  printf("w: %f ", rect->width);
  printf("h: %f \n\n", rect->height);
}

static inline bool check_collision_ray_bbox(Ray2D *mov, Rectangle *bbox,
                                            CollisionInfo *collision_info) {
  float left = bbox->x;
  float right = bbox->x + bbox->width;
  float top = bbox->y;
  float bottom = bbox->y + bbox->height;

  // Handle cases where ray direction is zero to avoid division by zero.
  if (mov->direction.x == 0 && (mov->origin.x < left || mov->origin.x > right))
    return false;
  if (mov->direction.y == 0 && (mov->origin.y < top || mov->origin.y > bottom))
    return false;

  Vector2 inv_dir = {1.0f / mov->direction.x, 1.0f / mov->direction.y};

  Vector2 t_near = {(left - mov->origin.x) * inv_dir.x,
                    (top - mov->origin.y) * inv_dir.y};
  Vector2 t_far = {(right - mov->origin.x) * inv_dir.x,
                   (bottom - mov->origin.y) * inv_dir.y};

  if (isnan(t_far.y) || isnan(t_far.x))
    return false;
  if (isnan(t_near.y) || isnan(t_near.x))
    return false;

  // Sort distances
  if (t_near.x > t_far.x) {
    float tmp = t_near.x;
    t_near.x = t_far.x;
    t_far.x = tmp;
  }
  if (t_near.y > t_far.y) {
    float tmp = t_near.y;
    t_near.y = t_far.y;
    t_far.y = tmp;
  }

  // Early exit if the ray misses the box
  if (t_near.x > t_far.y || t_near.y > t_far.x)
    return false;

  // The time of the nearest intersection is the furthest of the near times.
  float t_hit_near = fmaxf(t_near.x, t_near.y);
  // The time of the furthest intersection is the nearest of the far times.
  float t_hit_far = fminf(t_far.x, t_far.y);

  // If the intersection is behind the ray's origin, there's no collision.
  if (t_hit_far < 0)
    return false;

  if (collision_info) {
    collision_info->t_hit = t_hit_near;
    collision_info->contact_point.x =
        mov->origin.x + t_hit_near * mov->direction.x;
    collision_info->contact_point.y =
        mov->origin.y + t_hit_near * mov->direction.y;

    if (t_near.x > t_near.y)
      collision_info->contact_normal = (Vector2){inv_dir.x < 0 ? 1 : -1, 0};
    else if (t_near.x < t_near.y)
      collision_info->contact_normal = (Vector2){0, inv_dir.y < 0 ? 1 : -1};
    else
      // This case happens when hitting a corner perfectly. Can be handled as
      // needed.
      collision_info->contact_normal = (Vector2){0, 0};
  }

  return true;
}

static inline bool check_collision_entity_bbox(Ray2D *mov, Rectangle *bbox_mov,
                                               Rectangle *bbox_fixed,
                                               CollisionInfo *collision_info) {
  // To check for collision between two moving rectangles (AABBs), we can
  // simplify the problem by shrinking the moving box to a point (its top-left
  // corner) and expanding the static box by the dimensions of the moving box.
  // This is known as using the Minkowski sum.

  Rectangle expanded_target = {bbox_fixed->x - bbox_mov->width,
                               bbox_fixed->y - bbox_mov->height,
                               bbox_fixed->width + bbox_mov->width,
                               bbox_fixed->height + bbox_mov->height};

  // The ray's origin is the top-left of the moving box. We check if this ray
  // intersects the expanded static box.
  if (check_collision_ray_bbox(mov, &expanded_target, collision_info)) {
    // A collision is valid only if it happens within the current frame's
    // movement, which corresponds to a time `t` between 0 and 1.
    return (collision_info->t_hit >= 0.0f && collision_info->t_hit < 1.0f);
  }

  return false;
}

typedef void (*on_collision_callback)(void *entity_owner,
                                      const CollisionInfo *collision_info,
                                      float dt);

static inline void
run_collisions_on_entity(Entity *entity, t_Collision *static_colliders,
                         int collider_count, float dt,
                         on_collision_callback on_collision) {

  // We might collide multiple times, so we iterate to resolve complex cases
  // (like sliding into a corner).
  for (int i = 0; i < 4; i++) {
    // 1. Find the EARLIEST collision for this frame's movement
    Ray2D movement_ray = {entity->pos,
                          (Vector2){entity->vel.x * dt, entity->vel.y * dt}};

    CollisionInfo nearest_collision = {
        .t_hit = 1.0f}; // Assume no collision initially
    int nearest_collider_index = -1;
    bool did_collide = false;

    for (int j = 0; j < collider_count; j++) {
      CollisionInfo current_collision;
      Rectangle rec = {(f32)static_colliders[j].x, (f32)static_colliders[j].y,
                       (f32)static_colliders[j].w, (f32)static_colliders[j].h};
      if (check_collision_entity_bbox(&movement_ray, &entity->bbox, &rec,
                                      &current_collision)) {
        if (current_collision.t_hit < nearest_collision.t_hit) {
          current_collision.type = static_colliders[j].type;
          current_collision.id = static_colliders[j].id;
          nearest_collision = current_collision;
          nearest_collider_index = j;
          did_collide = true;
        }
      }
    }

    // 2. If a collision was found, trigger the callbacks
    if (did_collide) {
      // Trigger the callback to handle how entity react to collision
      if (on_collision) {
        on_collision(entity->owner, &nearest_collision, dt);
      }
    } else {
      // If no collision was found in this iteration, we can stop.
      break;
    }
  }
}

#endif // COLLISION_SYSTEM_H
