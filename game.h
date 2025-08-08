#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum
{
    STATE_NONE, // Game isn't running
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

extern int wave;

extern bool initialiseGameProps;
extern bool isGameOver;

extern bool shaking;
extern int shakeOffsetX, shakeOffsetY;
extern Uint32 shakeTimer;
extern Uint32 shakeDuration;
extern int shakeStrength;

// Camera Offset
float cameraOffsetX;
float cameraOffsetY;

extern bool flashRed;
extern Uint32 flashStartTime;
extern Uint32 flashDuration;

void init_game(void);

// Returns the current name time in ms (paused time excluded)
Uint32 get_game_ticks(void);

// Returns the detla time in seconds (float), capped if needed
float get_delta_time(void);

// Update tick timing - call this once per frame
void update_game_time(void);

// Pause/resume functions
void pause_game(void);
void resume_game(void);

// Returns whether game is currently paused
bool is_game_paused(void);

void render_background(SDL_Renderer *renderer);

void update_screen_shake(void);
void trigger_screen_shake(void);

void update_red_flash(void);
void trigger_red_flash(void);

void tick_waves(void);

void reduce_health(int amount);

#endif
