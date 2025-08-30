#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Common UI colours
static const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
static const SDL_Color COLOR_GOLD = {255, 215, 0, 255};
static const SDL_Color COLOR_YELLOW = {255, 255, 0, 255};
static const SDL_Color COLOR_RED = {255, 0, 0, 255};
static const SDL_Color COLOR_GREEN = {0, 255, 0, 255};
static const SDL_Color COLOR_BLUE = {0, 0, 255, 255};

void generate_text(TTF_Font *font, const char *text, int x, int y, SDL_Color color);
void render_stat(const char *label, int value, int x, int y, SDL_Color color);
void render_stats_panel(int x, int y, int lineHeight);

void render_panel(int x, int y, int w, int h);
void render_menu(const char *title, const char *options[], int optionCount, int selectedIndex, int paddingX, int paddingY);
void render_upgrade_menu(const char *title, const char *titles[], const char *descriptions[], int optionCount, int selectedIndex, int paddingX, int paddingY);
void render_health_bar(int x, int y, int width, int height);
void render_xp_bar(int x, int y, int width, int height);

void render_vignette();

#endif
