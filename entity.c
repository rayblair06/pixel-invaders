#include "entity.h"

/**
 * Creates an entity
 */
Entity create_entity(float x, float y, int w, int h)
{
    Entity entity;

    entity.x = x;
    entity.y = y;
    entity.w = w;
    entity.h = h;
    entity.rect.x = (int)x;
    entity.rect.y = (int)y;
    entity.rect.w = w;
    entity.rect.h = h;
    entity.active = true;

    return entity;
}

/**
 * Move an entity in the specified direction at that speed
 */
void move(Entity *entity, Movement move, float speed)
{
    switch (move)
    {
    case UP:
        entity->y -= speed;
        break;
    case DOWN:
        entity->y += speed;
        break;
    case LEFT:
        entity->x -= speed;
        break;
    case RIGHT:
        entity->x += speed;
        break;
    }

    update_entity_rect(entity);
}

/**
 * Updates entity position
 */
void update_entity_rect(Entity *entity)
{
    entity->rect.x = (int)entity->x;
    entity->rect.y = (int)entity->y;
}
