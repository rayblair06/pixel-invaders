#include "bullets.h"
#include "level_manager.h"
#include "player.h"
#include "upgrades.h"

/**
 * Upgrade List
 */
UpgradeInfo upgrades[UPGRADE_COUNT] = {
    {UPGRADE_PLAYER_SPEED_1, "Player Speed I", 1, -1},
    {UPGRADE_PLAYER_SPEED_2, "Player Speed II", 1, UPGRADE_PLAYER_SPEED_1},
    {UPGRADE_PLAYER_SPEED_3, "Player Speed III", UPGRADE_PLAYER_SPEED_2},

    {UPGRADE_BULLET_SPEED_1, "Bullet Speed I", 1, -1},
    {UPGRADE_BULLET_SPEED_2, "Bullet Speed II", 1, UPGRADE_BULLET_SPEED_1},
    {UPGRADE_BULLET_SPEED_3, "Bullet Speed III", 1, UPGRADE_BULLET_SPEED_2},

    {UPGRADE_MULTI_SHOT, "Multi-Shot", 4, -1},
    {UPGRADE_SPREAD_SHOT, "Spread-Shot", 4, UPGRADE_MULTI_SHOT},
    {UPGRADE_HEALTH_REGEN, "Health Regen", 4, -1},
    {UPGRADE_SHIELD, "Shield", 6, -1},
    {UPGRADE_PICKUP_MAGNET, "Pickup Magnet", 8, -1}};

/**
 * Which upgrades are active
 */
bool upgradesApplied[UPGRADE_COUNT] = {false};
UpgradeType options[3];

/**
 * Options for upgrade menu
 */
int optionCount = 0;

void init_upgrades(void)
{
    for (int i = 0; i < UPGRADE_COUNT; i++)
    {
        upgradesApplied[i] = false;
    }
}

void apply_upgrade(UpgradeType upgrade)
{
    switch (upgrade)
    {
    case UPGRADE_PLAYER_SPEED_1:
    case UPGRADE_PLAYER_SPEED_2:
    case UPGRADE_PLAYER_SPEED_3:
        playerSpeed += 1.0f;
        break;
    case UPGRADE_BULLET_SPEED_1:
    case UPGRADE_BULLET_SPEED_2:
    case UPGRADE_BULLET_SPEED_3:
        bulletSpeed += 1.5f;
        break;
    case UPGRADE_MULTI_SHOT:
        hasMultiShot = true;
        break;
    case UPGRADE_SPREAD_SHOT:
        hasSpreadShot = true;
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
    default:
        // do nothing
        break;
    }

    upgradesApplied[upgrade] = true;
}

static bool upgrade_is_unlocked(UpgradeInfo upgrade, int playerLevel)
{
    if (playerLevel < upgrade.unlockLevel)
        return false;
    if (upgrade.prerequisite != -1 && !upgradesApplied[upgrade.prerequisite])
        return false;
    return !upgradesApplied[upgrade.type];
}

void generate_upgrade_choices()
{
    optionCount = 0;

    // Build a list of available upgrades
    UpgradeType available[UPGRADE_COUNT];
    int availableCount = 0;

    for (int i = 0; i < UPGRADE_COUNT; i++)
    {
        if (upgrade_is_unlocked(upgrades[i], playerLevel))
        {
            available[availableCount++] = upgrades[i].type;
        }
    }

    // Shuffle the available upgradecs
    for (int i = availableCount - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        UpgradeType temp = available[i];
        available[i] = available[j];
        available[j] = temp;
    }

    // Pick up to 3 from the shuffled list
    optionCount = (availableCount < 3) ? availableCount : 3;

    for (int i = 0; i < optionCount; i++)
    {
        options[i] = available[i];
    }
}
