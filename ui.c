#include "constants.h"
#include "sprites.h"
#include "ui.h"

void render_panel(SDL_Renderer *renderer, int x, int y, int w, int h)
{
    const char tiles[] = {
        MENU_TL,
        MENU_T,
        MENU_TR,
        MENU_L,
        MENU_C,
        MENU_R,
        MENU_BL,
        MENU_B,
        MENU_BR};
    const int tileSize = TILE_SIZE;

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            int index = row * 3 + col;

            SDL_Rect src = get_sprite(tiles[index]);
            SDL_Texture *texture = get_sprite_texture(tiles[index]);

            int dstW = (col == 1) ? w - 2 * tileSize : tileSize;
            int dstH = (row == 1) ? h - 2 * tileSize : tileSize;

            int dstX = x + (col == 0 ? 0 : (col == 1 ? tileSize : w - tileSize));
            int dstY = y + (row == 0 ? 0 : (row == 1 ? tileSize : h - tileSize));

            SDL_Rect dst = {dstX, dstY, dstW, dstH};

            SDL_RenderCopy(renderer, texture, &src, &dst);
        }
    }
}

void render_menu(SDL_Renderer *renderer, TTF_Font *font, const char *options[], int optionCount, int selectedIndex, int paddingX, int paddingY)
{
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};

    int panelWidth = 300 + paddingX * 2;
    int panelHeight = optionCount * 40 + paddingY * 2;

    int panelX = SCREEN_WIDTH / 2 - panelWidth / 2;
    int panelY = SCREEN_HEIGHT / 2 - panelHeight / 2;

    render_panel(renderer, panelX, panelY, panelWidth, panelHeight);

    for (int i = 0; i < optionCount; i++)
    {
        const char *label = options[i];

        SDL_Surface *surface = TTF_RenderText_Solid(font, label, i == selectedIndex ? yellow : white);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dst = {
            panelX + paddingX + (panelWidth - 2 * paddingX) / 2 - surface->w / 2,
            panelY + paddingY + i * 40,
            surface->w,
            surface->h};

        SDL_RenderCopy(renderer, texture, NULL, &dst);

        if (i == selectedIndex)
        {
            SDL_Rect arrow = {dst.x - 30, dst.y + 4, 10, 10};
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &arrow);
        }

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}
