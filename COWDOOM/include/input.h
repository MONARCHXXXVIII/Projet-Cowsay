#ifndef COWDOOM_INPUT_H
#define COWDOOM_INPUT_H

#include <stdbool.h>

typedef struct {
    bool move_forward;
    bool move_backward;
    bool strafe_left;
    bool strafe_right;
    bool toggle_fullscreen;
    bool turn_left;
    bool turn_right;
    bool shoot;
    bool reload;
    bool menu_up;
    bool menu_down;
    bool menu_select;
    bool quit;
    int mouse_delta_x;
} InputState;

bool input_init(void);
void input_shutdown(void);
void input_poll(InputState *state);

#endif
