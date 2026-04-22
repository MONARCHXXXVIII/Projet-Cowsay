#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_MESSAGE "yo,riyad m'a cree"

/* ───────────────────────── Affichage de la bulle ───────────────────────── */

void affiche_bulle(const char *texte, int wrap, int think __attribute__((unused))) {
    int len = strlen(texte);
    int line_len = (wrap <= 0) ? len : wrap;

    /* Ligne supérieure */
    printf(" ");
    for (int i = 0; i < line_len + 2; i++) printf("_");
    printf("\n");

    /* Corps du texte avec retour à la ligne */
    int pos = 0;
    while (pos < len) {
        int end = pos + line_len;
        if (end > len) end = len;
        int is_first = (pos == 0);
        int is_last  = (end == len);

        if (len <= line_len) {
            /* Une seule ligne */
            printf("< %.*s >\n", len, texte);
        } else if (is_first) {
            printf("/ %-*.*s \\\n", line_len, line_len, texte + pos);
        } else if (is_last) {
            printf("\\ %-*.*s /\n", line_len, end - pos, texte + pos);
        } else {
            printf("| %-*.*s |\n", line_len, line_len, texte + pos);
        }
        pos = end;
        /* saute les espaces en début de ligne suivante */
        while (pos < len && texte[pos] == ' ') pos++;
    }

    /* Ligne inférieure */
    printf(" ");
    for (int i = 0; i < line_len + 2; i++) printf("-");
    printf("\n");
}

/* ───────────────────────── Affichage de la vache ───────────────────────── */

/*
 * eyes    : 2 caractères pour les yeux (défaut "oo")
 * tongue  : 2 caractères pour la langue (défaut "  ")
 * tail_ext: nombre de caractères supplémentaires pour la queue
 * think   : 0 = parole, 1 = pensée (flèche différente)
 */
void affiche_vache(const char *eyes, const char *tongue, int tail_ext, int think) {
    char e[3] = "oo";
    char t[3] = "  ";

    if (eyes   && strlen(eyes)   >= 2) { e[0] = eyes[0];   e[1] = eyes[1]; }
    if (tongue && strlen(tongue) >= 2) { t[0] = tongue[0]; t[1] = tongue[1]; }

    char arrow1 = think ? 'o' : '\\';
    char arrow2 = think ? 'o' : '\\';

    /* Queue de base + extension */
    printf("        %c  ^__%c \n", arrow1, '^');    printf("         %c (%c%c)\\_______\n", arrow2, e[0], e[1]);
    printf("           (__)\\       )"); 
    printf("\\/\\");
    for (int i = 0; i < tail_ext; i++) printf("/\\");
    printf("\n          ");
    printf("  %c%c ||----w |\n", t[0], t[1]);

    printf("               ||     ||\n");
}

/* ───────────────────────── Troupeau de vaches ───────────────────────── */

void affiche_troupeau(int n, const char *eyes, const char *tongue, int tail_ext) {
    for (int i = 0; i < n; i++) {
        printf("  Vache %d :\n", i + 1);
        affiche_vache(eyes, tongue, tail_ext, 0);
        printf("\n");
    }
}

/* ───────────────────────── Aide ───────────────────────── */

void aide() {
    printf("Usage: newcow [OPTIONS] \"message\"\n\n");
    printf("Options:\n");
    printf("  -e, --eyes   XX   Yeux de la vache (2 caractères, défaut: oo)\n");
    printf("  -T, --tongue XX   Langue de la vache (2 caractères, défaut: espaces)\n");
    printf("  -t, --tail   N    Allonge la queue de N caractères\n");
    printf("  -W, --wrap   N    Largeur de ligne du texte (défaut: 40)\n");
    printf("      --think       Bulle de pensée\n");
    printf("      --herd   N    Troupeau de N vaches\n");
    printf("  -h, --help        Affiche cette aide\n");
}

/* ───────────────────────── main ───────────────────────── */

int main(int argc, char *argv[]) {
    char eyes[3]   = "oo";
    char tongue[3] = "  ";
    int  tail_ext  = 0;
    int  wrap      = 40;
    int  think     = 0;
    int  herd      = 0;
    char *message  = NULL;

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eyes") == 0) && i + 1 < argc) {
            i++;
            if (strlen(argv[i]) >= 2) { eyes[0] = argv[i][0]; eyes[1] = argv[i][1]; }
            else { fprintf(stderr, "Erreur: --eyes attend 2 caractères\n"); return 1; }

        } else if ((strcmp(argv[i], "-T") == 0 || strcmp(argv[i], "--tongue") == 0) && i + 1 < argc) {
            i++;
            if (strlen(argv[i]) >= 2) { tongue[0] = argv[i][0]; tongue[1] = argv[i][1]; }
            else { fprintf(stderr, "Erreur: --tongue attend 2 caractères\n"); return 1; }

        } else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tail") == 0) && i + 1 < argc) {
            i++;
            tail_ext = atoi(argv[i]);
            if (tail_ext < 0) tail_ext = 0;

        } else if ((strcmp(argv[i], "-W") == 0 || strcmp(argv[i], "--wrap") == 0) && i + 1 < argc) {
            i++;
            wrap = atoi(argv[i]);
            if (wrap <= 0) wrap = 40;

        } else if (strcmp(argv[i], "--think") == 0) {
            think = 1;

        } else if (strcmp(argv[i], "--herd") == 0 && i + 1 < argc) {
            i++;
            herd = atoi(argv[i]);
            if (herd <= 0) herd = 1;

        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            aide();
            return 0;

        } else if (argv[i][0] != '-') {
            message = argv[i];

        } else {
            fprintf(stderr, "Option inconnue: %s\n", argv[i]);
            aide();
            return 1;
        }
    }

    if (message == NULL) {
        message = DEFAULT_MESSAGE;
    }

    if (herd > 0) {
        /* En mode troupeau, chaque vache répète le même message */
        affiche_bulle(message, wrap, think);
        affiche_troupeau(herd, eyes, tongue, tail_ext);
    } else {
        affiche_bulle(message, wrap, think);
        affiche_vache(eyes, tongue, tail_ext, think);
    }

    return 0;
}
