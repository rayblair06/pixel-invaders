#include <math.h>
#include "level_manager.h"
#include "upgrades.h"

int playerLevel = 1;
int experience = 0;
int experienceTotal = 0;         // Total amount of experience
int experienceToNextLevel = 300; // Level 1 starts at 300

static bool isLevelUpPending = false;
float experienceVisual = 0.0f; // Smoothly animated XP progress

void init_level_manager(void)
{
    playerLevel = 1;
    experience = 0;
    experienceToNextLevel = 300;
    isLevelUpPending = false;
}

static int calculate_experience_required(int playerLevel)
{
    // Non-linear progression
    return (int)(100 * pow(playerLevel, 1.5));
}

void add_experience(int amount)
{
    experience += amount;
    experienceTotal += amount;

    if (experience >= experienceToNextLevel)
    {
        experience -= experienceToNextLevel;
        playerLevel++;

        experienceToNextLevel = calculate_experience_required(playerLevel);

        generate_upgrade_choices();
        isLevelUpPending = true;
    }
}

bool is_level_up_pending(void)
{
    return isLevelUpPending;
}

void consume_level_up_pending(void)
{
    isLevelUpPending = false;
}

void update_experience_visual(float deltaTime)
{
    float targetExperience = (float)experience;
    float speed = 10.0f; // higher = faster smoothing
    experienceVisual += (targetExperience - experienceVisual) * deltaTime * speed;
}
