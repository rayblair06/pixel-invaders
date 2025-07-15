#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

typedef enum
{
    SND_SHOOT,
    SND_EXPLOSION,
    SND_PICKUP,
    SND_HIT,
    SND_COUNT,
    SND_SHIELD_BREAK,
} SoundID;

typedef enum
{
    MUS_GAME,
    MUS_GAMEOVER,
    MUS_COUNT
} MusicID;

void init_audio(void);
void play_sound(SoundID id);
void play_music(MusicID id, bool loop);
void stop_music(void);
void cleanup_audio(void);

#endif
