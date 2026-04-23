#include <stdio.h>
#include "Automate.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int lire_entree(const char* message){
    int entree;
    printf("%s", message);
    scanf("%d", &entree);
    return entree;
}

char* lire_chaine(const char* message){
    static char entree[100];  //sans static ici, la fonction provoque apparement toujours un BufferOverflow
    printf("%s", message);
    scanf("%99s", entree);
    return entree;
}



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



int minijeu(){
    affiche_bulle("Le but du jeu est de deviner le chiffre entre 1 et 10 auquel je pense.\n Si tu y arrives, tu sera recompensé par deux unités de foin", 40, 0);
    affiche_vache("^^", "  ", 3, 0);
    printf("\n\n\n");
    int guess = lire_entree("\nAlors, quelle est votre prediction ?   ");
    int num = rand()%10 + 1;
    if(guess==num){
        return 1;
    }
    else{
        return 0;
    }
}


int main(int argc, char * argv[]){
    system("chcp 65001");
    Automate tamagoshi;
    tamagoshi.etat_courant = 2;
    tamagoshi.etat_suivant = 2;
    tamagoshi.fitness = 5;
    int en;
    int stock = 10;
    int crop;
    int jouer_minjeu = 0;
    int tour = 0;

    int qtt_nourriture = 0;

    srand(time(NULL));
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
           "█████  ██  █   █  ██  █████  ███  ████ █   █ █████\n"
           "  █   █  █ ██ ██ █  █ █     █   █ █    █   █   █  \n"
           "  █   ████ █ █ █ ████ █ ███ █   █ ████ █████   █  \n"
           "  █   █  █ █   █ █  █ █   █ █   █    █ █   █   █  \n"
           "  █   █  █ █   █ █  █ █████  ███  ████ █   █ █████\n"
           "\n"
           "          ████ █████ █     █\n"
           "          █    █   █ █     █\n"
           "          █    █   █ █  █  █\n"
           "          ████ █████  ██ ██ \n\n\n\n\n\n\n\n\n\n\n");

    sleep(1);
    printf("\n[MAITRE DU JEU] : Je suis le maitre du tamagoshi, et je vous guiderai tout au long du jeu\n[MAITRE DU JEU] :  Qui suis-je ? Des fois Arthur, des fois Riyad, les deux a la fois, ou aucun des deux ça dépend.\n [MAITRE DU JEU] : Votre but sera de vous occuper de la vache pour la faire vivre le plus longtemps possible.\n");
    sleep(1);
    printf("[MAITRE DU JEU] : Au fait ca va etre pénible de toujours l'appeler 'vache', pourriez vous lui donner un nom ?\n");
    sleep(1);
    const char* nom_vache = lire_chaine("Quel nom donnerez vous a votre vache ? ");


    while(1){
        if(stock < 0){
            stock = 0;
        }
        tour+=1;
        printf("Tour %d\n", tour);
        printf("Stock actuel de foin : %d\n", stock);
        printf("[MAITRE DU JEU] : Il faut nourrir %s", nom_vache);
        qtt_nourriture = lire_entree(". Combien de nourriture lui donnez vous ?  \n");
        while(qtt_nourriture > stock){
            qtt_nourriture = lire_entree("[MAITRE DU JEU] :  Vous n'avez pas assez de foin en stock, entrez autre chose, ou 0 si vous n'avez rien\n");
        }
        transition(&tamagoshi, qtt_nourriture);
        crop = (rand()%6 + 1) - 3;
        stock += crop - qtt_nourriture;
        //printf("%d", tamagoshi.fitness);
        switch(tamagoshi.etat_courant){
        case 0:
            if(tamagoshi.fitness >=10){
                printf("[MAITRE DU JEU] :   GAME OVER : %s est décédée de suralimentation \n indice pour la prochaine fois : surveillez l'etat de santé de votre vache, et essayez d'optimiser vos reserves de foin\n [MAITRE DU JEU] : Vous avez débloqué la fin '' American Simulator ''\n", nom_vache);
                affiche_bulle(" ByeByeLife :( ", 40, 0);
                affiche_vache("XX","U ", 3, 0);
            }
            else{
                printf("[MAITRE DU JEU] :   GAME OVER : %s est morte de faim \n indice pour la prochaine fois : surveillez l'etat de santé de votre vache, et essayez d'optimiser vos reserves de foin\n [MAITRE DU JEU] : Vous avez débloqué la fin '' C'est la crise !! ''\n", nom_vache);
                affiche_bulle(" ByeByeLife :( ", 40, 0);
                affiche_vache("XX","U ", 3, 0);
            }
            return 0;
        case 1:
            if(tamagoshi.fitness >= 7){
                affiche_bulle(" LifeSucks :|, arrete de me nourrir \n", 40, 0);
                affiche_vache("@@","  ", 3, 0);
            }
            else{
                affiche_bulle(" LifeSucks :|, j'ai faim \n", 40, 0);
                affiche_vache("--"," U", 3, 0);
            }
            break;
        case 2:
            affiche_bulle(" LifeRoooooooooooooooooooooooocks :), on joue a un jeu ?", 40, 0);
            affiche_vache("^^", "  ", 3, 0);
            printf("\n\n\n");
            printf("[MAITRE DU JEU] : Acceptez vous le defi de %s", nom_vache);
            jouer_minjeu = lire_entree(" ??  (1 pour oui, 0 pour non)   ");
            if(jouer_minjeu >= 1){
                if(minijeu()==1){
                    affiche_bulle(" Bienjoué, tu as trouvé, comme promis voici ta récompense", 40, 0);
                    affiche_vache("^^", "  ", 3, 0);
                    printf("\n[MAITRE DU JEU] : %s vous a récompensé pour votre victoire, +2 foin\n", nom_vache);
                    stock += 2;
                }
                else{
                    affiche_bulle(" Raté, je pensais a autre chose, tu réessaiera la prochaine fois", 40, 0);
                    affiche_vache("^^", "  ", 3, 0);
                    printf("\n[MAITRE DU JEU] : vous n'avez pas relevé le défi, +0 foin\n");
                }
            }
            break;
        default:
            fprintf(stderr, "Erreur : on dirait que quelque chose s'est mal passé lors du calcul de l'etat suivant de la vache\n");
            return 1;
        }
    }
    return 0;
}
