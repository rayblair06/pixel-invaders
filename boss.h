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

    bool active;

    float attackTimer; // Time left to fire
    float targetX;     // Desired horizontal position (e.g. follow the player)

    float sideGunsAttackTimer; // Time left to fire
    float sideGunsTargetX;     // Desired horizontal position
    bool isReadyToShoot;       // Flag for ready to shoot side guns
    bool isSideGunsShooting;

    bool chargingLaser;
    float laserChargeTime; // Time left before firing
    float laserDuration;   // How long the laser stays
    bool laserFiring;
    float laserX;
    float laserImpactPulse; // For pulsing brightness
    float laserDamageCooldown;
    float laserDamageTimer;

    float laserSweepSpeed; // How fast the last sweeps left/right
    int laserDirection;    // -1 for left, +1 for right

    bool spawning; // Boss does a little entry
    float spawningSpeed;

    bool isMoving;      // Moving on the x axis
    bool moveDirection; // Direction boss moves (false - left, true - right)
    float movementSpeed;
    bool phaseTwo;
} Boss;

extern Boss currentBoss;
extern bool bossActive;
extern Uint32 bossSpawnTime;

void init_boss(void);
void spawn_boss(float x, float y, int wave);
void tick_boss(void);
void render_boss(int shakeX, int shakeY);
void render_boss_health();
void damage_boss(Boss *boss, int amount);

#endif
