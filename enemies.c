#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "sprites.h"

Enemy enemies[MAX_ENEMIES];
float enemySpeed = 1.0f;

bool enemyFrameToggle = false;
Uint32 lastFrameSwitch = 0;
const Uint32 frameInterval = 500; // ms

// Enemy Animation Frames
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
    }
}

/**
 * Render Active Enemies on Screen
 */
void render_enemies(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    Uint32 now = SDL_GetTicks();

    // Toggle enemy animations
    if (now - lastFrameSwitch > frameInterval)
    {
        enemyFrameToggle = !enemyFrameToggle;
        lastFrameSwitch = SDL_GetTicks();
    }

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!enemies[i].active)
            continue;

        // Alternative animation frame
        EnemyType type = enemies[i].type;
        SpriteID frame = enemyFrameToggle ? enemySprites[type].frameB : enemySprites[type].frameA;

        SDL_Rect src = get_sprite(frame);
        SDL_Texture *texture = get_sprite_texture(frame);

        SDL_Rect dst = enemies[i].entity.rect;
        dst.x += shakeX;
        dst.y += shakeY;

        SDL_RenderCopy(renderer, texture, &src, &dst);
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
        enemy.speed = 1.0f;
        enemy.health = 1;
        break;
    case ENEMY_FAST:
        enemy.speed = 2.0f;
        enemy.health = 1;
        break;
    case ENEMY_TANK:
        enemy.speed = 0.5;
        enemy.health = 3;
        break;
    case ENEMY_TYPE_COUNT:
        // do nothing
        break;
    }

    return enemy;
}
