#ifndef BULLETS_H
#define BULLETS_H

#include <SDL2/SDL.h>
#include "entity.h"

#define MAX_BULLETS 100

typedef Entity Bullet;

extern Entity bullets[MAX_BULLETS];
extern float bulletSpeed;
extern float bulletDamage;

#define MAX_ENEMY_BULLETS 50
typedef struct
{
    Entity entity;
    bool active;
} EnemyBullet;

extern EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];
extern float enemyBulletSpeed;

void init_bullets(void);
void spawn_bullet(float x, float y, float angle);
void tick_bullets(void);
void render_bullets(SDL_Renderer *renderer, int shakeX, int shakeY);

void init_enemy_bullets(void);
void spawn_enemy_bullet(float x, float y);
void tick_enemy_bullets(void);
void render_enemy_bullets(SDL_Renderer *renderer, int shakeX, int shakeY);

#endif
