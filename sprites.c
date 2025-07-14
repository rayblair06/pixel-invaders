#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "sprites.h"

typedef struct
{
    SDL_Texture *texture;
    int cols;
    int rows;
    int tileSize;
    int offset;
    int count;
} Spritesheet;

static Spritesheet sheets[MAX_SPRITESHEETS];
static int sheetCount = 0;

void init_sprites(SDL_Renderer *renderer)
{
    // Initialize Image package
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
    }

    sheetCount = 0;

    SDL_Surface *sheet0 = IMG_Load("assets/sprites/spritesheet.png");

    if (!sheet0)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, sheet0);
    sheets[sheetCount].cols = sheet0->w / SPRITE_SIZE;
    sheets[sheetCount].rows = sheet0->h / SPRITE_SIZE;
    sheets[sheetCount].tileSize = SPRITE_SIZE;
    sheets[sheetCount].offset = 0;
    sheets[sheetCount].count = 40;

    SDL_FreeSurface(sheet0);
    sheetCount++;
}

void cleanup_sprites(void)
{
    for (int i = 0; i < sheetCount; i++)
    {
        if (sheets[i].texture)
        {
            SDL_DestroyTexture(sheets[i].texture);
        }
    }

    sheetCount = 0;
}

SDL_Rect get_sprite(SpriteID id)
{
    for (int i = 0; i < sheetCount; i++)
    {
        if (id >= sheets[i].offset && id < sheets[i].offset + sheets[i].count)
        {
            int localID = id - sheets[i].offset;
            int col = localID % sheets[i].cols;
            int row = localID / sheets[i].cols;

            SDL_Rect rect = {
                col * sheets[i].tileSize,
                row * sheets[i].tileSize,
                sheets[i].tileSize,
                sheets[i].tileSize};

            return rect;
        }
    }

    // Fallback for empty rect
    SDL_Rect fallback = {0, 0, SPRITE_SIZE, SPRITE_SIZE};
    return fallback;
}

SDL_Texture *get_sprite_texture(SpriteID id)
{
    for (int i = 0; i < sheetCount; i++)
    {
        if (id >= sheets[i].offset && id < sheets[i].offset + sheets[i].count)
        {
            return sheets[i].texture;
        }
    }

    return NULL;
}
