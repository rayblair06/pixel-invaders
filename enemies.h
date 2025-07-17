#ifndef ENEMIES_H
#define ENEMIES_H

#include <SDL2/SDL.h>
#include "entity.h"
#include "sprites.h"

#define MAX_ENEMIES 50

struct Enemy;

typedef enum
{
    ENEMY_BASIC,
    ENEMY_FAST,
    ENEMY_TANK,
    ENEMY_TYPE_COUNT
} EnemyType;

typedef void (*EnemyBehaviour)(struct Enemy *);

typedef struct
{
    Entity entity;
    EnemyType type;
    EnemyBehaviour tick;
    bool active;
    bool canShoot;

    int health;
    float speed;

    Uint32 damageFlashTimer;

    bool isFadingOut;
    Uint32 fadeStartTime;
    Uint8 alpha;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];
extern float enemySpeed;

extern float enemyHealthMultiplier;
extern float enemySpeedMultiplier;

void init_enemies(void);
void spawn_enemy(float x, float y, EnemyType type);
void tick_enemies(void);
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY);
void update_enemy_animation_state(void);

Enemy create_enemy(float x, float y, EnemyType type);
SpriteID get_enemy_sprite(const Enemy *enemy);
void damage_enemy(Enemy *enemy);

#endif
