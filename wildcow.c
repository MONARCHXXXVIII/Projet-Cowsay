#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* ─── Dimensions du terminal (valeur par défaut) ─── */
#define TERM_WIDTH  80
#define TERM_HEIGHT 24
#define COW_WIDTH   22   /* largeur approximative du dessin vache */

/* ─── Utilitaires terminal ─── */
void clear_screen()            { printf("\033[H\033[J"); fflush(stdout); }
void goto_xy(int x, int y)     { printf("\033[%d;%dH", y, x); fflush(stdout); }
void hide_cursor()             { printf("\033[?25l"); fflush(stdout); }
void show_cursor()             { printf("\033[?25h"); fflush(stdout); }

/* ─── Dessin de la vache ─── */
/* direction : 1 = vers la droite (>), -1 = vers la gauche (<) */
/* eyes : chaîne de 2 caractères */
/* boom_fuse : -1 = pas de mèche, 0..N = longueur mèche */
void draw_cow(int x, int y, int direction, const char *eyes, int boom_fuse) {
    /* La vache regarde à droite par défaut, à gauche si direction == -1 */
    if (direction < 0) {
        /* Vache regardant à gauche */
        goto_xy(x, y);
        printf("        \\  ^__^");
        goto_xy(x, y + 1);
        printf("         \\ (%c%c)\\_______", eyes[0], eyes[1]);
        goto_xy(x, y + 2);
        printf("           (__)\\       )\\/\\");
        goto_xy(x, y + 3);
        printf("               ||----w |");
        goto_xy(x, y + 4);
        printf("               ||     ||");
    } else {
        /* Vache regardant à droite (miroir) — moonwalk / go_crazy */
        goto_xy(x, y);
        printf("           ^__^  /");
        goto_xy(x, y + 1);
        printf("   _______/(%c%c) /", eyes[0], eyes[1]);
        goto_xy(x, y + 2);
        printf("/\\/       /(__)");
        goto_xy(x, y + 3);
        printf("   | w----||");
        goto_xy(x, y + 4);
        printf("   ||     ||");
    }

    /* Mèche pour le mode boom */
    if (boom_fuse >= 0) {
        goto_xy(x + 17, y - 1);
        printf("o");
        for (int i = 0; i < boom_fuse; i++) printf("~");
        printf("*");
    }
    fflush(stdout);
}

/* ─── Explosion ─── */
void draw_explosion(int x, int y) {
    const char *frames[] = {
        "      ***      \n"
        "    *******    \n"
        "   *BOOM!!!!*  \n"
        "    *******    \n"
        "      ***      ",

        "   * * * * *   \n"
        "  *  BOOM!  *  \n"
        " *   !!!!   *  \n"
        "  *  ~~~~  *   \n"
        "   * * * * *   ",

        "  .  *  .  *   \n"
        "    . moo .    \n"
        "  * R.I.P. *   \n"
        "    . cow .    \n"
        "  .  *  .  *   ",
    };
    const char *colors[] = {"\033[1;33m", "\033[1;31m", "\033[0;37m"};

    for (int f = 0; f < 3; f++) {
        clear_screen();
        goto_xy(x - 5, y - 2);
        printf("%s%s\033[0m", colors[f], frames[f]);
        fflush(stdout);
        usleep(400000);
    }
}

/* ─── Décompte pour boom ─── */
void draw_countdown(int n, int x, int y) {
    for (int i = n; i > 0; i--) {
        clear_screen();
        draw_cow(x, y, 1, "oo", i - 1);
        goto_xy(x + 2, y - 2);
        printf("\033[1;31m EXPLOSION DANS %d... \033[0m", i);
        fflush(stdout);
        sleep(1);
    }
}

/* ════════════════════════════════════════════
   MODE PAR DÉFAUT : droite → gauche
   ════════════════════════════════════════════ */
void mode_default(char * pseudo) {
    hide_cursor();
    int y = TERM_HEIGHT / 2 - 2;

    for (int x = TERM_WIDTH - COW_WIDTH; x >= 1; x--) {
        clear_screen();
        draw_cow(x, y, -1, "oo", -1);
        goto_xy(1, TERM_HEIGHT);
        printf("Mode: -->  normal walk  <--  [Ctrl+C pour quitter]");
        usleep(80000);
    }

    /* La vache disparaît à gauche */
    clear_screen();
    goto_xy(1, TERM_HEIGHT / 2);
    printf("%s est rentrée chez elle. (ou elle s'est perdue ? On saura jamais)", pseudo);
    fflush(stdout);
    sleep(1);
    show_cursor();
}

/* ════════════════════════════════════════════
   MODE -hee_hee : MOONWALK
   Glisse de gauche à droite, mais la vache
   regarde toujours à gauche
   ════════════════════════════════════════════ */
void mode_moonwalk() {
    hide_cursor();
    int y = TERM_HEIGHT / 2 - 2;

    /* Intro */
    clear_screen();
    goto_xy(5, y - 2);
    printf("\033[1;35m ♪ Smooth Criminal ♪ \033[0m");
    fflush(stdout);
    sleep(1);

    /* Moonwalk : se déplace vers la droite mais la vache regarde à gauche */
    for (int x = 1; x <= TERM_WIDTH - COW_WIDTH; x++) {
        clear_screen();
        goto_xy(5, y - 2);
        printf("\033[1;35m ♪ hee hee ! ♪ \033[0m");
        /* On dessine la vache orientée gauche mais qui AVANCE vers droite */
        draw_cow(x, y, -1, "oo", -1);
        goto_xy(1, TERM_HEIGHT);
        printf("Mode: -hee_hee  [Moonwalk activated]");
        usleep(70000);
    }

    /* Final MJ pose */
    clear_screen();
    goto_xy(TERM_WIDTH / 2 - 5, y - 2);
    printf("\033[1;35m ♪ SHAMONE ♪ \033[0m");
    draw_cow(TERM_WIDTH / 2 - 4, y, -1, "**", -1);
    goto_xy(TERM_WIDTH / 2 - 4, y + 5);
    printf("\033[1;37m  ~ Michael Moo-Jackson ~ \033[0m");
    fflush(stdout);
    sleep(2);
    show_cursor();
}

/* ════════════════════════════════════════════
   MODE -boom : DÉCOMPTE + EXPLOSION
   ════════════════════════════════════════════ */
void mode_boom(char * pseudo) {
    hide_cursor();
    int y = TERM_HEIGHT / 2 - 2;
    int x = TERM_WIDTH / 2 - 5;

    /* La vache entre depuis la droite */
    for (int i = TERM_WIDTH - COW_WIDTH; i >= x; i--) {
        clear_screen();
        draw_cow(i, y, -1, "oo", -1);
        usleep(40000);
    }

    sleep(1);

    /* Décompte 5..1 avec mèche qui raccourcit */
    draw_countdown(5, x, y);

    /* Kaboom */
    draw_explosion(x + 5, y + 2);

    /* Message RIP */
    clear_screen();
    goto_xy(TERM_WIDTH / 2 - 8, TERM_HEIGHT / 2);
    printf("\033[1;31m  *** MOO IN PEACE ***  \033[0m");
    goto_xy(TERM_WIDTH / 2 - 8, TERM_HEIGHT / 2 + 1);
    printf("        (->ADMIN : %s has left the chat)", pseudo);
    fflush(stdout);
    sleep(2);
    show_cursor();
}

/* ════════════════════════════════════════════
   MODE -go_crazy : VA-ET-VIENT ALÉATOIRE
   Finit avec les yeux en xx (morte de vertige)
   ════════════════════════════════════════════ */
void mode_go_crazy(char * pseudo) {
    srand((unsigned)time(NULL));
    hide_cursor();
    int y    = TERM_HEIGHT / 2 - 2;
    int x    = TERM_WIDTH / 2;
    int dir  = 1;   /* 1 = droite, -1 = gauche */
    int speed = 100000; /* µs entre chaque frame */
    int steps = 0;
    int max_steps = 200;

    /* Phase 1 : va-et-vient de plus en plus rapide */
    while (steps < max_steps) {
        clear_screen();

        /* Accélération progressive */
        speed = 50000 - (steps * 200);
        if (speed < 5000) speed = 5000;

        /* Changement de direction aléatoire ou aux bords */
        if (x <= 1 || x >= TERM_WIDTH - COW_WIDTH) {
            dir = -dir;
        } else if (rand() % 15 == 0) {
            dir = -dir;
        }

        x += dir * 3;
        if (x < 1) x = 1;
        if (x > TERM_WIDTH - COW_WIDTH) x = TERM_WIDTH - COW_WIDTH;

        /* Yeux qui changent aléatoirement */
        const char *crazy_eyes[] = {"@@", "**", "OO", "^^", "??", "!!", "##", "oo"};
        int ei = rand() % 8;
        draw_cow(x, y, dir, crazy_eyes[ei], -1);

        goto_xy(1, TERM_HEIGHT);
        printf("\033[1;33m  GO CRAZY MODE — steps: %d/%d  \033[0m", steps, max_steps);
        fflush(stdout);
        usleep(speed);
        steps++;
    }

    /* Phase 2 : la vache s'arrête, yeux en xx, étourdie */
    clear_screen();
    int final_x = TERM_WIDTH / 2 - 4;
    draw_cow(final_x, y, 1, "xx", -1);
    goto_xy(final_x - 2, y - 2);
    printf("\033[1;36m  x_x  ~  trop de vertige...  ~  x_x \033[0m");
    goto_xy(final_x, y + 5);
    printf("\033[0;37m  *%s tourne en rond*  \033[0m", pseudo);
    fflush(stdout);
    sleep(2);
    show_cursor();
}

/* ─── main ─── */
int main(int argc, char *argv[]) {
    char * pseudo = "Cow";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-pseudo") == 0 || strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                pseudo = argv[i + 1];
            }
        }
    }
    if (argc == 0 || argc == 2 && strcmp(pseudo, "Cow") != 0 ) {
        mode_default(pseudo);
    } else if (strcmp(argv[1], "-hee_hee") == 0) {
        mode_moonwalk();
    } else if (strcmp(argv[1], "-boom") == 0) {
        mode_boom(pseudo);
    } else if (strcmp(argv[1], "-go_crazy") == 0) {
        mode_go_crazy(pseudo);
    } else {
        fprintf(stderr, "Usage: wildcow [-hee_hee | -boom | -go_crazy]\n");
        fprintf(stderr, "  (sans option) : marche de droite à gauche\n");
        fprintf(stderr, "  -hee_hee      : moonwalk façon MJ\n");
        fprintf(stderr, "  -boom         : décompte et explosion\n");
        fprintf(stderr, "  -go_crazy     : va-et-vient fou, finit avec yeux xx\n");
        return 1;
    }
    printf("\n");
    return 0;
}
