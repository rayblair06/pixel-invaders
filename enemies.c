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

Enemy enemies[MAX_ENEMIES];

bool enemyFrameToggle = false;
Uint32 lastFrameSwitch = 0;
const Uint32 frameInterval = 500; // ms

// Enemy Animation Frames
typedef struct
{
    SpriteID frameA;
    SpriteID frameB;
} EnemySpriteFrames;

const EnemySpriteFrames enemySprites[] = {
    [ENEMY_BASIC] = {SPR_SPACESHIP4_A, SPR_SPACESHIP4_A},
    [ENEMY_FAST] = {SPR_SPACESHIP5_A, SPR_SPACESHIP5_B},
    [ENEMY_TANK] = {SPR_SPACESHIP6_A, SPR_SPACESHIP6_B},
    [ENEMY_SHOOTER] = {SPR_SPACESHIP7_A, SPR_SPACESHIP7_A},
    [ENEMY_BRUTE] = {SPR_SPACESHIP8_A, SPR_SPACESHIP8_B},
};

/**
 * Initialise enemies as deactivated
 */
void init_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }
}

/**
 * Spawn a single enemy entity
 */
void spawn_enemy(float x, float y, EnemyType type)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
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
        if (!enemies[i].active)
            continue;

        Uint32 fadeDuration = 500; // ms

        Uint32 now = get_game_ticks();

        // Fade out on death
        if (enemies[i].isFadingOut)
        {
            Uint32 elapsed = now - enemies[i].fadeStartTime;
            float t = elapsed / (float)fadeDuration;
            if (t >= 1.0f)
            {
                enemies[i].active = false;
                enemies[i].isFadingOut = false;

                play_sound(SND_ENEMY_DEATH);

                spawn_pickup(
                    enemies[i].entity.pos.x + (enemies[i].entity.size.x / 2),
                    enemies[i].entity.pos.y);
            }
            else
            {
                enemies[i].alpha = 255 * (1.0f - t);
            }

            // Skip all actions dying
            continue;
        }

        move(&enemies[i].entity, DOWN, enemies[i].speed);

        if (enemies[i].entity.pos.y > 0)
        {
            enemies[i].canShoot = true;
        }

        // Only basic shoot
        if (enemies[i].type == ENEMY_BASIC && enemies[i].canShoot)
        {
            if (rand() % 500 == 0)
            {
                spawn_enemy_bullet(
                    enemies[i].entity.pos.x + enemies[i].entity.size.x / 2,
                    enemies[i].entity.pos.y + enemies[i].entity.size.y,
                    enemies[i].damage);
            }
        }
    }
}

/**
 * Render Active Enemies on Screen
 */
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    Uint32 now = get_game_ticks();

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        Uint32 flashDuration = 100; // ms

        SpriteID frame = get_enemy_sprite(&enemies[i]);

        SDL_Rect src = get_sprite(frame);
        SDL_Texture *texture = get_sprite_texture(frame);

        SDL_Rect dst = entity_rect(&enemies[i].entity);
        dst.x += shakeX;
        dst.y += shakeY;

        if (enemies[i].isFadingOut)
        {
            SDL_SetTextureAlphaMod(texture, enemies[i].alpha);
        }

        // Check if recently damaged
        if (now - enemies[i].damageFlashTimer < flashDuration)
        {
            SDL_SetTextureColorMod(texture, 255, 64, 64);
        }

        SDL_RenderCopy(renderer, texture, &src, &dst);

        // Reset colours and alpha
        SDL_SetTextureColorMod(texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(texture, 255);
    }
}

/**
 * Toggle enemy animations
 */
void update_enemy_animation_state(void)
{
    Uint32 now = get_game_ticks();

    if (now - lastFrameSwitch > frameInterval)
    {
        enemyFrameToggle = !enemyFrameToggle;
        lastFrameSwitch = now;
    }
}

/**
 * Helper function for creating a specific enemy type out of an entity
 */
Enemy create_enemy(float x, float y, EnemyType type)
{
    Enemy enemy = {0}; // Zero all fields

    enemy.active = true;
    enemy.type = type;
    enemy.entity = create_entity(x, y, 32 * 2, 32 * 2);

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

SpriteID get_enemy_sprite(const Enemy *enemy)
{
    EnemySpriteFrames frames = enemySprites[enemy->type];
    return enemyFrameToggle ? frames.frameB : frames.frameA;
}

void damage_enemy(Enemy *enemy)
{
    enemy->health = enemy->health - bulletDamage;

    if (enemy->health > 0)
    {
        enemy->damageFlashTimer = get_game_ticks();

        play_sound(SND_HIT);
    }
    else
    {
        // They dead, fade out and deactivate
        enemy->isFadingOut = true;
        enemy->fadeStartTime = get_game_ticks();
        enemy->alpha = 255;
        record_kill();
    }
}

void trigger_damage_radius(float x, float y, float radius)
{
    // Damage all enemies within the radius
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            continue;
        }

        float dx = enemies[i].entity.pos.x - x;
        float dy = enemies[i].entity.pos.y - y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < radius)
        {
            damage_enemy(&enemies[i]);
        }
    }
}
