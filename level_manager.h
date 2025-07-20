#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H

#include <stdbool.h>

// Current level and xp
extern int playerLevel;
extern int experience;
extern int experienceTotal;
extern int experienceToNextLevel;

/**
 * Initialise Level Manager
 */
void init_level_manager(void);

/**
 * Add XP and handle level-ups
 */
void add_experience(int amount);

/**
 * Check if a level is pending (e.g show upgrade menu)
 */
bool is_level_up_pending(void);

/**
 * Reset level-up pending flag (after upgrade selection)
 */
void consume_level_up_pending(void);

#endif
