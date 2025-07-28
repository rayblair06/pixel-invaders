#include <math.h>
#include <SDL_image.h>
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

    SDL_Surface *backgroundTile = IMG_Load("assets/sprites/background.png");

    if (!backgroundTile)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, backgroundTile);
    sheets[sheetCount].cols = backgroundTile->w / TILE_SIZE;
    sheets[sheetCount].rows = backgroundTile->h / TILE_SIZE;
    sheets[sheetCount].tileSize = TILE_SIZE;
    sheets[sheetCount].offset = 0;
    sheets[sheetCount].count = 1;

    SDL_FreeSurface(backgroundTile);
    sheetCount++;

    SDL_Surface *backgroundBuildingTile = IMG_Load("assets/sprites/SpaceInvaders_BackgroundBuildings.png");

    if (!backgroundBuildingTile)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, backgroundBuildingTile);
    sheets[sheetCount].cols = backgroundBuildingTile->w / TILE_SIZE;
    sheets[sheetCount].rows = backgroundBuildingTile->h / TILE_SIZE;
    sheets[sheetCount].tileSize = TILE_SIZE;
    sheets[sheetCount].offset = 1;
    sheets[sheetCount].count = 1;

    SDL_FreeSurface(backgroundBuildingTile);
    sheetCount++;

    SDL_Surface *backgroundFloorTile = IMG_Load("assets/sprites/SpaceInvaders_BackgroundFloor.png");

    if (!backgroundFloorTile)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, backgroundFloorTile);
    sheets[sheetCount].cols = backgroundFloorTile->w / TILE_SIZE;
    sheets[sheetCount].rows = backgroundFloorTile->h / TILE_SIZE;
    sheets[sheetCount].tileSize = TILE_SIZE;
    sheets[sheetCount].offset = 2;
    sheets[sheetCount].count = 1;

    SDL_FreeSurface(backgroundFloorTile);
    sheetCount++;

    SDL_Surface *menuPanel = IMG_Load("assets/sprites/SpaceInvaders_Borders.png");

    if (!menuPanel)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, menuPanel);
    sheets[sheetCount].cols = menuPanel->w / PANEL_SIZE;
    sheets[sheetCount].rows = menuPanel->h / PANEL_SIZE;
    sheets[sheetCount].tileSize = PANEL_SIZE;
    sheets[sheetCount].offset = 3;
    sheets[sheetCount].count = 9;

    SDL_FreeSurface(menuPanel);
    sheetCount++;

    SDL_Surface *sheet0 = IMG_Load("assets/sprites/spritesheet.png");

    if (!sheet0)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, sheet0);
    sheets[sheetCount].cols = sheet0->w / SPRITE_SIZE;
    sheets[sheetCount].rows = sheet0->h / SPRITE_SIZE;
    sheets[sheetCount].tileSize = SPRITE_SIZE;
    sheets[sheetCount].offset = 12;
    sheets[sheetCount].count = 7 * 5;

    SDL_FreeSurface(sheet0);
    sheetCount++;

    SDL_Surface *sheet1 = IMG_Load("assets/sprites/SpaceInvaders_Pickups.png");

    if (!sheet1)
    {
        printf("Failed to load spritesheet.png: %s\n", IMG_GetError());
    }

    sheets[sheetCount].texture = SDL_CreateTextureFromSurface(renderer, sheet1);
    sheets[sheetCount].cols = sheet1->w / SPRITE_SIZE;
    sheets[sheetCount].rows = sheet1->h / SPRITE_SIZE;
    sheets[sheetCount].tileSize = SPRITE_SIZE;
    sheets[sheetCount].offset = 12 + 7 * 5;
    sheets[sheetCount].count = 6 * 4;

    SDL_FreeSurface(sheet1);
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

    IMG_Quit();
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
        float angle1 = 1 * angleStep;
        float angle2 = (i + 1) * angleStep;

        int x1 = cx + radius * cosf(angle1);
        int y1 = cy + radius * sinf(angle1);
        int x2 = cx + radius * cosf(angle2);
        int y2 = cy + radius * sinf(angle2);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}
