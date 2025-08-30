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
void render_bullets(int shakeX, int shakeY);
void trigger_bullet_explosion(Bullet *bullet);

void init_enemy_bullets(void);
void spawn_enemy_bullet(float x, float y, float vx, float vy, int damage);
void entity_fire_straight(const Entity *entity, float speed, int damage);
void entity_fire_aimed(const Entity *entity, float tx, float ty, float speed, int damage);
void entity_fire_spread3(const Entity *entity, float baseSpeed, float angleDeg, int damage);
void entity_fire_twin(const Entity *entity, float speed, int damage, float offsetX);
void tick_enemy_bullets(void);
void render_enemy_bullets(int shakeX, int shakeY);

#endif
