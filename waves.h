#ifndef WAVES_H
#define WAVES_H

typedef enum
{
    WAVE_TYPE_EASY,
    WAVE_TYPE_MIXED,
    WAVE_TYPE_HARD,
    WAVE_TYPE_BOSS,
} WaveType;

typedef struct
{
    WaveType type;
    int basicCount;
    int fastCount;
    int tankCount;
    bool isBoss;
} WaveConfig;

WaveConfig get_wave_config(int waveNumber);

#endif
