#ifndef GEST_EFFETS_H
#define GEST_EFFETS_H
#include "gestioncombattant.h"

#include "gestioncombattant.h"
#include <stdbool.h>

#define MAX_EFFECTS 10 // Nombre maximum d'effets temporaires par combattant

// Enumération pour différencier le type de contrôleur d'un combattant
typedef enum { JOUEUR, ORDI } TypeJoueur;

typedef enum {
    EFFET_AUCUN,
    EFFET_POISON,
    EFFET_ETOURDISSEMENT, //tour perdu
    EFFET_BOOST_ATTAQUE, //+20% de dégats à chaque tour
    EFFET_BOOST_DEFENSE,
    EFFET_BRULURE, //-20% de vie à chaque tour
    EFFET_RECONSTITUTION, //+20% de vie à chaque tour
    EFFET_BOUCLIER //30% de bouclier
} TypeEffet;

typedef struct {
    TypeEffet type;
    int tours_restants;
    float puissance; // Pour les effets avec intensité variable
} EffetTemporaire;

// Fonctions de gestion des effets temporaires
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance);
void retirer_effet(EtatCombattant* cs, TypeEffet type);
void appliquer_effets(Combat* combat);       // Applique les effets temporaires en cours

#endif
