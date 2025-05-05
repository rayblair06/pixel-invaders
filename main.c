#include <SDL2/SDL.h>
#include <stdbool.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 5;

int main(int argc, char *argv[])
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Space Wars",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;

    // Main game loop
    SDL_Event event;

    // Initialize player position
    SDL_Rect player = {SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, 50, 20};

    // keep track of key states
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    // Main loop
    while (running)
    {
        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Move player based on key state
        if (keystate[SDL_SCANCODE_LEFT])
        {
            player.x -= PLAYER_SPEED;
        }

        if (keystate[SDL_SCANCODE_RIGHT])
        {
            player.x += PLAYER_SPEED;
        }

        // Keep player within screen bounds
        if (player.x < 0)
            player.x = 0;
        if (player.x > SCREEN_WIDTH - player.w)
            player.x = SCREEN_WIDTH - player.w;

        // Render screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);

        // Draw player
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green player
        SDL_RenderFillRect(renderer, &player);

        // Show the render
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
