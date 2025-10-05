#ifndef PARTICLE_H
#define PARTICLE_H

#define SLC_NO_LIB_PREFIX
#include "../vendor/raylib/raylib.h"
#include "../vendor/raylib/raymath.h"
#include "../vendor/slc.h"
#include <stdlib.h> // For rand()

typedef enum { PARTICLE_MODE_FADE, PARTICLE_MODE_INTERPOLATE } ParticleMode;

// The "blueprint" for a particle remains unchanged.
typedef struct ParticleDefinition {
  Vector2 pos;
  Vector2 vel;
  Color color;
  Color start_color;
  Color end_color;
  float radius;
  float lifetime;
} ParticleDefinition;

// REFACTOR: The Particle struct now composes a ParticleDefinition.
// It holds the blueprint of its properties, plus its own unique state.
typedef struct Particle {
  ParticleDefinition def; // All descriptive data for this particle
  float initial_life;     // Stored at birth for fade/lerp calculations
  bool is_active;
  ParticleMode mode;
} Particle;

// The manager for all particles.
typedef struct ParticleSystem {
  Particle *particles;
  int max_particles;
  int next_particle_idx;
} ParticleSystem;

// --- Helper Functions (unchanged) ---

static inline Color lerp_color(Color a, Color b, float t) {
  if (t < 0.0f)
    t = 0.0f;
  if (t > 1.0f)
    t = 1.0f;
  return (Color){
      .r = (unsigned char)(a.r + (b.r - a.r) * t),
      .g = (unsigned char)(a.g + (b.g - a.g) * t),
      .b = (unsigned char)(a.b + (b.b - a.b) * t),
      .a = (unsigned char)(a.a + (b.a - a.a) * t),
  };
}

static inline float get_random_float(float min, float max) {
  return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

static inline Particle *find_next_inactive_particle(ParticleSystem *ps) {
  for (int i = 0; i < ps->max_particles; ++i) {
    int looped_idx = (ps->next_particle_idx + i) % ps->max_particles;
    if (!ps->particles[looped_idx].is_active) {
      ps->next_particle_idx = (looped_idx + 1) % ps->max_particles;
      return &ps->particles[looped_idx];
    }
  }
  return NULL;
}

// --- Core System Functions ---

static inline ParticleSystem *particle_system_create(slc_MemArena *arena,
                                                     int max_particles) {
  ParticleSystem *ps =
      (ParticleSystem *)mem_arena_alloc(arena, sizeof(ParticleSystem));
  if (!ps)
    return NULL;

  ps->max_particles = max_particles;
  ps->next_particle_idx = 0;
  ps->particles =
      (Particle *)mem_arena_alloc(arena, sizeof(Particle) * max_particles);
  if (!ps->particles)
    return NULL;

  for (int i = 0; i < max_particles; ++i) {
    ps->particles[i].is_active = false;
  }
  return ps;
}

static inline void particle_system_update(ParticleSystem *ps, float dt) {
  for (int i = 0; i < ps->max_particles; ++i) {
    Particle *p = &ps->particles[i];
    if (!p->is_active)
      continue;

    // REFACTOR: Access data through the 'def' member
    p->def.lifetime -= dt;
    if (p->def.lifetime <= 0) {
      p->is_active = false;
      continue;
    }

    // REFACTOR: Update position and velocity through 'def'
    p->def.pos = Vector2Add(p->def.pos, Vector2Scale(p->def.vel, dt));
    p->def.vel.y += 20.0f * dt; // Simple gravity
  }
}

static inline void particle_system_draw(const ParticleSystem *ps) {
  for (int i = 0; i < ps->max_particles; ++i) {
    const Particle *p = &ps->particles[i];
    if (!p->is_active)
      continue;

    // REFACTOR: Access data through the 'def' member
    float t = 1.0f - (p->def.lifetime / p->initial_life);
    Color draw_color;

    if (p->mode == PARTICLE_MODE_INTERPOLATE) {
      draw_color = lerp_color(p->def.start_color, p->def.end_color, t);
    } else { // PARTICLE_MODE_FADE
      draw_color = Fade(p->def.color, p->def.lifetime / p->initial_life);
    }

    DrawCircleV(p->def.pos, p->def.radius, draw_color);
  }
}

static inline void particle_system_emit(ParticleSystem *ps,
                                        ParticleDefinition def,
                                        ParticleMode mode, int count) {
  for (int i = 0; i < count; ++i) {
    Particle *p = find_next_inactive_particle(ps);
    if (!p)
      return;

    // REFACTOR: This is much cleaner. Just copy the entire definition.
    p->def = def;

    // Set the unique state for this live particle
    p->initial_life = def.lifetime;
    p->mode = mode;
    p->is_active = true;
  }
}

#endif // PARTICLE_H
