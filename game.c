#include <stdbool.h>
#include "audio.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "player.h"
#include "sprites.h"
#include "waves.h"

float deltaTime;

bool initialiseGameProps = false;
bool isGameOver = false;
bool isEntitiesFrozen = false;

// Screen shake variables
bool shaking = false;
int shakeOffsetX = 0;
int shakeOffsetY = 0;
Uint32 shakeTimer = 0;
Uint32 shakeDuration = 300; // ms
int shakeStrength = 4;

// Red Flash
bool flashRed = false;
Uint32 flashStartTime = 0;
Uint32 flashDuration = 200; // ms

void init_game(void)
{
    isGameOver = isEntitiesFrozen = false;
    shakeOffsetX = shakeOffsetY = 0;
    shakeTimer = 0;
}

/**
 * Renders our game background
 */
void render_background(SDL_Renderer *renderer)
{
    // Load Sprites
    int bgTileW = 64;
    int bgTileH = 64;

    SDL_Texture *texture = get_sprite_texture(BG_TILE);

    // Render screen
    if (flashRed)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // flash red
        SDL_RenderClear(renderer);
    }
    else
    {
        // Draw Background
        for (int y = 0; y < SCREEN_HEIGHT; y += bgTileH)
        {
            for (int x = 0; x < SCREEN_WIDTH; x += bgTileW)
            {
                SDL_Rect dest = {x + shakeOffsetX, y + shakeOffsetY, bgTileW, bgTileH};
                SDL_RenderCopy(renderer, texture, NULL, &dest);
            }
        }
    }
}

/**
 * Handle the screen shake timer
 */
void update_screen_shake(void)
{
    Uint32 now = SDL_GetTicks();

    if (now - shakeTimer < shakeDuration)
    {
        shakeOffsetX = (rand() % (shakeStrength * 2 + 1)) - shakeStrength;
        shakeOffsetY = (rand() % (shakeStrength * 2 + 1)) - shakeStrength;

        return;
    }

    shaking = false;
    shakeOffsetX = 0;
    shakeOffsetY = 0;
}

/**
 * Shake the screen!
 */
void trigger_screen_shake(void)
{
    shaking = true;
    shakeTimer = SDL_GetTicks();
}

/**
 * Handle the red flash timer
 */
void update_red_flash(void)
{
    Uint32 now = SDL_GetTicks();

    if (flashRed && now - flashStartTime > flashDuration)
    {
        flashRed = false;
    }
}

/**
 * Flashes the screen red!
 */
void trigger_red_flash(void)
{
    flashRed = true;
    flashStartTime = SDL_GetTicks();
}

/**
 * Handle when a reduction of health
 */
void reduce_health(int amount)
{
    health = health - amount;

    play_sound(SND_HIT);

    trigger_screen_shake();
    trigger_red_flash();

    if (health <= 0)
    {
        trigger_player_explosion();

        isGameOver = true;

        // TODO: Need to trigger a gameover single event
        play_music(MUS_GAMEOVER, false);
    }
}
