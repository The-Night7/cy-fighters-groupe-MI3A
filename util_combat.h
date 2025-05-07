#ifndef UTIL_COMBAT_H
#define UTIL_COMBAT_H

#include "gestioncombattant.h"
#include <stdbool.h>
#include <stdio.h>

// Fonctions d'action
void utiliser_technique(EtatCombattant* attaquant, int tech_index, EtatCombattant* cible);

// Fonctions utilitaires
bool est_ko(Combattant* c);                  // Vérifie si un combattant est K.O.
bool verifier_victoire(Combat* combat);      // Vérifie si une équipe a gagné
void nettoyer_combat(Combat* combat);        // Libère la mémoire du combat

#endif /* UTIL_COMBAT_H */