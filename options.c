#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "options.h"
#include "ui.h"

VideoSettings gVideo = {VIDEO_WINDOWED, 0};

// Small curated list, apply enumerated via SDL later
Resolution gResList[] = {
    {1024, 768},
    {1280, 720},
    {1600, 900},
    {1920, 1080},
};
int gResCount = sizeof(gResList) / sizeof(gResList[0]);

static SDL_Window *s_window = NULL;
static bool s_open = false;
static int s_row = 0; // 0 = Mode, 1 = Resolution, 2 = Apply, 3 = Back
static char s_labels_mode[VIDEO_MODE_COUNT][16] = {"Windowed", "Borderless", "Fullscreen"};

// Input repeat
static float s_navCooldown = 0.0f;

static void set_borderless(SDL_Window *window, bool on)
{
    Uint32 flags = on ? SDL_FALSE : SDL_TRUE;
    SDL_SetWindowBordered(window, flags);
}

static void set_fullscreen(SDL_Window *window, bool on)
{
    Uint32 flags = on ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    SDL_SetWindowFullscreen(window, flags);
}

void options_init(SDL_Window *window)
{
    s_window = window;

    // Try to pick the closest list resolution to current window
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    int best = 0;
    int bestScore = 1e9;

    for (int i = 0; i < gResCount; ++i)
    {
        int dw = gResList[i].w - w;
        int dh = gResList[i].h - h;

        int score = dw * dw + dh * dh;

        if (score < bestScore)
        {
            bestScore = score;
            best = i;
        }
    }

    gVideo.resolutionIndex = best;
    gVideo.videoMode = VIDEO_WINDOWED;
    options_apply(NULL);
}

void options_apply(SDL_Window *window)
{
    SDL_Window *win = window ? window : s_window;
    if (!win)
        return;

    Resolution r = gResList[gVideo.resolutionIndex];

    switch (gVideo.videoMode)
    {
    case VIDEO_WINDOWED:
        set_fullscreen(win, false);
        set_borderless(win, false);
        SDL_SetWindowSize(win, r.w, r.h);
        SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        break;
    case VIDEO_BORDERLESS:
        set_fullscreen(win, false);
        set_borderless(win, true);

        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm);
        SDL_SetWindowSize(win, dm.w, dm.h);
        SDL_SetWindowPosition(win, 0, 0);
        break;
    case VIDEO_FULLSCREEN:
        set_borderless(win, true);
        set_fullscreen(win, true);
        break;
    case VIDEO_MODE_COUNT:
        fprintf(stderr, "Error: VIDEO_MODE_COUNT used as a real mode!\n");
        break;
    }
}

bool options_is_open(void) { return s_open; }
void options_open(void) { s_open = true, s_row = 0; }
void options_close(void) { s_open = false; }

void options_select_next_row(void) { s_row = (s_row + 1) % 4; }
void options_select_prev_row(void) { s_row = (s_row + 3) % 4; }

void options_adjust_value(int direction)
{
    if (s_row == 0)
    {
        // Mode
        int m = (int)gVideo.videoMode + direction;
        if (m < 0)
            m = VIDEO_MODE_COUNT - 1;
        if (m >= VIDEO_MODE_COUNT)
            m = 0;
        gVideo.videoMode = (VideoMode)m;
    }
    else if (s_row == 1)
    {
        // Resolution
        int idx = gVideo.resolutionIndex + direction;
        if (idx < 0)
            idx = gResCount - 1;
        if (idx >= gResCount)
            idx = 0;
        gVideo.resolutionIndex = idx;
    }
}

void options_handle_input(const Uint8 *keys, const Uint8 *prev)
{
    if (!s_open)
        return;

// edge detection helper
#define PRESSED(sc) (keys[sc] && !prev[sc])

    if (PRESSED(SDL_SCANCODE_ESCAPE))
    {
        options_close();
        return;
    }
    if (PRESSED(SDL_SCANCODE_UP))
        options_select_prev_row();
    if (PRESSED(SDL_SCANCODE_DOWN))
        options_select_next_row();
    if (PRESSED(SDL_SCANCODE_LEFT))
        options_adjust_value(-1);
    if (PRESSED(SDL_SCANCODE_RIGHT))
        options_adjust_value(+1);

    if (PRESSED(SDL_SCANCODE_RETURN) || PRESSED(SDL_SCANCODE_KP_ENTER))
    {
        if (s_row == 2)
        {
            options_apply(NULL);
        }
        else if (s_row == 3)
        {
            options_close();
        }
    }

#undef PRESSED
}

void options_render(SDL_Renderer *renderer, TTF_Font *font)
{
    if (!s_open)
        return;

#define OPTION_LABELS 4

    const char *labels[OPTION_LABELS];

    static char modeLine[64];
    static char resLine[64];

    snprintf(modeLine, sizeof(modeLine), "Mode: %s", s_labels_mode[gVideo.videoMode]);

    Resolution cr = gResList[gVideo.resolutionIndex];
    snprintf(resLine, sizeof(resLine), "Resolution: %dx%d", cr.w, cr.h);

    labels[0] = modeLine;
    labels[1] = resLine;
    labels[2] = "Apply";
    labels[3] = "Back";

    // Draw a panel + the menu list
    int panelW = 400, panelH = 240;
    int panelX = (SCREEN_WIDTH - panelW) / 2;
    int panelY = (SCREEN_HEIGHT - panelH) / 2;

    // render_panel(renderer, panelX, panelY, panelW, panelH);
    render_menu(renderer, font, "Options", labels, OPTION_LABELS, s_row, panelX + 25, panelY + 60);
}
