#include <SDL2/SDL.h>
#include "audio.h"
#include "boss.h"
#include "collisions.h"
#include "constants.h"
#include "bullets.h"
#include "entity.h"
#include "enemies.h"
#include "game.h"
#include "pickups.h"
#include "player.h"
#include "stats.h"

/**
 * Scale Rects for use in smaller/larger hitboxes
 */
static SDL_Rect scale_rect(SDL_Rect rect, float scale)
{
    if (scale <= 0.0f)
        return (SDL_Rect){rect.x, rect.y, 0, 0};

    int newWidth = (int)(rect.w * scale);
    int newHeight = (int)(rect.h * scale);
    int centerX = rect.x + rect.w / 2;
    int centerY = rect.y + rect.h / 2;

    SDL_Rect scaledRect;
    scaledRect.w = newWidth;
    scaledRect.h = newHeight;
    scaledRect.x = centerX - newWidth / 2;
    scaledRect.y = centerY - newHeight / 2;

    return scaledRect;
}

bool check_overlap(SDL_Rect a, float aScale, SDL_Rect b, float bScale)
{
    SDL_Rect scaledA = scale_rect(a, aScale);
    SDL_Rect scaledB = scale_rect(b, bScale);

    return SDL_HasIntersection(&scaledA, &scaledB);
}

void check_collisions(void)
{
    // --- Bullet vs Enemy ---
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].entity.isActive)
            continue;

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!enemies[j].entity.isActive)
                continue;

            if (enemies[j].entity.isDespawning)
                continue;

            if (check_overlap(
                    entity_rect(&bullets[i].entity),
                    bullets[i].entity.hitboxScale,
                    entity_rect(&enemies[j].entity),
                    enemies[j].entity.hitboxScale))
            {
                damage_enemy(&enemies[j], bulletDamage);

                bullets[i].pierceCount--;

                if (bullets[i].pierceCount <= 0)
                {
                    // Explode else disappear
                    if (hasExplosive)
                    {
                        trigger_bullet_explosion(&bullets[i]);
                        trigger_damage_radius(bullets[i].entity.pos.x, bullets[i].entity.pos.y, 50);
                    }
                    else
                    {
                        bullets[i].entity.isActive = false;
                    }
                }

                break;
            }
        }
    }

    // --- Bullet vs Player ---
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].entity.isActive)
            continue;

        if (check_overlap(
                entity_rect(&enemyBullets[i].entity),
                enemyBullets[i].entity.hitboxScale,
                entity_rect(&player.entity),
                player.entity.hitboxScale))
        {
            reduce_player_health(enemyBullets[i].damage);

            enemyBullets[i].entity.isActive = false;
        }
    }

    // --- Bullet vs Boss ---
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].entity.isActive)
            continue;

        if (!currentBoss.entity.isActive)
            continue;

        if (check_overlap(
                entity_rect(&bullets[i].entity),
                bullets[i].entity.hitboxScale,
                entity_rect(&currentBoss.entity),
                currentBoss.entity.hitboxScale))
        {
            // If our boss is still spawning, just remove bullet
            if (currentBoss.spawning)
            {
                bullets[i].entity.isActive = false;
                continue;
            }

            damage_boss(&currentBoss, bulletDamage);

            bullets[i].pierceCount--;

            if (bullets[i].pierceCount <= 0)
            {
                // Explode else disappear
                if (hasExplosive)
                {
                    trigger_bullet_explosion(&bullets[i]);
                    trigger_damage_radius(bullets[i].entity.pos.x, bullets[i].entity.pos.y, 50);
                }
                else
                {
                    bullets[i].entity.isActive = false;
                }
            }

            break;
        }
    }

    // --- Enemy vs Player ---
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].entity.isActive)
            continue;

        if (check_overlap(
                entity_rect(&enemies[i].entity),
                enemies[i].entity.hitboxScale,
                entity_rect(&player.entity),
                player.entity.hitboxScale))
        {
            // Double damage but enemy instantly disappears
            if (!enemies[i].entity.isDespawning)
            {
                damage_enemy(&enemies[i], 1000000); // Instant kill
                reduce_player_health(enemies[i].stats.damage * 2);
                record_kill();
            }
        }
    }

    // --- Pickup vs Player ---
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].entity.isActive)
            continue;

        if (check_overlap(
                entity_rect(&pickups[i].entity),
                pickups[i].entity.hitboxScale,
                entity_rect(&player.entity),
                player.entity.hitboxScale))
        {
            pickups[i].entity.isActive = false;

            play_sound(SND_PICKUP);

            add_experience(10);
        }
    }

    // --- Boss Beam vs Player ---
    if (currentBoss.laserFiring)
    {
        currentBoss.laserDamageTimer -= get_delta_time();

        SDL_Rect laserHitbox = {
            (int)currentBoss.laserX - 4,
            currentBoss.entity.pos.y + currentBoss.entity.size.y,
            8,
            SCREEN_HEIGHT - (currentBoss.entity.pos.y + currentBoss.entity.size.y)};
        float laserHitboxSale = 1.0f;

        if (currentBoss.laserDamageTimer <= 0.0f && check_overlap(entity_rect(&player.entity), player.entity.hitboxScale, laserHitbox, laserHitboxSale))
        {
            // TODO: Move to boss variable
            reduce_player_health(75);
            currentBoss.laserDamageTimer = currentBoss.laserDamageCooldown;
        }
    }

    // --- Enemy vs bottom screen ---
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].entity.isActive)
            continue;

        if (enemies[i].entity.pos.y > SCREEN_HEIGHT)
        {
            enemies[i].entity.isActive = false;
            record_kill();

            if (hasShield)
            {
                hasShield = false; // Shield breaks, no damage

                play_sound(SND_SHIELD_BREAK);
                break;
            }

            reduce_player_health(enemies[i].stats.damage);
        }
    }

    // --- Bullets vs top screen ---
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].entity.isActive)
            continue;

        // Disactivate when off screen
        if (bullets[i].entity.pos.y + bullets[i].entity.size.y < 0)
        {
            bullets[i].entity.isActive = false;
        }
    }
}
