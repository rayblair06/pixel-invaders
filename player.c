#include <SDL2/SDL_mixer.h>
#include "audio.h"
#include "constants.h"
#include "bullets.h"
#include "entity.h"
#include "game.h"
#include "player.h"
#include "sprites.h"

Entity player;
float playerSpeed = 4.0f;
bool isPlayerVisible = true;

int experience = 0;
int experienceToNextLevel = 300;

int level = 0;
bool isLevelUpPending = false;

bool isPlayerExploding = false;
int explosionFrame = 0;
Uint32 explosionStartTime = 0;
int explosionFrameDurtion = 150; // ms per frame
int explosionFrameCount = 2;

/**
 * Initialises the player
 */
void init_player(void)
{
    player = create_entity(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);
}

/**
 * Handle all of our 'tick' functionality of active player within the main game loop
 */
void tick_player(const Uint8 *keystate)
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

    // Shoot bullet if SPACE is pressed
    static bool spaceHeld = false;

    if (keystate[SDL_SCANCODE_SPACE])
    {
        // prevent holding space from firing too fast
        if (!spaceHeld)
        {
            trigger_player_shoot();
        }

        spaceHeld = true;
    }
    else
    {
        spaceHeld = false;
    }

    // Toggle explosion animations
    if (isPlayerExploding)
    {
        Uint32 elapsed = SDL_GetTicks() - explosionStartTime;
        explosionFrame = elapsed / explosionFrameDurtion;

        if (explosionFrame >= explosionFrameCount)
        {
            explosionFrame = explosionFrame - 1; // stop on last frame
            isPlayerExploding = false;
            isPlayerVisible = false;
        }
    }
}

/**
 * Draw the player on the screen
 */
void render_player(SDL_Renderer *renderer, SDL_Texture *spriteTexture, int shakeX, int shakeY)
{
    SDL_Rect src;

    if (!isPlayerVisible)
        return;

    if (isPlayerExploding)
    {
        SpriteID explosionSprites[] = {SPR_EXPLOSION_A, SPR_EXPLOSION_B};

        src = get_sprite(explosionSprites[explosionFrame]);
    }
    else
    {
        src = get_sprite(SPR_PLAYER);
    }

    SDL_Rect dst = player.rect;
    dst.x += shakeX;
    dst.y += shakeY;

    SDL_RenderCopy(renderer, spriteTexture, &src, &dst);
}

void trigger_player_shoot()
{
    // No shooty on gameover
    if (isGameOver)
        return;

    play_sound(SND_SHOOT);

    spawn_bullet(
        player.x + player.w / 2,
        player.y);
}

void trigger_player_explosion()
{
    isPlayerExploding = true;
    explosionStartTime = SDL_GetTicks();
    explosionFrame = 0;
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
