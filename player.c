#include <SDL2/SDL_mixer.h>
#include "constants.h"
#include "bullets.h"
#include "entity.h"
#include "game.h"
#include "player.h"
#include "sprites.h"

Entity player;
float playerSpeed = 4.0f;

int experience = 0;
int experienceToNextLevel = 300;

int level = 0;
bool isLevelUpPending = false;

/**
 * Initialises the player
 */
void init_player(void)
{
    player = create_entity(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);
}

void player_shoot(const Uint8 *keystate, Mix_Chunk *soundEffect)
{
    if (isGameOver)
        return;

    // Shoot bullet if SPACE is pressed
    static bool spaceHeld = false;

    if (keystate[SDL_SCANCODE_SPACE])
    {
        if (!spaceHeld)
        {                                        // prevent holding space from firing too fast
            Mix_PlayChannel(-1, soundEffect, 0); // TODO: Move Audio to it's own module

            spawn_bullet(
                player.x + player.w / 2,
                player.y);
        }

        spaceHeld = true;
    }
    else
    {
        spaceHeld = false;
    }
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

        // generate_upgrade_choices();
        isLevelUpPending = true;
    }
}
