#ifndef ENEMIES_H
#define ENEMIES_H

#include "entity.h"
#include <SDL2/SDL.h>

#define MAX_ENEMIES 50

extern Entity enemies[MAX_ENEMIES];
extern float enemySpeed;

void init_enemies(void);
void spawn_enemy(float x, float y);
void update_enemies(void);
void render_enemies(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY);

#endif
