#include "constants.h"
#include "bullets.h"
#include "enemies.h"
#include "game.h"
#include "sprites.h"
#include "waves.h"

Enemy enemies[MAX_ENEMIES];
float enemySpeed = 1.0f;

float enemyHealthMultiplier = 1.0f;
float enemySpeedMultiplier = 1.0f;

bool enemyFrameToggle = false;
Uint32 lastFrameSwitch = 0;
const Uint32 frameInterval = 500; // ms

// Enemy Animation Frames
typedef struct
{
    SpriteID frameA;
    SpriteID frameB;
} EnemySpriteFrames;

const EnemySpriteFrames enemySprites[] = {
    [ENEMY_BASIC] = {SPR_INVADER1_A, SPR_INVADER1_B},
    [ENEMY_FAST] = {SPR_INVADER2_A, SPR_INVADER2_B},
    [ENEMY_TANK] = {SPR_INVADER3_A, SPR_INVADER3_B},
};

/**
 * Initialise enemies as deactivated
 */
void init_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }
}

/**
 * Spawn a single enemy entity
 */
void spawn_enemy(float x, float y, EnemyType type)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
        {
            enemies[i] = create_enemy(x, y, type);
            break;
        }
    }
}

/**
 * Handle all of our 'tick' functionality of active enemies within the main game loop
 */
void tick_enemies(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        move(&enemies[i].entity, DOWN, enemySpeed);

        if (enemies[i].entity.y > 0)
        {
            enemies[i].canShoot = true;
        }

        // Only basic shoot
        if (enemies[i].type == ENEMY_BASIC && enemies[i].canShoot)
        {
            if (rand() % 500 == 0)
            {
                spawn_enemy_bullet(
                    enemies[i].entity.x + enemies[i].entity.w / 2,
                    enemies[i].entity.y + enemies[i].entity.h);
            }
        }
    }
}

/**
 * Render Active Enemies on Screen
 */
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    Uint32 now = SDL_GetTicks();

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        SpriteID frame = get_enemy_sprite(&enemies[i]);

        SDL_Rect src = get_sprite(frame);
        SDL_Texture *texture = get_sprite_texture(frame);

        SDL_Rect dst = enemies[i].entity.rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, texture, &src, &dst);
    }
}

/**
 * Toggle enemy animations
 */
void update_enemy_animation_state(void)
{
    Uint32 now = SDL_GetTicks();

    if (now - lastFrameSwitch > frameInterval)
    {
        enemyFrameToggle = !enemyFrameToggle;
        lastFrameSwitch = now;
    }
}

/**
 * Helper function for creating a specific enemy type out of an entity
 */
Enemy create_enemy(float x, float y, EnemyType type)
{
    Enemy enemy;

    enemy.active = true;
    enemy.type = type;
    enemy.entity = create_entity(x, y, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);

    switch (type)
    {
    case ENEMY_BASIC:
        enemy.health = baseEnemyHealth * 1;
        enemy.speed = baseEnemySpeed * 1.0f;
        break;
    case ENEMY_FAST:
        enemy.health = baseEnemyHealth * 1;
        enemy.speed = baseEnemySpeed * 2.0f;
        break;
    case ENEMY_TANK:
        enemy.health = baseEnemyHealth * 3;
        enemy.speed = baseEnemySpeed * 0.5f;
        break;
    case ENEMY_TYPE_COUNT:
        // do nothing
        break;
    }

    return enemy;
}

SpriteID get_enemy_sprite(const Enemy *enemy)
{
    EnemySpriteFrames frames = enemySprites[enemy->type];
    return enemyFrameToggle ? frames.frameB : frames.frameA;
}
