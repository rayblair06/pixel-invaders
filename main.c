#include <SDL2/SDL.h>
#include <stdbool.h>

// Runtime constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 5;
const int BULLET_SPEED = 10;
const int MAX_BULLETS = 100;
const int MAX_ENEMIES = 20;

typedef struct
{
    SDL_Rect rect;
    bool active;
} Bullet;

typedef struct
{
    SDL_Rect rect;
    bool active;
} Enemy;

//  AABB collision check (axis-aligned bounding box)
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
}

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

    // Bullet array
    Bullet bullets[MAX_BULLETS];
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }

    // Enemy array
    Enemy enemies[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    // Manually activate 5 enemies
    for (int i = 0; i < 5; i++)
    {
        enemies[i].active = true;
        enemies[i].rect.x = 100 + i * 120;
        enemies[i].rect.y = 50;
        enemies[i].rect.w = 40;
        enemies[i].rect.h = 30;
    }

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

        // Shoot bullet if SPACE is pressed
        static bool spaceHeld = false;
        if (keystate[SDL_SCANCODE_SPACE])
        {
            if (!spaceHeld)
            { // prevent holding space from firing too fast
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!bullets[i].active)
                    {
                        bullets[i].active = true;
                        bullets[i].rect.x = player.x + player.w / 2 - 2; // center bullet
                        bullets[i].rect.y = player.y;
                        bullets[i].rect.w = 4;
                        bullets[i].rect.h = 10;
                        break;
                    }
                }
            }

            spaceHeld = true;
        }
        else
        {
            spaceHeld = false;
        }

        // Move bullets
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                bullets[i].rect.y -= BULLET_SPEED;

                if (bullets[i].rect.y + bullets[i].rect.h < 0)
                {
                    // deactivate if off-screen
                    bullets[i].active = false;
                }
            }
        }

        // Move enemies down slowly
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active)
            {
                enemies[i].rect.y += 1; // move down 1 pixel

                if (enemies[i].rect.y > SCREEN_HEIGHT)
                {
                    enemies[i].active = false; // remove off-screen
                }
            }
        }

        // Check for collisions between bullets and enemies
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
                continue;

            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (!enemies[j].active)
                    continue;

                if (checkCollision(bullets[i].rect, enemies[j].rect))
                {
                    // Collision! Remove both

                    bullets[i].active = false;
                    enemies[j].active = false;
                    break;
                }
            }
        }

        // Render screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(renderer);

        // Draw player
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green player
        SDL_RenderFillRect(renderer, &player);

        // Draw bullets
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                SDL_RenderFillRect(renderer, &bullets[i].rect);
            }
        }

        // Draw enemies
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemies[i].active)
            {
                SDL_RenderFillRect(renderer, &enemies[i].rect);
            }
        }

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
