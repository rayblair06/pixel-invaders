#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "audio.h"
#include "constants.h"
#include "bullets.h"
#include "enemies.h"
#include "entity.h"
#include "game.h"
#include "sprites.h"
#include "player.h"

const int MAX_PICKUPS = 100;

typedef Entity Bullet;
typedef Entity Enemy;

typedef struct
{
    SDL_Rect rect;
    bool active;
    bool falling;
} Pickup;

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
Pickup pickups[MAX_PICKUPS];

typedef enum
{
    UPGRADE_PLAYER_SPEED,
    UPGRADE_BULLET_SPEED,
} UpgradeType;

#define UPGRADE_COUNT 2

const char *upgrade_names[] = {
    "Player Speed",
    "Bullet Speed",
};

void apply_upgrade(UpgradeType upgrade)
{
    switch (upgrade)
    {
    case UPGRADE_PLAYER_SPEED:
        // playerSpeed += 1.0f;
        break;
    case UPGRADE_BULLET_SPEED:
        bulletSpeed += 1.5f;
        break;
    }
}

UpgradeType options[3];
int optionCount = 0;

void generate_upgrade_choices()
{
    // limit to 2 for now
    while (optionCount < 2)
    {
        UpgradeType pick = rand() % UPGRADE_COUNT;

        bool alreadyIncluded = false;

        for (int i = 0; i < optionCount; i++)
        {
            if (options[i] == pick)
                alreadyIncluded = true;
        }

        if (!alreadyIncluded)
        {
            options[optionCount++] = pick;
        }
    }
}

int main(int argc, char *argv[])
{
    // Initialized game state
    init_game();

    bool choosingUpgrade = false;
    int selectedOption = 0;

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    init_sprites();
    init_audio();
    play_music(MUS_GAME, true);

    // Initialize Fonts
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
    SDL_Window *window = SDL_CreateWindow("Pixel Invaders: Rogue Mode",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *bgTexture = IMG_LoadTexture(renderer, "assets/background.png");
    SDL_Texture *spriteTexture = IMG_LoadTexture(renderer, "assets/spritesheet.png");

    if (!bgTexture || !spriteTexture)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
    }

    bool running = true;

    // Main game loop
    SDL_Event event;

    // keep track of key states
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    // Initialize player position
    init_player();

    init_bullets();
    init_enemies();

    // Set all starting Pickup entities as inactive
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        pickups[i].active = false;
    }

    // Manually activate 5 enemies
    for (int i = 0; i < 5; i++)
    {
        spawn_enemy(100 + i * 120, 50);
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

        // Trigger Upgrade Menu delay
        if (isLevelUpPending && !choosingUpgrade)
        {
            generate_upgrade_choices();

            choosingUpgrade = true;
            isLevelUpPending = false;
            isEntitiesFrozen = true;
        }

        // Upgrade Menu
        if (choosingUpgrade)
        {
            for (int i = 0; i < optionCount; i++)
            {
                SDL_Color white = {255, 255, 255};
                char label[64];
                snprintf(label, sizeof(label), "%s", upgrade_names[options[i]]);

                SDL_Surface *textSurface = TTF_RenderText_Solid(font, label, white);
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                SDL_Rect dst = {SCREEN_WIDTH / 2 - 100, 150 + i * 40, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &dst);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }

        if (choosingUpgrade)
        {
            if (keystate[SDL_SCANCODE_UP])
                selectedOption = (selectedOption - 1 + optionCount) % optionCount;
            if (keystate[SDL_SCANCODE_DOWN])
                selectedOption = (selectedOption + 1) % optionCount;

            if (keystate[SDL_SCANCODE_KP_ENTER])
            {
                apply_upgrade(options[selectedOption]);
                choosingUpgrade = false;
                isEntitiesFrozen = false;
            }
        }

        tick_player(keystate);
        tick_bullets();
        tick_enemies();
        tick_waves();

        // Check for collisions between bullets and enemies
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
                continue;

            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (!enemies[j].active)
                    continue;

                if (SDL_HasIntersection(&bullets[i].rect, &enemies[j].rect))
                {
                    // Collision! Remove both bullets and enemies

                    bullets[i].active = false;
                    enemies[j].active = false;

                    // Generate pickup
                    for (int k = 0; k < MAX_PICKUPS; k++)
                    {
                        if (!pickups[k].active)
                        {
                            pickups[k].active = true;
                            pickups[k].falling = true;
                            pickups[k].rect.x = enemies[j].rect.x + (enemies[j].rect.w / 2) - (pickups[k].rect.w / 2);
                            pickups[k].rect.y = enemies[j].rect.y;
                            pickups[k].rect.w = SPRITE_DRAW_SIZE;
                            pickups[k].rect.h = SPRITE_DRAW_SIZE;

                            break;
                        }
                    }

                    break;
                }
            }
        }

        // Pickups Handle
        for (int i = 0; i < MAX_PICKUPS; i++)
        {
            if (!pickups[i].active)
                continue;

            if (pickups[i].falling)
            {
                pickups[i].rect.y += 2;

                // Stop falling at player Y
                if (pickups[i].rect.y >= player.y)
                {
                    pickups[i].rect.y = player.y;
                    pickups[i].falling = false;
                }
            }

            // Check for collision with player
            if (SDL_HasIntersection(&player.rect, &pickups[i].rect))
            {
                pickups[i].active = false;

                add_experience(100);
            }
        }

        update_screen_shake();
        update_red_flash();
        render_background(renderer, bgTexture);

        render_player(renderer, spriteTexture, shakeOffsetX, shakeOffsetY);
        render_bullets(renderer, spriteTexture, shakeOffsetX, shakeOffsetY);
        render_enemies(renderer, spriteTexture, shakeOffsetX, shakeOffsetY);

        // Draw pickups
        for (int i = 0; i < MAX_PICKUPS; i++)
        {
            if (!pickups[i].active)
                continue;

            SDL_Rect pickupSrc = get_sprite(SPR_BULLET3); // Replace with pickup animation
            SDL_Rect pickupDraw = pickups[i].rect;
            pickupDraw.x += shakeOffsetX;
            pickupDraw.y += shakeOffsetY;

            SDL_RenderCopy(renderer, spriteTexture, &pickupSrc, &pickupDraw);
        }

        // Convert Experience to string
        char experienceText[32];
        sprintf(experienceText, "Experience: %d", experience);

        // Create surface and texture from text
        SDL_Color white = {225, 255, 255, 255};
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, experienceText, white);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Define destination rectrangle
        SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};

        // Render the experience text
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
        if (isGameOver)
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
        // printf("experience: %d\r", experience);
        fflush(stdout);
    }

    // Clean up
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(spriteTexture);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    cleanup_audio();
    SDL_Quit();
    TTF_Quit();

    return 0;
}
