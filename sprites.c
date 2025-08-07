#include <math.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "sprites.h"

const char *spritePaths[SPR_COUNT] = {
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

static Sprite sprites[SPR_COUNT];
static int loadedSpriteCount = 0;

void init_sprites(SDL_Renderer *renderer)
{
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
void draw_circle(SDL_Renderer *renderer, int cx, int cy, int radius)
{
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
