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
#include "ui.h"
#include "upgrades.h"
#include "waves.h"
#include "version.h"

GameState gameState = STATE_MAIN_MENU;

const char *mainMenuOptions[] = {"Start Game", "Quit"};

int selectedMenuOption = 0;
const int mainMenuOptionCount = 2;

void debug_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}

bool key_pressed(SDL_Scancode key, const Uint8 *current, const Uint8 *previous)
{
    return current[key] && !previous[key];
}

int main(int argc, char *argv[])
{
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

    bool running = true;

    // Main game loop
    SDL_Event event;

    // ✅ Force keyboard state to update
    SDL_PumpEvents();
    Uint8 prevKeystateBuffer[SDL_NUM_SCANCODES] = {0};

    SDL_Color white = {225, 255, 255, 255};

    // Start of deltaTime
    Uint32 lastTick = SDL_GetTicks();

    // Main loop
    while (running)
    {
        Uint32 now = SDL_GetTicks();
        float deltaTime = (now - lastTick) / 1000.0f;
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

        // Main Menu
        if (gameState == STATE_MAIN_MENU)
        {
            render_background(renderer);
            render_menu(renderer, font, "Pixel Invaders", mainMenuOptions, mainMenuOptionCount, selectedMenuOption, 16, 16);

            generate_text(renderer, font, GAME_VERSION, 10, SCREEN_HEIGHT - 30, white);

            if (key_pressed(SDL_SCANCODE_UP, keystate, prevKeystate))
                selectedMenuOption = (selectedMenuOption - 1 + mainMenuOptionCount) % mainMenuOptionCount;

            if (key_pressed(SDL_SCANCODE_DOWN, keystate, prevKeystate))
                selectedMenuOption = (selectedMenuOption + 1) % mainMenuOptionCount;

            if (key_pressed(SDL_SCANCODE_RETURN, keystate, prevKeystate))
            {
                switch (selectedMenuOption)
                {
                case 0:
                    init_game();
                    gameState = STATE_PLAYING;
                    initialiseGameProps = true;
                    play_music(MUS_GAME, true);

                    break;
                case 1:
                    running = false;
                    break;
                }
            }
        }

        // Main Game Loop
        if (gameState == STATE_PLAYING)
        {
            // Initialise these on first launch of game
            if (initialiseGameProps)
            {
                // Initialize player position
                init_player();
                init_level_manager();
                init_upgrades();

                init_bullets();
                init_enemy_bullets();
                init_enemies();
                init_pickups();
                init_waves();

                initialiseGameProps = false;
            }

            render_background(renderer);

            render_player(renderer, shakeOffsetX, shakeOffsetY);
            render_bullets(renderer, shakeOffsetX, shakeOffsetY);
            render_enemy_bullets(renderer, shakeOffsetX, shakeOffsetY);
            render_enemies(renderer, shakeOffsetX, shakeOffsetY);
            update_enemy_animation_state();
            render_pickups(renderer, shakeOffsetX, shakeOffsetY);
            render_particles(renderer);

            tick_player(keystate);
            tick_bullets();
            tick_enemy_bullets();
            tick_enemies();
            tick_pickups();
            tick_waves();
            tick_particles(deltaTime);

            if (bossActive)
                tick_boss(deltaTime);

            render_boss(renderer, shakeOffsetX, shakeOffsetY);
            render_boss_health(renderer, font);

            check_collisions();

            update_screen_shake();
            update_red_flash();

            render_stats_panel(renderer, font, 10, 10, 30);

            update_experience_visual(deltaTime);
            render_health_bar(renderer, font, SCREEN_WIDTH - 220, SCREEN_HEIGHT - 20, 200, 10);
            render_xp_bar(renderer, font, 10, SCREEN_HEIGHT - 20, 200, 10);

            // Boss overlay
            if (bossActive)
            {
                render_vignette(renderer);
            }

            if (bossActive && SDL_GetTicks() - bossSpawnTime < 2000)
            {
                generate_text(renderer, font, "The Abyssal Wraith Approaches!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, (SDL_Color){255, 0, 0, 255});
            }

            // Trigger Upgrade Menu delay
            if (is_level_up_pending() && !choosingUpgrade)
            {
                generate_upgrade_choices();

                choosingUpgrade = true;
                isEntitiesFrozen = true;
            }

            // Upgrade Menu
            if (choosingUpgrade)
            {
                // Reorganise labels so they match the selections
                const char *upgrade_labels[UPGRADE_COUNT];

                for (int i = 0; i < optionCount; i++)
                {
                    upgrade_labels[i] = upgrades[options[i]].name;
                }

                render_menu(renderer, font, "Choose an Upgrade", upgrade_labels, optionCount, selectedOption, 32, 32);
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
                    // debug_log("Player has selected upgrade: %s", options[selectedOption]);

                    apply_upgrade(options[selectedOption]);

                    choosingUpgrade = false;
                    isEntitiesFrozen = false;
                    consume_level_up_pending();
                }
            }

            // Game Over Screen
            if (isGameOver)
            {
                SDL_Surface *overSurface = TTF_RenderText_Blended(font, "GAME OVER", white);
                SDL_Texture *overTexture = SDL_CreateTextureFromSurface(renderer, overSurface);
                SDL_Rect overRect = {SCREEN_WIDTH / 2 - overSurface->w / 2, SCREEN_HEIGHT / 2 - overSurface->h / 2, overSurface->w, overSurface->h};
                SDL_RenderCopy(renderer, overTexture, NULL, &overRect);

                SDL_FreeSurface(overSurface);
                SDL_DestroyTexture(overTexture);

                // Enter to restart
                if (key_pressed(SDL_SCANCODE_RETURN, keystate, prevKeystate))
                {
                    gameState = STATE_MAIN_MENU;
                }
            }

            // Debugging!
            debug_log("Player X: %.2f, Y: %.2f", player.x, player.y);
            debug_log("Enemy count: %d", sizeof(enemies));
            debug_log("Level up triggered: %s", is_level_up_pending() ? "true" : "false");

            debug_log("Options count: %d", optionCount);

            debug_log("Player move speed: %.f2", playerSpeed);
            debug_log("Player bullet speed: %f.f2", bulletSpeed);
            debug_log("Player hasMultiShot: %s", hasMultiShot ? "true" : "false");
            debug_log("Player hasHealthRegen: %s", hasHealthRegen ? "true" : "false");
            debug_log("Player hasShield: %s", hasShield ? "true" : "false");
            debug_log("Player hasPickupMagnet: %s", hasPickupMagnet ? "true" : "false");

            debug_log("Spawning wave %d with enemyHealthMultiplier=%.2f\n", wave, enemyHealthMultiplier);
            debug_log("Spawning wave %d with enemySpeedMultiplier=%.2f\n", wave, enemySpeedMultiplier);
            debug_log("Spawning wave %d with enemyDamageMultiplier=%.2f\n", wave, enemyDamageMultiplier);
        }

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
