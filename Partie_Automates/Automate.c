#include "Automate.h"
#include <time.h>
#include <stdlib.h>

void transition(Automate *a, int lunchfood){
    int digestion = rand() % 3;

    a->fitness += (lunchfood - digestion -1);

    if(a->fitness >10) a->fitness = 10;

    if(a->fitness <= 0 || a->fitness >= 10){
        a->etat_suivant = 0;
    }
    else if((a->fitness >= 1 && a->fitness <= 3) ||
            (a->fitness >= 7 && a->fitness <= 9)){
        a->etat_suivant = 1;
    }
    else if(a->fitness >= 4 && a->fitness <= 6){
        a->etat_suivant = 2;
    }

    a->etat_courant = a->etat_suivant;
}
