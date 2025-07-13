#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "constants.h"
#include "entity.h"
#include "sprites.h"
#include "player.h"

const int MAX_BULLETS = 100;
const int MAX_ENEMIES = 20;
const int MAX_PICKUPS = 100;

typedef Entity Bullet;
typedef Entity Enemy;

typedef struct
{
    SDL_Rect rect;
    bool active;
    bool falling;
} Pickup;

Bullet bullets[MAX_BULLETS];
Enemy enemies[MAX_ENEMIES];
Pickup pickups[MAX_PICKUPS];

float bulletSpeed = 6.0f;
float enemySpeed = 1.0f;

typedef enum
{
    UPGRADE_PLAYER_SPEED,
    UPGRADE_BULLET_SPEED,
} UpgradeType;

#define UPGRADE_COUNT 2

const char *upgrade_names[] = {
    "Player Speed",
    "Bullet Speed",
};

void apply_upgrade(UpgradeType upgrade)
{
    switch (upgrade)
    {
    case UPGRADE_PLAYER_SPEED:
        // playerSpeed += 1.0f;
        break;
    case UPGRADE_BULLET_SPEED:
        bulletSpeed += 1.5f;
        break;
    }
}

UpgradeType options[3];
int optionCount = 0;

void generate_upgrade_choices()
{
    // limit to 2 for now
    while (optionCount < 2)
    {
        UpgradeType pick = rand() % UPGRADE_COUNT;

        bool alreadyIncluded = false;

        for (int i = 0; i < optionCount; i++)
        {
            if (options[i] == pick)
                alreadyIncluded = true;
        }

        if (!alreadyIncluded)
        {
            options[optionCount++] = pick;
        }
    }
}

void update_bullet(Entity *bullet, float speed)
{
    move(bullet, UP, speed);

    if (bullet->y + bullet->h < 0)
    {
        // disappear off screen
        bullet->active = false;
    }
}

void update_enemy(Entity *enemy, float speed)
{
    move(enemy, DOWN, speed);

    // Disappear off screen
    if (enemy->y > SCREEN_HEIGHT)
    {
        enemy->active = false;
    }
}

//  AABB collision check (axis-aligned bounding box)
bool checkCollision(SDL_Rect a, SDL_Rect b)
{
    return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h && a.y + a.h > b.y);
}

int main(int argc, char *argv[])
{
    // Initialized game state
    int lives = 3;
    bool gameOver = false;
    // Freeze movement
    bool freeze = false;

    int level = 0;
    int experience = 0;
    int experienceToNextLevel = 300;

    bool levelUpPending = false;
    bool choosingUpgrade = false;
    int selectedOption = 0;

    int wave = 1;
    Uint32 lastWaveTime = 0;
    const Uint32 waveInterval = 3000; // 3 seconds between waves

    bool flashRed = false;
    Uint32 flashStartTime = 0;
    const Uint32 flashDuration = 200; // 200 ms flash

    bool shaking = false;
    Uint32 shakeStartTime = 0;
    const Uint32 shakeDuration = 200;
    int shakeOffsetX = 0;
    int shakeOffsetY = 0;

    bool enemyFrameToggle = false;
    Uint32 lastFrameSwitch = 0;
    const Uint32 frameInterval = 500; // ms

    bool playerVisible = true;

    bool playerExploding = false;
    int explosionFrame = 0;
    Uint32 explosionStartTime = 0;
    const int explosionFrameDurtion = 150; // ms per frame
    const int explosionFrameCount = 2;

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    init_sprites();

    // Initialize Image package
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        printf("Failed to initialize SDL_image: %s\n", IMG_GetError());
    }

    // Initialize Fonts
    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("fonts/Roboto-Regular.ttf", 24);

    if (!font)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    // Initialize Sounds
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! %s\n", Mix_GetError());
        return 1;
    }

    Mix_Chunk *sfx_hit = Mix_LoadWAV("assets/hit.wav");
    Mix_Chunk *sfx_shoot = Mix_LoadWAV("assets/shoot.wav");

    if (!sfx_hit || !sfx_shoot)
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Music *gameMusic = Mix_LoadMUS("assets/game-music.mp3");
    Mix_Music *gameOverMusic = Mix_LoadMUS("assets/game-over-music.mp3");

    if (!gameMusic || !gameOverMusic)
    {
        printf("Failed to load music: %s\n", Mix_GetError());
        return 1;
    }

    // Play BG Music
    Mix_PlayMusic(gameMusic, -1);

    // Mix_VolumeMusic(70);

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Pixel Invaders: Rogue Mode",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load Sprites
    int bgTileW = 64;
    int bgTileH = 64;

    SDL_Texture *bgTexture = IMG_LoadTexture(renderer, "assets/background.png");
    SDL_Texture *spriteTexture = IMG_LoadTexture(renderer, "assets/spritesheet.png");

    if (!bgTexture || !spriteTexture)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
    }

    bool running = true;

    // Main game loop
    SDL_Event event;

    // Initialize player position
    init_player();

    // keep track of key states
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    // Set all starting Bullet entities as inactive
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullets[i].active = false;
    }

    // Set all starting Enemy entities as inactive
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].active = false;
    }

    // Set all starting Pickup entities as inactive
    for (int i = 0; i < MAX_PICKUPS; i++)
    {
        pickups[i].active = false;
    }

    // Manually activate 5 enemies
    for (int i = 0; i < 5; i++)
    {
        enemies[i] = create_entity(100 + i * 120, 50, SPRITE_DRAW_SIZE, SPRITE_DRAW_SIZE);
    }

    // Main loop
    while (running)
    {
        // Handle events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        // Trigger Upgrade Menu delay
        if (levelUpPending && !choosingUpgrade)
        {
            generate_upgrade_choices();
            choosingUpgrade = true;
            levelUpPending = false;
        }

        // Upgrade Menu
        if (choosingUpgrade)
        {
            for (int i = 0; i < optionCount; i++)
            {
                SDL_Color white = {255, 255, 255};
                char label[64];
                snprintf(label, sizeof(label), "%s", upgrade_names[options[i]]);

                SDL_Surface *textSurface = TTF_RenderText_Solid(font, label, white);
                SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

                SDL_Rect dst = {SCREEN_WIDTH / 2 - 100, 150 + i * 40, textSurface->w, textSurface->h};
                SDL_RenderCopy(renderer, textTexture, NULL, &dst);

                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);
            }
        }

        if (choosingUpgrade)
        {
            if (keystate[SDL_SCANCODE_UP])
                selectedOption = (selectedOption - 1 + optionCount) % optionCount;
            if (keystate[SDL_SCANCODE_DOWN])
                selectedOption = (selectedOption + 1) % optionCount;

            if (keystate[SDL_SCANCODE_KP_ENTER])
            {
                apply_upgrade(options[selectedOption]);
                choosingUpgrade = false;
            }
        }

        if (!gameOver)
        {
            // Move player based on key state
            update_player(keystate);
        }

        // Shoot bullet if SPACE is pressed
        static bool spaceHeld = false;
        if (!gameOver && keystate[SDL_SCANCODE_SPACE])
        {
            if (!spaceHeld)
            { // prevent holding space from firing too fast
                Mix_PlayChannel(-1, sfx_shoot, 0);

                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!bullets[i].active)
                    {
                        bullets[i] = create_entity(
                            player.x + (player.w / 2) - (bullets[i].rect.w / 2), // center bullet from player
                            player.y,
                            SPRITE_DRAW_SIZE,
                            SPRITE_DRAW_SIZE);

                        break;
                    }
                }
            }

            spaceHeld = true;
        }
        else
        {
            spaceHeld = false;
        }

        // Move bullets
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
            {
                update_bullet(&bullets[i], bulletSpeed);
            }
        }

        // Move enemies down slowly
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (!gameOver && enemies[i].active)
            {
                move(&enemies[i], DOWN, enemySpeed);

                if (enemies[i].rect.y > SCREEN_HEIGHT)
                {
                    enemies[i].active = false; // remove off-screen
                    Mix_PlayChannel(-1, sfx_hit, 0);
                    lives--;

                    // Trigger red flash
                    flashRed = true;
                    flashStartTime = SDL_GetTicks();

                    // Trigger screenshake
                    shaking = true;
                    shakeStartTime = SDL_GetTicks();

                    if (lives <= 0)
                    {
                        gameOver = true;
                        playerExploding = true;
                        explosionStartTime = SDL_GetTicks();
                        explosionFrame = 0;

                        // Play gameover music
                        Mix_HaltMusic();
                        Mix_PlayMusic(gameOverMusic, 1);
                    }
                }
            }
        }

        // Check for collisions between bullets and enemies
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
                continue;

            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                if (!enemies[j].active)
                    continue;

                if (SDL_HasIntersection(&bullets[i].rect, &enemies[j].rect))
                {
                    // Collision! Remove both bullets and enemies

                    bullets[i].active = false;
                    enemies[j].active = false;

                    // Generate pickup
                    for (int k = 0; k < MAX_PICKUPS; k++)
                    {
                        if (!pickups[k].active)
                        {
                            pickups[k].active = true;
                            pickups[k].falling = true;
                            pickups[k].rect.x = enemies[j].rect.x + (enemies[j].rect.w / 2) - (pickups[k].rect.w / 2);
                            pickups[k].rect.y = enemies[j].rect.y;
                            pickups[k].rect.w = SPRITE_DRAW_SIZE;
                            pickups[k].rect.h = SPRITE_DRAW_SIZE;

                            break;
                        }
                    }

                    break;
                }
            }
        }

        // Pickups Handle
        for (int i = 0; i < MAX_PICKUPS; i++)
        {
            if (!pickups[i].active)
                continue;

            if (pickups[i].falling)
            {
                pickups[i].rect.y += 2;

                // Stop falling at player Y
                if (pickups[i].rect.y >= player.y)
                {
                    pickups[i].rect.y = player.y;
                    pickups[i].falling = false;
                }
            }

            // Check for collision with player
            if (SDL_HasIntersection(&player.rect, &pickups[i].rect))
            {
                pickups[i].active = false;
                experience += 100;

                if (experience >= experienceToNextLevel)
                {
                    experience -= experienceToNextLevel;
                    level++;
                    experienceToNextLevel += 300;

                    generate_upgrade_choices();
                    levelUpPending = true;
                }
            }
        }

        // Waves!
        Uint32 now = SDL_GetTicks();

        if (!gameOver && now - lastWaveTime > waveInterval)
        {
            // Time for next wave!
            lastWaveTime = now;
            wave++;

            // Spawn enemies for this wave
            // Progressively more each wave
            int enemiesToSpawn = wave + 2;
            int spawned = 0;

            for (int i = 0; i < MAX_ENEMIES && spawned < enemiesToSpawn; i++)
            {
                if (!enemies[i].active)
                {
                    enemies[i] = create_entity(
                        rand() % (SCREEN_WIDTH - enemies[i].rect.w),
                        0,
                        SPRITE_DRAW_SIZE,
                        SPRITE_DRAW_SIZE);

                    spawned++;
                }
            }
        }

        // Flash Red when hurt
        if (flashRed && SDL_GetTicks() - flashStartTime > flashDuration)
        {
            flashRed = false;
        }

        // Shake
        if (shaking)
        {
            if (SDL_GetTicks() - shakeStartTime > shakeDuration)
            {
                shaking = false;
                shakeOffsetX = 0;
                shakeOffsetY = 0;
            }
            else
            {
                // Shamml random offset between -5 and 5 pixels
                shakeOffsetX = (rand() % 11) - 5;
                shakeOffsetY = (rand() % 11) - 5;
            }
        }

        // Toggle enemy animations
        if (now - lastFrameSwitch > frameInterval)
        {
            enemyFrameToggle = !enemyFrameToggle;
            lastFrameSwitch = SDL_GetTicks();
        }

        // Toggle explosion animations
        if (playerExploding)
        {
            Uint32 elapsed = SDL_GetTicks() - explosionStartTime;
            explosionFrame = elapsed / explosionFrameDurtion;

            if (explosionFrame >= explosionFrameCount)
            {
                explosionFrame = explosionFrame - 1; // stop on last frame
                playerExploding = false;
                playerVisible = false;
            }
        }

        // Render screen
        if (flashRed)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // flash red
            SDL_RenderClear(renderer);
        }
        else
        {
            // Draw Background
            for (int y = 0; y < SCREEN_HEIGHT; y += bgTileH)
            {
                for (int x = 0; x < SCREEN_WIDTH; x += bgTileW)
                {
                    SDL_Rect dest = {x + shakeOffsetX, y + shakeOffsetY, bgTileW, bgTileH};
                    SDL_RenderCopy(renderer, bgTexture, NULL, &dest);
                }
            }
        }

        // Draw player

        if (gameOver && playerExploding)
        {
            SpriteID explosionSprites[] = {SPR_EXPLOSION_A, SPR_EXPLOSION_B};
            SDL_Rect playerSrc = get_sprite(explosionSprites[explosionFrame]);

            SDL_RenderCopy(renderer, spriteTexture, &playerSrc, &player.rect);
        }
        else if (playerVisible)
        {
            render_player(renderer, spriteTexture, shakeOffsetX, shakeOffsetY);
        }

        // Draw bullets
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
                continue;

            SDL_Rect bulletSrc = get_sprite(SPR_BULLET1);
            SDL_RenderCopy(renderer, spriteTexture, &bulletSrc, &bullets[i].rect);
        }

        // Draw enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (!enemies[i].active)
                continue;

            SDL_Rect enemySrc = get_sprite(enemyFrameToggle ? SPR_INVADER1_A : SPR_INVADER1_B);
            SDL_RenderCopy(renderer, spriteTexture, &enemySrc, &enemies[i].rect);
        }

        // Draw pickups
        for (int i = 0; i < MAX_PICKUPS; i++)
        {
            if (!pickups[i].active)
                continue;

            SDL_Rect pickupSrc = get_sprite(SPR_BULLET3); // Replace with pickup animation
            SDL_Rect pickupDraw = pickups[i].rect;
            pickupDraw.x += shakeOffsetX;
            pickupDraw.y += shakeOffsetY;

            SDL_RenderCopy(renderer, spriteTexture, &pickupSrc, &pickupDraw);
        }

        // Convert Experience to string
        char experienceText[32];
        sprintf(experienceText, "Experience: %d", experience);

        // Create surface and texture from text
        SDL_Color white = {225, 255, 255, 255};
        SDL_Surface *textSurface = TTF_RenderText_Solid(font, experienceText, white);
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        // Define destination rectrangle
        SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};

        // Render the experience text
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        // Display current wave
        char waveText[32];
        sprintf(waveText, "Wave: %d", wave);

        SDL_Surface *waveSurface = TTF_RenderText_Solid(font, waveText, white);
        SDL_Texture *waveTexture = SDL_CreateTextureFromSurface(renderer, waveSurface);
        SDL_Rect waveRect = {10, 40, waveSurface->w, waveSurface->h};
        SDL_RenderCopy(renderer, waveTexture, NULL, &waveRect);

        // Draw lives on screen
        char livesText[32];
        sprintf(livesText, "Lives: %d", lives);

        SDL_Surface *livesSurface = TTF_RenderText_Solid(font, livesText, white);
        SDL_Texture *livesTexture = SDL_CreateTextureFromSurface(renderer, livesSurface);
        SDL_Rect livesRect = {10, 70, livesSurface->w, livesSurface->h};
        SDL_RenderCopy(renderer, livesTexture, NULL, &livesRect);

        // Render Game over screen
        if (gameOver)
        {
            SDL_Surface *overSurface = TTF_RenderText_Solid(font, "GAME OVER", white);
            SDL_Texture *overTexture = SDL_CreateTextureFromSurface(renderer, overSurface);
            SDL_Rect overRect = {SCREEN_WIDTH / 2 - overSurface->w / 2, SCREEN_HEIGHT / 2 - overSurface->h / 2, overSurface->w, overSurface->h};
            SDL_RenderCopy(renderer, overTexture, NULL, &overRect);
            SDL_FreeSurface(overSurface);
            SDL_DestroyTexture(overTexture);
        }

        // Clean up
        SDL_FreeSurface(livesSurface);
        SDL_DestroyTexture(livesTexture);

        SDL_FreeSurface(waveSurface);
        SDL_DestroyTexture(waveTexture);

        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        // Show the render
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS

        // Debugging!
        // printf("experience: %d\r", experience);
        fflush(stdout);
    }

    // Clean up
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(spriteTexture);
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    Mix_FreeChunk(sfx_hit);
    Mix_FreeChunk(sfx_shoot);
    Mix_FreeMusic(gameMusic);
    Mix_FreeMusic(gameOverMusic);
    Mix_CloseAudio();
    SDL_Quit();
    TTF_Quit();

    return 0;
}
