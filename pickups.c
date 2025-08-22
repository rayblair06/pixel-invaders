#include <math.h>
#include "constants.h"
#include "entity.h"
#include "game.h"
#include "pickups.h"
#include "player.h"
#include "sprites.h"

Pickup pickups[MAX_PICKUPS];

/**
 * Initialise pickups as deactivated
 */
void init_pickups(void)
{
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        pickups[i].entity.isActive = false;
    }
}

/**
 * Spawn a single Pickup entity
 */
void spawn_pickup(float x, float y)
{
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].entity.isActive)
        {
            // Define the entityCenter so when we render, we render include the entity width to ensure entities are rendered center to the created object
            pickups[i].entity = create_entity(
                x - (int)8 * 1.5,
                y,
                (int)16 * 1.5,
                (int)16 * 1.5);

            pickups[i].entity.despawningDuration = 5.0f; // despawn in 5 seconds

            SpriteID animFrames[] = {SPR_POWERUP_1};

            pickups[i].entity.anim = powerup1Anim;

            break;
        }
    }
}

/**
 * Handle all of our 'tick' functionality of active pickups within the main game loop
 */
void tick_pickups(void)
{
    // Pickups Handle
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].entity.isActive)
            continue;

        Pickup *pickup = &pickups[i];

        entity_tick(&pickup->entity);

        move(&pickup->entity, DOWN, 2);

        // Stop falling when reaches player and start despawning
        const float stopPositionY = (player.entity.pos.y + (player.entity.size.y / 2));
        const float timeToDespawn = 10.0f; // 10 seconds

        if (pickup->entity.pos.y >= stopPositionY)
        {
            pickup->entity.pos.y = stopPositionY;

            if (!pickup->entity.isDespawning)
            {
                entity_begin_despawn(&pickup->entity, timeToDespawn);
            }
        }

        // Despawning
        if (pickup->entity.isDespawning)
        {
            // Ran out of time, despawn.
            if (pickup->entity.hasDespawned)
            {
                pickup->entity.isActive = false;

                // Don't do anything after this
                continue;
            }
        }

        // Player Magnet
        if (hasPickupMagnet)
        {
            float dx = player.entity.pos.x + player.entity.size.x / 2 - pickup->entity.pos.x;
            float dy = player.entity.pos.y + player.entity.size.y / 2 - pickup->entity.pos.y;
            float distance = sqrtf(dx * dx + dy * dy);

            float magnetRange = 150.f;    // How far the feffect pulls from
            float managetStrength = 3.0f; // Pull speed

            if (distance < magnetRange)
            {
                pickup->entity.pos.x += (dx / distance) * managetStrength;
                pickup->entity.pos.y += (dy / distance) * managetStrength;
            }
        }
    }
}

/**
 * Render Active Pickups on Screen
 */
void render_pickups(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].entity.isActive)
            continue;

        Pickup *pickup = &pickups[i];

        SpriteID frame = pickup->entity.anim.frames[pickup->entity.anim.currentFrame];
        SDL_Rect src = get_sprite(frame);
        SDL_Texture *texture = get_sprite_texture(frame);

        SDL_Rect dst = entity_rect(&pickup->entity);
        dst.x += shakeX;
        dst.y += shakeY;

        // Despawning Flash && Fade out
        if (pickup->entity.isDespawning)
        {
            Uint32 elapsed = (Uint32)(pickup->entity.despawningTimer * 1000.0f);

            // Flash: Between 3s and 4s
            if (elapsed >= 3000 && elapsed < 4000)
            {
                pickup->entity.alpha = ((elapsed / 100) % 2 == 0) ? 255 : 0;
            }

            // Fade: between 4s and 5s
            else if (elapsed >= 4000 && elapsed <= 5000)
            {
                float fade = (elapsed - 4000) / 1000.0f;
                pickup->entity.alpha = (Uint8)(255 * (1.0f - fade));
            }
            else if (elapsed > 5000)
            {
                // Already handled by tick_picks - safe to pick
                continue;
            }
        }

        SDL_SetTextureAlphaMod(texture, pickup->entity.alpha);
        SDL_RenderCopy(renderer, texture, &src, &dst);

        // Reset Texture State
        SDL_SetTextureAlphaMod(texture, 255);
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }
}
