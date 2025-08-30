#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define SPRITE_SIZE 32
#define SPRITE_DRAW_SIZE SPRITE_SIZE * 2

typedef enum
{
    // Fallback (No image)
    SPR_NONE,

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

    // Laser
    SPR_LASER_CHARING1_A,
    SPR_LASER_CHARING1_B,
    SPR_LASER_CHARING1_C,
    SPR_LASER_CHARING1_D,
    SPR_LASER_CHARING1_E,
    SPR_LASER_CHARING1_F,

    SPR_LASER_FIRING1_A,
    SPR_LASER_FIRING1_B,
    SPR_LASER_FIRING1_C,
    SPR_LASER_FIRING1_D,
    SPR_LASER_FIRING1_E,
    SPR_LASER_FIRING1_F,

    SPR_LASER_DEACTIVATING1_A,
    SPR_LASER_DEACTIVATING1_B,
    SPR_LASER_DEACTIVATING1_C,

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

typedef struct
{
    SpriteID *frames; // pointer to array of sprite IDs
    int frameCount;   // how many frames in animation
    float frameTime;  // duration per frame
    float frameTimer; // accumulates time
    int currentFrame; // current frame index
    bool loop;        // animation loops
    bool hasEnded;    // animation has ended (only if loop = false)
} SpriteAnimation;

extern const SpriteAnimation spriteFallbackAnim;
extern const SpriteAnimation explosion1Anim;
extern const SpriteAnimation powerup1Anim;
extern const SpriteAnimation spaceship1Anim;
extern const SpriteAnimation spaceship2Anim;
extern const SpriteAnimation spaceship3Anim;
extern const SpriteAnimation spaceship4Anim;
extern const SpriteAnimation spaceship5Anim;
extern const SpriteAnimation spaceship6Anim;
extern const SpriteAnimation spaceship7Anim;
extern const SpriteAnimation spaceship8Anim;
extern const SpriteAnimation spaceship9Anim;

void init_sprites();
void cleanup_sprites(void);

SDL_Rect get_sprite(SpriteID id);
SDL_Texture *get_sprite_texture(SpriteID id);

void draw_circle(int cx, int cy, int radius);

#endif
