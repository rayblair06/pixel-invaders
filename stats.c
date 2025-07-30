#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "stats.h"
#include "ui.h"

RunData currentRun;
MetaData metaData;
RunHistory runHistory;

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

        // Attend upgrade name to summary
        char buffer[64];
        sprintf(buffer, "%s x%d", upgrades[upgradeIndex].name, currentRun.upgradesChosen[upgradeIndex]);
        strcat(currentRun.upgradeSummary, buffer);
    }
}

/**
 * Helper function for saving metaData
 */
void save_meta(void)
{
    FILE *file = fopen("save/meta_data.dat", "wb");

    if (file)
    {
        fwrite(&metaData, sizeof(MetaData), 1, file);
        fclose(file);
    }
}

/**
 * Helper function for loading metaData
 */
bool load_meta(MetaData *meta)
{
    FILE *file = fopen("save/meta_data.dat", "rb");

    if (!file)
        return false;

    fread(meta, sizeof(MetaData), 1, file);
    fclose(file);

    return true;
}

/**
 * Save last run to log for debugging
 */
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

/**
 * Helper function for loading run history from file
 */
bool load_run_history(void)
{
    FILE *file = fopen("save/run_history.dat", "rb");

    if (!file)
    {
        runHistory.runCount = 0;
        return false;
    }

    fread(&runHistory, sizeof(RunHistory), 1, file);
    fclose(file);
    return true;
}

/**
 * Helper function for saving run history to file
 */
void save_run_history(void)
{
    FILE *file = fopen("save/run_history.dat", "wb");

    if (file)
    {
        fwrite(&runHistory, sizeof(RunHistory), 1, file);
        fclose(file);
    }
}

/**
 * Add our run history to our runHistory array
 */
void add_run_to_history(RunData *run)
{
    if (runHistory.runCount >= RUN_HISTORY_LIMIT)
    {
        for (int i = RUN_HISTORY_LIMIT - 1; i > 0; i--)
        {
            runHistory.runs[i] = runHistory.runs[i - 1];
        }

        runHistory.runs[0] = *run;
    }
    else
    {
        // Insert at the front
        for (int i = runHistory.runCount; i > 0; i--)
        {
            runHistory.runs[i] = runHistory.runs[i - 1];
        }

        runHistory.runs[0] = *run;
        runHistory.runCount++;
    }

    save_run_history();
}

void render_run_summary(SDL_Renderer *renderer, TTF_Font *font, RunData *run)
{
    SDL_Color white = {255, 255, 255, 255};

    char line[128];

    int y = 100;
    generate_text(renderer, font, "=== Run Summary ===", 50, y, white);

    y += 40;
    sprintf(line, "Wave Reached: %d", run->finalWave);
    generate_text(renderer, font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total kills: %d", run->totalKills);
    generate_text(renderer, font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total boss kills: %d", run->bossKills);
    generate_text(renderer, font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total experience: %d", run->totalExperience);
    generate_text(renderer, font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total played: %d secs", run->timePlayed);
    generate_text(renderer, font, line, 50, y, white);

    y += 40;
    generate_text(renderer, font, "Upgrades:", 50, y, white);

    y += 30;
    if (sizeof(run->upgradeSummary) / sizeof(run->upgradeSummary)[0])
    {
        generate_text(renderer, font, "None", 50, y, white);
    }
    else
    {
        generate_text(renderer, font, run->upgradeSummary, 50, y, white);
    }
}
