#ifndef STATS_H
#define STATS_H

#include <stdbool.h>
#include "upgrades.h"

// Tracks data for a single run
typedef struct
{
    int finalWave;
    int totalExperience;
    int totalKills;
    int bossKills;
    int bulletsFired;
    int timePlayed; // in seconds
    int upgradesChosen[UPGRADE_COUNT];
} RunData;

// Tracks persisent meta-progression data
typedef struct
{
    int totalRuns;
    int bestWave;
    int totalExperienceEarned;
    bool metaUpgradesUnlocked[10];
} MetaData;

#define RUN_HISTORY_LIMIT 5

typedef struct
{
    RunData runs[RUN_HISTORY_LIMIT];
    int runCount;
} RunHistory;

extern RunData currentRun;
extern MetaData metaData;
extern RunHistory runHistory;

void init_stats(void);
void tick_run_time(float deltaTime);
void record_kill(void);
void record_boss_kill(void);
void record_bullets_fired(void);
void record_experience(int amount);
void record_upgrade(int upgradeIndex);

void save_meta(void);
bool load_meta(MetaData *meta);

void log_current_run(void);

bool load_run_history(void);
void save_run_history(void);
void add_run_to_history(RunData *run);

#endif
