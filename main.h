#include <stdio.h>

typedef struct {
    char nom[50];
    Combattant membres[3];
} Equipe;

typedef struct {
    int pvc; // points de vie courrant
    int pvm; // points de vie max
    int attaque;
    int defense;
    int agilite;
    int vitesse;

} Combattant;