#include <stdio.h>

typedef struct {
    char nom[50];
    Combattant membres[3];
} Equipe;

typedef struct {
    float pvc; // points de vie courrant
    float pvm; // points de vie max
    float attaque;
    float defense;
    float agilite;
    float vitesse;
    Techniques techniques[3];
} Combattant;

typedef struct {
    char nom[50];
    int valeur; // valeur appliquée à une propriété
    char description[250];
    int nb_tour_actifs;
    int nb_tour_recharge;
} Techniques;
