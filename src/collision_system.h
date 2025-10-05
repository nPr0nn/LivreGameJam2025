#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../vendor/raylib/raylib.h"
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
  // ✅ Convert from center-based bbox to min/max space
  float half_w = bbox->width / 2.0f;
  float half_h = bbox->height / 2.0f;

  float left = bbox->x - half_w;
  float right = bbox->x + half_w;
  float top = bbox->y - half_h;
  float bottom = bbox->y + half_h;

  Vector2 inv_dir = {1.0f / mov->direction.x, 1.0f / mov->direction.y};

  Vector2 t_near = {(left - mov->origin.x) * inv_dir.x,
                    (top - mov->origin.y) * inv_dir.y};
  Vector2 t_far = {(right - mov->origin.x) * inv_dir.x,
                   (bottom - mov->origin.y) * inv_dir.y};

  if (isnan(t_near.x) || isnan(t_near.y) || isnan(t_far.x) || isnan(t_far.y))
    return false;

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

  if (t_near.x > t_far.y || t_near.y > t_far.x)
    return false;

  float t_hit_near = fmaxf(t_near.x, t_near.y);
  float t_hit_far = fminf(t_far.x, t_far.y);

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
      collision_info->contact_normal = (Vector2){0, 0};
  }

  return true;
}

static inline bool check_collision_entity_bbox(Ray2D *mov, Rectangle *bbox_mov,
                                               Rectangle *bbox_fixed,
                                               CollisionInfo *collision_info) {
  // This is the expanded target (Minkowski sum)
  Rectangle expanded_target = {bbox_fixed->x, bbox_fixed->y,
                               bbox_fixed->width + bbox_mov->width,
                               bbox_fixed->height + bbox_mov->height};

  // The key change is to treat the player as a point (its center) and check
  // collision against the expanded (Minkowski) rectangle.
  if (check_collision_ray_bbox(mov, &expanded_target, collision_info)) {
    // Ensure the collision happens within this frame's movement (0 <= t < 1)
    return (collision_info->t_hit >= 0.0f && collision_info->t_hit < 1.0f);
  }

  return false;
}

// static inline void resolve_collisions_entity_bbox(
//     Rectangle *entity_bbox, Vector2 *pos, Vector2 *vel,
//     Rectangle *static_colliders, int collider_count, float dt, void *entity,
//     void *user_data, CollisionDetectedCallback on_detected,
//     CollisionResolveCallback on_resolve) {
//
//   // We might collide multiple times, so we iterate to resolve complex cases
//   // (like sliding into a corner).
//   for (int i = 0; i < 4; i++) {
//     // 1. Find the EARLIEST collision for this frame's movement
//     Ray2D movement_ray = {*pos, (Vector2){vel->x * dt, vel->y * dt}};
//
//     CollisionInfo nearest_collision = {
//         .t_hit = 1.0f}; // Assume no collision initially
//     int nearest_collider_index = -1;
//     bool did_collide = false;
//
//     for (int j = 0; j < collider_count; j++) {
//       CollisionInfo current_collision;
//       if (check_collision_entity_bbox(&movement_ray, entity_bbox,
//                                       &static_colliders[j],
//                                       &current_collision)) {
//         if (current_collision.t_hit < nearest_collision.t_hit) {
//           nearest_collision = current_collision;
//           nearest_collider_index = j;
//           did_collide = true;
//         }
//       }
//     }
//
//     // 2. If a collision was found, trigger the callbacks
//     if (did_collide) {
//       // A. Trigger the simple "detection" callback if it exists
//       if (on_detected) {
//         on_detected(entity, &nearest_collision, user_data);
//       }
//
//       // B. Trigger the "resolution" callback to handle the physics
//       if (on_resolve) {
//         on_resolve(entity, &nearest_collision, pos, vel, dt, user_data);
//       }
//     } else {
//       // If no collision was found in this iteration, we can stop.
//       break;
//     }
//   }
// }

#endif // COLLISION_SYSTEM_H
