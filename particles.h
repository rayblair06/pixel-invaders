#ifndef PARTICLES_H
#define PARTICLES_H

#include <SDL.h>
#include <stdbool.h>

#define MAX_PARTICLES 200

typedef struct
{
    float x, y;      // Position
    float vx, vy;    // Velocity
    float life;      // Remaining life in seconds
    float maxLife;   // Total life duration
    SDL_Color color; // Color of the particle
    bool active;
} Particle;

void init_particles(void);
void spawn_explosion_particles(float x, float y, int count);
void tick_particles(float deltaTime);
void render_particles(SDL_Renderer *renderer);

#endif
