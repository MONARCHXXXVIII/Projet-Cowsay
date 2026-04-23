#pragma once

typedef struct{
    int etat_courant;
    int etat_suivant;
    unsigned int fitness;
} Automate;

void transition(Automate * a, int lunchfood);
