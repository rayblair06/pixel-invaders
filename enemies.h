#ifndef ENEMIES_H
#define ENEMIES_H

#include <SDL2/SDL.h>
#include "entity.h"
#include "sprites.h"

#define MAX_ENEMIES 255

struct Enemy;

typedef enum
{
    ENEMY_DRONE,    // Basic shooter, medium pace
    ENEMY_ORBITER,  // Circular, erratic, no shooting, fast
    ENEMY_RAZOR,    // Boomerang, straight fast dash, no shooting
    ENEMY_VIPER,    // Fast, shooter
    ENEMY_SENTINEL, // Bigger winged shooter, twin shots, moderate pace

    ENEMY_TYPE_COUNT
} EnemyType;

typedef enum
{
    FIRE_NONE = 0,
    FIRE_STRAIGHT, // single bullet straight down
    FIRE_AIMED,    // aim at player
    FIRE_TWIN,     // two muzzle shot
    FIRE_SPREAD3,  // 3-way spread
} EnemyFirePattern;

typedef struct
{
    // Base values of unit
    float baseHealth;
    float baseSpeed;
    float baseDamage;

    // Current values of unit
    int health;
    float speed;
    int damage;

    // Shooting
    EnemyFirePattern firePattern;
    float fireCooldown; // seconds between "attack cycles"
    float fireTimer;    // counts down with dt
    float fireJitter;   // +- seconds added on each reset

    int burstSize;       // shots per burst (1 = none)
    int burstLeft;       // remaining shots in current burst
    float burstInterval; // time between shots inside the burst (seconds)
    float burstTimer;    // countdown between burst shots

    // Orbiter-only fields (ignored for other types)
    float ang;          // current angle (radians)
    float angSpeed;     // radians/src
    float orbitRadius;  // pixels
    float radiusDrift;  // pixels/sec (negative to tighten spiral)
    float descendSpeed; // pixels/sec (downward)
    float centerX;      // horizontal track center (pixels)
} EnemyStats;

typedef void (*EnemyBehaviour)(struct Enemy *);

typedef struct
{
    Entity entity;

    EnemyType type;
    EnemyBehaviour tick;
    bool canShoot;

    EnemyStats stats;

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
