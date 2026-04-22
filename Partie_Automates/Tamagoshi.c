#include <stdio.h>
#include "Automate.h"

int lire_entree(const char* message){
    int entree;
    printf("%s", message);
    scanf("%d", &entree);
    return entree;
}


int main(int argc, char * argv[]){
    int en = lire_entree("entrez un nombre :  ");
    printf("Vous avez tape %d", en);
    return 0;
}
