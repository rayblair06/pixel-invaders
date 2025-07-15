#ifndef UPGRADES_H
#define UPGRADES_H

typedef enum
{
    UPGRADE_PLAYER_SPEED,
    UPGRADE_BULLET_SPEED,
    UPGRADE_MULTI_SHOT,
    UPGRADE_HEALTH_REGEN,
    UPGRADE_SHIELD,
    UPGRADE_PICKUP_MAGNET,

    UPGRADE_COUNT
} UpgradeType;

extern const char *upgrade_names[];
extern UpgradeType options[UPGRADE_COUNT];
extern int optionCount;

void apply_upgrade(UpgradeType upgrade);
void generate_upgrade_choices(void);

#endif
