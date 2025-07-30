#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "audio.h"
#include "constants.h"
#include "bullets.h"
#include "entity.h"
#include "game.h"
#include "particles.h"
#include "player.h"
#include "stats.h"
#include "sprites.h"

Entity player;
float playerSpeed = 4.0f;
bool isPlayerVisible = true;

int health = 100;
int healthMax = 100;

bool isPlayerExploding = false;
int explosionFrame = 0;
Uint32 explosionStartTime = 0;
int explosionFrameDurtion = 150; // ms per frame
int explosionFrameCount = 2;

// Upgrades
bool hasMultiShot = false;
bool hasSpreadShot = false;
bool hasPiercing = false;
bool hasExplosive = false;
bool hasHealthRegen = false;
bool hasShield = false;
bool hasPickupMagnet = false;

Uint32 lastRegenTime = 0;
const Uint32 regenCooldown = 2000; // ms

int regenAmount = 10;
Uint32 lastRegenHealthTime = 0;
Uint32 regenHealthCooldown = 10000; // ms

/**
 * Initialises the player
 */
void init_player(void)
{
    health = 100;
    healthMax = 100;

    playerSpeed = 4.0f;

    isPlayerVisible = true;
    isPlayerExploding = false;

    hasMultiShot = false;
    hasHealthRegen = false;
    hasShield = false;
    hasPickupMagnet = false;

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

    // Regenerate health
    if (hasHealthRegen && health < healthMax)
    {
        Uint32 now = SDL_GetTicks();

        if (now - lastRegenHealthTime > regenHealthCooldown)
        {
            health = health + regenAmount;
            lastRegenHealthTime = now;
        }
    }
}

/**
 * Draw the player on the screen
 */
void render_player(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    SDL_Rect src;
    SDL_Texture *texture = get_sprite_texture(SPR_PLAYER);

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

    // Render shield around player
    if (hasShield)
    {
        float time = SDL_GetTicks() / 1000.0f;

        float pulse = sinf(time * 4.0f) * 2.0f;

        // Pulse Size
        int baseRadius = 28;
        int radius = baseRadius + (int)pulse;

        // Pulse Colour
        // Blue (0,128,255) to Cyan (0,255,255)
        Uint8 green = (Uint8)(128 + (127 * (pulse + 1) / 2));

        SDL_SetRenderDrawColor(renderer, 0, green, 255, 255);

        draw_circle(renderer, player.x + player.w / 2, player.y + player.h / 2, radius);
    }

    SDL_Rect dst = player.rect;
    dst.x += shakeX;
    dst.y += shakeY;

    SDL_RenderCopy(renderer, texture, &src, &dst);
}

void trigger_player_shoot()
{
    // No shooty on gameover
    if (isGameOver)
        return;

    int bulletsFired = hasMultiShot ? 3 : 1;
    float offSets[3] = {0, -10, 10}; // Center, left, right
    float spreadAngles[3] = {0.0f, 0.0f, 0.0f};

    if (hasSpreadShot)
    {
        spreadAngles[0] = -0.2f;
        spreadAngles[1] = 0.0f;
        spreadAngles[2] = 0.2f;
    }

    SoundID fireSound = hasMultiShot ? SND_SHOOT : SND_SHOOT;

    play_sound(fireSound);

    for (int i = 0; i < bulletsFired; i++)
    {
        record_bullets_fired();
        spawn_bullet(
            player.x + player.w / 2 + offSets[i],
            player.y, spreadAngles[i]);
    }
}

void trigger_player_explosion()
{
    isPlayerExploding = true;
    explosionStartTime = SDL_GetTicks();
    explosionFrame = 0;

    spawn_explosion_particles(player.x, player.y, 20);
    play_sound(SND_EXPLOSION);
}
