#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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
    // Initialized game state
    int lives = 3;
    bool gameOver = false;
    int score = 0;
    int wave = 1;
    Uint32 lastWaveTime = 0;
    const Uint32 waveInterval = 3000; // 3 seconds between waves
    bool flashRed = false;
    Uint32 flashStartTime = 0;
    const Uint32 flashDuration = 200; // 200 ms flash

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("fonts/Roboto-Regular.ttf", 24);

    if (!font)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

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

        if (!gameOver)
        {
            // Move player based on key state
            if (keystate[SDL_SCANCODE_LEFT])
            {
                player.x -= PLAYER_SPEED;
            }

            if (keystate[SDL_SCANCODE_RIGHT])
            {
                player.x += PLAYER_SPEED;
            }
        }

        // Keep player within screen bounds
        if (player.x < 0)
            player.x = 0;
        if (player.x > SCREEN_WIDTH - player.w)
            player.x = SCREEN_WIDTH - player.w;

        // Shoot bullet if SPACE is pressed
        static bool spaceHeld = false;
        if (!gameOver && keystate[SDL_SCANCODE_SPACE])
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
            if (!gameOver && enemies[i].active)
            {
                enemies[i].rect.y += 1; // move down 1 pixel

                if (enemies[i].rect.y > SCREEN_HEIGHT)
                {
                    enemies[i].active = false; // remove off-screen
                    lives--;

                    // Trigger red flash
                    flashRed = true;
                    flashStartTime = SDL_GetTicks();

                    if (lives <= 0)
                    {
                        gameOver = true;
                    }
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
                    score += 100;
                    break;
                }
            }
        }

        // Waves!
        Uint32 now = SDL_GetTicks();

        if (!gameOver && now - lastWaveTime > waveInterval)
        {
            // Time for next wave!
            lastWaveTime = now;
            wave++;

            // Spawn enemies for this wave
            // Progressively more each wave
            int enemiesToSpawn = wave + 2;
            int spawned = 0;

            for (int i = 0; i < MAX_ENEMIES && spawned < enemiesToSpawn; i++)
            {
                if (!enemies[i].active)
                {
                    enemies[i].active = true;
                    enemies[i].rect.w = 40;
                    enemies[i].rect.h = 30;
                    enemies[i].rect.x = rand() % (SCREEN_WIDTH - enemies[i].rect.w);
                    enemies[i].rect.y = 0;
                    spawned++;
                }
            }
        }

        // Flash Red when hurt
        if (flashRed && SDL_GetTicks() - flashStartTime > flashDuration)
        {
            flashRed = false;
        }

        // Render screen
        if (flashRed)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // flash red
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black background
        }
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

        // Convert score to string
        char scoreText[32];
        sprintf(scoreText, "Score: %d", score);

        // Create surface and texture from text
        SDL_Color white = {225, 255, 255, 255};
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText, white);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Define destination rectrangle
        SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};

        // Render the score text
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Display current wave
        char waveText[32];
        sprintf(waveText, "Wave: %d", wave);

        SDL_Surface *waveSurface = TTF_RenderText_Solid(font, waveText, white);
        SDL_Texture *waveTexture = SDL_CreateTextureFromSurface(renderer, waveSurface);
        SDL_Rect waveRect = {10, 40, waveSurface->w, waveSurface->h};
        SDL_RenderCopy(renderer, waveTexture, NULL, &waveRect);

        // Draw lives on screen
        char livesText[32];
        sprintf(livesText, "Lives: %d", lives);

        SDL_Surface *livesSurface = TTF_RenderText_Solid(font, livesText, white);
        SDL_Texture *livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);
        SDL_Rect livesRect = {10, 70, livesSurface->w, livesSurface->h};
        SDL_RenderCopy(renderer, livesTexture, NULL, &livesRect);

        // Render Game over screen
        if (gameOver)
        {
            SDL_Surface *overSurface = TTF_RenderText_Solid(font, "GAME OVER", white);
            SDL_Texture *overTexture = SDL_CreateTextureFromSurface(renderer, overSurface);
            SDL_Rect overRect = {SCREEN_WIDTH / 2 - overSurface->w / 2, SCREEN_HEIGHT / 2 - overSurface->h / 2, overSurface->w, overSurface->h};
            SDL_RenderCopy(renderer, overTexture, NULL, &overRect);
            SDL_FreeSurface(overSurface);
            SDL_DestroyTexture(overTexture);
        }

        // Clean up
        SDL_FreeSurface(livesSurface);
        SDL_DestroyTexture(livesTexture);

        SDL_FreeSurface(waveSurface);
        SDL_DestroyTexture(waveTexture);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Show the render
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS

        // Debugging!
        printf("Score: %d\r", score);
        fflush(stdout);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    SDL_Quit();
    TTF_Quit();

    return 0;
}
