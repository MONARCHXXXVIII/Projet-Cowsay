#include "Automate.h"
#include <time.h>

int transition(Automate a, int lunchfood){
    srand( time( NULL ) );
    int digestion = rand() % 3; 
    a.fitness -= (lunchfood + digestion);
    if(a.fitness <= 0 || a.fitness >= 10){
        a.etat_suivant = 0;
    }
    else if(1<= a.fitness <=  3 || 7<= a.fitness <=  9){
        a.etat_suivant = 1;
    }
    else if(4<= a.fitness <=  6){ 
        a.etat_suivant = 2;
    }
    a.etat_courant = a.etat_suivant;
}
