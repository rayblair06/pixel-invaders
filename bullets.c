#include "constants.h"
#include "bullets.h"
#include "sprites.h"

Entity bullets[MAX_BULLETS];
float bulletSpeed = 6.0f;

EnemyBullet enemyBullets[MAX_ENEMY_BULLETS];
float enemyBulletSpeed = 6.0f;

/**
 * Initialise bullets as deactivated
 */
void init_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }
}

/**
 * Spawn a single Bullet entity
 */
void spawn_bullet(float x, float y)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            // Define the entityCenter so when we render, we render include the bullet width to ensure bullets are rendered center to the created object
            const int entityCenter = SPRITE_DRAW_SIZE / 2;

            bullets[i] = create_entity(
                x - entityCenter,
                y,
                SPRITE_DRAW_SIZE,
                SPRITE_DRAW_SIZE);

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

        move(&bullets[i], UP, bulletSpeed);

        // Disactivate when off screen
        if (bullets[i].y + bullets[i].h < 0)
        {
            bullets[i].active = false;
        }
    }
}

/**
 * Render Active Bullets on Screen
 */
void render_bullets(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    SDL_Rect src = get_sprite(SPR_BULLET1);
    SDL_Texture *texture = get_sprite_texture(SPR_BULLET1);

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        SDL_Rect dst = bullets[i].rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}

void init_enemy_bullets(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        enemyBullets[i].active = false;
    }
}

void spawn_enemy_bullet(float x, float y)
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
