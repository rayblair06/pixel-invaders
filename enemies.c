#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "audio.h"
#include "bullets.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "player.h"
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
 * Helper: random number
 */
static float rand01(void) { return (float)(rand() % 10000) / 10000.0f; };

/**
 * Perform shoot based on enemy type
 */
static void enemy_perform_shot(Enemy *enemy)
{
    const int damage = enemy->stats.damage;
    const float speed = enemyBulletSpeed;

    float playerX = player.entity.pos.x;
    float playerY = player.entity.pos.y;

    switch (enemy->stats.firePattern)
    {
    case FIRE_STRAIGHT:
        enemy_fire_straight(enemy, speed, damage);
        break;
    case FIRE_AIMED:
        enemy_fire_aimed(enemy, playerX, playerY, speed, damage);
        break;
    case FIRE_SPREAD3:
        enemy_fire_spread3(enemy, speed, 12.0f, damage);
        break;
    case FIRE_TWIN:
        enemy_fire_twin(enemy, speed, damage, 10.0f);
        break;
    default:
        break;
    }
}

/**
 * 'Tick' enemy movement'
 */
static void update_enemy_movement(Enemy *enemy)
{
    if (enemy->type != ENEMY_ORBITER)
    {
        move(&enemy->entity, DOWN, enemy->stats.speed);
    }
    else
    {
        enemy->stats.ang += enemy->stats.angSpeed * get_delta_time();
        enemy->stats.orbitRadius += enemy->stats.radiusDrift * get_delta_time();

        if (enemy->stats.orbitRadius < 10.0f)
        {
            enemy->stats.orbitRadius = 10.0f; // don't collapse
        }

        float cx = enemy->stats.centerX;
        float cy = enemy->entity.pos.y + enemy->entity.size.y * 0.5f;

        // Advance downward
        cy += enemy->stats.descendSpeed * get_delta_time();

        // Horizontal orbit around center
        float xCenter = cx + enemy->stats.orbitRadius * sinf(enemy->stats.ang);

        // Clamp inside screen horizonally
        float halfW = enemy->entity.size.x * 0.5f;

        if (xCenter < halfW)
            xCenter = halfW;
        if (xCenter > SCREEN_WIDTH - halfW)
            xCenter = SCREEN_WIDTH - halfW;

        // Write back top-left
        enemy->entity.pos.x = xCenter - halfW;
        enemy->entity.pos.y = cy - enemy->entity.size.y * 0.5f;
    }
}

/**
 * 'Tick' 'enemy shooting
 */
static void update_enemy_shooting(Enemy *enemy)
{
    if (enemy->stats.firePattern == FIRE_NONE)
        return;

    float dt = get_delta_time();

    // Handles burst cadence if we're inside a burst
    if (enemy->stats.burstLeft > 0 && enemy->stats.burstLeft < enemy->stats.burstSize)
    {
        enemy->stats.burstTimer -= dt;

        if (enemy->stats.burstTimer <= 0.0f)
        {
            enemy_perform_shot(enemy);

            enemy->stats.burstLeft--;

            if (enemy->stats.burstLeft > 0)
            {
                enemy->stats.burstTimer = enemy->stats.burstInterval;
            }

            // If burst finishsed, set up next cycle
            if (enemy->stats.burstLeft == 0)
            {
                float jitter = (enemy->stats.fireJitter > 0.0f ? (rand01() * 2.0f - 1.0f) * enemy->stats.fireJitter : 0.0f);
                enemy->stats.fireTimer = SDL_max(0.05f, enemy->stats.fireCooldown + jitter);
                enemy->stats.burstLeft = enemy->stats.burstSize;
            }
        }

        return;
    }

    // Not currently bursting -> wait for cool down
    enemy->stats.fireTimer -= dt;

    if (enemy->stats.fireTimer <= 0.0f)
    {
        enemy_perform_shot(enemy);

        if (enemy->stats.burstSize > 1)
        {
            // Fired so reduce burst size
            enemy->stats.burstLeft = enemy->stats.burstSize - 1;
            enemy->stats.burstTimer = enemy->stats.burstInterval;
        }
        else
        {
            // Single-shot cycle
            float jitter = (enemy->stats.fireJitter > 0.0f ? (rand01() * 2.0f - 1.0f) * enemy->stats.fireJitter : 0.0f);
            enemy->stats.fireTimer = SDL_max(0.06f, enemy->stats.fireCooldown + jitter);
        }
    }
}

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
 * Initialise enemy shooting
 */
void init_enemy_shooting(Enemy *enemy)
{
    if (enemy->stats.firePattern == FIRE_NONE)
        return;

    // Randomize inital offset
    float jitter = (enemy->stats.fireJitter > 0.0f ? (rand01() * 2.0f - 1.0f) * enemy->stats.fireJitter : 0.0f);
    enemy->stats.fireTimer = SDL_max(0.05f, enemy->stats.fireCooldown + jitter);

    // Reset burst
    enemy->stats.burstLeft = enemy->stats.burstSize;
    enemy->stats.burstTimer = 0.0f;
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

        update_enemy_movement(enemy);
        update_enemy_shooting(enemy);
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

        enemy.stats.firePattern = FIRE_STRAIGHT;
        enemy.stats.fireCooldown = 4.8f;
        enemy.stats.fireJitter = 0.35f;
        enemy.stats.burstSize = 1;
        enemy.stats.burstInterval = 0.0f;
        break;
    case ENEMY_ORBITER:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.2f;
        enemy.stats.baseDamage = 0.0f;

        enemy.stats.firePattern = FIRE_NONE;

        // Randomize nice-but-fair parameters
        float margin = 60.0f;
        enemy.stats.centerX = margin + (rand() % (int)(SCREEN_WIDTH - 2 * margin));
        enemy.stats.orbitRadius = 40.0f + (rand() % 50);         // 40-90px
        enemy.stats.angSpeed = 2.5f + ((rand() % 200) / 100.0f); // 2.5-4.5 rad/s
        enemy.stats.radiusDrift = -8.0f;                         // tighten as it falls (spiral)
        enemy.stats.descendSpeed = 80.0f;

        enemy.stats.ang = (float)(rand() % 628) / 100.0f; // 0-6.28

        break;
    case ENEMY_RAZOR:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.5f;
        enemy.stats.baseDamage = 0.0f;

        enemy.stats.firePattern = FIRE_NONE;
        break;
    case ENEMY_VIPER:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.2f;
        enemy.stats.baseDamage = 1.0f;

        enemy.stats.firePattern = FIRE_AIMED;
        enemy.stats.fireCooldown = 3.8f;
        enemy.stats.fireJitter = 0.25f;
        enemy.stats.burstSize = 2;
        enemy.stats.burstInterval = 0.12f;
        break;
    case ENEMY_SENTINEL:
        enemy.stats.baseHealth = 1.0f;
        enemy.stats.baseSpeed = 1.2f;
        enemy.stats.baseDamage = 1.5f;

        enemy.stats.firePattern = FIRE_TWIN;
        enemy.stats.fireCooldown = 3.4f;
        enemy.stats.fireJitter = 0.4f;
        enemy.stats.burstSize = 1;
        enemy.stats.burstInterval = 0.0f;
        break;
    case ENEMY_TYPE_COUNT:
        // do nothing
        break;
    }

    // Calculate stats based on base values and wave
    enemy.stats.health = (int)(baseEnemyHealth * enemyHealthMultiplier * enemy.stats.baseHealth);
    enemy.stats.speed = baseEnemySpeed * enemySpeedMultiplier * enemy.stats.baseSpeed;
    enemy.stats.damage = (int)baseEnemyDamage * enemyDamageMultiplier * enemy.stats.baseDamage;

    init_enemy_shooting(&enemy);

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
