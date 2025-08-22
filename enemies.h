#ifndef ENEMIES_H
#define ENEMIES_H

#include <SDL2/SDL.h>
#include "entity.h"
#include "sprites.h"

#define MAX_ENEMIES 255

struct Enemy;

typedef enum
{
    ENEMY_BASIC,
    ENEMY_FAST,
    ENEMY_TANK,
    ENEMY_SHOOTER,
    ENEMY_BRUTE,
    ENEMY_TYPE_COUNT
} EnemyType;

typedef void (*EnemyBehaviour)(struct Enemy *);

typedef struct
{
    Entity entity;

    EnemyType type;
    EnemyBehaviour tick;
    bool canShoot;

    int health;
    float speed;
    int damage;

    Uint32 damageFlashTimer;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

void init_enemies(void);
void spawn_enemy(float x, float y, EnemyType type);
void tick_enemies(void);
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY);

Enemy create_enemy(float x, float y, EnemyType type);
void get_enemy_sprite(const Enemy *enemy);
void damage_enemy(Enemy *enemy, int amount);

// Move these to effects?
void trigger_damage_radius(float x, float y, float radius);

#endif
