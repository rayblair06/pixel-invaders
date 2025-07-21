#ifndef WAVES_H
#define WAVES_H

void init_waves(void);
void tick_waves(void);

extern int wave;
extern int baseEnemyHealth;
extern int baseEnemySpeed;
extern int baseEnemyDamage;

extern float enemyHealthMultiplier;
extern float enemySpeedMultiplier;
extern float enemyDamageMultiplier;

// #define GRID_COLS 10
// #define GRID_ROWS 4
// #define GRID_CELL_WIDTH 64
// #define GRID_CELL_HEIGHT 64
// #define GRID_X_OFFSET 100
// #define GRID_Y_OFFSET 50

// typedef struct
// {
//     int x;
//     int y;
// } GridCell;

// typedef enum
// {
//     WAVE_TYPE_EASY,
//     WAVE_TYPE_MIXED,
//     WAVE_TYPE_HARD,
//     WAVE_TYPE_BOSS,
// } WaveType;

// typedef struct
// {
//     WaveType type;
//     int basicCount;
//     int fastCount;
//     int tankCount;
//     bool isBoss;
// } WaveConfig;

// WaveConfig get_wave_config(int waveNumber);
// void get_random_grid_positions(GridCell *outCells, int count)
// {
//     GridCell cells[GRID_COLS * GRID_ROWS];
//     int totalCells = 0;

//     for (int row = 0; row < GRID_ROWS; row++)
//     {
//         for (int col = 0; col < GRID_COLS; col++)
//         {
//             cells[totalCells].x = GRID_X_OFFSET + col * GRID_CELL_WIDTH;
//             cells[totalCells].y = GRID_Y_OFFSET + row * GRID_CELL_HEIGHT;
//             totalCells++;
//         }
//     }

//     // Shuffle cells
//     for (int i = totalCells - 1; i > 0; i--)
//     {
//         int j = rand() % (i + 1);
//         GridCell temp = cells[i];
//         cells[i] = cells[j];
//         cells[j] = temp;
//     }

//     // Copy first `count` into output
//     for (int i = 0; i < count && i < totalCells; i++)
//     {
//         outCells[i] = cells[i];
//     }
// }

#endif
