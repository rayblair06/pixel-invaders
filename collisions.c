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

bool check_overlap(SDL_Rect a, SDL_Rect b)
{
    return SDL_HasIntersection(&a, &b);
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

            if (enemies[j].isFadingOut)
                continue;

            if (check_overlap(entity_rect(&bullets[i].entity), entity_rect(&enemies[j].entity)))
            {
                damage_enemy(&enemies[j]);

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

        if (check_overlap(entity_rect(&enemyBullets[i].entity), entity_rect(&player.entity)))
        {
            reduce_health(enemyBullets[i].damage);

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

        if (check_overlap(entity_rect(&bullets[i].entity), entity_rect(&currentBoss.entity)))
        {
            // If our boss is still spawning, just remove bullet
            if (currentBoss.spawning)
            {
                bullets[i].entity.isActive = false;
                continue;
            }

            currentBoss.health = currentBoss.health - bulletDamage;

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

        if (check_overlap(entity_rect(&enemies[i].entity), entity_rect(&player.entity)))
        {
            // Double damage but enemy instantly disappears
            reduce_health(enemies[i].damage * 2);

            enemies[i].entity.isActive = false;
            record_kill();
        }
    }

    // --- Pickup vs Player ---
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].entity.isActive)
            continue;

        if (check_overlap(entity_rect(&pickups[i].entity), entity_rect(&player.entity)))
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

        if (currentBoss.laserDamageTimer <= 0.0f && check_overlap(entity_rect(&player.entity), laserHitbox))
        {
            // TODO: Move to boss variable
            reduce_health(25);
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

            reduce_health(enemies[i].damage);
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
