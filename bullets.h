#ifndef BULLETS_H
#define BULLETS_H

#include <SDL.h>
#include "entity.h"

#define MAX_BULLETS 100

typedef struct
{
    Entity entity;
    bool active;
    int pierceCount; // number of hits before disappearing
    bool isMoving;

    // Explosion Effect
    bool isExploding;
    int explosionFrame;
    Uint32 explosionStartTime;
    int explosionFrameDuration; // ms
    int explosionFrameCount;
} Bullet;

extern Bullet bullets[MAX_BULLETS];
extern float bulletSpeed;
extern float bulletDamage;

#define MAX_ENEMY_BULLETS 50

typedef struct
{
    Entity entity;
    bool active;
    int damage;
} EnemyBullet;

extern EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];
extern float enemyBulletSpeed;

void init_bullets(void);
void spawn_bullet(float x, float y, float angle);
void tick_bullets(void);
void render_bullets(SDL_Renderer *renderer, int shakeX, int shakeY);
void trigger_bullet_explosion(Bullet *bullet);

void init_enemy_bullets(void);
void spawn_enemy_bullet(float x, float y, int damage);
void tick_enemy_bullets(void);
void render_enemy_bullets(SDL_Renderer *renderer, int shakeX, int shakeY);

#endif
