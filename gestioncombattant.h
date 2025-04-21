#ifndef GESTIONCOMBATTANT_H
#define GESTIONCOMBATTANT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_TECHNIQUES 3
#define MAX_TEAM_MEMBERS 3

typedef struct {
    char *nom;
    char *description;
    char *cible;
    float puissance;
    int nb_tour_recharge;
    bool activable;
    struct{
        bool possede;
        char *nom;
        int nb_tour_actifs;
    }Effet;
} Technique;

typedef struct {
    char *nom;
    struct{
        float courrante;
        float max;  
    }Vie;
    float attaque;
    float defense;
    float agility;
    float speed;
    Technique techniques[MAX_TECHNIQUES];
} Combattant;

typedef struct {
    char name[50];
    Combattant members[MAX_TEAM_MEMBERS];
    int member_count;
} Equipe;


Combattant *creer_combattant(const char *name);
void detruire_combattant(Combattant *Combattant);

void init_technique(Technique *tech, const char *name, const char *description, const char *cible,
    float puissance, int nb_tour_recharge, bool possede, const char *nom_effet, int nb_tour_actifs);

#endif