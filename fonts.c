#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "fonts.h"

static const int FONT_SIZES[FONTSIZE_COUNT] = {
    [FT_SMALL] = 16,
    [FT_MEDIUM] = 24,
    [FT_LARGE] = 32,
};

static const char *FONT_PATHS[FONTSTYLE_COUNT] = {
    [FT_PRIMARY] = "assets/fonts/PixelifySans-SemiBold.ttf",
    [FT_SECONDARY] = "assets/fonts/Roboto-Regular.ttf",
};

static TTF_Font *g_fonts[FONTSTYLE_COUNT][FONTSIZE_COUNT];
static int loadedFonts = 0;

bool init_fonts(void)
{
    // Initialize Fonts Lib
    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    for (int i = 0; i < FONTSTYLE_COUNT; i++)
    {
        for (int j = 0; j < FONTSIZE_COUNT; j++)
        {
            const char *path = FONT_PATHS[i];
            const int pts = FONT_SIZES[j];

            TTF_Font *font = TTF_OpenFont(path, pts);

            if (!font)
            {
                printf("Failed to load font: %s\n", TTF_GetError());
                return 1;
            }

            g_fonts[i][j] = font;

            loadedFonts++;
        }
    }

    return true;
}

TTF_Font *get_font(FontStyle style, FontSize size)
{
    return g_fonts[style][size];
}

void cleanup_fonts(void)
{
    for (int i = 0; i < FONTSTYLE_COUNT; i++)
    {
        for (int j = 0; j < FONTSIZE_COUNT; j++)
        {
            TTF_CloseFont(g_fonts[i][j]);
            g_fonts[i][j] = NULL;
        }
    }

    loadedFonts = 0;
    TTF_Quit();
}
