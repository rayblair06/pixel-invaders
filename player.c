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

Player player;

float playerVelX = 0.0f;
float playerVelY = 0.0f;
float playerAccel = 300.0f;    // pixels per second squared
float playerDrag = 400.0f;     // drag force when not accelerating
float playerMaxSpeed = 150.0f; // clamp max veolcity

float movementHoldTime = 0.0f;
float boostSpeedMultiplier = 1.0f;

const float maxBoost = 1.2f;  // Max speed multiplier
const float boostRate = 0.8f; // How fast the boost builds per second
const float decayRate = 1.2f; // How fast the boost decays when not held

int health = 100;
int healthMax = 100;

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

    playerMaxSpeed = 150.0f;

    hasMultiShot = false;
    hasHealthRegen = false;
    hasShield = false;
    hasPickupMagnet = false;

    player.entity = create_entity(SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 80, 32 * 2, 32 * 2);
    player.entity.anim = spaceship1Anim;
}

/**
 * Handle all of our 'tick' functionality of active player within the main game loop
 */
void tick_player(const Uint8 *keystate)
{
    float direction = 0;
    float deltaTime = get_delta_time();

    entity_tick(&player.entity);

    // If we've exploded, unalive player on animation end
    if (player.entity.isDespawning)
    {
        if (player.entity.anim.hasEnded == true)
        {
            player.entity.isActive = false;
        }

        return;
    }

    bool moving = keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_RIGHT];

    if (moving)
    {
        movementHoldTime += deltaTime;
        boostSpeedMultiplier += boostRate * deltaTime;

        if (boostSpeedMultiplier > maxBoost)
        {
            boostSpeedMultiplier = maxBoost;
        }
    }
    else
    {
        movementHoldTime = 0.0f;
        boostSpeedMultiplier -= decayRate * deltaTime;

        if (boostSpeedMultiplier < 1.0f)
        {
            boostSpeedMultiplier = 1.0f;
        }
    }

    // Display Boost particles
    // if (boostSpeedMultiplier > 1.0f)
    // {
    // float px = player.entity.pos.x + player.entity.size.x / 2;
    // float py = player.entity.pos.y + player.entity.size.y;
    // spawn_boost_particle(px, py);
    // }

    // Move player based on key state
    if (keystate[SDL_SCANCODE_LEFT])
    {
        direction -= 1;
    }

    if (keystate[SDL_SCANCODE_RIGHT])
    {
        direction += 1;
    }

    if (direction != 0)
    {
        // Accelerate in the desired direction
        playerVelX += direction * playerAccel * deltaTime;

        // If changing direction, reduce velocity sharply
        if ((direction < 0 && playerVelX > 0) || (direction > 0 && playerVelX < 0))
        {
            playerVelX += 0.85f; // dampen quickly when changing direction
        }
    }
    else
    {
        // Apply drag when no input
        if (playerVelX > 0)
        {
            playerVelX -= playerDrag * deltaTime;
            if (playerVelX < 0)
            {
                playerVelX = 0;
            }
        }
        else if (playerVelX < 0)
        {
            playerVelX += playerDrag * deltaTime;

            if (playerVelX > 0)
            {
                playerVelX = 0;
            }
        }
    }

    // Clamp max speed
    if (playerVelX > playerMaxSpeed)
    {
        playerVelX = playerMaxSpeed;
    }
    else if (playerVelX < -playerMaxSpeed)
    {
        playerVelX = -playerMaxSpeed;
    }

    // Apply to position
    player.entity.pos.x += (playerVelX * deltaTime * boostSpeedMultiplier);

    // Keep player within bounds
    if (player.entity.pos.x < 0)
    {
        player.entity.pos.x = 0;
    }

    if (player.entity.pos.x > SCREEN_WIDTH - player.entity.size.x)
    {
        player.entity.pos.x = SCREEN_WIDTH - player.entity.size.x;
    }

    // Camera Drift
    // float cameraDriftFactor = 0.2f; // how much drift is applied
    // float cameraSmoothness = 5.0f;  // higher = smoother (slower to catch up)

    // // Target offset based on player velocity
    // float targetX = -playerVelX * cameraDriftFactor;
    // float targetY = -playerVelY * cameraDriftFactor; // Will most likely remain 0 as we don't move on Y for player

    // // Smoothly approach target offset
    // cameraOffsetX += (targetX - cameraOffsetX) * deltaTime * cameraSmoothness;
    // cameraOffsetY += (targetY - cameraOffsetY) * deltaTime * cameraSmoothness;

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

    // Regenerate health
    if (hasHealthRegen && health < healthMax)
    {
        Uint32 now = get_game_ticks();

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
    if (!player.entity.isActive)
        return;

    SpriteID frame = player.entity.anim.frames[player.entity.anim.currentFrame];
    SDL_Rect src = get_sprite(frame);
    SDL_Texture *texture = get_sprite_texture(frame);

    // Render shield around player
    if (hasShield)
    {
        float time = get_game_ticks() / 1000.0f;

        float pulse = sinf(time * 4.0f) * 2.0f;

        // Pulse Size
        int baseRadius = 28;
        int radius = baseRadius + (int)pulse;

        // Pulse Colour
        // Blue (0,128,255) to Cyan (0,255,255)
        Uint8 green = (Uint8)(128 + (127 * (pulse + 1) / 2));

        SDL_SetRenderDrawColor(renderer, 0, green, 255, 255);

        draw_circle(renderer, player.entity.pos.x + player.entity.size.x / 2, player.entity.pos.y + player.entity.size.y / 2, radius);
    }

    SDL_Rect dst = entity_rect(&player.entity);
    dst.x += shakeX;
    dst.y += shakeY;

    SDL_SetTextureAlphaMod(texture, player.entity.alpha);
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
            player.entity.pos.x + player.entity.size.x / 2 + offSets[i],
            player.entity.pos.y, spreadAngles[i]);
    }
}

void trigger_player_explosion()
{
    // Duration should be longer than animation, we'll trigger despawn after animation has finished
    const float despawnDuration = 0.5f;
    entity_begin_despawn(
        &player.entity,
        despawnDuration);

    player.entity.anim = explosion1Anim;

    spawn_explosion_particles(
        player.entity.pos.x + (player.entity.size.x / 2),
        player.entity.pos.y + (player.entity.size.y / 2),
        20);

    play_sound(SND_EXPLOSION);
}
