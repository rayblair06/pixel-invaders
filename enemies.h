#ifndef ENEMIES_H
#define ENEMIES_H

#include <SDL2/SDL.h>
#include "entity.h"
#include "sprites.h"

#define MAX_ENEMIES 50

typedef enum
{
    ENEMY_BASIC,
    ENEMY_FAST,
    ENEMY_TANK,
    ENEMY_TYPE_COUNT
} EnemyType;

typedef struct
{
    SpriteID frameA;
    SpriteID frameB;
} EnemySpriteFrames;

typedef struct
{
    Entity entity;
    EnemyType type;
    bool active;

    int health;
    float speed;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];
extern float enemySpeed;

void init_enemies(void);
void spawn_enemy(float x, float y, EnemyType type);
void tick_enemies(void);
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY);

Enemy create_enemy(float x, float y, EnemyType type);

#endif
