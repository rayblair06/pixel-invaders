#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "bullets.h"
#include "level_manager.h"
#include "player.h"
#include "stats.h"
#include "upgrades.h"

/**
 * Upgrade List
 */
UpgradeInfo upgrades[UPGRADE_COUNT] = {
    // Offensive Upgrades
    {UPGRADE_PIERCE, "Piercing Bullets", "Bullets pass through one extra enemy.", 4, -1},
    {UPGRADE_MULTI_SHOT, "Multi-Shot", "Fire additional bullets in parallel.", 8, -1},
    {UPGRADE_EXPLOSIVE, "Explosive Bullets", "Bullets explode on impact, damaging nearby foes.", 10, -1},
    {UPGRADE_SPREAD_SHOT, "Spread-Shot", "Fire a wider cone of bullets at once.", 12, UPGRADE_MULTI_SHOT},

    // Defensive Upgrades
    {UPGRADE_HEALTH_REGEN, "Health Regen", "Slowly regenerate health over time.", 4, -1},
    {UPGRADE_SHIELD, "Shield", "Blocks one hit, regenerates between waves.", 6, -1},
    {UPGRADE_PICKUP_MAGNET, "Pickup Magnet", "Pulls in experience orbs from further away.", 8, -1},

    // Attribute Upgrades
    {UPGRADE_THRUSTERS_1, "Ship Thrusters I", "Increase acceleration for tighter control.", 1, -1},
    {UPGRADE_THRUSTERS_2, "Ship Thrusters II", "Further boost acceleration.", 1, UPGRADE_THRUSTERS_1},
    {UPGRADE_THRUSTERS_3, "Ship Thrusters III", "Maximum acceleration upgrade.", 1, UPGRADE_THRUSTERS_2},

    {UPGRADE_AFTERBURNERS_1, "Afterburners I", "Reduce drag for smoother movement.", 1, -1},
    {UPGRADE_AFTERBURNERS_2, "Afterburners II", "Further reduce drag for speed retention.", 1, UPGRADE_AFTERBURNERS_1},
    {UPGRADE_AFTERBURNERS_3, "Afterburners III", "Minimal drag for peak maneuverability.", 1, UPGRADE_AFTERBURNERS_2},

    {UPGRADE_DRIVE_CORE_1, "Drive Core I", "Increase top movement speed.", 1, -1},
    {UPGRADE_DRIVE_CORE_2, "Drive Core II", "Further increase top speed.", 1, UPGRADE_DRIVE_CORE_1},
    {UPGRADE_DRIVE_CORE_3, "Drive Core III", "Maximum ship speed unlocked.", 1, UPGRADE_DRIVE_CORE_2},

    {UPGRADE_BULLET_SPEED_1, "Bullet Speed I", "Bullets travel faster.", 1, -1},
    {UPGRADE_BULLET_SPEED_2, "Bullet Speed II", "Further increase bullet speed.", 1, UPGRADE_BULLET_SPEED_1},
    {UPGRADE_BULLET_SPEED_3, "Bullet Speed III", "Maximum bullet velocity.", 1, UPGRADE_BULLET_SPEED_2},

    {UPGRADE_BULLET_DAMAGE_1, "Bullet Damage I", "Increase bullet damage.", 1, -1},
    {UPGRADE_BULLET_DAMAGE_2, "Bullet Damage II", "Further increase bullet damage.", 1, UPGRADE_BULLET_DAMAGE_1},
    {UPGRADE_BULLET_DAMAGE_3, "Bullet Damage III", "Maximum bullet damage boost.", 1, UPGRADE_BULLET_DAMAGE_2},
};

/**
 * Which upgrades are active
 */
bool upgradesApplied[UPGRADE_COUNT] = {false};
UpgradeType options[3];

/**
 * Options for upgrade menu
 */
int optionCount = 0;

bool choosingUpgrade = false;
int selectedOption = 0;

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
    case UPGRADE_THRUSTERS_1:
    case UPGRADE_THRUSTERS_2:
    case UPGRADE_THRUSTERS_3:
        playerAccel += 50.0f;
        break;
    case UPGRADE_AFTERBURNERS_1:
    case UPGRADE_AFTERBURNERS_2:
    case UPGRADE_AFTERBURNERS_3:
        playerDrag += 50.0f;
        break;
    case UPGRADE_DRIVE_CORE_1:
    case UPGRADE_DRIVE_CORE_2:
    case UPGRADE_DRIVE_CORE_3:
        playerMaxSpeed += 50.0f;
        break;
    case UPGRADE_BULLET_SPEED_1:
    case UPGRADE_BULLET_SPEED_2:
    case UPGRADE_BULLET_SPEED_3:
        bulletSpeed += 1.5f;
        break;
    case UPGRADE_BULLET_DAMAGE_1:
    case UPGRADE_BULLET_DAMAGE_2:
    case UPGRADE_BULLET_DAMAGE_3:
        bulletDamage += 1.5f;
        break;
    case UPGRADE_MULTI_SHOT:
        hasMultiShot = true;
        break;
    case UPGRADE_SPREAD_SHOT:
        hasSpreadShot = true;
        break;
    case UPGRADE_EXPLOSIVE:
        hasExplosive = true;
        break;
    case UPGRADE_PIERCE:
        hasPiercing = true;
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

    record_upgrade(upgrade);
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
