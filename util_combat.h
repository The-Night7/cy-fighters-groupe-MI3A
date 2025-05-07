#ifndef UTIL_COMBAT_H
#define UTIL_COMBAT_H

#include "gestioncombattant.h"
#include "aff_combat.h"
#include "gest_combat.h"
#include "gest_effets.h"
#include <stdbool.h>
#include <stdio.h>

// Fonctions d'action
void utiliser_technique(EtatCombattant* attaquant, int tech_index, EtatCombattant* cible);

// Fonctions utilitaires
bool est_ko(Combattant* c);
bool verifier_victoire(Combat* combat);
void nettoyer_combat(Combat* combat);

#endif /* UTIL_COMBAT_H */