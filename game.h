#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

typedef enum
{
    STATE_NONE, // Game isn't running
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

extern float deltaTime;
extern int wave;

extern bool initialiseGameProps;
extern bool isGameOver;
extern bool isEntitiesFrozen;

extern bool shaking;
extern int shakeOffsetX, shakeOffsetY;
extern Uint32 shakeTimer;
extern Uint32 shakeDuration;
extern int shakeStrength;

extern bool flashRed;
extern Uint32 flashStartTime;
extern Uint32 flashDuration;

void init_game(void);
void render_background(SDL_Renderer *renderer);

void update_screen_shake(void);
void trigger_screen_shake(void);

void update_red_flash(void);
void trigger_red_flash(void);

void tick_waves(void);

void reduce_health(int amount);

#endif
