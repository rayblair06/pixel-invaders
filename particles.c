#include "particles.h"
#include <stdlib.h>
#include <math.h>

static Particle particles[MAX_PARTICLES];

void init_particles(void)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].active = false;
    }
}

void spawn_explosion_particles(float x, float y, int count)
{
    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < MAX_PARTICLES; j++)
        {
            if (!particles[j].active)
            {
                particles[j].active = true;
                particles[j].x = x;
                particles[j].y = y;

                // Random velocity direction
                float angle = ((float)rand() / RAND_MAX) * 2 * M_PI;
                float speed = 50 + rand() % 150; // pixels per second
                particles[j].vx = cosf(angle) * speed;
                particles[j].vy = sinf(angle) * speed;

                particles[j].maxLife = particles[j].life = 0.5 + (rand() % 50) / 100.0f; // 0.5 - 1.0 sec
                particles[j].color = (SDL_Color){255, 200 + rand() % 55, 0, 255};        // orange-yellow
                break;
            }
        }
    }
}

void tick_particles(float deltaTime)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        particles[i].x += particles[i].vx * deltaTime;
        particles[i].y += particles[i].vy * deltaTime;
        particles[i].life -= deltaTime;

        if (particles[i].life <= 0)
        {
            particles[i].active = false;
        }
    }
}

void render_particles(SDL_Renderer *renderer)
{
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (!particles[i].active)
            continue;

        float alpha = particles[i].life / particles[i].maxLife; // fade over time
        SDL_SetRenderDrawColor(renderer,
                               particles[i].color.r,
                               particles[i].color.g,
                               particles[i].color.b,
                               (Uint8)(alpha * 255));

        SDL_RenderDrawPoint(renderer, (int)particles[i].x, (int)particles[i].y);
    }
}
