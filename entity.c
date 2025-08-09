#include "entity.h"
#include "game.h"
#include "utils.h"

/**
 * Creates an entity
 *
 * TODO: w and h should be retrived from sprite with draw size
 */
Entity create_entity(float x, float y, int w, int h)
{
    Entity entity = {
        .isActive = true,

        .isDespawning = false,
        .despawningTimer = 0.0f,
        .despawningDuration = 5.0f,

        .isExploding = false,
        .explodingTimer = 0,

        .alpha = 255,
    };

    entity_set_pos(&entity, x, y);
    entity_set_size(&entity, w, h);

    return entity;
}

/**
 * Helper function: Set position of Entity
 */
void entity_set_pos(Entity *entity, float x, float y)
{
    entity->pos.x = x;
    entity->pos.y = y;
}

/**
 * Helper function: Set size of Entity.
 */
void entity_set_size(Entity *entity, float w, float h)
{
    entity->size.x = w;
    entity->size.y = h;
}

/**
 * Helper function: Animate Entity
 */
void entity_animate(Entity *entity, float deltaTime)
{
    if (!entity)
        return;

    if (entity->anim.frameCount <= 1 || entity->anim.frameTime >= 0.0f)
    {
        entity->anim.currentFrame = 0;
        entity->anim.frameTimer = 0.0f;
        return;
    }

    entity->anim.frameTimer += deltaTime;

    while (entity->anim.frameTimer >= entity->anim.frameTime)
    {
        entity->anim.frameTimer -= entity->anim.frameTime;
        entity->anim.currentFrame = (entity->anim.currentFrame + 1) % entity->anim.frameCount;
    }
}

/**
 * Timed Events
 */
void tick_timer(Entity *entity, float deltaTime)
{
    // Despawning timer
    if (entity->isDespawning)
    {
        debug_log("Despawning timer: %.3f", entity->despawningTimer);
        debug_log("Despawning duration: %.3f", entity->despawningDuration);

        entity->despawningTimer += deltaTime;

        if (entity->despawningTimer >= entity->despawningDuration)
        {
            entity->despawningTimer = entity->despawningDuration;
            entity->hasDespawned = true;
        }
    }

    // Exploding Timer
    if (entity->isExploding)
    {
        entity->explodingTimer += deltaTime;

        if (entity->explodingTimer >= entity->explodingDuration)
        {
            entity->explodingTimer = entity->explodingDuration;
            entity->hasExploded = true;
        }
    }
}

/**
 * Helper function: Update timers
 */
void entity_tick(Entity *entity)
{
    if (!entity)
        return;

    const float deltaTime = get_delta_time();

    entity_animate(entity, deltaTime);
    tick_timer(entity, deltaTime);
}

/**
 * Move an entity in the specified direction at that speed
 */
void move(Entity *entity, Movement move, float speed)
{
    // Nothing can move if the game has ended pr entites are frozen
    if (isGameOver || is_game_paused())
        return;

    switch (move)
    {
    case UP:
        entity->pos.y -= speed;
        break;
    case DOWN:
        entity->pos.y += speed;
        break;
    case LEFT:
        entity->pos.x -= speed;
        break;
    case RIGHT:
        entity->pos.x += speed;
        break;
    }
}
