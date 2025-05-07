#ifndef GEST_EFFETS_H
#define GEST_EFFETS_H

#include "gestioncombattant.h"

// Fonctions de gestion des effets
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance);
void retirer_effet(EtatCombattant* cs, TypeEffet type);
void appliquer_effets(Combat* combat);
#endif
