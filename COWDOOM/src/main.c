#include "game.h"
#include "input.h"
#include "video.h"

#include <stdio.h>
#include <time.h>

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

int main(void) {
    const int cols = 200;
    const int rows = 62;
    const int char_w = 6;
    const int char_h = 11;

    Game game;
    if (!video_init(cols, rows, char_w, char_h, "COWDOOM ASCII")) {
        fprintf(stderr, "Impossible d'initialiser SDL2 (fenetre/rendu).\n");
        return 1;
    }

    game_init(&game, cols, rows);

    if (!input_init()) {
        fprintf(stderr, "Impossible d'initialiser l'entree clavier.\n");
        video_shutdown();
        return 1;
    }

    double previous = now_seconds();
    while (game.running) {
        double current = now_seconds();
        double dt = current - previous;
        previous = current;

        if (dt > 0.1) {
            dt = 0.1;
        }

        game_tick(&game, dt);
        game_render(&game);

        struct timespec sleep_time = {.tv_sec = 0, .tv_nsec = 16000000};
        nanosleep(&sleep_time, NULL);
    }

    input_shutdown();
    game_shutdown();
    video_shutdown();
    return 0;
}
