#include <math.h>
#include "audio.h"
#include "constants.h"
#include "bullets.h"
#include "player.h"
#include "sprites.h"

Bullet bullets[MAX_BULLETS];
float bulletSpeed = 6.0f;
float bulletDamage = 1.0f;

EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];
float enemyBulletSpeed = 6.0f;

/**
 * Initialise bullets as deactivated
 */
void init_bullets(void)
{
    bulletSpeed = 6.0f;
    enemyBulletSpeed = 6.0f;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }
}

/**
 * Spawn a single Bullet entity
 */
void spawn_bullet(float x, float y, float angle)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            // Define the entityCenter so when we render, we render include the bullet width to ensure bullets are rendered center to the created object
            const int entityCenter = SPRITE_DRAW_SIZE / 2;

            bullets[i].active = true;
            bullets[i].isMoving = true;
            bullets[i].pierceCount = hasPiercing ? 3 : 1;
            bullets[i].entity = create_entity(
                x - entityCenter,
                y,
                SPRITE_DRAW_SIZE,
                SPRITE_DRAW_SIZE);
            bullets[i].entity.angle = angle;

            bullets[i].isExploding = false;
            bullets[i].explosionFrame = 0;
            bullets[i].explosionStartTime = 0;
            bullets[i].explosionFrameDuration = 250; // ms
            bullets[i].explosionFrameCount = 2;

            break;
        }
    }
}

/**
 * Handle all of our 'tick' functionality of active bullets within the main game loop
 */
void tick_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        if (bullets[i].isMoving)
        {
            move(&bullets[i].entity, UP, bulletSpeed);

            if (bullets[i].entity.angle != 0)
            {
                bullets[i].entity.x += cosf(bullets[i].entity.angle) * bulletSpeed;
                update_entity_rect(&bullets[i].entity);
            }
        }

        // Animate Bullet Explosion
        if (bullets[i].isExploding)
        {
            Uint32 elapsed = SDL_GetTicks() - bullets[i].explosionStartTime;
            bullets[i].explosionFrame = elapsed / bullets[i].explosionFrameDuration;

            if (bullets[i].explosionFrame >= bullets[i].explosionFrameCount)
            {
                bullets[i].explosionFrame = bullets[i].explosionFrame - 1;
                bullets[i].isExploding = false;
                bullets[i].active = false;
            }
        }
    }
}

/**
 * Render Active Bullets on Screen
 */
void render_bullets(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    SDL_Rect src;
    SDL_Texture *texture = get_sprite_texture(SPR_BULLET1);

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        SDL_Rect dst = bullets[i].entity.rect;
        dst.x += shakeX;
        dst.y += shakeY;

        if (bullets[i].isExploding)
        {
            SpriteID explosionSprites[] = {SPR_EXPLOSION_A, SPR_EXPLOSION_B};

            src = get_sprite(explosionSprites[bullets[i].explosionFrame]);
        }
        else
        {
            src = get_sprite(SPR_BULLET1);
        }

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}

void trigger_bullet_explosion(Bullet *bullet)
{
    bullet->isMoving = false;
    bullet->isExploding = true;
    bullet->explosionStartTime = SDL_GetTicks();
    bullet->explosionFrame = 0;
    play_sound(SND_EXPLOSION);
}

void init_enemy_bullets(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        enemyBullets[i].active = false;
    }
}

void spawn_enemy_bullet(float x, float y, int damage)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].active)
        {
            // Define the entityCenter so when we render, we render include the bullet width to ensure bullets are rendered center to the created object
            const int entityCenter = SPRITE_DRAW_SIZE / 2;

            enemyBullets[i].active = true;
            enemyBullets[i].entity = create_entity(
                x - entityCenter,
                y,
                SPRITE_DRAW_SIZE,
                SPRITE_DRAW_SIZE);
            enemyBullets[i].damage = damage;

            break;
        }
    }
}

void tick_enemy_bullets(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].active)
            continue;

        move(&enemyBullets[i].entity, DOWN, enemyBulletSpeed);

        // Disactivate when off screen
        if (enemyBullets[i].entity.y + enemyBullets[i].entity.h < 0)
        {
            enemyBullets[i].active = false;
        }
    }
}

void render_enemy_bullets(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    SDL_Rect src = get_sprite(SPR_BULLET2);
    SDL_Texture *texture = get_sprite_texture(SPR_BULLET2);

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].active)
            continue;

        SDL_Rect dst = enemyBullets[i].entity.rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}
