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
void render_pickups(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY)
{
    SDL_Rect src = get_sprite(SPR_BULLET3);

    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        if (!pickups[i].active)
            continue;

        SDL_Rect dst = pickups[i].rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, spriteTexture, &src, &dst);
    }
}
