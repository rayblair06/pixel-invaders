#include "constants.h"
#include "ui.h"

void render_menu(SDL_Renderer *renderer, TTF_Font *font, const char *options[], int optionCount, int selectedIndex, int yStart)
{
    SDL_Color white = {255, 255, 255};
    SDL_Color yellow = {255, 255, 0};

    for (int i = 0; i < optionCount; i++)
    {
        const char *label = options[i];

        SDL_Surface *surface = TTF_RenderText_Solid(font, label, i == selectedIndex ? yellow : white);

        if (!surface)
            continue;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        if (!texture)
        {
            SDL_FreeSurface(surface);
            continue;
        }

        SDL_Rect dst = {
            SCREEN_WIDTH / 2 - surface->w / 2,
            yStart + i * 40,
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
