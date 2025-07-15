#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

#define PANEL_SIZE 16
#define SPRITE_SIZE 16
#define TILE_SIZE 64
#define SPRITE_DRAW_SIZE SPRITE_SIZE * 2

#define MAX_SPRITESHEETS 8
#define MAX_SPRITES_PER_SHEET 64

typedef enum
{
    // background.png
    BG_TILE,

    // SpaceInvaders_BackgroundBuildings.png
    BG_TILE_BUILDING,

    // SpaceInvaders_BackgroundFloor.png
    BG_TILE_FLOOR,

    // SpaceInvaders_Borders.png
    MENU_TL,
    MENU_T,
    MENU_TR,
    MENU_L,
    MENU_C,
    MENU_R,
    MENU_BL,
    MENU_B,
    MENU_BR,

    // spritesheet.png
    // Row 0
    SPR_INVADER1_A,
    SPR_INVADER1_B,
    SPR_BULLET1,
    SPR_UFO,
    SPR_PLAYER,
    SPR_EMPTY_1,
    SPR_EMPTY_2,

    // Row 1
    SPR_INVADER2_A,
    SPR_INVADER2_B,
    SPR_BULLET2,
    SPR_SHIELD1_LEFT,
    SPR_SHIELD1_RIGHT,
    SPR_BULLET3,
    SPR_EMPTY_3,

    // Row 2
    SPR_INVADER3_A,
    SPR_INVADER3_B,
    SPR_BULLET4,
    SPR_SHIELD2_LEFT,
    SPR_SHIELD2_RIGHT,
    SPR_EMPTY_4,
    SPR_EMPTY_5,

    // Row 3
    SPR_INVADER4_A,
    SPR_INVADER4_B,
    SPR_EXPLOSION_A,
    SPR_SHIELD3_LEFT,
    SPR_SHIELD3_RIGHT,
    SPR_ARROW_LEFT,
    SPR_ARROW_RIGHT,

    // Row 4
    SPR_INVADER5_A,
    SPR_INVADER5_B,
    SPR_EXPLOSION_B,
    SPR_SHIELD4_LEFT,
    SPR_SHIELD4_RIGHT,
    SPR_EMPTY_6,
    SPR_EMPTY_7,

    // pickups.png
    // Row 0
    SPR_PICKUP_A,
    SPR_PICKUP_B,
    SPR_PICKUP_C,
    SPR_PICKUP_D,
    SPR_PICKUP_E,

    // Row 1
    SPR_S_PICKUP_A,
    SPR_S_PICKUP_B,
    SPR_EMPTY_8,
    SPR_EMPTY_9,
    SPR_EMPTY_10,
    SPR_EMPTY_11,

    // Row 2
    SPR_L_PICKUP_A,
    SPR_L_PICKUP_B,
    SPR_EMPTY_12,
    SPR_EMPTY_13,
    SPR_EMPTY_14,
    SPR_EMPTY_15,

    // Row 3
    SPR_H_PICKUP_A,
    SPR_H_PICKUP_B,
    SPR_EMPTY_16,
    SPR_EMPTY_17,
    SPR_EMPTY_18,
    SPR_EMPTY_19,

    SPR_COUNT
} SpriteID;

void init_sprites(SDL_Renderer *renderer);
void cleanup_sprites(void);

SDL_Rect get_sprite(SpriteID id);
SDL_Texture *get_sprite_texture(SpriteID id);

void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius);

#endif
