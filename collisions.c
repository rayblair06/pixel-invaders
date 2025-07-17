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

            if (check_overlap(bullets[i].rect, enemies[j].entity.rect))
            {
                bullets[i].active = false;

                damage_enemy(&enemies[j]);

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
            enemyBullets[i].active = false;

            lose_life();
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

            lose_life();
        }
    }
}
