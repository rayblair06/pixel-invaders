#ifndef UPGRADES_H
#define UPGRADES_H

typedef enum
{
    // Offensive Upgrades
    UPGRADE_PIERCE,
    UPGRADE_MULTI_SHOT,
    UPGRADE_EXPLOSIVE,
    UPGRADE_SPREAD_SHOT,

    // Defensive Upgrades
    UPGRADE_HEALTH_REGEN,
    UPGRADE_SHIELD,
    UPGRADE_PICKUP_MAGNET,

    // Attribute Upgrades
    UPGRADE_THRUSTERS_1,
    UPGRADE_THRUSTERS_2,
    UPGRADE_THRUSTERS_3,

    UPGRADE_AFTERBURNERS_1,
    UPGRADE_AFTERBURNERS_2,
    UPGRADE_AFTERBURNERS_3,

    UPGRADE_DRIVE_CORE_1,
    UPGRADE_DRIVE_CORE_2,
    UPGRADE_DRIVE_CORE_3,

    UPGRADE_BULLET_SPEED_1,
    UPGRADE_BULLET_SPEED_2,
    UPGRADE_BULLET_SPEED_3,

    UPGRADE_BULLET_DAMAGE_1,
    UPGRADE_BULLET_DAMAGE_2,
    UPGRADE_BULLET_DAMAGE_3,

    UPGRADE_COUNT
} UpgradeType;

typedef struct
{
    UpgradeType type;
    const char *name;
    const char *description;
    int unlockLevel;          // Level requirement
    UpgradeType prerequisite; // Upgrades to be unlocked first
} UpgradeInfo;

extern UpgradeInfo upgrades[UPGRADE_COUNT];
extern bool upgradesApplied[UPGRADE_COUNT];
extern UpgradeType options[3];
extern int optionCount;

extern bool choosingUpgrade;
extern int selectedOption;

void init_upgrades(void);
void apply_upgrade(UpgradeType upgrade);
void generate_upgrade_choices(void);

#endif
