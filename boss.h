#ifndef BOSS_H
#define BOSS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "entity.h"
#include "sprites.h"

typedef struct
{
    Entity entity;
    int health;
    int healthMax;
    float attackTimer;
    bool active;
    bool spawning; // Boss does a little entry
    float spawningSpeed;
    bool moveDirection; // Direction boss moves (false - left, true - right)
    float movementSpeed;
    bool phaseTwo;
} Boss;

extern Boss currentBoss;
extern bool bossActive;

void init_boss(void);
void spawn_boss(float x, float y, int wave);
void tick_boss(float deltaTime);
void render_boss(SDL_Renderer *renderer, int shakeX, int shakeY);
void render_boss_health(SDL_Renderer *renderer, TTF_Font *font);

#endif
