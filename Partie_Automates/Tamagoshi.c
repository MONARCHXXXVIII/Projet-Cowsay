#include <stdio.h>
#include "Automate.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


char* lire_entree(const char* message){
    static char entree[100];  //sans static ici, la fonction provoque apparement toujours un BufferOverflow
    printf("%s", message);
    fgets(entree, sizeof(entree), stdin);
    entree[strcspn(entree, "\n")] = 0;
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



int minijeu(int adm){
    affiche_bulle("Le but du jeu est de deviner le chiffre entre 1 et 10 auquel je pense.\n Si tu y arrives, tu sera recompensé par deux unités de foin", 40, 0);
    affiche_vache("^^", "  ", 3, 0);
    printf("\n\n\n");
    int num = rand()%10 + 1;
    if(adm==1){
        printf("[MODE ADMIN] : elle pense a %d", num);
    }
    char* guess = lire_entree("\nAlors, quelle est votre prediction ?   ");
    if(strcmp(guess, "07072007") == 0){
        return 2;
    }
    if(atoi(guess)==num){
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

    char* qtt_nourriture = 0;
    int mode_admin = 0;


    if(argc > 1 && strcmp(argv[1], "--mode-admin") == 0){
    mode_admin = 1;
    }

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
           "          ████ █████  ██ ██ \n\n\n\n\n\n\n\n");

    sleep(1);
    if(mode_admin==1){
        printf("███████████████████████████████████████████████████████████████████████████\n");
        printf("███***********************MODE ADMIN ACTIF******************************███\n");
        printf("███████████████████████████████████████████████████████████████████████████\n");
    }
    sleep(3);
    printf("\n[MAITRE DU JEU] : Je suis le maitre du tamagoshi, et je vous guiderai tout au long du jeu\n");
    sleep(3);
    printf("[MAITRE DU JEU] :  Qui suis-je ? Des fois Arthur, des fois Riyad, les deux a la fois, ou aucun des deux ça dépend.\n");
    sleep(3);
    printf("[MAITRE DU JEU] : Votre but sera de vous occuper de la vache pour la faire vivre le plus longtemps possible.\n");
    sleep(3);
    printf("[MAITRE DU JEU] : Au fait ca va etre pénible de toujours l'appeler 'vache', pourriez vous lui donner un nom ?\n");
    sleep(3);
    char* nom_vache = malloc(100);
    strcpy(nom_vache, lire_entree("Quel nom donnerez vous a votre vache ? "));


    while(1){
        if(stock < 0){
            stock = 0;
        }
        tour+=1;
        printf("Tour %d\n", tour);
        printf("Stock actuel de foin : %d\n", stock);
        if(mode_admin==1){
            printf("[MODE ADMIN] : santé de la vache : %d\n", tamagoshi.fitness);
        }
        printf("[MAITRE DU JEU] : Il faut nourrir %s", nom_vache);
        qtt_nourriture = lire_entree(". Combien de nourriture lui donnez vous ?  \n");
        while(strcmp(qtt_nourriture, "1W4NTH4YB4L3")==0){
            stock+=2;
            printf("[MAITRE DU JEU] :  Code de triche entré avec succès, +2 foin");
            qtt_nourriture = lire_entree(". Combien de nourriture lui donnez vous ?  \n");
        }
        if(strcmp(qtt_nourriture, "28082007")==0){
            printf("\033[H\033[J\n\n\n\n\n");
            char* sortie = lire_entree("[MAITRE DU JEU] : AVERTISSEMENT, les dialogues qui vont suivre sont assez trash, souhaitez-vous continuer (o/n)");
            if(strcmp(sortie, "o")==0){
                printf("[Riyad] : J'viens d'avoir une idée, au lieu de s'embeter a nourir cette vache, si on en faisait un barbecue ??\n");
                sleep(5);
                printf("[Vous] : Bonne idée, je commencais a avoir faim, c'est d'ailleurs pour ca que j'ai tapé ce code de triche\n");
                sleep(5);
                printf("[Artur] : Bon bah let's go, riyad allume le barbecue pendant qu'on s'occupe de la vache\n");
                sleep(5);
                printf("[MAITRE DU JEU] : Et vous trois finirez rassasié au terme d'un repas de 4heures dans lequel vous vous etes délécté de la vache\n");
                sleep(5);
                printf("[MAITRE DU JEU] : Vous avez débloqué la fin spéciale '' Bifteack haché de Tamagoshi '' \n");
                free(nom_vache);
                return 0;

            }
        }
        while(atoi(qtt_nourriture) > stock){
            qtt_nourriture = lire_entree("[MAITRE DU JEU] :  Vous n'avez pas assez de foin en stock, entrez autre chose, ou 0 si vous n'avez rien\n");
        }
        transition(&tamagoshi, atoi(qtt_nourriture));
        crop = (rand()%6 + 1) - 3;
        stock += crop - atoi(qtt_nourriture);
        switch(tamagoshi.etat_courant){
        case 0:
            if(tamagoshi.fitness >=10){
                printf("[MAITRE DU JEU] :   GAME OVER : %s est décédée de suralimentation \n[MAITRE DU JEU] :   indice pour la prochaine fois : surveillez l'etat de santé de votre vache, et essayez d'optimiser vos reserves de foin\n[MAITRE DU JEU] : Vous avez débloqué la fin '' American Simulator ''\n", nom_vache);
                affiche_bulle(" ByeByeLife :( ", 40, 0);
                affiche_vache("XX","U ", 3, 0);
            }
            else{
                printf("[MAITRE DU JEU] :   GAME OVER : %s est morte de faim \n[MAITRE DU JEU] :   indice pour la prochaine fois : surveillez l'etat de santé de votre vache, et essayez d'optimiser vos reserves de foin\n[MAITRE DU JEU] : Vous avez débloqué la fin '' C'est la crise !! ''\n", nom_vache);
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
            jouer_minjeu = atoi(lire_entree(" ??  (1 pour oui, 0 pour non)   "));
            if(jouer_minjeu >= 1){
                int sortie = minijeu(mode_admin);
                if(sortie==1){
                    affiche_bulle(" Bienjoué, tu as trouvé, comme promis voici ta récompense", 40, 0);
                    affiche_vache("^^", "  ", 3, 0);
                    printf("\n[MAITRE DU JEU] : %s vous a récompensé pour votre victoire, +2 foin\n", nom_vache);
                    stock += 2;
                }
                else if(sortie==2){
                    printf("\033[H\033[J");
                    affiche_bulle(" QU'A TU FAIS ?????? TU AS TUE LES CREATEURS DU JEU ", 40, 0);
                    affiche_vache("OO", "  ", 3, 0);
                    sleep(5);
                    printf("\033[H\033[J");
                    affiche_bulle(" A CAUSE DE TOI RIYAD ET ARTHUR SONT.....MORTS ", 40, 0);
                    affiche_vache("OO", "  ", 3, 0);
                    sleep(5);
                    printf("\033[H\033[J");
                    affiche_bulle(" CELA VEUT DIRE QUE.......MOI AUSSI JE VAIS MOURIR PAR TA FAUTE ", 40, 0);
                    affiche_vache("OO", "  ", 3, 0);
                    sleep(5);
                    printf("\033[H\033[J");
                    affiche_bulle(" ByeByeLife :( ", 40, 0);
                    affiche_vache("XX", "  ", 3, 0);
                    sleep(5);
                    printf("\033[H\033[J");
                    printf("\n[MAITRE DU JEU] : *mort aussi* \n");
                    sleep(5);
                    printf("\033[H\033[J");
                    printf("        *Par votre faute, toute personne concernée par ce jeu a part vous sont morts.*\n");
                    sleep(5);
                    printf("\033[H\033[J");
                    printf("        *Le code que vous avez tapé dans le défi de la vache etait une combinaison maudite, qui a pour effet de detruire le monde dans lequel elle est tapée* \n");
                    sleep(5);
                    printf("\033[H\033[J");
                    printf("        *...*");
                    sleep(10);
                    printf("\033[H\033[J");
                    printf("        Vous avez débloqué la fin spéciale '' Apocalypse '' \n");
                    free(nom_vache);  //pour eviter une fuite memoire si on arrete le programme ici
                    return 0;
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
            free(nom_vache);
            return 1;
        }
    }
    free(nom_vache);
    return 0;
}
