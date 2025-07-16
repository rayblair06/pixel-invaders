#include <stdbool.h>
#include "enemies.h"
#include "waves.h"

WaveConfig get_wave_config(int waveNumber)
{
    WaveConfig wave = {0};

    if (waveNumber % 5 == 0)
    {
        wave.type = WAVE_TYPE_BOSS;
        wave.isBoss = true;
    }
    else
    {
        wave.type = WAVE_TYPE_MIXED;
        wave.basicCount = 3 + waveNumber;
        wave.fastCount = waveNumber >= 3 ? 2 : 0;
        wave.tankCount = waveNumber >= 4 ? 1 : 0;
    }

    return wave;
}
