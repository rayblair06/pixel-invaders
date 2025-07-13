#include <stdbool.h>
#include "audio.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "player.h"

int score = 0;
int lives = 3;

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

// Waves
int wave = 1;
Uint32 lastWaveTime = 0;
Uint32 waveInterval = 3000; // ms

void init_game(void)
{
    score = 0;
    score = 3;
    isGameOver = isEntitiesFrozen = false;
    shakeOffsetX = shakeOffsetY = 0;
    shakeTimer = 0;
}

/**
 * Renders our game background
 */
void render_background(SDL_Renderer *renderer, SDL_Texture *bgTexture)
{
    // Load Sprites
    // TODO: Move to somewhere better
    int bgTileW = 64;
    int bgTileH = 64;

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
                SDL_RenderCopy(renderer, bgTexture, NULL, &dest);
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
 * Handle all of our 'tick' functionality of spawning new enemies within the main game loop
 */
void tick_waves(void)
{
    // Waves!
    Uint32 now = SDL_GetTicks();

    // If we're making a selection, don't generate new waves
    if (isLevelUpPending)
    {
        // lastWaveTime = lastWaveTime + now;
    }

    if (!isGameOver && now - lastWaveTime > waveInterval)
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
            spawn_enemy(
                rand() % (SCREEN_WIDTH - enemies[i].rect.w),
                0);

            spawned++;
        }
    }
}

void add_score(int amount)
{
    score += amount;
}

/**
 * Handle when a life is losed
 */
void lose_life(void)
{
    lives--;

    play_sound(SND_HIT);

    trigger_screen_shake();
    trigger_red_flash();

    if (lives <= 0)
    {
        trigger_player_explosion();

        isGameOver = true;

        // TODO: Need to trigger a gameover single event
        play_music(MUS_GAMEOVER, false);
    }
}
