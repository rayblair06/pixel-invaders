#include "bullets.h"
#include "player.h"
#include "upgrades.h"

const char *upgrade_names[] = {
    "Player Speed",
    "Bullet Speed",
    "Multi-Shot",
    "Health Regen",
    "Shield",
    "Pickup Magnet",
};

UpgradeType options[UPGRADE_COUNT] = {
    UPGRADE_PLAYER_SPEED,
    UPGRADE_BULLET_SPEED,
    UPGRADE_MULTI_SHOT,
    UPGRADE_HEALTH_REGEN,
    UPGRADE_SHIELD,
    UPGRADE_PICKUP_MAGNET};

int optionCount = 0;

bool upgradesApplied[UPGRADE_COUNT] = {false};

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
    case UPGRADE_COUNT:
        // do nothing
        break;
    }

    upgradesApplied[upgrade] = true;
}

void generate_upgrade_choices()
{
    optionCount = 0;

    // Build a list of available upgrades
    UpgradeType available[UPGRADE_COUNT];
    int availableCount = 0;

    for (int i = 0; i < UPGRADE_COUNT; i++)
    {
        if (!upgradesApplied[i])
        {
            available[availableCount++] = (UpgradeType)i;
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
    int maxOptions = (availableCount < 3) ? availableCount : 3;

    for (int i = 0; i < maxOptions; i++)
    {
        options[i] = available[i];
    }

    optionCount = maxOptions;
}
