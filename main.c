#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "audio.h"
#include "boss.h"
#include "constants.h"
#include "collisions.h"
#include "bullets.h"
#include "enemies.h"
#include "entity.h"
#include "fonts.h"
#include "game.h"
#include "level_manager.h"
#include "options.h"
#include "sprites.h"
#include "particles.h"
#include "pickups.h"
#include "player.h"
#include "scene_manager.h"
#include "stats.h"
#include "ui.h"
#include "upgrades.h"
#include "waves.h"
#include "version.h"

bool running = true;

void stop_game(void)
{
    running = false;
}

int main(int argc, char *argv[])
{

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    init_app();
    init_fonts();

    // Icon
    SDL_Surface *icon = IMG_Load("assets/icon.png");
    if (icon)
    {
        SDL_SetWindowIcon(app()->window, icon);
        SDL_FreeSurface(icon);
    }

    options_init();

    init_sprites();
    init_audio();

    // Play menu music
    play_music(MUS_MENU, true);

    // Main game loop
    SDL_Event event;

    // ✅ Force keyboard state to update
    SDL_PumpEvents();
    Uint8 prevKeystateBuffer[SDL_NUM_SCANCODES] = {0};

    // Main loop
    while (running)
    {
        update_game_time();

        // Black out the background, clear the old render so we don't get flicker
        SDL_SetRenderDrawColor(app()->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app()->renderer);

        // Handle events and keystates
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        const Uint8 *prevKeystate = prevKeystateBuffer;

        manage_scene(
            keystate,
            prevKeystate);

        memcpy(prevKeystateBuffer, SDL_GetKeyboardState(NULL), SDL_NUM_SCANCODES);

        // Show the new render
        SDL_RenderPresent(app()->renderer);

        SDL_Delay(16); // ~60 FPS

        fflush(stdout);
    }

    // Clean up
    cleanup_sprites();
    SDL_DestroyRenderer(app()->renderer);
    SDL_DestroyWindow(app()->window);
    cleanup_fonts();
    cleanup_audio();
    SDL_Quit();
    TTF_Quit();

    return 0;
}
