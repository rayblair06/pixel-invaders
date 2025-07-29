#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum
{
    SCENE_MAIN_MENU,
    SCENE_GAME,
    SCENE_PREVIOUS_RUNS,
} Scene;

void manage_scene(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate);

void scene_main_menu(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate);
void scene_game(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate);
void scene_previous_runs(SDL_Renderer *renderer, TTF_Font *font, const Uint8 *keystate, const Uint8 *prevKeystate);

#endif
