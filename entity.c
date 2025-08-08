#include "entity.h"
#include "game.h"

/**
 * Creates an entity
 *
 * TODO: w and h should be retrived from sprite with draw size
 */
Entity create_entity(float x, float y, int w, int h)
{
    Entity entity;

    entity_set_pos(&entity, x, y);
    entity_set_size(&entity, w, h);

    entity.isActive = true;

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
void entity_animate(Entity *entity)
{
    float deltaTime = get_delta_time();

    if (entity->anim.frameCount <= 1)
    {
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
