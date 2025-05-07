#ifndef AFF_COMBAT_H
#define AFF_COMBAT_H

#include "gestioncombattant.h"
#include <stdbool.h>
#include <stdio.h>

// Fonctions d'affichage pour le combat
void afficher_combat(const Combat* combat);
void afficher_statuts_combat(Combat* combat);
void afficher_menu_actions(EtatCombattant* joueur);
void afficher_resultat_combat(Combat* combat);

#endif /* AFF_COMBAT_H */
