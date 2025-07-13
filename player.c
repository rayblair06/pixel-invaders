#include "constants.h"
#include "entity.h"
#include "player.h"
#include "sprites.h"

Entity player;
float playerSpeed = 4.0f;

int experience = 0;
int level = 0;
int experienceToNextLevel = 300;

/**
 * Initialises the player
 */
void init_player(void)
{
    player = create_entity(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);
}

/**
 * Handler for functionality of player
 */
void update_player(const Uint8 *keystate)
{
    // Move player based on key state
    if (keystate[SDL_SCANCODE_LEFT])
    {
        move(&player, LEFT, playerSpeed);
    }

    if (keystate[SDL_SCANCODE_RIGHT])
    {
        move(&player, RIGHT, playerSpeed);
    }

    // Keep player within screen bounds
    if (player.x < 0)
        player.x = 0;
    if (player.x > SCREEN_WIDTH - player.w)
        player.x = SCREEN_WIDTH - player.w;

    update_entity_rect(&player);
}

/**
 * Draw the player on the screen
 */
void render_player(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY)
{
    SDL_Rect src = get_sprite(SPR_PLAYER);
    SDL_Rect dst = player.rect;
    dst.x += shakeX;
    dst.y += shakeY;

    SDL_RenderCopy(renderer, spriteTexture, &src, &dst);
}

/**
 * Add experience to player
 */
void add_experience(int amount)
{
    experience += amount;

    if (experience >= experienceToNextLevel)
    {
        experience -= experienceToNextLevel;
        level++;
        experienceToNextLevel += 300;
    }
}
