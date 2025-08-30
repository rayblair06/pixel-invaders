#include <math.h>
#include "audio.h"
#include "constants.h"
#include "bullets.h"
#include "enemies.h"
#include "entity.h"
#include "game.h"
#include "particles.h"
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
        bullets[i].entity.isActive = false;
    }
}

/**
 * Spawn a single Bullet entity
 */
void spawn_bullet(float x, float y, float angle)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].entity.isActive)
        {
            // Define the entityCenter so when we render, we render include the bullet width to ensure bullets are rendered center to the created object
            bullets[i].entity = create_entity(
                x - (int)8 * 2,
                y,
                16 * 2,
                16 * 2);

            bullets[i].entity.isActive = true;
            bullets[i].isMoving = true;
            bullets[i].pierceCount = hasPiercing ? 3 : 1;
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
        if (!bullets[i].entity.isActive)
            continue;

        if (bullets[i].isMoving)
        {
            move(&bullets[i].entity, UP, bulletSpeed);

            if (bullets[i].entity.angle != 0)
            {
                bullets[i].entity.pos.x += cosf(bullets[i].entity.angle) * bulletSpeed;
            }
        }

        // Animate Bullet Explosion
        if (bullets[i].isExploding)
        {
            Uint32 elapsed = get_game_ticks() - bullets[i].explosionStartTime;
            bullets[i].explosionFrame = elapsed / bullets[i].explosionFrameDuration;

            if (bullets[i].explosionFrame >= bullets[i].explosionFrameCount)
            {
                bullets[i].explosionFrame = bullets[i].explosionFrame - 1;
                bullets[i].isExploding = false;
                bullets[i].entity.isActive = false;
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
    SDL_Texture *texture = get_sprite_texture(SPR_PROJECTILE_1);

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].entity.isActive)
            continue;

        SDL_Rect dst = entity_rect(&bullets[i].entity);
        dst.x += shakeX;
        dst.y += shakeY;

        if (bullets[i].isExploding)
        {
            SpriteID explosionSprites[] = {SPR_EXPLOSION1_A, SPR_EXPLOSION1_B, SPR_EXPLOSION1_C, SPR_EXPLOSION1_D, SPR_EXPLOSION1_E};

            src = get_sprite(explosionSprites[bullets[i].explosionFrame]);
        }
        else
        {
            src = get_sprite(SPR_PROJECTILE_1);
        }

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}

void trigger_bullet_explosion(Bullet *bullet)
{
    bullet->isMoving = false;
    bullet->isExploding = true;
    bullet->explosionStartTime = get_game_ticks();
    bullet->explosionFrame = 0;

    spawn_explosion_particles(bullet->entity.pos.x, bullet->entity.pos.y, 20);
    play_sound(SND_EXPLOSION);
}

void init_enemy_bullets(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        enemyBullets[i].entity.isActive = false;
    }
}

void spawn_enemy_bullet(float x, float y, float vx, float vy, int damage)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].entity.isActive)
        {
            enemyBullets[i].entity = create_entity(
                x - (int)8 * 2,
                y,
                16 * 2,
                16 * 2);

            enemyBullets[i].entity.isActive = true;
            enemyBullets[i].damage = damage;
            play_sound(SND_SHOOT2);

            break;
        }
    }
}

/**
 * Shoot type: fire one straight down from the enemy's center-bottom
 */
void enemy_fire_straight(const Enemy *enemy, float speed, int damage)
{
    float cx = enemy->entity.pos.x + enemy->entity.size.x * 0.5f;
    float y = enemy->entity.pos.y + enemy->entity.size.y;

    spawn_enemy_bullet(cx, y, 0.0f, speed, damage);
}

/**
 * Shoot type: fire aimed at x, y
 */
void enemy_fire_aimed(const Enemy *enemy, float tx, float ty, float speed, int damage)
{
    float sx = enemy->entity.pos.x + enemy->entity.size.x * 0.5f;
    float sy = enemy->entity.pos.y + enemy->entity.size.y;

    float dx = tx - sx;
    float dy = ty - sy;

    float len = SDL_sqrtf(dx * dx + dy * dy);

    spawn_enemy_bullet(sx, sy, (dx / len) * speed, (dy / len) * speed, damage);
}

/**
 * Shoot type: fire 3-way spread (straight + slight angles)
 */
void enemy_fire_spread3(const Enemy *enemy, float baseSpeed, float angleDeg, int damage)
{
    float cx = enemy->entity.pos.x + enemy->entity.size.x * 0.5f;
    float y = enemy->entity.pos.y + enemy->entity.size.y;

    // Center
    spawn_enemy_bullet(cx, y, 0.0f, baseSpeed, damage);

    // left/right wiht small X velocity
    float a = angleDeg * 3.1415926f / 180.0f;
    float vx = baseSpeed * SDL_sinf(a);
    float vy = baseSpeed * SDL_cosf(a);

    spawn_enemy_bullet(cx, y, -vx, vy, damage);
    spawn_enemy_bullet(cx, y, vx, vy, damage);
}

/**
 * Shoot type: two muzzles
 */
void enemy_fire_twin(const Enemy *enemy, float speed, int damage, float offsetX)
{
    float leftX = enemy->entity.pos.x + enemy->entity.size.x * 0.5f - offsetX;
    float rightX = enemy->entity.pos.x + enemy->entity.size.x * 0.5f + offsetX;
    float y = enemy->entity.pos.y + enemy->entity.size.y;

    spawn_enemy_bullet(leftX, y, 0.0f, speed, damage);
    spawn_enemy_bullet(rightX, y, 0.0f, speed, damage);
}

void tick_enemy_bullets(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].entity.isActive)
            continue;

        move(&enemyBullets[i].entity, DOWN, enemyBulletSpeed);

        // Disactivate when off screen
        if (enemyBullets[i].entity.pos.y + enemyBullets[i].entity.size.y < 0)
        {
            enemyBullets[i].entity.isActive = false;
        }
    }
}

void render_enemy_bullets(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    SDL_Rect src = get_sprite(SPR_PROJECTILE_2);
    SDL_Texture *texture = get_sprite_texture(SPR_PROJECTILE_2);

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].entity.isActive)
            continue;

        SDL_Rect dst = entity_rect(&enemyBullets[i].entity);
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}
