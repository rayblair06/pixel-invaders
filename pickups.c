#include <math.h>
#include "constants.h"
#include "pickups.h"
#include "player.h"
#include "sprites.h"

Entity pickups[MAX_PICKUPS];

/**
 * Initialise pickups as deactivated
 */
void init_pickups(void)
{
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        pickups[i].active = false;
    }
}

/**
 * Spawn a single Pickup entity
 */
void spawn_pickup(float x, float y)
{
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].active)
        {
            // Define the entityCenter so when we render, we render include the entity width to ensure entities are rendered center to the created object
            const int entityCenter = SPRITE_DRAW_SIZE / 2;

            pickups[i] = create_entity(
                x - entityCenter,
                y,
                SPRITE_DRAW_SIZE,
                SPRITE_DRAW_SIZE);

            pickups[i].despawningDuration = 5000; // despawn in 5 seconds

            pickups[i].animFrame = 0;
            pickups[i].animFrameCount = 5;
            pickups[i].animStartTime = SDL_GetTicks();
            pickups[i].animFrameDuration = 100;

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
        if (!pickups[i].active)
            continue;

        move(&pickups[i], DOWN, 2);

        // Animation
        Uint32 elapsed = SDL_GetTicks() - pickups[i].animStartTime;
        pickups[i].animFrame = (elapsed / pickups[i].animFrameDuration) % pickups[i].animFrameCount;

        // Stop falling at player Y and start despawning
        if (pickups[i].y >= player.y)
        {
            pickups[i].y = player.y;

            if (!pickups[i].despawning)
            {
                pickups[i].despawning = true;
                pickups[i].despawningTime = SDL_GetTicks();
            }
        }

        // Despawning
        if (pickups[i].despawning)
        {
            // Ran out of time, despawn.
            if (SDL_GetTicks() - pickups[i].despawningTime > pickups[i].despawningDuration)
            {
                pickups[i].active = false;

                // Don't do anything after this
                continue;
            }
        }

        // Player Magnet
        if (hasPickupMagnet)
        {
            float dx = player.x + player.w / 2 - pickups[i].x;
            float dy = player.y + player.h / 2 - pickups[i].y;
            float distance = sqrtf(dx * dx + dy * dy);

            float magnetRange = 150.f;    // How far the feffect pulls from
            float managetStrength = 3.0f; // Pull speed

            if (distance < magnetRange)
            {
                pickups[i].x += (dx / distance) * managetStrength;
                pickups[i].y += (dy / distance) * managetStrength;
            }
        }
    }
}

/**
 * Render Active Pickups on Screen
 */
void render_pickups(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    const char frameId[] = {
        SPR_PICKUP_A,
        SPR_PICKUP_B,
        SPR_PICKUP_C,
        SPR_PICKUP_D,
        SPR_PICKUP_E};

    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].active)
            continue;

        Pickup *pickup = &pickups[i];

        // Default full opacity
        Uint8 alpha = 255;

        if (pickup->despawning)
        {
            Uint32 timeSinceDespawning = SDL_GetTicks() - pickup->despawningTime;
            Uint32 lastSecondOfDespawning = pickup->despawningDuration - 1000;

            if (timeSinceDespawning >= lastSecondOfDespawning && timeSinceDespawning <= pickup->despawningDuration)
            {
                // Fade from 255 to 0 between 4000ms and 5000ms
                float fadeProgress = (timeSinceDespawning - lastSecondOfDespawning) / 1000.0f;
                alpha = (Uint8)(255 * (1.0f - fadeProgress));
            }
            else if (timeSinceDespawning > pickup->despawningDuration)
            {
                // Already handled by tick_picks - safe to pick
                continue;
            }
        }

        SDL_Rect src = get_sprite(frameId[pickups[i].animFrame]);
        SDL_Texture *texture = get_sprite_texture(frameId[pickups[i].animFrame]);

        SDL_Rect dst = pickups[i].rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_RenderCopy(renderer, texture, &src, &dst);
        SDL_SetTextureAlphaMod(texture, 255); // Reset for other uses
    }
}
