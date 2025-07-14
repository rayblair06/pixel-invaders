#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void render_menu(SDL_Renderer *renderer, TTF_Font *font, const char *options[], int optionCount, int selectedIndex, int yStart);

#endif
