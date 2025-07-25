#include <SDL2/SDL.h>
#include "audio.h"
#include "collisions.h"
#include "constants.h"
#include "bullets.h"
#include "enemies.h"
#include "pickups.h"
#include "player.h"
#include "game.h"

bool check_overlap(SDL_Rect a, SDL_Rect b)
{
    return SDL_HasIntersection(&a, &b);
}

void check_collisions(void)
{
    // --- Bullet vs Enemy ---
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        for (int j = 0; j < MAX_ENEMIES; j++)
        {
            if (!enemies[j].active)
                continue;

            if (enemies[j].isFadingOut)
                continue;

            if (check_overlap(bullets[i].entity.rect, enemies[j].entity.rect))
            {
                damage_enemy(&enemies[j]);

                bullets[i].pierceCount--;

                if (bullets[i].pierceCount <= 0)
                {
                    // Explode else disappear
                    if (hasExplosive)
                    {
                        trigger_bullet_explosion(&bullets[i]);
                        trigger_damage_radius(bullets[i].entity.x, bullets[i].entity.y, 50);
                    }
                    else
                    {
                        bullets[i].active = false;
                    }
                }

                break;
            }
        }
    }

    // --- Bullet vs Player ---
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        if (!enemyBullets[i].active)
            continue;

        if (check_overlap(enemyBullets[i].entity.rect, player.rect))
        {
            reduce_health(enemyBullets[i].damage);

            enemyBullets[i].active = false;
        }
    }

    // --- Enemy vs Player ---
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        if (check_overlap(enemies[i].entity.rect, player.rect))
        {
            // Double damage but enemy instantly disappears
            reduce_health(enemies[i].damage * 2);

            enemies[i].active = false;
        }
    }

    // --- Pickup vs Player ---
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].active)
            continue;

        if (check_overlap(pickups[i].rect, player.rect))
        {
            pickups[i].active = false;

            play_sound(SND_PICKUP);

            add_experience(100);
        }
    }

    // --- Enemy vs bottom screen ---
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        if (enemies[i].entity.y > SCREEN_HEIGHT)
        {
            enemies[i].active = false;

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
        if (!bullets[i].active)
            continue;

        // Disactivate when off screen
        if (bullets[i].entity.y + bullets[i].entity.h < 0)
        {
            bullets[i].active = false;
        }
    }
}
