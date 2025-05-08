#ifndef GEST_EFFETS_H
#define GEST_EFFETS_H

#include "gestioncombattant.h"

// Fonctions de gestion des effets
void appliquer_effet(EtatCombattant* cible, TypeEffet effet, int duree, float puissance);
// Fonction pour retirer un effet spécifique
void retirer_effet_type(Combat* combat, EtatCombattant* cible, TypeEffet type);
// Fonction pour retirer un effet à un index donné
void retirer_effet_index(Combat* combat, EtatCombattant* cible, int index_effet);
void appliquer_effets(Combat* combat);
TypeEffet convertir_nom_effet(const char* nom_effet);
const char* obtenir_nom_effet(TypeEffet effet);

#endif
