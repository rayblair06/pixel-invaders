#include "bullets.h"
#include "player.h"
#include "upgrades.h"

UpgradeType options[UPGRADE_COUNT];
int optionCount = 0;

void apply_upgrade(UpgradeType upgrade)
{
    switch (upgrade)
    {
    case UPGRADE_PLAYER_SPEED:
        playerSpeed += 1.0f;
        break;
    case UPGRADE_BULLET_SPEED:
        bulletSpeed += 1.5f;
        break;
    case UPGRADE_MULTI_SHOT:
        hasMultiShot = true;
        break;
    case UPGRADE_HEALTH_REGEN:
        hasHealthRegen = true;
        break;
    case UPGRADE_SHIELD:
        hasShield = true;
        break;
    case UPGRADE_PICKUP_MAGNET:
        hasPickupMagnet = true;
        break;
    }
}

void generate_upgrade_choices()
{
    // limit to 2 for now
    while (optionCount < 2)
    {
        UpgradeType pick = rand() % UPGRADE_COUNT;

        bool alreadyIncluded = false;

        for (int i = 0; i < optionCount; i++)
        {
            if (options[i] == pick)
                alreadyIncluded = true;
        }

        if (!alreadyIncluded)
        {
            options[optionCount++] = pick;
        }
    }
}
