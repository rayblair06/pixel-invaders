#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "sprites.h"

// 2D vector helper
typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    // Lifecycle/state
    bool isActive;
    bool isDespawning;
    Uint32 despawningTime;    // seconds since start of despawn
    float despawningDuration; // seconds to finish despawn

    // transform/kinematics
    Vec2 pos;    // top-left
    Vec2 size;   // width/height
    Vec2 vel;    // velocity (px/s)
    Vec2 acc;    // aceleration (px/s^2)
    float angle; // degrees

    int z; // render order (lowest bottom)

    // animation
    SpriteAnimation anim;
} Entity;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Movement;

Entity create_entity(float x, float y, int w, int h);

/**
 * Helper function: Generate SDL_Rect from Entity
 */
static inline SDL_Rect entity_rect(const Entity *entity)
{
    SDL_Rect rect = {(int)entity->pos.x, (int)entity->pos.y, (int)entity->size.x, (int)entity->size.y};
    return rect;
}

void entity_set_pos(Entity *entity, float x, float y);
void entity_set_size(Entity *entity, float w, float h);
void entity_animate(Entity *entity);

void move(Entity *entity, Movement move, float speed);

#endif
