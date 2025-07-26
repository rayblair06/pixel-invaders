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

    currentBoss.attackTimer = 3.0f;
    currentBoss.telegraphing = false;
    currentBoss.telegraphTime = 0.0f;

    currentBoss.phaseTwo = false;
    currentBoss.active = true;

    currentBoss.spawning = true;
    bossSpawnTime = SDL_GetTicks();
    currentBoss.spawningSpeed = 0.5f;

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
            currentBoss.spawning = false;
        }

        // Nothing beyond this should run if spawning
        return;
    }

    // Randomly change direction once per frame
    if (rand() % 100 == 0)
    {
        currentBoss.moveDirection = !currentBoss.moveDirection;
    }

    // Move left or right

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

    // Attack pattern
    currentBoss.attackTimer -= deltaTime;

    if (currentBoss.telegraphing)
    {
        currentBoss.telegraphTime -= deltaTime;

        if (currentBoss.telegraphTime <= 0)
        {
            boss_fire_laser();
            currentBoss.telegraphing = false;
            currentBoss.attackTimer = currentBoss.phaseTwo ? 1.5f : 3.0f; // Reset attack timer
        }
    }
    else if (currentBoss.attackTimer <= 0)
    {
        // Start telegraph phase (flash warning)
        currentBoss.telegraphing = true;
        currentBoss.telegraphTime = 1.0f; // 1 second telegraph

        if (!currentBoss.phaseTwo)
        {
            play_sound(SND_CHARGE1);
        }
        else
        {
            play_sound(SND_CHARGE2);
        }
    }

    // Phase 2
    if (currentBoss.health <= 0 && !currentBoss.phaseTwo)
    {
        // Regains health for phase 2
        currentBoss.phaseTwo = true;
        currentBoss.health = currentBoss.healthMax;

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

    if (currentBoss.telegraphing)
    {
        // Alternative between red and normal for flashing effect
        if ((SDL_GetTicks() / 100) % 2 == 0)
        {
            SDL_SetTextureColorMod(texture, 255, 50, 50); // reddish
        }
        else
        {
            SDL_SetTextureColorMod(texture, 255, 255, 255); // normal
        }
    }
    else
    {
        SDL_SetTextureColorMod(texture, 255, 255, 255); // normal
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
