#include "audio.h"
#include "boss.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "player.h"
#include "particles.h"
#include "ui.h"
#include "waves.h"

Boss currentBoss;
bool bossActive = false;
Uint32 bossSpawnTime;

void init_boss(void)
{
    bossActive = false;
}

void spawn_boss(float x, float y, int wave)
{
    currentBoss.entity = create_entity(x, y, SPRITE_DRAW_SIZE * 4, SPRITE_DRAW_SIZE * 4);
    currentBoss.health = currentBoss.healthMax = 50 + (25 * (wave % bossWave));

    currentBoss.phaseTwo = false;
    currentBoss.active = true;

    currentBoss.attackTimer = 3.0f;

    currentBoss.chargingLaser = false;
    currentBoss.laserFiring = false;
    currentBoss.laserChargeTime = 0.0f;
    currentBoss.laserDuration = 0.0f;
    currentBoss.laserX = SCREEN_WIDTH / 2;

    currentBoss.laserSweepSpeed = 200.0f; // pixels per second
    currentBoss.laserDirection = 1;

    currentBoss.spawning = true;
    bossSpawnTime = SDL_GetTicks();
    currentBoss.spawningSpeed = 0.5f;

    currentBoss.isMoving = false; // Doesn't 'move' until spawned
    currentBoss.moveDirection = (bool)rand() % 2;
    currentBoss.movementSpeed = 2.0f + (1.0f * (wave % bossWave));
    bossActive = true;

    play_music(MUS_BOSS, true);
}

static void boss_fire_laser(void)
{
    // TODO: Replace with real attack
    play_sound(SND_LASER);
    spawn_explosion_particles(currentBoss.entity.x + currentBoss.entity.rect.w / 2, currentBoss.entity.y + currentBoss.entity.rect.h, 30);
}

void tick_boss(float deltaTime)
{
    if (!bossActive)
        return;

    // Move boss into view
    if (currentBoss.spawning)
    {
        if (currentBoss.entity.y < 70)
        {
            move(&currentBoss.entity, DOWN, currentBoss.spawningSpeed);
        }
        else
        {
            currentBoss.isMoving = true;
            currentBoss.spawning = false;
        }

        // Nothing beyond this should run if spawning
        return;
    }

    // Randomly change direction once per frame
    if (currentBoss.isMoving)
    {
        if (rand() % 100 == 0)
        {
            currentBoss.moveDirection = !currentBoss.moveDirection;
        }

        // Move left or right
        float bossMovementSpeed = !currentBoss.phaseTwo ? currentBoss.movementSpeed : currentBoss.movementSpeed + 0.5f;

        if (currentBoss.moveDirection)
        {
            move(&currentBoss.entity, RIGHT, currentBoss.movementSpeed);
        }

        if (!currentBoss.moveDirection)
        {
            move(&currentBoss.entity, LEFT, currentBoss.movementSpeed);
        }

        // Check boundaries
        if (currentBoss.entity.x >= SCREEN_WIDTH - currentBoss.entity.rect.w)
        {
            currentBoss.moveDirection = false; // Go left
        }

        if (currentBoss.entity.x <= 0)
        {
            currentBoss.moveDirection = true; // Go right
        }
    }

    // Laser Attack logic
    if (!currentBoss.chargingLaser && !currentBoss.laserFiring)
    {
        currentBoss.attackTimer -= deltaTime;

        if (currentBoss.attackTimer <= 0)
        {
            // Start charing the laster
            currentBoss.chargingLaser = true;
            currentBoss.laserChargeTime = !currentBoss.phaseTwo ? 1.5f : 1.0f; // Telegraph for 1.5 / 1.0 seconds

            if (!currentBoss.phaseTwo)
            {
                play_sound(SND_CHARGE1);
            }
            else
            {
                play_sound(SND_CHARGE2);
            }
        }
    }
    else if (currentBoss.chargingLaser)
    {
        currentBoss.laserChargeTime -= deltaTime;

        currentBoss.laserX = currentBoss.entity.rect.x + (currentBoss.entity.rect.w / 2); // Follow center of boss

        if (currentBoss.laserChargeTime <= 0)
        {
            // Fire laser
            currentBoss.chargingLaser = false;
            currentBoss.laserFiring = true;
            currentBoss.laserDuration = !currentBoss.phaseTwo ? 1.0f : 1.5f; // Laser lasts for 1.0 / 1.5 seconds
            play_sound(SND_LASER);
        }
    }
    else if (currentBoss.laserFiring)
    {
        currentBoss.laserDuration -= deltaTime;

        currentBoss.isMoving = false; // Stop to fire

        // In Phase 2, sweep the laser horizontally
        if (currentBoss.phaseTwo)
        {
            currentBoss.laserX += currentBoss.laserDirection * currentBoss.laserSweepSpeed * deltaTime;

            // Reverse direction if we hit screen bounds
            if (currentBoss.laserX < 20)
            {
                currentBoss.laserX = 20;
                currentBoss.laserDirection = 1;
            }
            else if (currentBoss.laserX > SCREEN_WIDTH - 20)
            {
                currentBoss.laserX = SCREEN_WIDTH - 20;
                currentBoss.laserDirection = -1;
            }
        }

        if (currentBoss.laserDuration <= 0)
        {
            currentBoss.laserFiring = false;
            currentBoss.isMoving = true;                                   // Resume moving
            currentBoss.attackTimer = !currentBoss.phaseTwo ? 3.5f : 2.0f; // Reset timer
        }
    }

    // Phase 2
    if (currentBoss.health <= 0 && !currentBoss.phaseTwo)
    {
        // Regains health for phase 2
        currentBoss.phaseTwo = true;
        currentBoss.health = currentBoss.healthMax;
        currentBoss.laserSweepSpeed = currentBoss.laserSweepSpeed + 50.0f;

        play_sound(SND_BOSS_ROAR);
    }

    // Check defeat
    if (currentBoss.health <= 0 && currentBoss.phaseTwo)
    {
        bossActive = false;
        play_sound(SND_EXPLOSION);
        spawn_explosion_particles(currentBoss.entity.x + currentBoss.entity.rect.w / 2, currentBoss.entity.y + currentBoss.entity.rect.h, 50);
        play_music(MUS_GAME, true); // return music to mornal
    }
}

void render_boss(SDL_Renderer *renderer, int shakeX, int shakeY)
{
    if (!bossActive)
        return;

    SDL_Rect src = get_sprite(SPR_INVADER5_A); // temp boss sprite
    SDL_Texture *texture = get_sprite_texture(SPR_INVADER5_A);

    SDL_Rect dst = currentBoss.entity.rect;
    dst.x += shakeX;
    dst.y += shakeY;

    if (currentBoss.chargingLaser)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Pulse effect
        float pulse = (sinf(SDL_GetTicks() / 200.0f) + 1.0f) / 2.0f;
        Uint8 alpha = (Uint8)(50 + 150 * pulse);

        if (currentBoss.phaseTwo)
        {
            // Draw a vertical band indicating sweep area
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, alpha);

            SDL_Rect warningLine = {
                (int)currentBoss.laserX - 2,
                currentBoss.entity.rect.y + currentBoss.entity.rect.h,
                4,
                SCREEN_HEIGHT - (currentBoss.entity.rect.y + currentBoss.entity.rect.h)};

            SDL_RenderFillRect(renderer, &warningLine);
        }
        else
        {
            // Draw a red line where the laser will fire
            for (int i = -2; i <= 2; i++)
            {
                Uint8 glowAlpha = alpha / (1 + abs(i));
                SDL_SetRenderDrawColor(renderer, 255, 50, 50, glowAlpha);
                SDL_RenderDrawLine(renderer,
                                   (int)currentBoss.laserX + i,
                                   currentBoss.entity.rect.y + currentBoss.entity.rect.h,
                                   (int)currentBoss.laserX + i,
                                   SCREEN_HEIGHT);
            }
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    if (currentBoss.laserFiring)
    {
        // Draw the actual laser beam
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 220);
        SDL_Rect laser = {
            (int)currentBoss.laserX - 4,
            currentBoss.entity.rect.y + currentBoss.entity.rect.h,
            8,
            SCREEN_HEIGHT - (currentBoss.entity.rect.y + currentBoss.entity.rect.h)};

        SDL_RenderFillRect(renderer, &laser);
    }

    SDL_RenderCopy(renderer, texture, &src, &dst);
}

void render_boss_health(SDL_Renderer *renderer, TTF_Font *font)
{
    if (!bossActive)
        return;

    float healthRatio = (float)currentBoss.health / (float)currentBoss.healthMax;

    SDL_Rect bg = {100, 30, SCREEN_WIDTH - 200, 20};
    SDL_Rect fill = {100, 30, (int)((SCREEN_WIDTH - 200) * healthRatio), 20};

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_RenderFillRect(renderer, &fill);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bg);

    generate_text(renderer, font, "The Abyssal Wraith", SCREEN_WIDTH / 2 - 100, 50, (SDL_Color){255, 255, 255});
}
