#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

void debug_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}
