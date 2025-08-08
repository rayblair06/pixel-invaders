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

typedef struct
{
    SpriteID *frames; // pointer to array of sprite IDs
    int frameCount;   // how many frames in animation
    float frameTime;  // duration per frame
    float frameTimer; // accumulates time
    int currentFrame; // current frame index
} SpriteAnimation;

/**
 * Animation Sets
 */
static const SpriteID explosion1Frames[] = {
    SPR_EXPLOSION1_A,
    SPR_EXPLOSION1_B,
    SPR_EXPLOSION1_C,
    SPR_EXPLOSION1_D,
    SPR_EXPLOSION1_E};

static const SpriteAnimation explosion1Anim = {
    .frames = (SpriteID *)explosion1Frames,
    .frameCount = sizeof(explosion1Frames) / sizeof(explosion1Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship1Frames[] = {
    SPR_SPACESHIP1_A,
    SPR_SPACESHIP1_B};

static const SpriteAnimation spaceship1Anim = {
    .frames = (SpriteID *)spaceship1Frames,
    .frameCount = sizeof(spaceship1Frames) / sizeof(spaceship1Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship2Frames[] = {
    SPR_SPACESHIP2_A,
    SPR_SPACESHIP2_B};

static const SpriteAnimation spaceship2Anim = {
    .frames = (SpriteID *)spaceship2Frames,
    .frameCount = sizeof(spaceship2Frames) / sizeof(spaceship2Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship3Frames[] = {
    SPR_SPACESHIP3_A,
    SPR_SPACESHIP3_B};

static const SpriteAnimation spaceship3Anim = {
    .frames = (SpriteID *)spaceship3Frames,
    .frameCount = sizeof(spaceship3Frames) / sizeof(spaceship3Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship4Frames[] = {
    SPR_SPACESHIP4_A};

static const SpriteAnimation spaceship4Anim = {
    .frames = (SpriteID *)spaceship4Frames,
    .frameCount = sizeof(spaceship4Frames) / sizeof(spaceship4Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship5Frames[] = {
    SPR_SPACESHIP5_A,
    SPR_SPACESHIP5_B};

static const SpriteAnimation spaceship5Anim = {
    .frames = (SpriteID *)spaceship5Frames,
    .frameCount = sizeof(spaceship5Frames) / sizeof(spaceship5Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship6Frames[] = {
    SPR_SPACESHIP6_A,
    SPR_SPACESHIP6_B};

static const SpriteAnimation spaceship6Anim = {
    .frames = (SpriteID *)spaceship6Frames,
    .frameCount = sizeof(spaceship6Frames) / sizeof(spaceship6Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship7Frames[] = {
    SPR_SPACESHIP7_A};

static const SpriteAnimation spaceship7Anim = {
    .frames = (SpriteID *)spaceship7Frames,
    .frameCount = sizeof(spaceship7Frames) / sizeof(spaceship7Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship8Frames[] = {
    SPR_SPACESHIP8_A,
    SPR_SPACESHIP8_B};

static const SpriteAnimation spaceship8Anim = {
    .frames = (SpriteID *)spaceship8Frames,
    .frameCount = sizeof(spaceship8Frames) / sizeof(spaceship8Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

static const SpriteID spaceship9Frames[] = {
    SPR_SPACESHIP8_A,
    SPR_SPACESHIP8_B};

static const SpriteAnimation spaceship9Anim = {
    .frames = (SpriteID *)spaceship9Frames,
    .frameCount = sizeof(spaceship9Frames) / sizeof(spaceship9Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
};

void init_sprites(SDL_Renderer *renderer);
void cleanup_sprites(void);

SDL_Rect get_sprite(SpriteID id);
SDL_Texture *get_sprite_texture(SpriteID id);

void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius);

#endif
