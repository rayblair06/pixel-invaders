#ifndef OPTIONS_H
#define OPTIONS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef enum
{
    VIDEO_WINDOWED,
    VIDEO_BORDERLESS,
    VIDEO_FULLSCREEN,
    VIDEO_MODE_COUNT,
} VideoMode;

typedef struct
{
    int w, h;
} Resolution;

typedef struct
{
    VideoMode videoMode;
    int resolutionIndex;
} VideoSettings;

extern VideoSettings gVideo;
extern Resolution gResList[];
extern int gResCount;

// lifecycle
void options_init(SDL_Window *window);
void options_apply(SDL_Window *window);
void options_render(SDL_Renderer *renderer, TTF_Font *font);
void options_handle_input(const Uint8 *keys, const Uint8 *prev);

// helpers
void options_select_next_row(void);
void options_select_prev_row(void);
void options_adjust_value(int direction);

bool options_is_open(void);
void options_open(void);
void options_close(void);

#endif
