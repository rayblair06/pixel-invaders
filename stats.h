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

extern RunData currentRun;
extern MetaData metaData;

void init_stats(void);
void tick_run_time(float deltaTime);
void record_kill(void);
void record_boss_kill(void);
void record_bullets_fired(void);
void record_experience(int amount);
void record_upgrade(int upgradeIndex);

void save_run(void);
bool load_last_run(RunData *run);

void save_meta(void);
bool load_meta(MetaData *meta);

void log_current_run(void);

#endif
