#ifndef COMBAT_H
#define COMBAT_H

#include "gestioncombattant.h"

#define MAX_EFFECTS 10 // Nombre maximum d'effets temporaires par combattant

// Enumération pour différencier le type de contrôleur d'un combattant
typedef enum { JOUEUR, ORDI } TypeJoueur;

// Structure pour gérer l'état d'un combattant pendant le combat
typedef struct {
    Combattant* combattant;     // Pointeur vers le combattant réel
    float turn_meter;           // Jauge de tour : quand >=100, le combattant peut agir
    int cooldowns[MAX_TECHNIQUES]; // Cooldowns des techniques spéciales
    TypeJoueur controleur;      // Indique si le combattant est contrôlé par un joueur ou l'ordinateur
} CombatantState;

// Structure principale du combat, gère les deux équipes et les états des participants
typedef struct {
    Equipe* equipe1;                // Pointeur vers la première équipe
    Equipe* equipe2;                // Pointeur vers la seconde équipe
    CombatantState* participants;   // Tableau des états des combattants
    int nombre_participants;        // Nombre total de combattants en combat
    int tour;                       // Compteur de tours
} Combat;

// Fonctions de base pour le cycle de vie du combat
void initialiser_combat(Combat* combat, Equipe* eq1, Equipe* eq2); // Initialise le combat
void gerer_tour_combat(Combat* combat);                            // Gère un tour complet de combat
void nettoyer_combat(Combat* combat);                              // Libère la mémoire du combat

// Fonctions d'action
void attaque_base(CombatantState* attaquant, CombatantState* cible); // Effectue une attaque de base
void utiliser_technique(CombatantState* attaquant, int tech_index, CombatantState* cible);
void utiliser_technique(CombatantState* attaquant, int tech_index, CombatantState* cible); // Utilise une technique spéciale

// Fonctions utilitaires
bool est_ko(Combattant* c);                  // Vérifie si un combattant est K.O.
void appliquer_effets(Combat* combat);       // Applique les effets temporaires en cours
bool verifier_victoire(Combat* combat);      // Vérifie si une équipe a gagné

#endif
