#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>

#define SPRITE_SIZE 32
#define SPRITE_DRAW_SIZE SPRITE_SIZE * 2

typedef enum
{
    // Asteroids
    SPR_ASTERRIOD1,
    SPR_ASTERRIOD2,

    // Backgrounds
    BG_TILE1,
    BG_TILE2,

    // BackgroundPlanets
    BG_PLANET1,
    BG_PLANET2,
    BG_PLANET3,
    BG_PLANET4,

    // Explosions
    SPR_EXPLOSION1_A,
    SPR_EXPLOSION1_B,
    SPR_EXPLOSION1_C,
    SPR_EXPLOSION1_D,
    SPR_EXPLOSION1_E,

    // Powerups
    SPR_POWERUP_1,
    SPR_POWERUP_2,
    SPR_POWERUP_3,
    SPR_POWERUP_4,
    SPR_POWERUP_5,

    // Projectiles
    SPR_PROJECTILE_1,
    SPR_PROJECTILE_2,
    SPR_PROJECTILE_3,
    SPR_PROJECTILE_4,
    SPR_PROJECTILE_5,
    SPR_PROJECTILE_6,

    // Spaceships
    SPR_SPACESHIP1_A,
    SPR_SPACESHIP1_B,
    SPR_SPACESHIP2_A,
    SPR_SPACESHIP2_B,
    SPR_SPACESHIP3_A,
    SPR_SPACESHIP3_B,
    SPR_SPACESHIP4_A,
    SPR_SPACESHIP5_A,
    SPR_SPACESHIP5_B,
    SPR_SPACESHIP6_A,
    SPR_SPACESHIP6_B,
    SPR_SPACESHIP7_A,
    SPR_SPACESHIP8_A,
    SPR_SPACESHIP8_B,
    SPR_SPACESHIP9_A,
    SPR_SPACESHIP9_B,

    SPR_COUNT
} SpriteID;

void init_sprites(SDL_Renderer *renderer);
void cleanup_sprites(void);

SDL_Rect get_sprite(SpriteID id);
SDL_Texture *get_sprite_texture(SpriteID id);

void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius);

#endif
