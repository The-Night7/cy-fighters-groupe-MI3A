#ifndef GESTIONCOMBATTANT_H
#define GESTIONCOMBATTANT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_TECHNIQUES 3
#define MAX_TEAM_MEMBERS 3
#define MAX_EFFECTS 10

// Énumérations
typedef enum { JOUEUR, ORDI } TypeJoueur;
typedef enum {
    EFFET_AUCUN,
    EFFET_POISON,
    EFFET_ETOURDISSEMENT,
    EFFET_BOOST_ATTAQUE,
    EFFET_BOOST_DEFENSE,
    EFFET_BRULURE,
    EFFET_RECONSTITUTION,
    EFFET_BOUCLIER
} TypeEffet;

// Structures de base
typedef struct {
    TypeEffet type;
    int tours_restants;
    float puissance;
} EffetTemporaire;
typedef struct {
    char *nom;
    char *description;
    char *cible;
    int ncible;
    float puissance;
    int nb_tour_recharge;
    bool activable;
    int type;
    struct {
        bool possede;
        char *nom;
        int nb_tour_actifs;
    } Effet;
} Technique;

typedef struct {
    char *nom;
    struct {
        float courrante;
        float max;  
    } Vie;
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

typedef struct EtatCombattant {
    Combattant* combattant;
    float turn_meter;
    TypeJoueur controleur;
    int cooldowns[MAX_TECHNIQUES];
    EffetTemporaire effets[MAX_EFFECTS];
    int nb_effets;
} EtatCombattant;

typedef struct Combat {
    Equipe* equipe1;
    Equipe* equipe2;
    EtatCombattant* participants;
    int nombre_participants;
    int tour;
} Combat;

// Fonctions de base
Combattant *creer_combattant(const char *name);
void detruire_combattant(Combattant *Combattant);
void init_technique(Technique *tech, const char *name, const char *description, 
                   const char *cible, int ncible, float puissance, 
                   int nb_tour_recharge, bool possede, const char *nom_effet, 
                   int nb_tour_actifs, int type);

#endif