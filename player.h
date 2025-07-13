#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "entity.h"

extern Entity player;
extern float playerSpeed;

extern int experience, experienceToNextLevel;

extern int level;
extern bool isLevelUpPending;

void init_player();
void player_shoot(const Uint8 *keystate, Mix_Chunk *soundEffect);
void update_player(const Uint8 *keystate);
void render_player(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY);
void add_experience(int amount);

#endif
