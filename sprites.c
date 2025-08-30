#include <math.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "game.h"
#include "sprites.h"

const char *spritePaths[SPR_COUNT] = {
    [SPR_NONE] = "assets/sprites/Explosion/explosion1.png",

    [SPR_ASTERRIOD1] = "assets/sprites/Asteroids/asteroid1.png",
    [SPR_ASTERRIOD2] = "assets/sprites/Asteroids/asteroid2.png",

    [BG_TILE1] = "assets/sprites/Backgrounds/background.png",
    [BG_TILE2] = "assets/sprites/Backgrounds/background starts.png",

    [BG_PLANET1] = "assets/sprites/Planets/planet1.png",
    [BG_PLANET2] = "assets/sprites/Planets/planet2.png",
    [BG_PLANET3] = "assets/sprites/Planets/planet3.png",
    [BG_PLANET4] = "assets/sprites/Planets/planet4.png",

    [SPR_EXPLOSION1_A] = "assets/sprites/Explosion/explosion1.png",
    [SPR_EXPLOSION1_B] = "assets/sprites/Explosion/explosion2.png",
    [SPR_EXPLOSION1_C] = "assets/sprites/Explosion/explosion3.png",
    [SPR_EXPLOSION1_D] = "assets/sprites/Explosion/explosion4.png",
    [SPR_EXPLOSION1_E] = "assets/sprites/Explosion/explosion5.png",

    [SPR_POWERUP_1] = "assets/sprites/Powerups/powerup1.png",
    [SPR_POWERUP_2] = "assets/sprites/Powerups/powerup2.png",
    [SPR_POWERUP_3] = "assets/sprites/Powerups/powerup3.png",
    [SPR_POWERUP_4] = "assets/sprites/Powerups/powerup4.png",
    [SPR_POWERUP_5] = "assets/sprites/Powerups/powerup5.png",

    [SPR_PROJECTILE_1] = "assets/sprites/Projectiles/projectile1.png",
    [SPR_PROJECTILE_2] = "assets/sprites/Projectiles/projectile2.png",
    [SPR_PROJECTILE_3] = "assets/sprites/Projectiles/projectile3.png",
    [SPR_PROJECTILE_4] = "assets/sprites/Projectiles/projectile4.png",
    [SPR_PROJECTILE_5] = "assets/sprites/Projectiles/projectile5.png",
    [SPR_PROJECTILE_6] = "assets/sprites/Projectiles/projectile6.png",

    [SPR_LASER_CHARING1_A] = "assets/sprites/Projectiles/Blast/blast1.png",
    [SPR_LASER_CHARING1_B] = "assets/sprites/Projectiles/Blast/blast2.png",
    [SPR_LASER_CHARING1_C] = "assets/sprites/Projectiles/Blast/blast3.png",
    [SPR_LASER_CHARING1_D] = "assets/sprites/Projectiles/Blast/blast4.png",
    [SPR_LASER_CHARING1_E] = "assets/sprites/Projectiles/Blast/blast5.png",
    [SPR_LASER_CHARING1_F] = "assets/sprites/Projectiles/Blast/blast6.png",

    [SPR_LASER_FIRING1_A] = "assets/sprites/Projectiles/Blast/blast7.png",
    [SPR_LASER_FIRING1_B] = "assets/sprites/Projectiles/Blast/blast8.png",
    [SPR_LASER_FIRING1_C] = "assets/sprites/Projectiles/Blast/blast9.png",
    [SPR_LASER_FIRING1_D] = "assets/sprites/Projectiles/Blast/blast10.png",
    [SPR_LASER_FIRING1_E] = "assets/sprites/Projectiles/Blast/blast11.png",
    [SPR_LASER_FIRING1_F] = "assets/sprites/Projectiles/Blast/blast12.png",

    [SPR_LASER_DEACTIVATING1_A] = "assets/sprites/Projectiles/Blast/blast13.png",
    [SPR_LASER_DEACTIVATING1_B] = "assets/sprites/Projectiles/Blast/blast14.png",
    [SPR_LASER_DEACTIVATING1_C] = "assets/sprites/Projectiles/Blast/blast15.png",

    [SPR_SPACESHIP1_A] = "assets/sprites/Spaceships/Spaceship 1/spaceship1-1.png",
    [SPR_SPACESHIP1_B] = "assets/sprites/Spaceships/Spaceship 1/spaceship1-2.png",
    [SPR_SPACESHIP2_A] = "assets/sprites/Spaceships/Spaceship 2/spaceship2-1.png",
    [SPR_SPACESHIP2_B] = "assets/sprites/Spaceships/Spaceship 2/spaceship2-2.png",
    [SPR_SPACESHIP3_A] = "assets/sprites/Spaceships/Spaceship 3/spaceship3-1.png",
    [SPR_SPACESHIP3_B] = "assets/sprites/Spaceships/Spaceship 3/spaceship3-2.png",
    [SPR_SPACESHIP4_A] = "assets/sprites/Spaceships/Spaceship 4/spaceship4.png",
    [SPR_SPACESHIP5_A] = "assets/sprites/Spaceships/Spaceship 5/spaceship5-1.png",
    [SPR_SPACESHIP5_B] = "assets/sprites/Spaceships/Spaceship 5/spaceship5-2.png",
    [SPR_SPACESHIP6_A] = "assets/sprites/Spaceships/Spaceship 6/spaceship6-1.png",
    [SPR_SPACESHIP6_B] = "assets/sprites/Spaceships/Spaceship 6/spaceship6-2.png",
    [SPR_SPACESHIP7_A] = "assets/sprites/Spaceships/Spaceship 7/spaceship7.png",
    [SPR_SPACESHIP8_A] = "assets/sprites/Spaceships/Spaceship 8/spaceship8-1.png",
    [SPR_SPACESHIP8_B] = "assets/sprites/Spaceships/Spaceship 8/spaceship8-2.png",
    [SPR_SPACESHIP9_A] = "assets/sprites/Spaceships/Spaceship 9/spaceship9-1.png",
    [SPR_SPACESHIP9_B] = "assets/sprites/Spaceships/Spaceship 9/spaceship9-2.png",
};

typedef struct
{
    SDL_Texture *texture;
    int width;
    int height;
} Sprite;

/**
 * Animation Sets
 */
static const SpriteID spriteFallbackFrames[] = {
    SPR_EXPLOSION1_A,
    SPR_EXPLOSION1_B,
    SPR_EXPLOSION1_C,
    SPR_EXPLOSION1_D,
    SPR_EXPLOSION1_E};

const SpriteAnimation spriteFallbackAnim = {
    .frames = (SpriteID *)spriteFallbackFrames,
    .frameCount = sizeof(spriteFallbackFrames) / sizeof(spriteFallbackFrames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID explosion1Frames[] = {
    SPR_EXPLOSION1_A,
    SPR_EXPLOSION1_B,
    SPR_EXPLOSION1_C,
    SPR_EXPLOSION1_D,
    SPR_EXPLOSION1_E};

const SpriteAnimation explosion1Anim = {
    .frames = (SpriteID *)explosion1Frames,
    .frameCount = sizeof(explosion1Frames) / sizeof(explosion1Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = false,
    .hasEnded = false,
};

static const SpriteID powerup1Frames[] = {
    SPR_POWERUP_1};

const SpriteAnimation powerup1Anim = {
    .frames = (SpriteID *)powerup1Frames,
    .frameCount = sizeof(powerup1Frames) / sizeof(powerup1Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship1Frames[] = {
    SPR_SPACESHIP1_A,
    SPR_SPACESHIP1_B};

const SpriteAnimation spaceship1Anim = {
    .frames = (SpriteID *)spaceship1Frames,
    .frameCount = sizeof(spaceship1Frames) / sizeof(spaceship1Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship2Frames[] = {
    SPR_SPACESHIP2_A,
    SPR_SPACESHIP2_B};

const SpriteAnimation spaceship2Anim = {
    .frames = (SpriteID *)spaceship2Frames,
    .frameCount = sizeof(spaceship2Frames) / sizeof(spaceship2Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship3Frames[] = {
    SPR_SPACESHIP3_A,
    SPR_SPACESHIP3_B};

const SpriteAnimation spaceship3Anim = {
    .frames = (SpriteID *)spaceship3Frames,
    .frameCount = sizeof(spaceship3Frames) / sizeof(spaceship3Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship4Frames[] = {
    SPR_SPACESHIP4_A};

const SpriteAnimation spaceship4Anim = {
    .frames = (SpriteID *)spaceship4Frames,
    .frameCount = sizeof(spaceship4Frames) / sizeof(spaceship4Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship5Frames[] = {
    SPR_SPACESHIP5_A,
    SPR_SPACESHIP5_B};

const SpriteAnimation spaceship5Anim = {
    .frames = (SpriteID *)spaceship5Frames,
    .frameCount = sizeof(spaceship5Frames) / sizeof(spaceship5Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship6Frames[] = {
    SPR_SPACESHIP6_A,
    SPR_SPACESHIP6_B};

const SpriteAnimation spaceship6Anim = {
    .frames = (SpriteID *)spaceship6Frames,
    .frameCount = sizeof(spaceship6Frames) / sizeof(spaceship6Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship7Frames[] = {
    SPR_SPACESHIP7_A};

const SpriteAnimation spaceship7Anim = {
    .frames = (SpriteID *)spaceship7Frames,
    .frameCount = sizeof(spaceship7Frames) / sizeof(spaceship7Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship8Frames[] = {
    SPR_SPACESHIP8_A,
    SPR_SPACESHIP8_B};

const SpriteAnimation spaceship8Anim = {
    .frames = (SpriteID *)spaceship8Frames,
    .frameCount = sizeof(spaceship8Frames) / sizeof(spaceship8Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static const SpriteID spaceship9Frames[] = {
    SPR_SPACESHIP9_A,
    SPR_SPACESHIP9_B};

const SpriteAnimation spaceship9Anim = {
    .frames = (SpriteID *)spaceship9Frames,
    .frameCount = sizeof(spaceship9Frames) / sizeof(spaceship9Frames[0]),
    .frameTime = 0.1f,
    .currentFrame = 0,
    .frameTimer = 0.0f,
    .loop = true,
    .hasEnded = false,
};

static Sprite sprites[SPR_COUNT];
static int loadedSpriteCount = 0;

void init_sprites()
{
    SDL_Renderer *renderer = app()->renderer;

    // Initialize Image package
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
    }

    loadedSpriteCount = 0;

    // Dynamically load sprites in
    for (int i = 0; i < SPR_COUNT; i++)
    {
        SDL_Surface *sprite = IMG_Load(spritePaths[i]);

        if (!sprite)
        {
            printf("Failed to load sprite: %s\n", IMG_GetError());
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, sprite);

        if (!texture)
        {
            printf("Failed to create texture: %s\n", SDL_GetError());
            SDL_FreeSurface(sprite);
            continue;
        }

        sprites[loadedSpriteCount].texture = texture;
        sprites[loadedSpriteCount].width = sprite->w;
        sprites[loadedSpriteCount].height = sprite->h;

        SDL_FreeSurface(sprite);
        loadedSpriteCount++;
    }
}

void cleanup_sprites(void)
{
    for (int i = 0; i < loadedSpriteCount; i++)
    {
        if (sprites[i].texture)
        {
            SDL_DestroyTexture(sprites[i].texture);
        }
    }

    loadedSpriteCount = 0;

    IMG_Quit();
}

SDL_Rect get_sprite(SpriteID id)
{
    Sprite *sprite = &sprites[id];

    // x and y are zero is they are single images, not spritesheet
    SDL_Rect rect = {
        0,
        0,
        sprite->width,
        sprite->height};

    return rect;
}

SDL_Texture *get_sprite_texture(SpriteID id)
{
    Sprite *sprite = &sprites[id];

    return sprite->texture;
}

/**
 * Draw a Circle
 * Maybe move to utils lib
 */
void draw_circle(int cx, int cy, int radius)
{
    SDL_Renderer *renderer = app()->renderer;

    const int segments = 64;
    float angleStep = 2 * M_PI / segments;

    for (int i = 0; i < segments; i++)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        int x1 = cx + radius * cosf(angle1);
        int y1 = cy + radius * sinf(angle1);
        int x2 = cx + radius * cosf(angle2);
        int y2 = cy + radius * sinf(angle2);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}
