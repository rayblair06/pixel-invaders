#include <stdlib.h>
#include <stdbool.h>
#include "audio.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "player.h"
#include "sprites.h"
#include "waves.h"

static App g_app;

App *app(void) { return &g_app; }

bool initialiseGameProps = false;
bool isGameOver = false;

static Uint32 lastFrameTicks = 0;
static float deltaTime = 0.0f;

static bool isPaused = false;
static Uint32 pauseStart = 0;
static Uint32 totalPauseTime = 0;

// Screen shake (rapid shake) variables
bool shaking = false;
int shakeOffsetX = 0;
int shakeOffsetY = 0;
Uint32 shakeTimer = 0;
Uint32 shakeDuration = 300; // ms
int shakeStrength = 4;

// Camera Offset (smooth drift)
float cameraOffsetX = 0;
float cameraOffsetY = 0;

// Red Flash
bool flashRed = false;
Uint32 flashStartTime = 0;
Uint32 flashDuration = 200; // ms

void init_app()
{
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetHint(SDL_HINT_RENDER_LOGICAL_SIZE_MODE, "letterbox");

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Pixel Invaders: Rogue Mode",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Scale to resolution
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_RenderSetIntegerScale(renderer, SDL_FALSE);

    g_app.window = window;
    g_app.renderer = renderer;
}

void init_game(void)
{
    isGameOver = false;
    shakeOffsetX = shakeOffsetY = 0;
    cameraOffsetX = cameraOffsetY = 0;
    shakeTimer = 0;
}

/**
 * Returns the current name time in ms (paused time excluded)
 */
Uint32 get_game_ticks(void)
{
    Uint32 now = SDL_GetTicks();
    return isPaused ? pauseStart - totalPauseTime : now - totalPauseTime;
}

/**
 * Returns the delta time in seconds (float), capped if needed
 */
float get_delta_time(void)
{
    return isPaused ? 0.0f : deltaTime;
}

/**
 * Update tick timing - call this once per frame
 */
void update_game_time(void)
{
    if (isPaused)
    {
        deltaTime = 0.0f;
        return;
    }

    Uint32 now = SDL_GetTicks();
    Uint32 adjustedNow = now - totalPauseTime;

    if (lastFrameTicks == 0)
    {
        lastFrameTicks = adjustedNow;
        deltaTime = 0.0f;
        return;
    }

    Uint32 frameTicks = adjustedNow - lastFrameTicks;
    deltaTime = (float)frameTicks / 1000.0f; // convert to seconds
    lastFrameTicks = adjustedNow;

    // Clamp to prevent huge spikes and negative
    if (deltaTime < 0.0f)
        deltaTime = 0.0f;
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;
}

/**
 * Reset tick timing - call this on new games
 */
void reset_game_time(void)
{
    Uint32 now = SDL_GetTicks();

    pauseStart = 0;
    isPaused = false;

    totalPauseTime = now;

    lastFrameTicks = 0;
    deltaTime = 0.0f;
}

void pause_game(void)
{
    if (!isPaused)
    {
        pauseStart = SDL_GetTicks();
        isPaused = true;
    }
}

void resume_game(void)
{
    if (isPaused)
    {
        Uint32 now = SDL_GetTicks();
        totalPauseTime += now - pauseStart;
        isPaused = false;
    }
}

/**
 * Returns whether game is currently paused
 */
bool is_game_paused(void)
{
    return isPaused;
}

// Background scroll speeds (parallax effect)
static float starOffsetY = 0;
static float skyOffsetY = 0;

/**
 * Background Constants
 */
static const int TILE_WIDTH = 180;
static const int TILE_HEIGHT = 320;

#define TILES_X ((SCREEN_WIDTH / TILE_WIDTH) + 2)
#define TILES_Y ((SCREEN_HEIGHT / TILE_HEIGHT) + 2)

void render_parallax_layer(SDL_Renderer *renderer, SDL_Texture *texture, float offsetX, float offsetY)
{
    const int ox = ((int)offsetX) % TILE_WIDTH;
    const int oy = ((int)offsetY) % TILE_HEIGHT;

    for (int y = 0; y < TILES_Y; y++)
    {
        for (int x = 0; x < TILES_X; x++)
        {
            int drawX = x * TILE_WIDTH - ox;
            int drawY = y * TILE_HEIGHT - oy;

            SDL_Rect dst = {drawX, drawY, TILE_WIDTH, TILE_HEIGHT};
            SDL_RenderCopy(renderer, texture, NULL, &dst);
        }
    }
}

/**
 * Renders our game background
 */
void render_background()
{
    SDL_Renderer *renderer = app()->renderer;
    SDL_Texture *bgSkyTexture = get_sprite_texture(BG_TILE1);
    SDL_Texture *bgStarsTexture = get_sprite_texture(BG_TILE2);

    // Scroll speeds
    float skySpeed = 10.0f;  // for background, moves slow
    float starSpeed = 50.0f; // near background, moves faster

    // Update offests
    skyOffsetY += skySpeed * get_delta_time();
    starOffsetY += starSpeed * get_delta_time();

    // Render screen
    if (flashRed)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // flash red
        SDL_RenderClear(renderer);
    }
    else
    {
        render_parallax_layer(renderer, bgSkyTexture, 0, skyOffsetY);
        render_parallax_layer(renderer, bgStarsTexture, 0, starOffsetY);
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
void reduce_player_health(int amount)
{
    health = health - amount;

    entity_trigger_hit(&player.entity, 0.05f);
    play_sound(SND_HIT);
    trigger_screen_shake();
    trigger_red_flash();

    if (health <= 0)
    {
        // Can't go below zero
        if (health < 0)
        {
            health = 0;
        }

        trigger_player_explosion();

        isGameOver = true;

        // TODO: Need to trigger a gameover single event
        play_music(MUS_GAMEOVER, false);
    }
}
