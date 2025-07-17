#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void generate_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);
void render_stat(SDL_Renderer *renderer, TTF_Font *font, const char *label, int value, int x, int y, SDL_Color color);
void render_stats_panel(SDL_Renderer *renderer, TTF_Font *font, int x, int y, int lineHeight);

void render_panel(SDL_Renderer *renderer, int x, int y, int w, int h);
void render_menu(SDL_Renderer *renderer, TTF_Font *font, const char *title, const char *options[], int optionCount, int selectedIndex, int paddingX, int paddingY);

#endif
