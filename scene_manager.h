#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum
{
    SCENE_MAIN_MENU,
    SCENE_GAME,
    SCENE_PREVIOUS_RUNS,
    SCENE_OPTIONS,
} Scene;

void manage_scene(const Uint8 *keystate, const Uint8 *prevKeystate);

void scene_main_menu(const Uint8 *keystate, const Uint8 *prevKeystate);
void scene_previous_runs(const Uint8 *keystate, const Uint8 *prevKeystate);
void scene_options(const Uint8 *keystate, const Uint8 *prevKeystate);
void scene_game(const Uint8 *keystate, const Uint8 *prevKeystate);

#endif
