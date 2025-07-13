#include "constants.h"
#include "enemies.h"
#include "sprites.h"

Entity enemies[MAX_ENEMIES];
float enemySpeed = 1.0f;

int enemyFrameCounter = 0;
int enemyFrameDuration = 15;

/**
 * Initialise enemies as deactivated
 */
void init_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }
}

void spawn_enemy(float x, float y)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i] = create_entity(x, y, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);
            break;
        }
    }
}

void update_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        move(&enemies[i], DOWN, enemySpeed);

        if (enemies[i].y > SCREEN_HEIGHT)
        {
            // TODO: All of the handling from main.c should be moved to here such as:
            //  Disappearance and game actions
            // But due to the tight coupling of game events, it's not possible at this time
        }
    }
}

void render_enemies(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY)
{
    // Alternative animation frame
    SpriteID frame = (enemyFrameCounter / enemyFrameDuration) % 2 == 0 ? SPR_INVADER1_A : SPR_INVADER1_B;

    SDL_Rect src = get_sprite(frame);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        SDL_Rect dst = enemies[i].rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, spriteTexture, &src, &dst);
    }
}
