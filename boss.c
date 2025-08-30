#include <stdlib.h>
#include "audio.h"
#include "boss.h"
#include "bullets.h"
#include "constants.h"
#include "enemies.h"
#include "fonts.h"
#include "game.h"
#include "player.h"
#include "particles.h"
#include "stats.h"
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
    currentBoss.entity = create_entity(
        x - (128 / 2),
        y,
        128 * 2,
        72 * 2);
    currentBoss.entity.hitboxScale = 0.9f;
    currentBoss.entity.anim = spaceship9Anim;
    currentBoss.health = currentBoss.healthMax = 200 + (25 * (wave % bossWave));

    currentBoss.phaseTwo = false;
    currentBoss.active = true;

    currentBoss.attackTimer = 3.0f;
    currentBoss.targetX = currentBoss.entity.pos.x;

    currentBoss.sideGunsAttackTimer = 2.0f;
    currentBoss.isReadyToShoot = false;
    currentBoss.isSideGunsShooting = false;

    currentBoss.chargingLaser = false;
    currentBoss.laserFiring = false;
    currentBoss.laserChargeTime = 0.0f;
    currentBoss.laserDuration = 0.0f;
    currentBoss.laserX = SCREEN_WIDTH / 2;
    currentBoss.laserImpactPulse = 0.0f;

    currentBoss.laserDamageCooldown = 0.5f;
    currentBoss.laserDamageTimer = 0.0f;

    currentBoss.laserSweepSpeed = 200.0f; // pixels per second
    currentBoss.laserDirection = 1;

    currentBoss.spawning = true;
    bossSpawnTime = get_game_ticks();
    currentBoss.spawningSpeed = 0.5f;

    currentBoss.isMoving = false; // Doesn't 'move' until spawned
    currentBoss.moveDirection = (bool)rand() % 2;
    currentBoss.movementSpeed = 2.0f + (1.0f * (wave % bossWave));
    bossActive = true;

    play_sound(SND_BOSS_ROAR);
    play_music(MUS_BOSS, true);
}

static void boss_fire_laser(void)
{
    // TODO: Replace with real attack
    play_sound(SND_LASER);
    spawn_explosion_particles(currentBoss.entity.pos.x + currentBoss.entity.size.x / 2, currentBoss.entity.pos.y + currentBoss.entity.size.y, 30);
}

void tick_boss()
{
    if (!bossActive)
        return;

    float deltaTime = get_delta_time();

    entity_tick(&currentBoss.entity);

    // Move boss into view
    if (currentBoss.spawning)
    {
        if (currentBoss.entity.pos.y < 70)
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

    // Bobbing
    float bobbing = 0.5f * sinf(get_game_ticks() / 500.0f);
    currentBoss.entity.pos.y = currentBoss.entity.pos.y + bobbing;

    if (currentBoss.isMoving)
    {
        static float wanderTimer = 0.0f;
        static float randomOffset = 0.0f;

        // Randomly wander
        wanderTimer -= deltaTime;

        if (wanderTimer <= 0.0f)
        {
            wanderTimer = 1.5f + (rand() % 1500) / 1000.0f; // random 1.5 to 3.0 seconds
            randomOffset = (rand() % 41 - 20);              // Random offset between -20 and +20 pixels
        }

        // Slowly decay draftOffset back towards 0
        randomOffset *= (1.0f - 2.0f * deltaTime);
        if (fabs(randomOffset) < 0.1f)
            randomOffset = 0.0f; // Avoid jitter

        // Determine target position (player's x + some random offset)
        float playerCenter = player.entity.pos.x + player.entity.size.x / 2;
        currentBoss.targetX = playerCenter + randomOffset;
        currentBoss.sideGunsTargetX = playerCenter + randomOffset;

        // Move towards targetX
        float bossMovementSpeed = !currentBoss.phaseTwo ? currentBoss.movementSpeed : currentBoss.movementSpeed + 0.5f;
        float smoothing = bossMovementSpeed * deltaTime;
        if (smoothing > 1.0f)
            smoothing = 1.0f;

        currentBoss.entity.pos.x += (currentBoss.targetX - currentBoss.entity.pos.x - currentBoss.entity.size.x / 2) * smoothing;
    }

    // Keep within screen bounds
    if (currentBoss.entity.pos.x < 0)
    {
        currentBoss.entity.pos.x = 0;
    }

    if (currentBoss.entity.pos.x > SCREEN_WIDTH - currentBoss.entity.size.x)
    {
        currentBoss.entity.pos.x = SCREEN_WIDTH - currentBoss.entity.size.x;
    }

    // // Randomly change direction once per frame
    // if (currentBoss.isMoving)
    // {
    //     if (rand() % 100 == 0)
    //     {
    //         currentBoss.moveDirection = !currentBoss.moveDirection;
    //     }

    //     // Move left or right
    //     float bossMovementSpeed = !currentBoss.phaseTwo ? currentBoss.movementSpeed : currentBoss.movementSpeed + 0.5f;

    //     if (currentBoss.moveDirection)
    //     {
    //         move(&currentBoss.entity, RIGHT, currentBoss.movementSpeed);
    //     }

    //     if (!currentBoss.moveDirection)
    //     {
    //         move(&currentBoss.entity, LEFT, currentBoss.movementSpeed);
    //     }

    //     // Check boundaries
    //     if (currentBoss.entity.pos.x >= SCREEN_WIDTH - currentBoss.entity.size.x)
    //     {
    //         currentBoss.moveDirection = false; // Go left
    //     }

    //     if (currentBoss.entity.pos.x <= 0)
    //     {
    //         currentBoss.moveDirection = true; // Go right
    //     }
    // }

    // Side guns logic
    if (!currentBoss.isReadyToShoot && !currentBoss.isSideGunsShooting)
    {
        currentBoss.sideGunsAttackTimer -= deltaTime;

        if (currentBoss.sideGunsAttackTimer <= 0)
        {
            currentBoss.isReadyToShoot = true;
        }
    }

    if (currentBoss.isReadyToShoot)
    {
        currentBoss.isSideGunsShooting = true;
        currentBoss.isReadyToShoot = false;
    }

    if (!currentBoss.isReadyToShoot && currentBoss.isSideGunsShooting)
    {
        int bossCenterX = currentBoss.entity.pos.x + (currentBoss.entity.size.x / 2);
        int bossTurrentY = currentBoss.entity.pos.y + (currentBoss.entity.size.y - 35);

        int bossTurrentOneX = bossCenterX - 80;
        int bossTurrentTwoX = bossCenterX - 55;
        int bossTurrentThreeX = bossCenterX + 55;
        int bossTurrentFourX = bossCenterX + 80;

        // TODO: Move to Boss struct
        const int damage = 10;

        // Fire 4 bullets
        // TODO: Y offsetchat
        entity_fire_twin(&currentBoss.entity, enemyBulletSpeed, damage, 80.0f);
        entity_fire_twin(&currentBoss.entity, enemyBulletSpeed, damage, 55.0f);

        // Reset firing logic
        currentBoss.isSideGunsShooting = false;
        currentBoss.sideGunsAttackTimer = !currentBoss.phaseTwo ? 2.0f : 0.5f;
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

        currentBoss.laserX = currentBoss.entity.pos.x + (currentBoss.entity.size.x / 2); // Follow center of boss

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

        currentBoss.laserImpactPulse += deltaTime * 6.0f; // Fast oscillation
        if (currentBoss.laserImpactPulse > 2 * M_PI)
        {
            currentBoss.laserImpactPulse -= 2 * M_PI;
        }

        // In Phase 2, sweep the laser horizontally
        if (currentBoss.phaseTwo)
        {
            // Phase 2: Sweeping laser
            currentBoss.laserX += currentBoss.laserDirection * currentBoss.laserSweepSpeed * deltaTime;

            // Reverse direction if we hit screen bounds
            if (currentBoss.laserX <= 20)
            {
                currentBoss.laserX = 20;
                currentBoss.laserDirection = 1;
            }
            else if (currentBoss.laserX >= SCREEN_WIDTH - 20)
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
        spawn_explosion_particles(currentBoss.entity.pos.x + currentBoss.entity.size.x / 2, currentBoss.entity.pos.y + currentBoss.entity.size.y, 50);
        play_music(MUS_GAME, true); // return music to normal
    }
}

void render_boss(int shakeX, int shakeY)
{
    SDL_Renderer *renderer = app()->renderer;

    if (!bossActive)
        return;

    entity_render(&currentBoss.entity, shakeX, shakeY);

    // Flash red when damaged
    if (currentBoss.entity.isHit)
    {
        currentBoss.entity.color.r = 255;
        currentBoss.entity.color.g = 64;
        currentBoss.entity.color.b = 64;
    }
    else
    {
        currentBoss.entity.color.r = 255;
        currentBoss.entity.color.g = 255;
        currentBoss.entity.color.b = 255;
    }

    if (currentBoss.chargingLaser)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Pulse effect
        float pulse = (sinf(get_game_ticks() / 200.0f) + 1.0f) / 2.0f;
        Uint8 alpha = (Uint8)(50 + 150 * pulse);

        if (currentBoss.phaseTwo)
        {
            // Phase 2: Sweep telegtaph - multiple vertical lines
            int lineSpacing = 40; // Distance between telegraph lines

            for (int x = 20; x < SCREEN_WIDTH - 20; x += lineSpacing)
            {
                for (int i = -2; i <= 2; i++)
                {
                    Uint8 glowAlpha = alpha / (1 + abs(i));
                    SDL_SetRenderDrawColor(renderer, 255, 50, 50, glowAlpha);
                    SDL_RenderDrawLine(renderer,
                                       x + i,
                                       currentBoss.entity.pos.y + currentBoss.entity.size.y,
                                       x + i,
                                       SCREEN_HEIGHT);
                }
            }
        }
        else
        {
            // Phase 1: Single glowing telegraph line
            for (int i = -2; i <= 2; i++)
            {
                Uint8 glowAlpha = alpha / (1 + abs(i));
                SDL_SetRenderDrawColor(renderer, 255, 50, 50, glowAlpha);
                SDL_RenderDrawLine(renderer,
                                   (int)currentBoss.laserX + i,
                                   currentBoss.entity.pos.y + currentBoss.entity.size.y,
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
            currentBoss.entity.pos.y + currentBoss.entity.size.y,
            8,
            SCREEN_HEIGHT - (currentBoss.entity.pos.y + currentBoss.entity.size.y)};

        SDL_RenderFillRect(renderer, &laser);

        // TODO: Lol, this looks shit but kind of works?!
        // Pulse alpha and size
        float pulse = (sinf(currentBoss.laserImpactPulse) + 1.0f) / 2.0f;
        int impactSize = 20 + (int)(10 * pulse); // Size oscillates slightly
        Uint8 alpha = 180 + (Uint8)(75 * pulse); // Alpha oscillates 180-255

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, alpha); // Bright yelllowish spark

        SDL_Rect impact = {
            (int)currentBoss.laserX - impactSize / 2,
            SCREEN_HEIGHT - impactSize, // Floor impact
            impactSize,
            impactSize / 2};
        SDL_RenderFillRect(renderer, &impact);

        // Draw a second softer glow around it
        SDL_SetRenderDrawColor(renderer, 255, 100, 50, alpha / 2);
        SDL_Rect glow = {
            impact.x - 5,
            impact.y - 5,
            impact.w + 10,
            impact.h + 10};
        SDL_RenderFillRect(renderer, &glow);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        // Particles!
        spawn_explosion_particles(currentBoss.laserX, SCREEN_HEIGHT, 10);

        trigger_screen_shake();
    }
}

void render_boss_health()
{
    const TTF_Font *font = get_font(FT_SECONDARY, FT_MEDIUM);
    SDL_Renderer *renderer = app()->renderer;

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

    generate_text(font, "The Abyssal Wraith", SCREEN_WIDTH / 2 - 100, 50, COLOR_WHITE);
}

void damage_boss(Boss *boss, int amount)
{
    boss->health = boss->health - amount;

    entity_trigger_hit(&boss->entity, 0.05f);
    play_sound(SND_HIT);

    if (boss->health <= 0)
    {
        // They dead, fade out and deactivate
        entity_begin_despawn(&boss->entity, 0.5f);

        record_boss_kill();
    }
}
