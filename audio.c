#include <stdio.h>
#include "audio.h"

const char *soundPaths[SND_COUNT] = {
    [SND_SHOOT] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/BLLTBy_Bullet By Centred Fast_02.wav",
    [SND_LASER] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/SCIMisc_Reload Alien Tech_02.wav",
    [SND_CHARGE1] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/BEEP_Targeting Loop_06.wav",
    [SND_CHARGE2] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/LASRGun_Electron Impeller Charged Fire_02.wav",
    [SND_ENEMY_DEATH] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/ANMLBat_Creature Vocalisation_01.wav",
    [SND_EXPLOSION] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/EXPLDsgn_Explosion Impact_14.wav",
    [SND_PICKUP] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/UIBeep_Lock On_05.wav",
    [SND_HIT] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/BLLTRico_Ricochet Metallic_04.wav",
    [SND_SHIELD_BREAK] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/GUNMech_Scope Deactivate_11.wav",
    [SND_BOSS_ROAR] = "new-assets/audio/sounds/Sci Fi Weapons Cyberpunk Arsenal Preview/Audio/CREAMnstr_Beast Vocalisation_09.wav",
};

static int loadedSounds = 0;

static Mix_Chunk *sounds[SND_COUNT];

const char *musicPaths[MUS_COUNT] = {
    [MUS_MENU] = "new-assets/audio/music/menu.wav",
    [MUS_GAME] = "new-assets/audio/music/battle.wav",
    [MUS_GAMEOVER] = "new-assets/audio/music/menu.wav", // TODO:
    [MUS_BOSS] = "new-assets/audio/music/battle.wav",
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
