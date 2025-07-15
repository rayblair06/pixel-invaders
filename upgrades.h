#ifndef UPGRADES_H
#define UPGRADES_H

#define UPGRADE_COUNT 6

typedef enum
{
    UPGRADE_PLAYER_SPEED,
    UPGRADE_BULLET_SPEED,
    UPGRADE_MULTI_SHOT,
    UPGRADE_HEALTH_REGEN,
    UPGRADE_SHIELD,
    UPGRADE_PICKUP_MAGNET,
} UpgradeType;

extern UpgradeType options[UPGRADE_COUNT];
extern int optionCount;

void apply_upgrade(UpgradeType upgrade);
void generate_upgrade_choices(void);

#endif
