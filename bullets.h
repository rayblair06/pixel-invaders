#ifndef BULLETS_H
#define BULLETS_H

#include <SDL2/SDL.h>
#include "enemies.h"
#include "entity.h"

#define MAX_BULLETS 255

typedef struct
{
    Entity entity;

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

#define MAX_ENEMY_BULLETS 255

typedef struct
{
    Entity entity;

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
void spawn_enemy_bullet(float x, float y, float vx, float vy, int damage);
void enemy_fire_straight(const Enemy *enemy, float speed, int damage);
void enemy_fire_aimed(const Enemy *enemy, float tx, float ty, float speed, int damage);
void enemy_fire_spread3(const Enemy *enemy, float baseSpeed, float angleDeg, int damage);
void enemy_fire_twin(const Enemy *enemy, float speed, int damage, float offsetX);
void tick_enemy_bullets(void);
void render_enemy_bullets(SDL_Renderer *renderer, int shakeX, int shakeY);

#endif
