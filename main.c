#include <SDL2/SDL.h>
#include <stdbool.h>

const int SCREEN_WIDTH = 800 const

    int
    main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Space Wars",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERED_ACCELERATED);

    bool running = true;
    SDL_EVENT event;

    SDL_Rect player = {SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, 50, 20};

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);

        // Draw player
        SDL_setRenderDrawColor(renderer, 0, 255, 0, 255); // green player
        SDL_RenderFillRect(renderer, &player);

        // Show the render
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
