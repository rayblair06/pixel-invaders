#ifndef FONTS_H
#define FONTS_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum
{
    FT_SMALL,
    FT_MEDIUM,
    FT_LARGE,

    FONTSIZE_COUNT,
} FontSize;

typedef enum
{
    FT_PRIMARY,
    FT_SECONDARY,

    FONTSTYLE_COUNT,
} FontStyle;

bool init_fonts(void);
TTF_Font *get_font(FontStyle font, FontSize size);
void cleanup_fonts(void);

#endif
