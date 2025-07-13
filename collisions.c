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

            if (check_overlap(bullets[i].rect, enemies[j].rect))
            {
                bullets[i].active = false;
                enemies[j].active = false;

                play_sound(SND_EXPLOSION);

                spawn_pickup(
                    enemies[j].rect.x + (enemies[j].rect.w / 2),
                    enemies[j].rect.y);

                break;
            }
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

        if (enemies[i].y > SCREEN_HEIGHT)
        {
            enemies[i].active = false;
            lose_life();
        }
    }
}
