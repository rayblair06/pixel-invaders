#include <stdbool.h>
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "waves.h"

int wave = 0;

/**
 * Base Values are starting values, these start at 1 as if 1 is normal.
 * Maybe if we introduce a 'hardcore' or more difficult mode, these will be set at 2+
 */
int baseEnemyHealth = 1;
int baseEnemySpeed = 1;
int baseEnemyDamage = 1;

/**
 * Multipliers start at 1.0f and increase each wave.
 * Like base values you could maybe start these at a higher value for more difficult modes?
 */
float enemyHealthMultiplier = 1.0f;
float enemySpeedMultiplier = 1.0f;
float enemyDamageMultiplier = 1.0f;

/**
 * The amount by which we increase each wave
 */
float increaseEnemyHealthMultiplier = 0.5f;
float increaseEnemySpeedMultiplier = 0.5f;
float increaseEnemyDamageMultiplier = 0.5f;

static Uint32 lastWaveTime = 0;
static const Uint32 waveDelay = 3000;
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
    baseEnemyHealth = 1;
    baseEnemySpeed = 1;
    baseEnemyDamage = 1;

    enemyHealthMultiplier = 1.0f;
    enemySpeedMultiplier = 1.0f;
    enemyDamageMultiplier = 1.0f;

    increaseEnemyHealthMultiplier = 0.5f;
    increaseEnemySpeedMultiplier = 0.5f;
    increaseEnemyDamageMultiplier = 0.5f;
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
        else
        {
            typesAvailable[typesCount++] = ENEMY_BASIC;
            typesAvailable[typesCount++] = ENEMY_FAST;
            typesAvailable[typesCount++] = ENEMY_TANK;
            typesAvailable[typesCount++] = ENEMY_SHOOTER;
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
    }

    // Enemies dead and no wave active, let's spawn some new enemies!
    if (alive == 0 && !waveActive)
    {
        Uint32 now = SDL_GetTicks();

        if (now - lastWaveTime > waveDelay)
        {
            // check for boss
            if (wave % 5 == 0 && wave != 0)
            {
                // Spawn a boss here instead
                spawn_enemy(SCREEN_WIDTH / 2 - 32, 50, ENEMY_BOSS);
            }
            else
            {
                spawn_wave();
            }

            wave++;
            waveActive = true;

            // Every 5 waves, increase base difficulty
            if (wave % 5 == 1 && wave != 0)
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
