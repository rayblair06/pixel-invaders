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

        // Stop falling at player Y
        if (pickups[i].y >= player.y)
        {
            pickups[i].y = player.y;
        }

        // Check for collision with player
        if (SDL_HasIntersection(&player.rect, &pickups[i].rect))
        {
            pickups[i].active = false;

            add_experience(100);
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

        SDL_Rect src = get_sprite(frameId[pickups[i].animFrame]);
        SDL_Texture *texture = get_sprite_texture(frameId[pickups[i].animFrame]);

        SDL_Rect dst = pickups[i].rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}
