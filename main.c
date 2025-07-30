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
#include "level_manager.h"
#include "game.h"
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

    // Initialize Fonts
    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 24);

    if (!font)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Pixel Invaders: Rogue Mode",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    init_sprites(renderer);
    init_audio();

    // Play menu music
    play_music(MUS_MENU, true);

    // Main game loop
    SDL_Event event;

    // ✅ Force keyboard state to update
    SDL_PumpEvents();
    Uint8 prevKeystateBuffer[SDL_NUM_SCANCODES] = {0};

    // Start of deltaTime
    Uint32 lastTick = SDL_GetTicks();

    // Main loop
    while (running)
    {
        Uint32 now = SDL_GetTicks();
        deltaTime = (now - lastTick) / 1000.0f;
        lastTick = now;

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
            renderer,
            font,
            keystate,
            prevKeystate);

        memcpy(prevKeystateBuffer, SDL_GetKeyboardState(NULL), SDL_NUM_SCANCODES);

        // Show the render
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS

        fflush(stdout);
    }

    // Clean up
    cleanup_sprites();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    cleanup_audio();
    SDL_Quit();
    TTF_Quit();

    return 0;
}
