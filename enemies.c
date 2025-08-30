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
    [ENEMY_DRONE] = &spaceship4Anim,
    [ENEMY_ORBITER] = &spaceship5Anim,
    [ENEMY_RAZOR] = &spaceship6Anim,
    [ENEMY_VIPER] = &spaceship7Anim,
    [ENEMY_SENTINEL] = &spaceship8Anim};

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

        move(&enemy->entity, DOWN, enemy->stats.speed);

        // Once into the playarea enable shooting
        if (enemy->stats.hasWeapons && enemy->entity.pos.y > 0)
        {
            enemy->canShoot = true;
        }

        // Only enemies that canShoot, shoot!
        if (enemy->canShoot)
        {
            if (rand() % 500 == 0)
            {
                play_sound(SND_SHOOT2);

                spawn_enemy_bullet(
                    enemy->entity.pos.x + enemy->entity.size.x / 2,
                    enemy->entity.pos.y + enemy->entity.size.y,
                    enemy->stats.damage);
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
    case ENEMY_DRONE:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.0f;
        enemy.stats.baseDamage = 1.0f;
        enemy.stats.hasWeapons = true;
        break;
    case ENEMY_ORBITER:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.2f;
        enemy.stats.baseDamage = 0.0f;
        enemy.stats.hasWeapons = false;
        break;
    case ENEMY_RAZOR:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.5f;
        enemy.stats.baseDamage = 0.0f;
        enemy.stats.hasWeapons = false;
        break;
    case ENEMY_VIPER:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.2f;
        enemy.stats.baseDamage = 1.0f;
        enemy.stats.hasWeapons = true;
        break;
    case ENEMY_SENTINEL:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.2f;
        enemy.stats.baseDamage = 1.5f;
        enemy.stats.hasWeapons = true;
        break;
    case ENEMY_TYPE_COUNT:
        // do nothing
        break;
    }

    // Calculate stats based on base values and wave
    enemy.stats.health = (int)(baseEnemyHealth * enemyHealthMultiplier * enemy.stats.baseHealth);
    enemy.stats.speed = baseEnemySpeed * enemySpeedMultiplier * enemy.stats.baseSpeed;
    enemy.stats.damage = (int)baseEnemyDamage * enemyDamageMultiplier * enemy.stats.baseDamage;

    return enemy;
}

void damage_enemy(Enemy *enemy, int amount)
{
    enemy->stats.health = enemy->stats.health - amount;

    entity_trigger_hit(&enemy->entity, 0.05f);
    play_sound(SND_HIT);

    if (enemy->stats.health <= 0)
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
