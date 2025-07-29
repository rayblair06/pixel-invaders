#include <stdio.h>
#include <string.h>
#include "stats.h"

RunData currentRun;
MetaData metaData;

// Initialise stats at the start of a run
void init_stats(void)
{
    memset(&currentRun, 0, sizeof(RunData));
    load_meta(&metaData);
}

void tick_run_time(float deltaTime)
{
    currentRun.timePlayed += (int)deltaTime;
}

void record_kill(void)
{
    currentRun.totalKills++;
}

void record_boss_kill(void)
{
    currentRun.bossKills++;
}

void record_bullets_fired(void)
{
    currentRun.bulletsFired++;
}

void record_experience(int amount)
{
    currentRun.totalExperience += amount;
}

void record_upgrade(int upgradeIndex)
{
    if (upgradeIndex >= 0 && upgradeIndex < UPGRADE_COUNT)
    {
        currentRun.upgradesChosen[upgradeIndex]++;
    }
}

void save_run(void)
{
    FILE *file = fopen("save/run_data.dat", "wb");

    if (file)
    {
        fwrite(&currentRun, sizeof(RunData), 1, file);
        fclose(file);
    }
}

bool load_last_run(RunData *run)
{
    FILE *file = fopen("save/run_data.dat", "rb");

    if (!file)
        return false;

    fread(run, sizeof(RunData), 1, file);
    fclose(file);
    return true;
}

void save_meta(void)
{
    FILE *file = fopen("save/meta_data.dat", "wb");

    if (file)
    {
        fwrite(&metaData, sizeof(MetaData), 1, file);
        fclose(file);
    }
}

bool load_meta(MetaData *meta)
{
    FILE *file = fopen("save/meta_data.dat", "rb");

    if (!file)
        return false;

    fread(meta, sizeof(MetaData), 1, file);
    fclose(file);

    return true;
}

void log_current_run(void)
{
    FILE *log = fopen("last_run.log", "w");

    if (log)
    {
        fprintf(log, "Wave Reached: %d\n", currentRun.finalWave);
        fprintf(log, "Total Kills: %d\n", currentRun.totalKills);
        fprintf(log, "Total Boss Kills: %d\n", currentRun.bossKills);
        fprintf(log, "Experience: %d\n", currentRun.totalExperience);
        fprintf(log, "Time Played: %d seconds\n", currentRun.timePlayed);

        fprintf(log, "Upgrades:\n");
        for (int i = 0; i < UPGRADE_COUNT; i++)
        {
            if (currentRun.upgradesChosen[i] > 0)
            {
                fprintf(log, "  Upgrade %s: %d times\n", upgrades[i].name, currentRun.upgradesChosen[i]);
            }
        }
    }

    fclose(log);
}
