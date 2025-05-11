#ifndef GEST_COMBAT_H // Directive de garde pour éviter les inclusions multiples
#define GEST_COMBAT_H // Définition de la macro de garde

#include "gestioncombattant.h" // Inclut les fonctionnalités de gestion des combattants
#include "util_combat.h" // Inclut les utilitaires de combat
#include "aff_combat.h" // Inclut les fonctions d'affichage du combat
#include "gest_effets.h" // Inclut la gestion des effets

#define MAX_PARTICIPANTS 6  // Maximum de 3 combattants par équipe, donc 6 au total

typedef enum { // Définition d'une énumération pour les niveaux de difficulté
    DIFFICULTE_FACILE = 0, // Niveau facile avec valeur 0
    DIFFICULTE_MOYENNE = 1, // Niveau moyen avec valeur 1
    DIFFICULTE_DIFFICILE = 2, // Niveau difficile avec valeur 2
    DIFFICULTE_MAX // Marqueur de fin d'énumération
} NiveauDifficulte; // Type énuméré pour la difficulté
// Fonction de validation
static inline bool difficulte_valide(NiveauDifficulte diff) { // Fonction inline vérifiant si la difficulté est valide
    return diff >= DIFFICULTE_FACILE && diff < DIFFICULTE_MAX; // Retourne vrai si la difficulté est dans la plage valide
}

// Fonctions du combat
void initialiser_combat_mode(Combat* combat, Equipe* eq1, Equipe* eq2, bool mode_jvj); // Initialise un nouveau combat
void gerer_tour_combat(Combat* combat); // Gère un tour complet de combat
void attaque_base(EtatCombattant* attaquant, EtatCombattant* cible); // Effectue une attaque de base
void gerer_tour_joueur(Combat* combat, EtatCombattant* joueur); // Gère le tour d'un joueur
int choisir_cible(Combat* combat, TypeJoueur controleur, int tech_index, EtatCombattant* attaquant); // Permet de choisir une cible
int lire_entier_securise(); // Lit un entier de manière sécurisée
float calculer_degats(Combattant* attaquant, Technique* tech, Combattant* cible); // Calcule les dégâts d'une attaque
void transition_joueurs(Combat* combat, EtatCombattant* joueur_suivant); // Gère la transition entre les joueurs
void gerer_tour_ia(Combat* combat, EtatCombattant* ia, NiveauDifficulte difficulte); // Gère le tour de l'IA
void configurer_combat(Combat* combat, bool mode_jvj, NiveauDifficulte* difficulte); // Configure les paramètres du combat

#endif /* GEST_COMBAT_H */ // Fin de la directive de garde
