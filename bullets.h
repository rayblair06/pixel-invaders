#ifndef BULLETS_H
#define BULLETS_H

#include <SDL2/SDL.h>
#include "entity.h"

#define MAX_BULLETS 100

typedef Entity Bullet;

extern Entity bullets[MAX_BULLETS];
extern float bulletSpeed;

void init_bullets(void);
void spawn_bullet(float x, float y);
void tick_bullets(void);
void render_bullets(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY);

#endif
