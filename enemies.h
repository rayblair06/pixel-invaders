#ifndef ENEMIES_H
#define ENEMIES_H

#include <SDL2/SDL.h>
#include "entity.h"

#define MAX_ENEMIES 50

typedef Entity Enemy;

extern Entity enemies[MAX_ENEMIES];
extern float enemySpeed;

void init_enemies(void);
void spawn_enemy(float x, float y);
void tick_enemies(void);
void render_enemies(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY);

#endif
