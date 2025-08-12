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

        .anim = spriteFallbackAnim,
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

    if (entity->anim.frameCount <= 1)
    {
        entity->anim.currentFrame = 0;
        entity->anim.frameTimer = 0.0f;
        return;
    }

    entity->anim.frameTimer += deltaTime;

    while (entity->anim.frameTimer >= entity->anim.frameTime)
    {
        entity->anim.frameTimer -= entity->anim.frameTime;

        if (entity->anim.loop)
        {
            entity->anim.currentFrame = (entity->anim.currentFrame + 1) % entity->anim.frameCount;
        }
        else
        {
            // Stop at last frame
            if (entity->anim.currentFrame < entity->anim.frameCount - 1)
            {
                entity->anim.currentFrame++;
            }

            if (entity->anim.currentFrame == entity->anim.frameCount - 1)
            {
                entity->anim.hasEnded = true;
            }
        }
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
        entity->despawningTimer += deltaTime;

        if (entity->despawningTimer >= entity->despawningDuration)
        {
            entity->despawningTimer = entity->despawningDuration;
            entity->hasDespawned = true;
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
 * Helper function: Render entity
 * TODO: Move renderer, shakeX, and shakeY to globals?
 * TODO: Make more defensive, check it can render entity or fallback to no image?
 */
void entity_render(Entity *entity, SDL_Renderer *renderer, int shakeX, int shakeY)
{
    if (!entity)
        return;

    // Clamp frame
    if (entity->anim.currentFrame < 0 || entity->anim.currentFrame > entity->anim.frameCount)
    {
        entity->anim.currentFrame = 0;
    }

    SpriteID frame = entity->anim.frames[entity->anim.currentFrame];
    SDL_Rect src = get_sprite(frame);
    SDL_Texture *texture = get_sprite_texture(frame);

    SDL_Rect dst = entity_rect(entity);
    dst.x += shakeX;
    dst.y += shakeY;

    SDL_SetTextureAlphaMod(texture, entity->alpha);
    SDL_RenderCopy(renderer, texture, &src, &dst);
}

/**
 * Helper function trigger despawn
 */
void entity_begin_despawn(Entity *entity, float duration_seconds)
{
    entity->isDespawning = true;
    entity->despawningTimer = 0.0f;
    entity->despawningDuration = duration_seconds;
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
