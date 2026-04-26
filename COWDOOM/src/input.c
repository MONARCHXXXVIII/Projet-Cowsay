#include "input.h"

#include <SDL2/SDL.h>
#include <string.h>

bool input_init(void) {
    return true;
}

void input_shutdown(void) {
    /* Nothing to do for SDL keyboard polling. */
}

void input_poll(InputState *state) {
    memset(state, 0, sizeof(*state));

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            state->quit = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_F11) {
            state->toggle_fullscreen = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_SPACE) {
            state->shoot = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_r) {
            state->reload = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_UP) {
            state->menu_up = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.sym == SDLK_DOWN) {
            state->menu_down = true;
        }
        if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
            (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)) {
            state->menu_select = true;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            state->shoot = true;
        }
        if (event.type == SDL_MOUSEMOTION) {
            state->mouse_delta_x += event.motion.xrel;
        }
    }

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    state->move_forward  = keys[SDL_SCANCODE_Z] || keys[SDL_SCANCODE_W];
    state->move_backward  = keys[SDL_SCANCODE_S];
    state->strafe_left    = keys[SDL_SCANCODE_Q] || keys[SDL_SCANCODE_A];
    state->strafe_right   = keys[SDL_SCANCODE_D];
    state->turn_left      = keys[SDL_SCANCODE_LEFT];
    state->turn_right     = keys[SDL_SCANCODE_RIGHT];

    if (keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_X]) {
        state->quit = true;
    }
}
