#include "audio.h"
#include "bullets.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "pickups.h"
#include "sprites.h"
#include "waves.h"

Enemy enemies[MAX_ENEMIES];

float enemyHealthMultiplier = 1.0f;
float enemySpeedMultiplier = 1.0f;

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
    [ENEMY_BASIC] = {SPR_INVADER1_A, SPR_INVADER1_B},
    [ENEMY_FAST] = {SPR_INVADER2_A, SPR_INVADER2_B},
    [ENEMY_TANK] = {SPR_INVADER3_A, SPR_INVADER3_B},
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

        Uint32 now = SDL_GetTicks();

        // Fade out on death
        if (enemies[i].isFadingOut)
        {
            Uint32 elapsed = now - enemies[i].fadeStartTime;
            float t = elapsed / (float)fadeDuration;
            if (t >= 1.0f)
            {
                enemies[i].active = false;
                enemies[i].isFadingOut = false;

                play_sound(SND_EXPLOSION);

                spawn_pickup(
                    enemies[i].entity.rect.x + (enemies[i].entity.rect.w / 2),
                    enemies[i].entity.rect.y);
            }
            else
            {
                enemies[i].alpha = 255 * (1.0f - t);
            }

            // Skip all actions dying
            continue;
        }

        move(&enemies[i].entity, DOWN, enemies[i].speed);

        if (enemies[i].entity.y > 0)
        {
            enemies[i].canShoot = true;
        }

        // Only basic shoot
        if (enemies[i].type == ENEMY_BASIC && enemies[i].canShoot)
        {
            if (rand() % 500 == 0)
            {
                spawn_enemy_bullet(
                    enemies[i].entity.x + enemies[i].entity.w / 2,
                    enemies[i].entity.y + enemies[i].entity.h);
            }
        }
    }
}

/**
 * Render Active Enemies on Screen
 */
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    Uint32 now = SDL_GetTicks();

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        Uint32 flashDuration = 100; // ms

        SpriteID frame = get_enemy_sprite(&enemies[i]);

        SDL_Rect src = get_sprite(frame);
        SDL_Texture *texture = get_sprite_texture(frame);

        SDL_Rect dst = enemies[i].entity.rect;
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
    Uint32 now = SDL_GetTicks();

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
    Enemy enemy;

    enemy.active = true;
    enemy.type = type;
    enemy.entity = create_entity(x, y, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);

    switch (type)
    {
    case ENEMY_BASIC:
        enemy.health = baseEnemyHealth * 2;
        enemy.speed = baseEnemySpeed * 1.0f;
        break;
    case ENEMY_FAST:
        enemy.health = baseEnemyHealth * 1;
        enemy.speed = baseEnemySpeed * 2.0f;
        break;
    case ENEMY_TANK:
        enemy.health = baseEnemyHealth * 5;
        enemy.speed = baseEnemySpeed * 0.5f;
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
    enemy->health--;

    if (enemy->health > 0)
    {
        enemy->damageFlashTimer = SDL_GetTicks();

        play_sound(SND_HIT);
    }
    else
    {
        // They dead, fade out and deactivate
        enemy->isFadingOut = true;
        enemy->fadeStartTime = SDL_GetTicks();
        enemy->alpha = 255;
    }
}
