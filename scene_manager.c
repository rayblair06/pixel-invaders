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
#include "utils.h"
#include "waves.h"
#include "version.h"

GameState gameState = STATE_NONE;
Scene scene = SCENE_MAIN_MENU;

extern void stop_game();

const char *
    mainMenuOptions[] = {"Start Game",
                         "Previous Runs",
                         "Quit"};

int selectedMenuOption = 0;
const int mainMenuOptionCount = sizeof(mainMenuOptions) / sizeof(mainMenuOptions[0]);

bool key_pressed(SDL_Scancode key, const Uint8 *current, const Uint8 *previous)
{
    return current[key] && !previous[key];
}

void render_run_history(SDL_Renderer *renderer, TTF_Font *font)
{
    SDL_Color white = {255, 255, 255, 255};

    generate_text(renderer, font, "=== Last Runs===", 50, 100, white);

    for (int i = 0; i < runHistory.runCount; i++)
    {
        char line[128];
        RunData *run = &runHistory.runs[i];

        sprintf(line, "Run %d: Wave %d, Kills %d, EXP %d", i + 1, run->finalWave, run->totalKills, run->totalExperience);

        generate_text(renderer, font, line, 50, 100 + i * 40, white);
    }
}

/**
 * Manage which scene we're in
 */
void manage_scene(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate)
{
    switch (scene)
    {
    case SCENE_MAIN_MENU:
        scene_main_menu(renderer, font, keystate, prevKeystate);
        break;
    case SCENE_GAME:
        scene_game(renderer, font, keystate, prevKeystate);
        break;
    case SCENE_PREVIOUS_RUNS:
        scene_previous_runs(renderer, font, keystate, prevKeystate);
        break;
    }
}

void scene_main_menu(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate)
{
    SDL_Color white = {225, 255, 255, 255};

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
            scene = SCENE_GAME;
            gameState = STATE_PLAYING;
            initialiseGameProps = true;
            play_music(MUS_GAME, true);

            break;
        case 1:
            scene = SCENE_PREVIOUS_RUNS;

            break;
        case 2:
            stop_game();
            break;
        }
    }
}

void scene_previous_runs(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate)
{
    SDL_Color white = {225, 255, 255, 255};

    render_background(renderer);

    char statsLineTotalRuns[64];
    sprintf(statsLineTotalRuns, "Total runs %d", metaData.totalRuns);
    generate_text(renderer, font, statsLineTotalRuns, 10, 10, white);

    char statsLineBestWave[64];
    sprintf(statsLineBestWave, "Best Wave %d", metaData.bestWave);
    generate_text(renderer, font, statsLineBestWave, 10, 40, white);

    char statsLineTotalExperience[64];
    sprintf(statsLineTotalExperience, "Total Experience %d", metaData.totalExperienceEarned);
    generate_text(renderer, font, statsLineTotalExperience, 10, 70, white);

    render_run_history(renderer, font);

    if (key_pressed(SDL_SCANCODE_RETURN, keystate, prevKeystate) || key_pressed(SDL_SCANCODE_KP_ENTER, keystate, prevKeystate))
    {
        scene = SCENE_MAIN_MENU;
    }
}

void scene_game(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate)
{
    SDL_Color white = {225, 255, 255, 255};

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
        init_stats();

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
    tick_run_time(deltaTime);

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
        // TODO: Looks shite
        // render_vignette(renderer);
    }

    if (bossActive && SDL_GetTicks() - bossSpawnTime < 2000)
    {
        generate_text(renderer, font, "The Abyssal Wraith Approaches!", SCREEN_WIDTH / 2 - 175, SCREEN_HEIGHT / 2, (SDL_Color){255, 0, 0, 255});
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

        // Update stats
        currentRun.finalWave = wave;
        add_run_to_history(&currentRun);
        log_current_run();

        metaData.totalRuns++;
        if (wave > metaData.bestWave)
            metaData.bestWave = wave;
        metaData.totalExperienceEarned += currentRun.totalExperience;
        save_meta();

        // Enter to restart
        if (key_pressed(SDL_SCANCODE_RETURN, keystate, prevKeystate))
        {
            scene = SCENE_MAIN_MENU;
        }
    }

    // Debugging!
    debug_log("Time: %d", SDL_GetTicks);
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
