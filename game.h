#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

extern int score;
extern int lives;
extern int wave;

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

extern int wave;
extern Uint32 lastWaveTime;
extern Uint32 waveInterval;

void init_game(void);
void render_background(SDL_Renderer *renderer);

void update_screen_shake(void);
void trigger_screen_shake(void);

void update_red_flash(void);
void trigger_red_flash(void);

void tick_waves(void);

void add_score(int amount);
void lose_life(void);

#endif
