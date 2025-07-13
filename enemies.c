#include "constants.h"
#include "enemies.h"
#include "sprites.h"

Entity enemies[MAX_ENEMIES];
float enemySpeed = 1.0f;

bool enemyFrameToggle = false;
Uint32 lastFrameSwitch = 0;
const Uint32 frameInterval = 500; // ms

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

/**
 * Spawn a single enemy entity
 */
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

/**
 * Handle functionality of active enemies
 */
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

/**
 * Render Active Enemies on Screen
 */
void render_enemies(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY)
{
    Uint32 now = SDL_GetTicks();

    // Toggle enemy animations
    if (now - lastFrameSwitch > frameInterval)
    {
        enemyFrameToggle = !enemyFrameToggle;
        lastFrameSwitch = SDL_GetTicks();
    }

    // Alternative animation frame
    SpriteID frame = enemyFrameToggle ? SPR_INVADER1_A : SPR_INVADER1_B;

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
