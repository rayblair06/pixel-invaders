#ifndef ENTITY_H
#define ENTITY_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct
{
    bool active;
    bool despawning;
    Uint32 despawningTime;
    int despawningDuration;

    float x, y; // Position
    int w, h;   // Size
    float angle;

    float vx; // horizonal velocity
    float vy; // vertical velocity

    SDL_Rect rect;

    // animation state
    int animFrame;
    int animFrameCount;
    Uint32 animStartTime;
    int animFrameDuration;
} Entity;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Movement;

Entity create_entity(float x, float y, int w, int h);
void move(Entity *entity, Movement move, float speed);
void update_entity_rect(Entity *entity);

#endif
