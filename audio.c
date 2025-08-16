#include <stdio.h>
#include "audio.h"

const char *soundPaths[SND_COUNT] = {
    [SND_SHOOT] = "assets/audio/sounds/BLLTBy_Bullet By Centred Fast_02.wav",
    [SND_LASER] = "assets/audio/sounds/SCIMisc_Reload Alien Tech_02.wav",
    [SND_CHARGE1] = "assets/audio/sounds/BEEP_Targeting Loop_06.wav",
    [SND_CHARGE2] = "assets/audio/sounds/LASRGun_Electron Impeller Charged Fire_02.wav",
    [SND_ENEMY_DEATH] = "assets/audio/sounds/ANMLBat_Creature Vocalisation_01.wav",
    [SND_EXPLOSION] = "assets/audio/sounds/EXPLDsgn_Explosion Impact_14.wav",
    [SND_PICKUP] = "assets/audio/sounds/UIBeep_Lock On_05.wav",
    [SND_HIT] = "assets/audio/sounds/BLLTRico_Ricochet Metallic_04.wav",
    [SND_SHIELD_BREAK] = "assets/audio/sounds/GUNMech_Scope Deactivate_11.wav",
    [SND_BOSS_ROAR] = "assets/audio/sounds/scream.wav",
};

static int loadedSounds = 0;

static Mix_Chunk *sounds[SND_COUNT];

const char *musicPaths[MUS_COUNT] = {
    [MUS_MENU] = "assets/audio/music/menu.wav",
    [MUS_GAME] = "assets/audio/music/battle.wav",
    [MUS_GAMEOVER] = "assets/audio/music/menu.wav",
    [MUS_BOSS] = "assets/audio/music/in-the-wreckage.wav",
};

static int loadedMusic = 0;

static Mix_Music *music[MUS_COUNT];

void init_audio(void)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Mix_OpenAudio Error: %s\n", Mix_GetError());
        return;
    }

    // Load sounds
    for (int i = 0; i < SND_COUNT; i++)
    {
        if (!soundPaths[i])
            continue;

        sounds[i] = Mix_LoadWAV(soundPaths[i]);

        if (!sounds[i])
        {
            printf("Failed to load sound [%d]: %s\n Path: %s\n", i, Mix_GetError(), soundPaths[i]);
        }
        else
        {
            loadedSounds++;
        }
    }

    // Load Music
    for (int i = 0; i < MUS_COUNT; i++)
    {
        if (!musicPaths[i])
            continue;
        music[i] = Mix_LoadMUS(musicPaths[i]);

        if (!music[i])
        {
            printf("Failed to load music [%d]: %s\n Path: %s\n", i, Mix_GetError(), musicPaths[i]);
        }
        else
        {
            loadedMusic++;
        }
    }
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
        Mix_PlayMusic(music[id], loop ? -1 : 0);
    }
}

void stop_music(void)
{
    Mix_HaltMusic();
}

void cleanup_audio(void)
{
    for (int i = 0; i < loadedSounds; i++)
    {
        if (sounds[i])
        {
            Mix_FreeChunk(sounds[i]);
            sounds[i] = NULL;
        }
    }

    for (int i = 0; i < loadedMusic; i++)
    {
        if (music[i])
        {
            Mix_FreeMusic(music[i]);
            music[i] = NULL;
        }
    }

    Mix_CloseAudio();
}
