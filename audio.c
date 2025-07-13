#include <stdio.h>
#include "audio.h"

Mix_Chunk *sounds[SND_COUNT];
Mix_Music *music[MUS_COUNT];

void init_audio(void)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Mix_OpenAudio Error: %s\n", Mix_GetError());
        return;
    }

    // Load sounds
    sounds[SND_SHOOT] = Mix_LoadWAV("assets/shoot.wav");
    sounds[SND_HIT] = Mix_LoadWAV("assets/hit.wav");

    // Load Music
    music[MUS_GAME] = Mix_LoadMUS("assets/game-music.mp3");
    music[MUS_GAMEOVER] = Mix_LoadMUS("assets/game-over-music.mp3");
}

void play_sound(SoundID id)
{
    if (sounds[id])
    {
        Mix_PlayChannel(-1, sounds[id], 0);
    }
}

void play_music(MusicID id, bool loop)
{
    if (music[id])
    {
        stop_music();
        Mix_PlayMusic(music[id], loop ? -1 : 1);
    }
}

void stop_music(void)
{
    Mix_HaltMusic();
}

void cleanup_audio(void)
{
    for (int i = 0; i < SND_COUNT; i++)
    {
        if (sounds[i])
            Mix_FreeChunk(sounds[i]);
    }

    for (int i = 0; i < MUS_COUNT; i++)
    {
        if (music[i])
            Mix_FreeMusic(music[i]);
    }

    Mix_CloseAudio();
}
