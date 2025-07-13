#ifndef PICKUPS_H
#define PICKUPS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "entity.h"

#define MAX_PICKUPS 100

typedef Entity Pickup;

extern Entity pickups[MAX_PICKUPS];

void init_pickups(void);
void spawn_pickup(float x, float y);
void tick_pickups(void);
void render_pickups(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY);

#endif
