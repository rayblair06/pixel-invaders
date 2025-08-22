#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

typedef enum
{
    SND_SHOOT1,
    SND_SHOOT2,
    SND_LASER,
    SND_CHARGE1,
    SND_CHARGE2,
    SND_ENEMY_DEATH,
    SND_EXPLOSION,
    SND_PICKUP,
    SND_HIT,
    SND_SHIELD_BREAK,
    SND_BOSS_ROAR,
    SND_COUNT,
} SoundID;

typedef enum
{
    MUS_MENU,
    MUS_GAME,
    MUS_GAMEOVER,
    MUS_BOSS,
    MUS_COUNT
} MusicID;

void init_audio(void);
void play_sound(SoundID id);
void play_music(MusicID id, bool loop);
void stop_music(void);
void cleanup_audio(void);

#endif
