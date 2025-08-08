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

            pickups[i].entity.despawningDuration = 5000; // despawn in 5 seconds

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

        move(&pickups[i].entity, DOWN, 2);

        // Animation
        entity_animate(&pickups[i].entity);

        // Stop falling at player Y and start despawning
        if (pickups[i].entity.pos.y >= player.entity.pos.y)
        {
            pickups[i].entity.pos.y = player.entity.pos.y;

            if (!pickups[i].entity.isDespawning)
            {
                pickups[i].entity.isDespawning = true;
                pickups[i].entity.despawningTime = get_game_ticks();
            }
        }

        // Despawning
        if (pickups[i].entity.isDespawning)
        {
            // Ran out of time, despawn.
            if (get_game_ticks() - pickups[i].entity.despawningTime > pickups[i].entity.despawningDuration)
            {
                pickups[i].entity.isActive = false;

                // Don't do anything after this
                continue;
            }
        }

        // Player Magnet
        if (hasPickupMagnet)
        {
            float dx = player.entity.pos.x + player.entity.size.x / 2 - pickups[i].entity.pos.x;
            float dy = player.entity.pos.y + player.entity.size.y / 2 - pickups[i].entity.pos.y;
            float distance = sqrtf(dx * dx + dy * dy);

            float magnetRange = 150.f;    // How far the feffect pulls from
            float managetStrength = 3.0f; // Pull speed

            if (distance < magnetRange)
            {
                pickups[i].entity.pos.x += (dx / distance) * managetStrength;
                pickups[i].entity.pos.y += (dy / distance) * managetStrength;
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

        // Default full opacity
        Uint8 alpha = 255;

        if (pickup->entity.isDespawning)
        {
            Uint32 elapsed = get_game_ticks() - pickup->entity.despawningTime;

            // Flash: Between 3s and 4s
            if (elapsed >= 3000 && elapsed < 4000)
            {
                alpha = ((elapsed / 100) % 2 == 0) ? 255 : 0;
            }

            // Fade: between 4s and 5s
            else if (elapsed >= 4000 && elapsed <= 5000)
            {
                float fade = (elapsed - 4000) / 1000.0f;
                alpha = (Uint8)(255 * (1.0f - fade));
            }
            else if (elapsed > 5000)
            {
                // Already handled by tick_picks - safe to pick
                continue;
            }
        }

        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_RenderCopy(renderer, texture, &src, &dst);

        // Reset Texture State
        SDL_SetTextureAlphaMod(texture, 255);
        SDL_SetTextureColorMod(texture, 255, 255, 255);
    }
}
