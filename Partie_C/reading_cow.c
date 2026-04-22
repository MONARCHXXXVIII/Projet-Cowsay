#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TEXT   256
#define BUBBLE_W   40

/* ─── Utilitaires terminal ─── */
void clear_screen() { printf("\033[H\033[J"); fflush(stdout); }
void hide_cursor()  { printf("\033[?25l");    fflush(stdout); }
void show_cursor()  { printf("\033[?25h");    fflush(stdout); }

/* ─── Affiche la bulle avec le texte avalé + caractère en cours dans la gueule ─── */
/*
 * swallowed : texte déjà avalé
 * current   : caractère en cours de lecture ('\0' si aucun)
 */
void affiche_reading_cow(const char *swallowed, char current) {
    int len     = strlen(swallowed);
    int bub_len = len < BUBBLE_W ? len : BUBBLE_W;
    if (bub_len < 5) bub_len = 5;   /* bulle minimale */

    /* ── Bulle de texte (texte avalé) ── */
    printf(" ");
    for (int i = 0; i < bub_len + 2; i++) printf("_");
    printf("\n");

    if (len == 0) {
        printf("< %-*s >\n", bub_len, "");
    } else {
        /* N'affiche que les derniers bub_len caractères si trop long */
        const char *display = (len > bub_len) ? swallowed + len - bub_len : swallowed;
        printf("< %-*s >\n", bub_len, display);
    }

    printf(" ");
    for (int i = 0; i < bub_len + 2; i++) printf("-");
    printf("\n");

    /* ── Corps de la vache ── */
    printf("        \\  ^__^\n");

    /* Yeux normaux, sauf si on vient de finir (current == EOF marqueur) */
    if (current == '\0') {
        /* Lecture terminée : yeux satisfaits */
        printf("         \\ (^^)\\_______\n");
    } else {
        printf("         \\ (oo)\\_______\n");
    }

    /* La gueule : affiche le caractère courant */
    if (current != '\0' && current != '\n') {
        printf("           (__)\\       )\\/\\\n");
        printf("            %c  ||----w |\n", current);
    } else {
        printf("           (__)\\       )\\/\\\n");
        printf("               ||----w |\n");
    }
    printf("               ||     ||\n");

    fflush(stdout);
}

int main(int argc, char *argv[]) {
    FILE *fp;

    if (argc >= 2) {
        fp = fopen(argv[1], "r");
        if (!fp) {
            fprintf(stderr, "Erreur : impossible d'ouvrir '%s'\n", argv[1]);
            return 1;
        }
    } else {
        /* Lecture sur stdin */
        fp = stdin;
        fprintf(stderr, "Lecture sur stdin (Ctrl+D pour terminer)...\n");
        sleep(1);
    }

    char swallowed[MAX_TEXT + 1];
    memset(swallowed, 0, sizeof(swallowed));
    int pos = 0;

    hide_cursor();

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        /* On ignore les retours chariot Windows */
        if (ch == '\r') continue;

        clear_screen();

        /* Affiche la vache avec le caractère courant dans la gueule */
        affiche_reading_cow(swallowed, (char)ch);

        printf("\n Caractères lus : %d\n", pos + 1);
        fflush(stdout);

        sleep(1);

        /* Avale le caractère */
        if (pos < MAX_TEXT) {
            swallowed[pos++] = (char)ch;
            swallowed[pos]   = '\0';
        }
    }

    /* Affichage final : tout avalé */
    clear_screen();
    affiche_reading_cow(swallowed, '\0');
    printf("\n La vache a tout lu ! (%d caractères avalés)\n", pos);
    fflush(stdout);
    sleep(2);

    show_cursor();

    if (fp != stdin) fclose(fp);
    return 0;
}
