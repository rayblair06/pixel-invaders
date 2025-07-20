#ifndef UPGRADES_H
#define UPGRADES_H

typedef enum
{
    UPGRADE_PLAYER_SPEED_1,
    UPGRADE_PLAYER_SPEED_2,
    UPGRADE_PLAYER_SPEED_3,

    UPGRADE_BULLET_SPEED_1,
    UPGRADE_BULLET_SPEED_2,
    UPGRADE_BULLET_SPEED_3,

    UPGRADE_MULTI_SHOT,
    UPGRADE_HEALTH_REGEN,
    UPGRADE_SHIELD,
    UPGRADE_PICKUP_MAGNET,

    UPGRADE_COUNT
} UpgradeType;

typedef struct
{
    UpgradeType type;
    const char *name;
    int unlockLevel;          // Level requirement
    UpgradeType prerequisite; // Upgrades to be unlocked first
} UpgradeInfo;

extern UpgradeInfo upgrades[UPGRADE_COUNT];
extern bool upgradesApplied[UPGRADE_COUNT];
extern UpgradeType options[3];
extern int optionCount;

void init_upgrades(void);
void apply_upgrade(UpgradeType upgrade);
void generate_upgrade_choices(void);

#endif
