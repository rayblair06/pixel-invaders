#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "audio.h"
#include "bullets.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "pickups.h"
#include "sprites.h"
#include "stats.h"
#include "waves.h"
#include "utils.h"

Enemy enemies[MAX_ENEMIES];

bool enemyFrameToggle = false;
Uint32 lastFrameSwitch = 0;
const Uint32 frameInterval = 500; // ms

const SpriteAnimation *enemySprites[] = {
    [ENEMY_BASIC] = &spaceship4Anim,
    [ENEMY_FAST] = &spaceship5Anim,
    [ENEMY_TANK] = &spaceship6Anim,
    [ENEMY_SHOOTER] = &spaceship7Anim,
    [ENEMY_BRUTE] = &spaceship8Anim};

/**
 * Initialise enemies as deactivated
 */
void init_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].entity.isActive = false;
    }
}

/**
 * Spawn a single enemy entity
 */
void spawn_enemy(float x, float y, EnemyType type)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].entity.isActive)
        {
            enemies[i] = create_enemy(x, y, type);
            break;
        }
    }
}

/**
 * Handle all of our 'tick' functionality of active enemies within the main game loop
 */
void tick_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].entity.isActive)
            continue;

        Enemy *enemy = &enemies[i];

        entity_tick(&enemy->entity);

        // Fade out on death
        if (enemy->entity.isDespawning)
        {
            float elapsed = enemy->entity.despawningTimer / enemy->entity.despawningDuration;

            // Fade out
            enemy->entity.alpha = (Uint8)(255.0f * (1.0f - elapsed));

            if (enemy->entity.hasDespawned)
            {
                enemy->entity.isActive = false;

                play_sound(SND_ENEMY_DEATH);

                spawn_pickup(
                    enemy->entity.pos.x + (enemy->entity.size.x / 2),
                    enemy->entity.pos.y);
            }

            // Skip all actions dying
            continue;
        }

        move(&enemy->entity, DOWN, enemy->speed);

        if (enemy->type == ENEMY_BASIC && enemy->entity.pos.y > 0)
        {
            enemy->canShoot = true;
        }

        // Only enemies that canShoot, shoot!
        if (enemy->canShoot)
        {
            if (rand() % 500 == 0)
            {
                spawn_enemy_bullet(
                    enemy->entity.pos.x + enemy->entity.size.x / 2,
                    enemy->entity.pos.y + enemy->entity.size.y,
                    enemy->damage);
            }
        }
    }
}

/**
 * Render Active Enemies on Screen
 */
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].entity.isActive)
            continue;

        Enemy *enemy = &enemies[i];

        // Flash red when damaged
        if (enemy->entity.isHit)
        {
            enemy->entity.color.r = 255;
            enemy->entity.color.g = 64;
            enemy->entity.color.b = 64;
        }
        else
        {
            enemy->entity.color.r = 255;
            enemy->entity.color.g = 255;
            enemy->entity.color.b = 255;
        }

        entity_render(&enemy->entity, renderer, shakeX, shakeY);
    }
}

/**
 * Helper function for creating a specific enemy type out of an entity
 */
Enemy create_enemy(float x, float y, EnemyType type)
{
    Enemy enemy = {0}; // Zero all fields

    enemy.entity = create_entity(x, y, 32 * 2, 32 * 2);

    enemy.entity.isActive = true;

    enemy.type = type;

    enemy.entity.anim = *enemySprites[enemy.type];

    switch (type)
    {
    case ENEMY_BASIC:
        enemy.health = (int)(baseEnemyHealth * enemyHealthMultiplier * 2.0f);
        enemy.speed = baseEnemySpeed * enemySpeedMultiplier * 1.0f;
        enemy.damage = (int)baseEnemyDamage * enemyDamageMultiplier * 1.0f;
        break;
    case ENEMY_FAST:
        enemy.health = (int)(baseEnemyHealth * enemyHealthMultiplier * 1.0f);
        enemy.speed = baseEnemySpeed * enemySpeedMultiplier * 1.2f;
        enemy.damage = baseEnemyDamage * enemyDamageMultiplier * 0.8f;
        break;
    case ENEMY_TANK:
        enemy.health = (int)(baseEnemyHealth * enemyHealthMultiplier * 5.0f);
        enemy.speed = baseEnemySpeed * enemySpeedMultiplier * 0.5f;
        enemy.damage = (int)baseEnemyDamage * enemyDamageMultiplier * 1.5f;
        break;
    case ENEMY_SHOOTER:
        enemy.health = (int)(baseEnemyHealth * enemyHealthMultiplier * 2.0f);
        enemy.speed = baseEnemySpeed * enemySpeedMultiplier * 1.2f;
        enemy.damage = (int)baseEnemyDamage * enemyDamageMultiplier * 1.0f;
        break;
    case ENEMY_BRUTE:
        enemy.health = (int)(baseEnemyHealth * enemyHealthMultiplier * 8.0f);
        enemy.speed = baseEnemySpeed * enemySpeedMultiplier * 0.5f;
        enemy.damage = (int)baseEnemyDamage * enemyDamageMultiplier * 2.5f;
        break;
    case ENEMY_TYPE_COUNT:
        // do nothing
        break;
    }

    return enemy;
}

void damage_enemy(Enemy *enemy, int amount)
{
    enemy->health = enemy->health - amount;

    entity_trigger_hit(&enemy->entity, 0.05f);
    play_sound(SND_HIT);

    if (enemy->health <= 0)
    {
        // They dead, fade out and deactivate
        entity_begin_despawn(&enemy->entity, 0.5f);

        record_kill();
    }
}

void trigger_damage_radius(float x, float y, float radius)
{
    // Damage all enemies within the radius
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].entity.isActive)
        {
            continue;
        }

        float dx = enemies[i].entity.pos.x - x;
        float dy = enemies[i].entity.pos.y - y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < radius)
        {
            damage_enemy(&enemies[i], bulletDamage / 2);
        }
    }
}
