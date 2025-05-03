#ifndef COMBAT_H
#define COMBAT_H

#include "gestioncombattant.h"

#define MAX_EFFECTS 10 // Nombre maximum d'effets temporaires par combattant

// Enumération pour différencier le type de contrôleur d'un combattant
typedef enum { JOUEUR, ORDI } TypeJoueur;

typedef enum {
    EFFET_AUCUN,
    EFFET_POISON,
    EFFET_ETOURDISSEMENT,
    EFFET_BOOST_ATTAQUE,
    EFFET_BOOST_DEFENSE,
    EFFET_BRULURE
} TypeEffet;

typedef struct {
    TypeEffet type;
    int tours_restants;
    float puissance; // Pour les effets avec intensité variable
} EffetTemporaire;

// Structure pour gérer l'état d'un combattant pendant le combat
typedef struct {
    Combattant* combattant;     // Pointeur vers le combattant réel
    float turn_meter;           // Jauge de tour : quand >=100, le combattant peut agir
    int cooldowns[MAX_TECHNIQUES]; // Cooldowns des techniques spéciales
    TypeJoueur controleur;      // Indique si le combattant est contrôlé par un joueur ou l'ordinateur
    EffetTemporaire effets[MAX_EFFECTS]; // Tableau d'effets actifs
    int nb_effets;
} EtatCombattant;

// Structure principale du combat, gère les deux équipes et les états des participants
typedef struct {
    Equipe* equipe1;                // Pointeur vers la première équipe
    Equipe* equipe2;                // Pointeur vers la seconde équipe
    EtatCombattant* participants;   // Tableau des états des combattants
    int nombre_participants;        // Nombre total de combattants en combat
    int tour;                       // Compteur de tours
} Combat;
// Fonctions de base pour le cycle de vie du combat
void initialiser_combat(Combat* combat, Equipe* eq1, Equipe* eq2); // Initialise le combat
void gerer_tour_combat(Combat* combat);                            // Gère un tour complet de combat
void nettoyer_combat(Combat* combat);                              // Libère la mémoire du combat

// Fonctions d'action
void attaque_base(EtatCombattant* attaquant, EtatCombattant* cible); // Effectue une attaque de base
void utiliser_technique(EtatCombattant* attaquant, int tech_index, EtatCombattant* cible); // Utilise une technique spéciale

// Fonctions utilitaires
bool est_ko(Combattant* c);                  // Vérifie si un combattant est K.O.
void appliquer_effets(Combat* combat);       // Applique les effets temporaires en cours
bool verifier_victoire(Combat* combat);      // Vérifie si une équipe a gagné
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance);
void retirer_effet(EtatCombattant* cs, TypeEffet type);
void afficher_combat(const Combat* combat);
void afficher_statuts_combat(Combat* combat); // Modifié pour correspondre à l'implémentation
void afficher_menu_actions(EtatCombattant* joueur); // Ajout de la déclaration manquante
int choisir_cible(Combat* combat, TypeJoueur controleur); // Ajout de la déclaration manquante

#endif
