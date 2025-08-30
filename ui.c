#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "constants.h"
#include "fonts.h"
#include "game.h"
#include "level_manager.h"
#include "player.h"
#include "sprites.h"
#include "ui.h"
#include "waves.h"

// replace sprintf with safer sprintf_s
#define _CRT_SECURE_NO_WARNINGS

/**
 * Helper function for generating text on screen
 */
void generate_text(TTF_Font *font, const char *text, int x, int y, SDL_Color color)
{
    SDL_Renderer *renderer = app()->renderer;

    // Create surface and texture from text
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Define destination rectrangle
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};

    // Render the text
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

/**
 * Helper function to generate a single stat
 */
void render_stat(const char *label, int value, int x, int y, SDL_Color color)
{
    const TTF_Font *font = get_font(FT_SECONDARY, FT_MEDIUM);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s : %d", label, value);
    generate_text(font, buffer, x, y, color);
}

/**
 * Render stats panel
 */
void render_stats_panel(int x, int y, int lineHeight)
{
    render_stat("Wave", wave, x, y + lineHeight * 0, COLOR_WHITE);
}

// void render_panel(SDL_Renderer *renderer, int x, int y, int w, int h)
// {
//     const char tiles[] = {
//         MENU_TL,
//         MENU_T,
//         MENU_TR,
//         MENU_L,
//         MENU_C,
//         MENU_R,
//         MENU_BL,
//         MENU_B,
//         MENU_BR};
//     const int tileSize = TILE_SIZE;

//     for (int row = 0; row < 3; row++)
//     {
//         for (int col = 0; col < 3; col++)
//         {
//             int index = row * 3 + col;

//             SDL_Rect src = get_sprite(tiles[index]);
//             SDL_Texture *texture = get_sprite_texture(tiles[index]);

//             int dstW = (col == 1) ? w - 2 * tileSize : tileSize;
//             int dstH = (row == 1) ? h - 2 * tileSize : tileSize;

//             int dstX = x + (col == 0 ? 0 : (col == 1 ? tileSize : w - tileSize));
//             int dstY = y + (row == 0 ? 0 : (row == 1 ? tileSize : h - tileSize));

//             SDL_Rect dst = {dstX, dstY, dstW, dstH};

//             SDL_RenderCopy(renderer, texture, &src, &dst);
//         }
//     }
// }

void render_menu(const char *title, const char *options[], int optionCount, int selectedIndex, int paddingX, int paddingY)
{
    const TTF_Font *font = get_font(FT_SECONDARY, FT_MEDIUM);
    SDL_Renderer *renderer = app()->renderer;

    int titleHeight = (title != NULL) ? 40 : 0;

    int panelWidth = 300 + paddingX * 2;
    int panelHeight = titleHeight + optionCount * 40 + paddingY * 2;

    int panelX = SCREEN_WIDTH / 2 - panelWidth / 2;
    int panelY = SCREEN_HEIGHT / 2 - panelHeight / 2;

    // render_panel(renderer, panelX, panelY, panelWidth, panelHeight);

    // Optional: Generate Menu Title
    if (title != NULL)
    {
        TTF_Font *titleFont = TTF_OpenFont("assets/fonts/PixelifySans-SemiBold.ttf", 42);
        SDL_Color titleColor = COLOR_GOLD;
        SDL_Surface *titleSurface = TTF_RenderText_Blended(titleFont, title, titleColor);
        SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

        SDL_Color shadowColor = {0, 0, 0, 150};
        SDL_Surface *shadowSurface = TTF_RenderText_Blended(titleFont, title, shadowColor);
        SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);

        int titleX = panelX + panelWidth / 2 - titleSurface->w / 2;
        int titleY = panelY + paddingY - 40;
        int titleWidth = titleSurface->w;
        int titleHeight = titleSurface->h;

        SDL_Rect shadowRect = {
            titleX + 2,
            titleY + 2,
            titleWidth,
            titleHeight};

        SDL_RenderCopy(renderer, shadowTexture, NULL, &shadowRect);

        SDL_Rect titleRect = {
            titleX,
            titleY,
            titleWidth,
            titleHeight};

        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(shadowSurface);
        SDL_DestroyTexture(shadowTexture);
    }

    int optionStartY = panelY + paddingY + titleHeight;

    for (int i = 0; i < optionCount; i++)
    {
        const char *label = options[i];

        SDL_Surface *surface = TTF_RenderText_Blended(font, label, i == selectedIndex ? COLOR_YELLOW : COLOR_WHITE);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dst = {
            panelX + paddingX + (panelWidth - 2 * paddingX) / 2 - surface->w / 2,
            optionStartY + i * 40,
            surface->w,
            surface->h};

        SDL_RenderCopy(renderer, texture, NULL, &dst);

        if (i == selectedIndex)
        {
            int x = dst.x - 20;
            int y = dst.y + dst.h / 2;

            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

            for (int dy = -6; dy <= 6; dy++)
            {
                int lineLength = 6 - abs(dy);
                SDL_RenderDrawLine(renderer, x, y + dy, x + lineLength, y + dy);
            }
        }

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void render_upgrade_menu(const char *title, const char *titles[], const char *descriptions[], int optionCount, int selectedIndex, int paddingX, int paddingY)
{
    const TTF_Font *font = get_font(FT_SECONDARY, FT_MEDIUM);
    SDL_Renderer *renderer = app()->renderer;
    SDL_Color white = {255, 255, 255};
    SDL_Color gold = {255, 215, 0};
    SDL_Color yellow = {255, 255, 0};

    TTF_Font *titleFont = TTF_OpenFont("assets/fonts/PixelifySans-SemiBold.ttf", 42);
    TTF_Font *descriptionFont = TTF_OpenFont("assets/fonts/PixelifySans-SemiBold.ttf", 20);

    int titleHeight = (title != NULL) ? 40 : 0;

    int panelWidth = 300 + paddingX * 2;
    int panelHeight = titleHeight + optionCount * 40 + paddingY * 2;

    int panelX = SCREEN_WIDTH / 2 - panelWidth / 2;
    int panelY = SCREEN_HEIGHT / 2 - panelHeight / 2;

    // render_panel(renderer, panelX, panelY, panelWidth, panelHeight);

    // Optional: Generate Menu Title
    if (title != NULL)
    {
        SDL_Color titleColor = gold;
        SDL_Surface *titleSurface = TTF_RenderText_Blended(titleFont, title, titleColor);
        SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

        SDL_Color shadowColor = {0, 0, 0, 150};
        SDL_Surface *shadowSurface = TTF_RenderText_Blended(titleFont, title, shadowColor);
        SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);

        int titleX = panelX + panelWidth / 2 - titleSurface->w / 2;
        int titleY = panelY + paddingY - 40;
        int titleWidth = titleSurface->w;
        int titleHeight = titleSurface->h;

        SDL_Rect shadowRect = {
            titleX + 2,
            titleY + 2,
            titleWidth,
            titleHeight};

        SDL_RenderCopy(renderer, shadowTexture, NULL, &shadowRect);

        SDL_Rect titleRect = {
            titleX,
            titleY,
            titleWidth,
            titleHeight};

        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);
        SDL_FreeSurface(shadowSurface);
        SDL_DestroyTexture(shadowTexture);
    }

    int optionStartY = panelY + paddingY + titleHeight;

    for (int i = 0; i < optionCount; i++)
    {
        const char *label = titles[i];

        SDL_Surface *surface = TTF_RenderText_Blended(font, label, i == selectedIndex ? yellow : white);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dst = {
            panelX + paddingX + (panelWidth - 2 * paddingX) / 2 - surface->w / 2,
            optionStartY + i * 40,
            surface->w,
            surface->h};

        SDL_RenderCopy(renderer, texture, NULL, &dst);

        if (i == selectedIndex)
        {
            int x = dst.x - 20;
            int y = dst.y + dst.h / 2;

            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

            for (int dy = -6; dy <= 6; dy++)
            {
                int lineLength = 6 - abs(dy);
                SDL_RenderDrawLine(renderer, x, y + dy, x + lineLength, y + dy);
            }
        }

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void render_health_bar(int x, int y, int width, int height)
{
    const TTF_Font *font = get_font(FT_SECONDARY, FT_MEDIUM);
    SDL_Renderer *renderer = app()->renderer;
    SDL_Color white = {255, 255, 255, 255};

    // Draw background bar
    SDL_Rect bgRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255); // dark grey
    SDL_RenderFillRect(renderer, &bgRect);

    // Draw filled bar
    SDL_Rect fillRect = {x, y, (int)(width * ((float)health / healthMax)), height};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
    SDL_RenderFillRect(renderer, &fillRect);

    // Draw border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bgRect);

    // Draw experience text "current / total"
    char healthLabel[32];
    sprintf(healthLabel, "%d / %d", health, healthMax);

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, healthLabel, white);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x + (width - textSurface->w / 2) / 2, y + (height - textSurface->h / 2) / 2, textSurface->w / 2, textSurface->h / 2};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void render_xp_bar(int x, int y, int width, int height)
{
    const TTF_Font *font = get_font(FT_SECONDARY, FT_MEDIUM);
    SDL_Renderer *renderer = app()->renderer;

    float experienceProgress = experienceVisual / (float)experienceToNextLevel;

    // Draw background bar
    SDL_Rect bgRect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255); // dark grey
    SDL_RenderFillRect(renderer, &bgRect);

    // Draw filled bar
    SDL_Rect fillRect = {x, y, (int)(width * experienceProgress), height};
    SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255); // cyan blue
    SDL_RenderFillRect(renderer, &fillRect);

    // Draw border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bgRect);

    // Draw experience text "current / required"
    char experienceLabel[32];
    sprintf(experienceLabel, "%d / %d", experience, experienceToNextLevel);

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, experienceLabel, COLOR_WHITE);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x + (width - textSurface->w / 2) / 2, y + (height - textSurface->h / 2) / 2, textSurface->w / 2, textSurface->h / 2};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Draw level number overlay
    char levelLabel[16];
    sprintf(levelLabel, "Lv %d", playerLevel);
    generate_text(font, levelLabel, 220, SCREEN_HEIGHT - 28, COLOR_WHITE);
}

void render_vignette()
{
    SDL_Renderer *renderer = app()->renderer;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100); // dim

    int border = 25; // thickness of dark edges

    SDL_Rect top = {0, 0, SCREEN_WIDTH, border};
    SDL_Rect bottom = {0, SCREEN_HEIGHT - border, SCREEN_WIDTH, border};
    SDL_Rect left = {0, 0, border, SCREEN_HEIGHT};
    SDL_Rect right = {SCREEN_WIDTH - border, 0, border, SCREEN_HEIGHT};

    SDL_RenderFillRect(renderer, &top);
    SDL_RenderFillRect(renderer, &bottom);
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
