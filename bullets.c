#include "constants.h"
#include "bullets.h"
#include "sprites.h"

Entity bullets[MAX_BULLETS];
float bulletSpeed = 6.0f;

void init_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }
}

void spawn_bullet(float x, float y)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            // Define the bulletCenter so when we render, we render include the bullet width to ensure bullets are rendered center to the created object
            const int bulletCenter = SPRITE_DRAW_SIZE / 2;

            bullets[i] = create_entity(
                x - bulletCenter,
                y,
                SPRITE_DRAW_SIZE,
                SPRITE_DRAW_SIZE);

            break;
        }
    }
}

void update_bullets(void)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        move(&bullets[i], UP, bulletSpeed);

        // Disactivate when off screen
        if (bullets[i].y + bullets[i].h < 0)
        {
            bullets[i].active = false;
        }
    }
}

void render_bullets(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY)
{
    SDL_Rect src = get_sprite(SPR_BULLET1);

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        SDL_Rect dst = bullets[i].rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, spriteTexture, &src, &dst);
    }
}
