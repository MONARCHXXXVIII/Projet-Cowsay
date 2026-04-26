#include "video.h"

#include <SDL2/SDL.h>
#include <ctype.h>
#include <stddef.h>

typedef struct {
    char c;
    unsigned char rows[7];
} Glyph;

static SDL_Window *s_window = NULL;
static SDL_Renderer *s_renderer = NULL;
static bool s_fullscreen = false;
static int s_base_cols = 0;
static int s_base_rows = 0;

static void apply_mouse_lock(bool locked) {
    /* Sous WSL, SDL_SetRelativeMouseMode et SDL_WarpMouseInWindow ne sont pas fiables.
       On se contente de cacher le curseur en fullscreen. */
    if (!s_window) {
        return;
    }
    SDL_ShowCursor(locked ? SDL_DISABLE : SDL_ENABLE);
}

int video_consume_mouse_dx(void) {
    if (!s_window) {
        return 0;
    }

    int dx = 0;
    int dy = 0;
    /* SDL_GetRelativeMouseState retourne le delta depuis le dernier appel.
       SDL_SetRelativeMouseMode confine deja la souris, pas besoin de warp. */
    SDL_GetRelativeMouseState(&dx, &dy);

    if (dx != 0 || dy != 0) {
        (void)dy; /* on n'utilise pas l'axe vertical pour l'instant */
    }

    return dx;
}


static const Glyph s_glyphs[] = {
    {' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    {'#', {0x0A, 0x1F, 0x0A, 0x0A, 0x1F, 0x0A, 0x00}},
    {'@', {0x0E, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0E}},
    {'=', {0x00, 0x1F, 0x00, 0x1F, 0x00, 0x00, 0x00}},
    {'-', {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}},
    {'.', {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C}},
    {',', {0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x08}},
    {':', {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00}},
    {'/', {0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00}},
    {'>', {0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10}},
    {'+', {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}},
    {'*', {0x11, 0x04, 0x1F, 0x04, 0x11, 0x00, 0x00}},
    {'!', {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04}},
    {'0', {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}},
    {'1', {0x04, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1F}},
    {'2', {0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F}},
    {'3', {0x1F, 0x01, 0x02, 0x06, 0x01, 0x11, 0x0E}},
    {'4', {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}},
    {'5', {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}},
    {'6', {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}},
    {'7', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}},
    {'8', {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}},
    {'9', {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}},
    {'A', {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
    {'B', {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}},
    {'C', {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}},
    {'D', {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C}},
    {'E', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}},
    {'F', {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}},
    {'G', {0x0E, 0x11, 0x10, 0x13, 0x11, 0x11, 0x0E}},
    {'H', {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}},
    {'I', {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x1F}},
    {'J', {0x1F, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0C}},
    {'K', {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}},
    {'L', {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}},
    {'M', {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}},
    {'N', {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11}},
    {'O', {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
    {'P', {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}},
    {'Q', {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}},
    {'R', {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}},
    {'S', {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E}},
    {'T', {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}},
    {'U', {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}},
    {'V', {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}},
    {'W', {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A}},
    {'X', {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}},
    {'Y', {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}},
    {'Z', {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}},
};

static const unsigned char *glyph_for(char c) {
    char u = (char)toupper((unsigned char)c);
    for (size_t i = 0; i < sizeof(s_glyphs) / sizeof(s_glyphs[0]); ++i) {
        if (s_glyphs[i].c == u) {
            return s_glyphs[i].rows;
        }
    }
    return s_glyphs[0].rows;
}

static SDL_Color color_for_char(char c) {
    switch (c) {
        case '@': return (SDL_Color){255, 246, 214, 255};
        case '#': return (SDL_Color){244, 223, 170, 255};
        case 'H': return (SDL_Color){220, 188, 133, 255};
        case 'X': return (SDL_Color){188, 148, 102, 255};
        case '=': return (SDL_Color){154, 116, 78, 255};
        case '-': return (SDL_Color){116, 85, 58, 255};
        case ':': return (SDL_Color){82, 126, 138, 255};
        case ',': return (SDL_Color){68, 94, 84, 255};
        case '.': return (SDL_Color){52, 70, 62, 255};
        case '/': return (SDL_Color){88, 122, 150, 255};
        case 'E': return (SDL_Color){255, 124, 124, 255};
        case 'K': return (SDL_Color){255, 84, 84, 255};
        case 'C': return (SDL_Color){90, 228, 255, 255};
        case 'B': return (SDL_Color){70, 42, 30, 255};
        case '*': return (SDL_Color){255, 240, 40, 255};
        case '!': return (SDL_Color){255, 60, 60, 255};
        case '+': return (SDL_Color){230, 230, 230, 255};
        case '>': return (SDL_Color){250, 214, 70, 255};
        case 'W':
        case 'O': return (SDL_Color){255, 150, 120, 255};
        case 'M':
        case 'u':
        case 'h': return (SDL_Color){255, 210, 130, 255};
        default: return (SDL_Color){214, 226, 224, 255};
    }
}

bool video_init(int cols, int rows, int char_width, int char_height, const char *title) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return false;
    }

    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "0");

    s_base_cols = cols;
    s_base_rows = rows;

    int win_w = cols * char_width;
    int win_h = rows * char_height;

    s_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!s_window) {
        SDL_Quit();
        return false;
    }

    s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!s_renderer) {
        SDL_DestroyWindow(s_window);
        s_window = NULL;
        SDL_Quit();
        return false;
    }

    SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_NONE);
    return true;
}

void video_shutdown(void) {
    apply_mouse_lock(false);

    if (s_renderer) {
        SDL_DestroyRenderer(s_renderer);
        s_renderer = NULL;
    }
    if (s_window) {
        SDL_DestroyWindow(s_window);
        s_window = NULL;
    }
    SDL_Quit();
}

void video_toggle_fullscreen(void) {
    if (!s_window) {
        return;
    }

    s_fullscreen = !s_fullscreen;
    Uint32 mode = s_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    SDL_SetWindowFullscreen(s_window, mode);
    apply_mouse_lock(s_fullscreen);
}

void video_clear(void) {
    SDL_SetRenderDrawColor(s_renderer, 8, 12, 18, 255);
    SDL_RenderClear(s_renderer);
}

static void draw_char(char c, int cx, int cy, int cell_w, int cell_h) {
    if (c == ' ') {
        return;
    }

    SDL_Color color = color_for_char(c);
    SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);

    int margin_x = cell_w / 7;
    int margin_y = cell_h / 9;
    int draw_w = cell_w - margin_x * 2;
    int draw_h = cell_h - margin_y * 2;
    if (draw_w < 5 || draw_h < 7) {
        return;
    }

    int px_w = draw_w / 5;
    int px_h = draw_h / 7;
    if (px_w < 1) px_w = 1;
    if (px_h < 1) px_h = 1;

    int glyph_w = px_w * 5;
    int glyph_h = px_h * 7;
    int ox = cx + (cell_w - glyph_w) / 2;
    int oy = cy + (cell_h - glyph_h) / 2;

    const unsigned char *rows = glyph_for(c);
    for (int y = 0; y < 7; ++y) {
        for (int x = 0; x < 5; ++x) {
            if ((rows[y] >> (4 - x)) & 1U) {
                SDL_Rect px = {ox + x * px_w, oy + y * px_h, px_w, px_h};
                SDL_RenderFillRect(s_renderer, &px);
            }
        }
    }
}

void video_draw_ascii(const char *buffer, int cols, int rows) {
    if (!buffer || !s_renderer || cols <= 0 || rows <= 0) {
        return;
    }

    int win_w = 0;
    int win_h = 0;
    SDL_GetRendererOutputSize(s_renderer, &win_w, &win_h);

    int cell_w = win_w / cols;
    int cell_h = win_h / rows;
    int used_w = cell_w * cols;
    int used_h = cell_h * rows;
    int offset_x = (win_w - used_w) / 2;
    int offset_y = (win_h - used_h) / 2;

    if (cell_w < 4 || cell_h < 6) {
        return;
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            char c = buffer[y * cols + x];
            draw_char(c, offset_x + x * cell_w, offset_y + y * cell_h, cell_w, cell_h);
        }
    }

    (void)s_base_cols;
    (void)s_base_rows;
}

void video_draw_border_flash(float alpha) {
    if (!s_renderer || alpha <= 0.0f) {
        return;
    }
    if (alpha > 1.0f) alpha = 1.0f;

    int win_w = 0;
    int win_h = 0;
    SDL_GetRendererOutputSize(s_renderer, &win_w, &win_h);

    Uint8 a = (Uint8)(alpha * 180.0f);
    SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(s_renderer, 220, 30, 30, a);

    int thickness = win_h / 12;
    if (thickness < 4) thickness = 4;

    SDL_Rect rects[4] = {
        {0, 0, win_w, thickness},
        {0, win_h - thickness, win_w, thickness},
        {0, 0, thickness, win_h},
        {win_w - thickness, 0, thickness, win_h},
    };
    SDL_RenderFillRects(s_renderer, rects, 4);
    SDL_SetRenderDrawBlendMode(s_renderer, SDL_BLENDMODE_NONE);
}

void video_present(void) {
    SDL_RenderPresent(s_renderer);
}
