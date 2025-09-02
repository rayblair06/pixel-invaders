#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <direct.h>
#endif

#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "stats.h"
#include "ui.h"

RunData currentRun;
MetaData metaData;
RunHistory runHistory;

static void ensure_save_directory(void)
{
#ifdef _WIN32
    _mkdir("save");
#else
    mkdir("save", 0755);
#endif
}

// Initialise stats at the start of a run
void init_stats(void)
{
    memset(&currentRun, 0, sizeof(RunData));
    load_meta(&metaData);
    load_run_history();
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
        sprintf(buffer, "%s x%d, ", upgrades[upgradeIndex].name, currentRun.upgradesChosen[upgradeIndex]);

        size_t used = strlen(currentRun.upgradeSummary);
        size_t left = strlen(currentRun.upgradeSummary) - used - 1;

        if (left > 0)
        {
            strncat(currentRun.upgradeSummary, buffer, left);
        }
    }
}

/**
 * Helper function for saving metaData
 */
void save_meta(void)
{
    ensure_save_directory();
    FILE *file = fopen("save/meta_data.dat", "wb");

    if (!file)
    {
        perror("fopen meta_data.dat");
        return;
    }

    fwrite(&metaData, sizeof(MetaData), 1, file);

    fclose(file);
}

/**
 * Helper function for loading metaData
 */
bool load_meta(MetaData *meta)
{
    ensure_save_directory();
    FILE *file = fopen("save/meta_data.dat", "rb");

    if (!file)
    {
        return false;
    }

    bool ok = fread(meta, sizeof(MetaData), 1, file) == 1;
    fclose(file);

    return ok;
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
    ensure_save_directory();
    FILE *file = fopen("save/run_history.dat", "rb");

    if (!file)
    {
        runHistory.runCount = 0;
        return false;
    }

    if (fread(&runHistory, sizeof(RunHistory), 1, file) != 1)
    {
        fclose(file);
        runHistory.runCount = 0;
        return false;
    }

    fclose(file);
    return true;
}

/**
 * Helper function for saving run history to file
 */
void save_run_history(void)
{
    ensure_save_directory();
    FILE *file = fopen("save/run_history.dat", "wb");

    if (!file)
    {
        perror("fopen run_history.dat");
        return;
    }

    if (fwrite(&runHistory, sizeof(RunHistory), 1, file) != 1)
    {
        perror("fwrite run_history.dat");
    }

    fclose(file);
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

void render_run_summary(TTF_Font *font, RunData *run)
{
    SDL_Color white = {255, 255, 255, 255};

    char line[128];

    int y = 100;
    generate_text(font, "=== Run Summary ===", 50, y, white);

    y += 40;
    sprintf(line, "Wave Reached: %d", run->finalWave);
    generate_text(font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total kills: %d", run->totalKills);
    generate_text(font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total boss kills: %d", run->bossKills);
    generate_text(font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total experience: %d", run->totalExperience);
    generate_text(font, line, 50, y, white);

    y += 40;
    sprintf(line, "Total played: %d secs", run->timePlayed);
    generate_text(font, line, 50, y, white);

    y += 40;
    generate_text(font, "Upgrades:", 50, y, white);

    y += 30;
    if (run->upgradeSummary[0] == '\0')
    {

        generate_text(font, "None", 50, y, white);
    }
    else
    {
        generate_text(font, run->upgradeSummary, 50, y, white);
    }
}
