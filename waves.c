#include <stdbool.h>
#include "boss.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "waves.h"

int wave = 0;
const int bossWave = 10;

/**
 * Base Values are starting values, these start at 1 as if 1 is normal.
 * Maybe if we introduce a 'hardcore' or more difficult mode, these will be set at 2+
 */
const int BASE_ENEMY_HEALTH = 1;
const int BASE_ENEMY_SPEED = 1;
const int BASE_ENEMY_DAMAGE = 10;

int baseEnemyHealth;
int baseEnemySpeed;
int baseEnemyDamage;

/**
 * Multipliers start at 1.0f and increase each wave.
 * Like base values you could maybe start these at a higher value for more difficult modes?
 */
const float BASE_HEALTH_MULTLIPLIER = 1.0f;
const float BASE_SPEED_MULTLIPLIER = 1.0f;
const float BASE_DAMAGE_MULTLIPLIER = 1.0f;

// Reduced because I suck at my own game
// BASE_HEALTH_MULTLIPLIER = 0.2f;
// BASE_SPEED_MULTLIPLIER = 1.0f;
// BASE_DAMAGE_MULTLIPLIER = 0.2f;

float enemyHealthMultiplier;
float enemySpeedMultiplier;
float enemyDamageMultiplier;

/**
 * The amount by which we increase each wave
 */
const float INCREASE_ENEMY_HEALTH_MULTIPLIER = 0.5f;
const float INCREASE_ENEMY_SPEED_MULTIPLIER = 0.5f;
const float INCREASE_ENEMY_DAMAGE_MULTIPLIER = 0.5f;

float increaseEnemyHealthMultiplier;
float increaseEnemySpeedMultiplier;
float increaseEnemyDamageMultiplier;

static Uint32 lastWaveTime = 0;
static Uint32 waveDelay = 3000;
static bool waveActive = false;

#define GRID_COLS 8
#define GRID_ROWS 5
#define CELL_WIDTH 80
#define CELL_HEIGHT 50
#define JITTER 12

void init_waves(void)
{
    wave = 0;
    waveActive = false;
    lastWaveTime = SDL_GetTicks();

    baseEnemyHealth = BASE_ENEMY_HEALTH;
    baseEnemySpeed = BASE_ENEMY_SPEED;
    baseEnemyDamage = BASE_ENEMY_DAMAGE;

    enemyHealthMultiplier = BASE_HEALTH_MULTLIPLIER;
    enemySpeedMultiplier = BASE_SPEED_MULTLIPLIER;
    enemyDamageMultiplier = BASE_DAMAGE_MULTLIPLIER;

    increaseEnemyHealthMultiplier = INCREASE_ENEMY_HEALTH_MULTIPLIER;
    increaseEnemySpeedMultiplier = INCREASE_ENEMY_SPEED_MULTIPLIER;
    increaseEnemyDamageMultiplier = INCREASE_ENEMY_DAMAGE_MULTIPLIER;
}

static void spawn_wave(void)
{
    int totalEnemies = 5 + wave * 2;

    int gridX = (SCREEN_WIDTH - GRID_COLS * CELL_WIDTH) / 2;
    int gridY = 50;

    // Prepare grid position
    int positions[GRID_COLS * GRID_ROWS];
    int available = 0;

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            positions[available++] = row * GRID_COLS + col;
        }
    }

    // Shuffle grid positions
    for (int i = available - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = positions[i];

        positions[i] = positions[j];
        positions[j] = temp;
    }

    // Spawn enemies
    for (int i = 0; i < totalEnemies && i < available; i++)
    {
        int position = positions[i];
        int row = position / GRID_COLS;
        int col = position % GRID_COLS;

        int jitterX = (rand() % (JITTER * 2 + 1)) - JITTER;
        int jitterY = (rand() % (JITTER * 2 + 1)) - JITTER;

        // Generate new enemies off screen
        int offScreenDiff = (SCREEN_HEIGHT / 2);

        float x = gridX + col * CELL_WIDTH + jitterX;
        float y = gridY + row * CELL_HEIGHT + jitterY - offScreenDiff;

        // Enemy Type based on wave
        EnemyType typesAvailable[ENEMY_TYPE_COUNT];
        int typesCount = 0;

        // Only basic before wave 3
        if (wave < 3)
        {
            typesAvailable[typesCount++] = ENEMY_BASIC;
        }
        else if (wave < 7)
        {
            typesAvailable[typesCount++] = ENEMY_BASIC;
            typesAvailable[typesCount++] = ENEMY_FAST;
        }
        else if (wave < 10)
        {
            typesAvailable[typesCount++] = ENEMY_BASIC;
            typesAvailable[typesCount++] = ENEMY_FAST;
            typesAvailable[typesCount++] = ENEMY_TANK;
        }
        else if (wave < 12)
        {
            typesAvailable[typesCount++] = ENEMY_BASIC;
            typesAvailable[typesCount++] = ENEMY_FAST;
            typesAvailable[typesCount++] = ENEMY_TANK;
            typesAvailable[typesCount++] = ENEMY_SHOOTER;
        }
        else
        {
            typesAvailable[typesCount++] = ENEMY_BASIC;
            typesAvailable[typesCount++] = ENEMY_FAST;
            typesAvailable[typesCount++] = ENEMY_TANK;
            typesAvailable[typesCount++] = ENEMY_SHOOTER;
            typesAvailable[typesCount++] = ENEMY_BRUTE;
        }

        EnemyType type = typesAvailable[rand() % typesCount];

        spawn_enemy(x, y, type);
    }

    waveActive = true;
}

/**
 * Handle all of our 'tick' functionality of spawning new enemies within the main game loop
 */
void tick_waves(void)
{
    int alive = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i].active)
            alive++;

        if (bossActive)
            alive++;
    }

    // Enemies dead and no wave active, let's spawn some new enemies!
    if (alive == 0 && !waveActive)
    {
        Uint32 now = SDL_GetTicks();

        if (now - lastWaveTime > waveDelay)
        {
            // check for boss
            if (wave % bossWave == 0 && wave != 0)
            {
                // Spawn a boss here instead
                spawn_boss(SCREEN_WIDTH / 2 - 64, -200, wave);
            }
            else
            {
                spawn_wave();
            }

            wave++;
            waveActive = true;

            // After boss wave, increase base difficulty
            if (wave % bossWave == 1 && wave != 1)
            {
                enemyHealthMultiplier = enemyHealthMultiplier + increaseEnemyHealthMultiplier;
                enemySpeedMultiplier = enemySpeedMultiplier + increaseEnemySpeedMultiplier;
                enemyDamageMultiplier = enemyDamageMultiplier + increaseEnemyDamageMultiplier;
            }

            lastWaveTime = now;
        }
    }
    // If enemies still alive, keep wave marked active
    else if (alive > 0)
    {

        waveActive = true;
    }
    else
    {
        waveActive = false;
    }
}
