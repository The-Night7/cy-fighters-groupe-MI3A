#include <stdio.h>

typedef struct {
    char nom[50];
    int valeur;
    char description[250];
    int nb_tour_actifs;
    int nb_tour_recharge;
} Techniques;

typedef struct {
    float pvc;
    float pvm;
    float attaque;
    float defense;
    float agilite;
    float vitesse;
    Techniques techniques[3];
} Combattant;

typedef struct {
    char nom[50];
    Combattant membres[3];
} Equipe;