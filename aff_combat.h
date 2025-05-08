#ifndef AFF_COMBAT_H
#define AFF_COMBAT_H

#include "gestioncombattant.h"
#include "util_combat.h"
#include "gest_effets.h"

// Fonctions d'affichage
void afficher_combat(const Combat* combat);
void afficher_statuts_combat(Combat* combat);
void afficher_menu_actions(EtatCombattant* joueur);
void afficher_resultat_combat(Combat* combat);

#endif
