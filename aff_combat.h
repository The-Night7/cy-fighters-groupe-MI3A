#ifndef AFF_COMBAT_H // Empêche les inclusions multiples du header
#define AFF_COMBAT_H // Définit le header

#include "gestioncombattant.h" // Inclut le header pour la gestion des combattants
#include "util_combat.h" // Inclut le header pour les utilitaires de combat
#include "gest_effets.h" // Inclut le header pour la gestion des effets
// Fonctions d'affichage
void afficher_combat(const Combat* combat); // Fonction pour afficher l'état du combat
void afficher_statuts_combat(Combat* combat); // Fonction pour afficher les statuts des combattants
void afficher_menu_actions(EtatCombattant* joueur); // Fonction pour afficher le menu des actions possibles
void afficher_resultat_combat(Combat* combat); // Fonction pour afficher le résultat final du combat

#endif // Fin de la définition du header
