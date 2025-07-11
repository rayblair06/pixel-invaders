#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

#define SPRITE_SIZE 16
#define SPRITE_DRAW_SIZE SPRITE_SIZE * 2

typedef enum
{
    SPR_PLAYER,
    SPR_BULLET1,
    SPR_BULLET2,
    SPR_INVADER1_A,
    SPR_INVADER1_B,
    SPR_INVADER2_A,
    SPR_INVADER2_B,
    SPR_INVADER3_A,
    SPR_INVADER3_B,
    SPR_UFO,
    SPR_EXPLOSION1,
    SPR_EXPLOSION2,
    SPR_SHIELD_LEFT,
    SPR_SHIELD_MID,
    SPR_SHIELD_RIGHT,
    SPR_COUNT
} SpriteID;

void init_sprites(void);
SDL_Rect get_sprite(SpriteID id);

#endif
