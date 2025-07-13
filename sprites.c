#include <SDL2/SDL_image.h>
#include "sprites.h"

static SDL_Rect spriteRects[SPR_COUNT];

void init_sprites()
{
    // Initialize Image package
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
    }

    spriteRects[SPR_PLAYER] = (SDL_Rect){4 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_BULLET1] = (SDL_Rect){2 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_BULLET2] = (SDL_Rect){2 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_BULLET3] = (SDL_Rect){2 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_INVADER1_A] = (SDL_Rect){0 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_INVADER1_B] = (SDL_Rect){1 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_INVADER2_A] = (SDL_Rect){0 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_INVADER2_B] = (SDL_Rect){1 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_INVADER3_A] = (SDL_Rect){0 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_INVADER3_B] = (SDL_Rect){1 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_UFO] = (SDL_Rect){3 * SPRITE_SIZE, 0 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_EXPLOSION_A] = (SDL_Rect){2 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_EXPLOSION_B] = (SDL_Rect){2 * SPRITE_SIZE, 4 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_SHIELD_LEFT] = (SDL_Rect){3 * SPRITE_SIZE, 2 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_SHIELD_MID] = (SDL_Rect){3 * SPRITE_SIZE, 1 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
    spriteRects[SPR_SHIELD_RIGHT] = (SDL_Rect){3 * SPRITE_SIZE, 3 * SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
}

SDL_Rect get_sprite(SpriteID id)
{
    return spriteRects[id];
}
