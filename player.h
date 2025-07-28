#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "entity.h"

extern Entity player;
extern float playerSpeed;
extern bool isPlayerVisible;

extern int level;

extern int health;
extern int healthMax;

extern bool isPlayerExploding;
extern int explosionFrame;
extern Uint32 explosionStartTime;
extern int explosionFrameDurtion;
extern int explosionFrameCount;

// Upgrades
extern bool hasMultiShot;
extern bool hasSpreadShot;
extern bool hasPiercing;
extern bool hasExplosive;
extern bool hasHealthRegen;
extern bool hasShield;
extern bool hasPickupMagnet;

extern Uint32 lastRegenTime;

void init_player();
void tick_player(const Uint8 *keystate);
void render_player(SDL_Renderer *renderer, int shakeX, int shakeY);

void trigger_player_shoot();
void trigger_player_explosion();

void add_experience(int amount);

#endif
