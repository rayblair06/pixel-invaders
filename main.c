#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "audio.h"
#include "constants.h"
#include "collisions.h"
#include "bullets.h"
#include "enemies.h"
#include "entity.h"
#include "game.h"
#include "sprites.h"
#include "pickups.h"
#include "player.h"
#include "ui.h"

typedef enum
{
    UPGRADE_PLAYER_SPEED,
    UPGRADE_BULLET_SPEED,
} UpgradeType;

#define UPGRADE_COUNT 2

const char *upgrade_names[] = {
    "Player Speed",
    "Bullet Speed"};

void debug_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void apply_upgrade(UpgradeType upgrade)
{
    switch (upgrade)
    {
    case UPGRADE_PLAYER_SPEED:
        playerSpeed += 1.0f;
        break;
    case UPGRADE_BULLET_SPEED:
        bulletSpeed += 1.5f;
        break;
    }
}

bool key_pressed(SDL_Scancode key, const Uint8 *current, const Uint8 *previous)
{
    return current[key] && !previous[key];
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

    init_sprites(renderer);
    init_audio();
    play_music(MUS_GAME, true);

    bool running = true;

    // Main game loop
    SDL_Event event;

    // ✅ Force keyboard state to update
    SDL_PumpEvents();
    Uint8 prevKeystateBuffer[SDL_NUM_SCANCODES] = {0};

    // Initialize player position
    init_player();

    init_bullets();
    init_enemies();
    init_pickups();

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

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        const Uint8 *prevKeystate = prevKeystateBuffer;

        tick_player(keystate);
        tick_bullets();
        tick_enemies();
        tick_pickups();
        tick_waves();

        check_collisions();

        update_screen_shake();
        update_red_flash();
        render_background(renderer);

        render_player(renderer, shakeOffsetX, shakeOffsetY);
        render_bullets(renderer, shakeOffsetX, shakeOffsetY);
        render_enemies(renderer, shakeOffsetX, shakeOffsetY);
        render_pickups(renderer, shakeOffsetX, shakeOffsetY);

        // Convert Experience to string
        char experienceText[32];
        sprintf(experienceText, "Experience: %d", experience);

        // Create surface and texture from text
        SDL_Color white = {225, 255, 255, 255};
        SDL_Surface *experienceSurface = TTF_RenderText_Solid(font, experienceText, white);
        SDL_Texture *experienceTexture = SDL_CreateTextureFromSurface(renderer, experienceSurface);

        // Define destination rectrangle
        SDL_Rect experienceRect = {10, 10, experienceSurface->w, experienceSurface->h};

        // Render the experience text
        SDL_RenderCopy(renderer, experienceTexture, NULL, &experienceRect);

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

        // Upgrade Menu
        if (choosingUpgrade)
        {
            render_menu(renderer, font, upgrade_names, 2, selectedOption, 150);
        }

        // Trigger Upgrade Menu delay
        if (isLevelUpPending && !choosingUpgrade)
        {
            generate_upgrade_choices();

            choosingUpgrade = true;
            isEntitiesFrozen = true;
        }

        if (choosingUpgrade)
        {
            debug_log("Upgrade menu is active.");
            debug_log("selectedOption: %d", selectedOption);

            if (key_pressed(SDL_SCANCODE_UP, keystate, prevKeystate))
                debug_log("UP is held");
            if (key_pressed(SDL_SCANCODE_DOWN, keystate, prevKeystate))
                debug_log("DOWN is held");
            if (key_pressed(SDL_SCANCODE_RETURN, keystate, prevKeystate))
                debug_log("RETURN is held");

            if (key_pressed(SDL_SCANCODE_UP, keystate, prevKeystate))
                selectedOption = (selectedOption - 1 + optionCount) % optionCount;
            if (key_pressed(SDL_SCANCODE_DOWN, keystate, prevKeystate))
                selectedOption = (selectedOption + 1) % optionCount;

            if (key_pressed(SDL_SCANCODE_RETURN, keystate, prevKeystate) || key_pressed(SDL_SCANCODE_KP_ENTER, keystate, prevKeystate))
            {
                apply_upgrade(options[selectedOption]);

                choosingUpgrade = false;
                isEntitiesFrozen = false;
                isLevelUpPending = false;
            }
        }

        memcpy(prevKeystateBuffer, SDL_GetKeyboardState(NULL), SDL_NUM_SCANCODES);

        // Clean up
        SDL_FreeSurface(livesSurface);
        SDL_DestroyTexture(livesTexture);

        SDL_FreeSurface(waveSurface);
        SDL_DestroyTexture(waveTexture);

        SDL_FreeSurface(experienceSurface);
        SDL_DestroyTexture(experienceTexture);

        // Show the render
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS

        // Debugging!
        debug_log("Player X: %.2f, Y: %.2f", player.x, player.y);
        debug_log("Enemy count: %d", sizeof(enemies));
        debug_log("Level up triggered: %s", isLevelUpPending ? "true" : "false");

        debug_log("Options count: %d", optionCount);
        debug_log("Selected option: %d", selectedOption);

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
