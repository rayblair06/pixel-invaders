#include "audio.h"
#include "boss.h"
#include "constants.h"
#include "enemies.h"
#include "game.h"
#include "player.h"
#include "particles.h"
#include "ui.h"

Boss currentBoss;
bool bossActive = false;

void init_boss(void)
{
    bossActive = false;
}

void spawn_boss(float x, float y, int wave)
{
    currentBoss.entity = create_entity(x, y, SPRITE_DRAW_SIZE * 4, SPRITE_DRAW_SIZE * 4);
    currentBoss.health = currentBoss.healthMax = 25 + (25 * (wave % 5));
    currentBoss.attackTimer = 3.0f;
    currentBoss.phaseTwo = false;
    currentBoss.active = true;
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
    if (currentBoss.entity.y < 50)
    {
        move(&currentBoss.entity, DOWN, 0.5f);
    }

    // Move left or right
    if (currentBoss.entity.y >= 50)
    {
        if (currentBoss.entity.x <= 0)
        {
            move(&currentBoss.entity, RIGHT, 2.0f);
        }

        if (currentBoss.entity.x >= 0)
        {
            move(&currentBoss.entity, LEFT, 2.0f);
        }
    }

    // Attack pattern
    currentBoss.attackTimer -= deltaTime;

    if (currentBoss.attackTimer <= 0)
    {
        boss_fire_laser();
        currentBoss.attackTimer = currentBoss.phaseTwo ? 1.5f : 3.0f;
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
    SDL_RenderCopy(renderer, texture, &src, &dst);
}

void render_boss_health(SDL_Renderer *renderer, TTF_Font *font)
{
    if (!bossActive)
        return;

    float healthRatio = (float)currentBoss.health / (float)currentBoss.healthMax;

    SDL_Rect bg = {100, SCREEN_HEIGHT - 30, SCREEN_WIDTH - 200, 20};
    SDL_Rect fill = {100, SCREEN_HEIGHT - 30, (int)((SCREEN_WIDTH - 200) * healthRatio), 20};

    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &bg);

    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_RenderFillRect(renderer, &fill);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &bg);

    generate_text(renderer, font, "The Abyssal Wraith", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 50, (SDL_Color){255, 255, 255});
}
